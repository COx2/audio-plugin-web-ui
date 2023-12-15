#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
namespace
{
    std::string getMimeType(std::string const& ext)
    {
        static std::unordered_map<std::string, std::string> mimeTypes{
          { ".html",   "text/html" },
          { ".js",     "application/javascript" },
          { ".css",    "text/css" },
          { ".json",   "application/json"},
          { ".svg", "image/svg+xml"},
          { ".svgz", "image/svg+xml"},
        };

        if (mimeTypes.count(ext) > 0)
        {
            return mimeTypes.at(ext);
        }

        return "application/octet-stream";
    }

    enum
    {
        paramControlHeight = 40,
        paramLabelWidth    = 80,
        paramSliderWidth   = 300
    };
}

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p)
    , processorRef (p)
    , valueTreeState (p.getAPVTS())
{
    juce::ignoreUnused (processorRef);
    
    gainLabel.setText ("Gain", juce::dontSendNotification);
    addAndMakeVisible (gainLabel);

    addAndMakeVisible (gainSlider);
    gainAttachment.reset (new SliderAttachment (valueTreeState, "gain", gainSlider));
    
    invertButton.setButtonText ("Invert Phase");
    addAndMakeVisible (invertButton);
    invertAttachment.reset (new ButtonAttachment (valueTreeState, "invertPhase", invertButton));

    choc::ui::WebView::Options options;
    options.enableDebugMode = false;

    auto asset_directory = juce::File::getSpecialLocation(juce::File::SpecialLocationType::currentExecutableFile).getSiblingFile("WebView");
    
    options.fetchResource = [this, assetDirectory = asset_directory](const choc::ui::WebView::Options::Path& path)
        -> std::optional<choc::ui::WebView::Options::Resource> {
            auto relative_path = "." + (path == "/" ? "/index.html" : path);
            auto file_to_read = assetDirectory.getChildFile(relative_path);

            juce::Logger::outputDebugString(file_to_read.getFullPathName());

            juce::MemoryBlock memory_block;
            if (!file_to_read.existsAsFile() || !file_to_read.loadFileAsData(memory_block))
                return {};

            return choc::ui::WebView::Options::Resource {
                std::vector<uint8_t>(memory_block.begin(), memory_block.end()),
                getMimeType(file_to_read.getFileExtension().toStdString())
        };
    };

    chocWebView = std::make_unique<choc::ui::WebView>(options);
#if JUCE_WINDOWS
    juceHwndView = std::make_unique<juce::HWNDComponent>();
    juceHwndView->setHWND(chocWebView->getViewHandle());
    addAndMakeVisible(juceHwndView.get());
#elif JUCE_MAC
    juceNsView = std::make_unique<juce::NSViewComponent>();
    juceNsView->setView(chocWebView->getViewHandle());
    addAndMakeVisible(juceNsView.get());
#elif JUCE_LINUX
    juceXEmbedView = std::make_unique<juce::XEmbedComponent>(chocWebView->getViewHandle());
    addAndMakeVisible(juceXEmbedView.get());
#endif

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 800);
    setResizable(true, true);

    auto web_view_callback_on_toggle_changed =
        [safe_this = juce::Component::SafePointer(this)](const choc::value::ValueView& args)
        -> choc::value::Value {
        if (safe_this.getComponent() == nullptr)
        {
            return choc::value::Value(-1);
        }

        const auto choc_json_string = choc::json::toString(args);
        //juce::Logger::outputDebugString(choc_json_string);

        const auto juce_json = juce::JSON::parse(choc_json_string);
        safe_this->valueTreeState.getParameter("invertPhase")->setValueNotifyingHost((bool)juce_json[0]["toggleValue"]);

        return choc::value::Value(0);
    };

    auto web_view_callback_on_sliider_changed =
        [safe_this = juce::Component::SafePointer(this)](const choc::value::ValueView& args)
        -> choc::value::Value {
        if (safe_this.getComponent() == nullptr)
        {
        return choc::value::Value(-1);
        }

        const auto choc_json_string = choc::json::toString(args);
        //juce::Logger::outputDebugString(choc_json_string);

        const auto juce_json = juce::JSON::parse(choc_json_string);
        //if (false)
        //{
        //    juce::Logger::outputDebugString(juce_json[0]["sliderName"]);
        //    juce::Logger::outputDebugString(juce_json[0]["sliderValue"]);
        //    juce::Logger::outputDebugString(juce_json[0]["sliderRangeMin"]);
        //    juce::Logger::outputDebugString(juce_json[0]["sliderRangeMax"]);
        //}

        const auto normalised_value = juce::jmap<float>(
        (float)juce_json[0]["sliderValue"], 
        (float)juce_json[0]["sliderRangeMin"], (float)juce_json[0]["sliderRangeMax"],
        0.0f, 1.0f);

        // Should fix range convert.
        safe_this->valueTreeState.getParameter("gain")->setValueNotifyingHost(normalised_value);

        return choc::value::Value(0);
    };

    auto web_view_callback_on_initial_update =
        [safe_this = juce::Component::SafePointer(this)](const choc::value::ValueView& args)
        -> choc::value::Value {
        if (safe_this.getComponent() == nullptr)
        {
          return choc::value::Value(-1);
        }

        safe_this->parameterChanged("gain", safe_this->valueTreeState.getRawParameterValue("gain")->load());
        safe_this->parameterChanged("invertPhase", safe_this->valueTreeState.getRawParameterValue("invertPhase")->load());

        return choc::value::Value(0);
    };

    chocWebView->bind("onToggleChanged", web_view_callback_on_toggle_changed);
    chocWebView->bind("onSliderChanged", web_view_callback_on_sliider_changed);
    chocWebView->bind("onInitialUpdate", web_view_callback_on_initial_update);

//  #if JUCE_DEBUG
//      chocWebView->navigate("http://localhost:5173/");
//  #endif

    valueTreeState.addParameterListener("gain", this);
    valueTreeState.addParameterListener("invertPhase", this);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
  valueTreeState.removeParameterListener("gain", this);
  valueTreeState.removeParameterListener("invertPhase", this);
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void AudioPluginAudioProcessorEditor::resized()
{
    auto rect_ui = getLocalBounds();

    auto gainRect = rect_ui.removeFromTop (paramControlHeight);
    gainLabel .setBounds (gainRect.removeFromLeft (paramLabelWidth));
    gainSlider.setBounds (gainRect);

    invertButton.setBounds (rect_ui.removeFromTop (paramControlHeight));

    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
#if JUCE_WINDOWS
    juceHwndView->setBounds(getLocalBounds());
#elif JUCE_MAC
    juceNsView->setBounds(getLocalBounds());
#elif JUCE_LINUX
    juceXEmbedView->setBounds(getLocalBounds());
#endif
}

void AudioPluginAudioProcessorEditor::parameterChanged(const juce::String& parameterID, float newValue)
{
  if (parameterID == "gain")
  {
      juce::DynamicObject::Ptr json = new juce::DynamicObject();
      json->setProperty("parameterName", "gain");
      json->setProperty("parameterValue", newValue);
      const auto js_args_json = juce::JSON::toString(json.get());
      juce::String javascript = juce::String("onParameterChanged(") + js_args_json + juce::String(")");

      //juce::Logger::outputDebugString(juce::JSON::toString(javascript));

      this->chocWebView->evaluateJavascript(javascript.toStdString());
  }
  else if (parameterID == "invertPhase")
  {
      juce::DynamicObject::Ptr json = new juce::DynamicObject();
      json->setProperty("parameterName", "invertPhase");
      json->setProperty("parameterValue", newValue);
      const auto js_args_json = juce::JSON::toString(json.get());
      juce::String javascript = juce::String("onParameterChanged(") + js_args_json + juce::String(")");

      //juce::Logger::outputDebugString(juce::JSON::toString(javascript));

      this->chocWebView->evaluateJavascript(javascript.toStdString());
  }
}
