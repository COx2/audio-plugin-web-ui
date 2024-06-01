#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "WebViewBundleData.h"

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

    static std::string getExtension (juce::String filename)
    {
        return filename.fromLastOccurrenceOf (".", false, false).toStdString();
    }

    std::optional<choc::ui::WebView::Options::Resource> getResourceFromZipFile (juce::ZipFile& zipFile, const std::string& path)
    {
        auto file_path = (path == "/" ? "/index.html" : path);

        file_path = file_path.erase(0, 1);

        const auto zip_entry_idx = zipFile.getIndexOfFileName (file_path);

        if (zip_entry_idx < 0)
        {
            return std::nullopt;
        }

        const auto* zip_entry = zipFile.getEntry (zip_entry_idx);
        if (zip_entry == nullptr)
        {
            return std::nullopt;
        }

        auto zipped_reader = std::unique_ptr<juce::InputStream>(zipFile.createStreamForEntry (*zip_entry));
        std::string mime_type = getMimeType ("." + getExtension (zip_entry->filename));
        uint64_t uncompressed_size = zip_entry->uncompressedSize;

        juce::MemoryBlock memory_block (uncompressed_size);

        zipped_reader->read (memory_block.begin(), uncompressed_size);

        const auto&& str_view = std::string_view (memory_block.begin(), memory_block.getSize());

        return choc::ui::WebView::Options::Resource (str_view, mime_type);
    }
}

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p), valueTreeState (p.getAPVTS()), misWebViewBundle (WebView::WebViewBundle_zip, WebView::WebViewBundle_zipSize, false)
{
    zipWebViewBundle = std::make_unique<juce::ZipFile> (misWebViewBundle);

    juce::ignoreUnused (processorRef);
    
    gainLabel.setText ("Gain", juce::dontSendNotification);
    addAndMakeVisible (gainLabel);

    addAndMakeVisible (gainSlider);
    gainAttachment.reset (new SliderAttachment (valueTreeState, "gain", gainSlider));
    
    invertButton.setButtonText ("Invert Phase");
    addAndMakeVisible (invertButton);
    invertAttachment.reset (new ButtonAttachment (valueTreeState, "invertPhase", invertButton));

    // Create webview.
    choc::ui::WebView::Options options;

#if JUCE_DEBUG
    options.enableDebugMode = true;
#else
    options.enableDebugMode = false;
#endif

    chocWebView = std::make_unique<choc::ui::WebView> (options);

    options.fetchResource = [this] (const std::string& path)
        -> std::optional<choc::ui::WebView::Options::Resource>
    {
        if (this->zipWebViewBundle.get() == nullptr)
        {
            return std::nullopt;
        }

        return getResourceFromZipFile (*this->zipWebViewBundle, path);
    };

    chocWebView = std::make_unique<choc::ui::WebView>(options);

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

#if WEB_VIEW_FROM_SERVER
    chocWebView->navigate ("http://localhost:5173");
#endif

    juceWebViewHolder = createJUCEWebViewHolder (*chocWebView.get());
    addAndMakeVisible (juceWebViewHolder.get());

    // Parameter binding.
    valueTreeState.addParameterListener("gain", this);
    valueTreeState.addParameterListener("invertPhase", this);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 800);
    setResizable (true, true);
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
    juceWebViewHolder->setBounds (getLocalBounds());
}

void AudioPluginAudioProcessorEditor::parameterChanged(const juce::String& parameterID, float newValue)
{
    juce::String javascript = "";

    if (parameterID == "gain")
    {
        juce::DynamicObject::Ptr json = new juce::DynamicObject();
        json->setProperty ("parameterName", "gain");
        json->setProperty ("parameterValue", newValue);
        const auto js_args_json = juce::JSON::toString (json.get());
        javascript = juce::String ("onParameterChanged(") + js_args_json + juce::String (")");
    }
    else if (parameterID == "invertPhase")
    {
        juce::DynamicObject::Ptr json = new juce::DynamicObject();
        json->setProperty ("parameterName", "invertPhase");
        json->setProperty ("parameterValue", newValue);
        const auto js_args_json = juce::JSON::toString (json.get());
        javascript = juce::String ("onParameterChanged(") + js_args_json + juce::String (")");
    }

    if (javascript.isNotEmpty())
    {
        if (juce::MessageManager::getInstance()->isThisTheMessageThread())
        {
            const bool result = chocWebView->evaluateJavascript (javascript.toStdString());
            if (! result)
            {
                juce::Logger::outputDebugString ("Failed: " + javascript);
            }
        }
        else
        {
            juce::MessageManager::callAsync (
                [safe_this = juce::Component::SafePointer (this), javascript]
                {
                    if (safe_this.getComponent() == nullptr)
                    {
                        return;
                    }

                    const bool result = safe_this->chocWebView->evaluateJavascript (javascript.toStdString());
                    if (! result)
                    {
                        juce::Logger::outputDebugString ("Failed: " + javascript);
                    }
                });
        }
    }
}
