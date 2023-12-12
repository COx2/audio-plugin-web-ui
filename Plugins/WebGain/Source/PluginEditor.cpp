#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "WebViewBundleData.h"

//==============================================================================
namespace
{
    std::string getMimeType(std::string const& ext) {
    static std::unordered_map<std::string, std::string> mimeTypes{
        { ".html",   "text/html" },
        { ".js",     "application/javascript" },
        { ".css",    "text/css" },
        { ".vrm",    "model/gltf-binary"},
    };

    if (mimeTypes.count(ext) > 0)
        return mimeTypes.at(ext);

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
    
    valueParameterGain.addListener(this);
    valueParameterInvertPhase.addListener(this);
    
    choc::ui::WebView::Options options;
    options.enableDebugMode = true;

    // auto asset_directory = juce::File("/path/to/asset");
    
    // options.fetchResource = [this, assetDirectory = asset_directory](const choc::ui::WebView::Options::Path& path)
    // -> std::optional<choc::ui::WebView::Options::Resource> {
    //   auto relative_path = "." + (path == "/" ? "/index.html" : path);
    //   auto file_to_read = assetDirectory.getChildFile(relative_path);

    //   juce::Logger::outputDebugString(file_to_read.getFullPathName());

    //   juce::MemoryBlock memory_block;
    //   if (!file_to_read.existsAsFile() || !file_to_read.loadFileAsData(memory_block))
    //     return {};

    //   return choc::ui::WebView::Options::Resource {
    //       std::vector<uint8_t>(memory_block.begin(), memory_block.end()),
    //       getMimeType(file_to_read.getFileExtension().toStdString())
    //   };
    // };

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
    setSize (800, 600);
    setResizable(true, true);

    auto web_view_on_click_button =
        [safe_this = juce::Component::SafePointer(this)](const choc::value::ValueView& args) -> choc::value::Value {

        auto message = "web_view_on_click_button() called with args: " + choc::json::toString(args);
        juce::Logger::outputDebugString(choc::json::toString(args));

        auto json = juce::JSON::parse(choc::json::toString(args));
        juce::Logger::outputDebugString(juce::JSON::toString(json));

        if (safe_this.getComponent() == nullptr)
        {
            return choc::value::Value(-1);
        }

        juce::AlertWindow::showMessageBoxAsync(
            juce::MessageBoxIconType::WarningIcon,
            "onClickButton is called from WebView!!!", message);

        return choc::value::Value(0);
    };

    auto web_view_on_sliider_changed =
        [safe_this = juce::Component::SafePointer(this)](const choc::value::ValueView& args) -> choc::value::Value {

        auto message = "web_view_on_sliider_changed() called with args: " + choc::json::toString(args);
        juce::Logger::outputDebugString(choc::json::toString(args));

        auto json = juce::JSON::parse(choc::json::toString(args));
        juce::Logger::outputDebugString(juce::JSON::toString(json));

        if (safe_this.getComponent() == nullptr)
        {
            return choc::value::Value(-1);
        }

        return choc::value::Value(0);
    };

    chocWebView->bind("onClickButton", web_view_on_click_button);
    chocWebView->bind("onSliderChanged", web_view_on_sliider_changed);

    const auto html = juce::String::createStringFromData(WebView::view_html, WebView::view_htmlSize);
    chocWebView->setHTML(html.toStdString());
    
    valueParameterGain.referTo(valueTreeState.getParameterAsValue("gain"));
    valueParameterInvertPhase.referTo(valueTreeState.getParameterAsValue("invertPhase"));
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    valueParameterGain.removeListener(this);
    valueParameterInvertPhase.removeListener(this);
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
    juceHwndView->setBounds(rect_ui);
#elif JUCE_MAC
    juceNsView->setBounds(rect_ui);
#elif JUCE_LINUX
    juceXEmbedView->setBounds(getLocalBounds());
#endif
}


void AudioPluginAudioProcessorEditor::valueChanged (juce::Value& value)
{
    if(value.refersToSameSourceAs(valueParameterGain))
    {
        
    }
    else if(value.refersToSameSourceAs(valueParameterInvertPhase))
    {
        
    }
    
    {
        juce::DynamicObject::Ptr json = new juce::DynamicObject();
        json->setProperty("message", "hello from cpp");
        const auto json_string = juce::JSON::toString(json.get());
        
        juce::String javascript = juce::String("HelloWithJson(") + json_string + juce::String(")");
        
        juce::Logger::outputDebugString(juce::JSON::toString(javascript));
        
        this->chocWebView->evaluateJavascript(javascript.toStdString());
    }
}
