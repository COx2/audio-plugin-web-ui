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
}

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);

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

    //chocWebView->navigate("http://www.google.com");

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
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("WebGain", getLocalBounds(), juce::Justification::centred, 1);
}

void AudioPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
#if JUCE_WINDOWS
    juceHwndView->setBounds(getLocalBounds());
#elif JUCE_MAC
    juceNsView->setBounds(getLocalBounds());
#elif JUCE_LINUX
    juceXEmbedView->setBounds(getLocalBounds());
#endif

    juce::MessageManager::callAsync(
        [safe_this = juce::Component::SafePointer(this)]() {
        if (safe_this.getComponent() == nullptr)
        {
        return;
        }

        {
        safe_this->chocWebView->evaluateJavascript("Hello()");
        }

        {
        juce::DynamicObject::Ptr json = new juce::DynamicObject();
        json->setProperty("message", "hello from cpp");
        const auto json_string = juce::JSON::toString(json.get());

        juce::String javascript = juce::String("HelloWithJson(") + json_string + juce::String(")");

        juce::Logger::outputDebugString(juce::JSON::toString(javascript));

        safe_this->chocWebView->evaluateJavascript(javascript.toStdString());
        }
        }
    );
}
