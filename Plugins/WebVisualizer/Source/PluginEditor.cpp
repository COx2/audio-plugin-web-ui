#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "WebViewBundleData.h"

#ifndef WEB_VIEW_FROM_SERVER
    #define WEB_VIEW_FROM_SERVER 0
#endif

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

    // Create webview.
    choc::ui::WebView::Options options;

#if JUCE_DEBUG
    options.enableDebugMode = true;
#else
    options.enableDebugMode = false;
#endif

    chocWebView = std::make_unique<choc::ui::WebView> (options);

    auto web_view_callback_on_hello_to_native =
        [safe_this = juce::Component::SafePointer (this)] (const choc::value::ValueView& args)
        -> choc::value::Value
    {
        juce::Logger::outputDebugString ("web_view_callback_hello_to_native called.");

        if (safe_this.getComponent() == nullptr)
        {
            return choc::value::Value (-1);
        }

        const auto choc_json_string = choc::json::toString (args);
        juce::Logger::outputDebugString (choc_json_string);

        return choc::value::Value (0);
    };

    auto web_view_callback_on_request_audio_buffer =
        [safe_this = juce::Component::SafePointer(this)](const choc::value::ValueView& args)
        -> choc::value::Value {
        if (safe_this.getComponent() == nullptr)
        {
            return choc::value::Value(-1);
        }

        const auto choc_json_string = choc::json::toString(args);
        //juce::Logger::outputDebugString(choc_json_string);

        auto fifo_access = safe_this->processorRef.getStereoAudioFIFOAccess();

        const int num_samples_to_read = 240;

        if (fifo_access.left->getUsedSlots() > num_samples_to_read && fifo_access.right->getUsedSlots() > num_samples_to_read)
        {
            std::vector<float> left_array;
            std::vector<float> right_array;
            
            for (int sample_idx = 0; sample_idx < num_samples_to_read; sample_idx++)
            {
                {
                    float value = 0.0f;
                    fifo_access.left->pop (value);
                    left_array.push_back (value);
                }
                {
                    float value = 0.0f;
                    fifo_access.right->pop (value);
                    right_array.push_back (value);
                }
            }

            auto object_to_send = choc::json::create (
                "audio_samples_left", choc::value::createArray (left_array),
                "audio_samples_right", choc::value::createArray (right_array)
            );

            return choc::value::Value (object_to_send);
        }


        return safe_this->cachedValueForView;
    };

    auto web_view_callback_on_toggle_changed =
        [safe_this = juce::Component::SafePointer (this)] (const choc::value::ValueView& args)
        -> choc::value::Value
    {
        if (safe_this.getComponent() == nullptr)
        {
            return choc::value::Value (-1);
        }

        const auto choc_json_string = choc::json::toString (args);

        const auto juce_json = juce::JSON::parse (choc_json_string);
        safe_this->valueTreeState.getParameter ("invertPhase")->setValueNotifyingHost ((bool) juce_json[0]["toggleValue"]);

        return choc::value::Value (0);
    };

    auto web_view_callback_on_sliider_changed =
        [safe_this = juce::Component::SafePointer (this)] (const choc::value::ValueView& args)
        -> choc::value::Value
    {
        if (safe_this.getComponent() == nullptr)
        {
            return choc::value::Value (-1);
        }

        const auto choc_json_string = choc::json::toString (args);

        const auto juce_json = juce::JSON::parse (choc_json_string);

        const auto normalised_value = juce::jmap<float> (
            (float) juce_json[0]["sliderValue"],
            (float) juce_json[0]["sliderRangeMin"],
            (float) juce_json[0]["sliderRangeMax"],
            0.0f,
            1.0f);

        // Should fix range convert.
        safe_this->valueTreeState.getParameter ("gain")->setValueNotifyingHost (normalised_value);

        return choc::value::Value (0);
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

    chocWebView->bind ("onHelloToNative", web_view_callback_on_hello_to_native);
    chocWebView->bind ("onRequestAudioBuffer", web_view_callback_on_request_audio_buffer);
    chocWebView->bind ("onToggleChanged", web_view_callback_on_toggle_changed);
    chocWebView->bind ("onSliderChanged", web_view_callback_on_sliider_changed);
    chocWebView->bind ("onInitialUpdate", web_view_callback_on_initial_update);

#if WEB_VIEW_FROM_SERVER
    chocWebView->navigate ("http://localhost:5173");
#else
    const auto html = juce::String::createStringFromData (WebView::view_html, WebView::view_htmlSize);
    chocWebView->setHTML (html.toStdString());
#endif

    juceWebViewHolder = createJUCEWebViewHolder (*chocWebView.get());
    addAndMakeVisible (juceWebViewHolder.get());
    
    // Parameter binding.
    valueTreeState.addParameterListener("gain", this);
    valueTreeState.addParameterListener("invertPhase", this);
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 400);
    setResizable (true, true);

    startTimerHz (30);
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

void AudioPluginAudioProcessorEditor::timerCallback()
{
    // Process onReceiveAudioBuffer.
    {
        auto fifo_access = this->processorRef.getStereoAudioFIFOAccess();

        auto object_to_send = choc::json::create();
        bool should_eval_js = false;
        const int num_samples_to_read = 2400;

        if (fifo_access.left->getUsedSlots() > num_samples_to_read)
        {
            std::vector<float> left_array;

            for (int sample_idx = 0; sample_idx < num_samples_to_read; sample_idx++)
            {
                float value = 0.0f;
                fifo_access.left->pop (value);
                left_array.push_back (value);
            }

            object_to_send.addMember ("audio_samples_left", choc::value::createArray (left_array));

            should_eval_js = true;
        }

        if (fifo_access.right->getUsedSlots() > num_samples_to_read)
        {
            std::vector<float> right_array;

            for (int sample_idx = 0; sample_idx < num_samples_to_read; sample_idx++)
            {
                float value = 0.0f;
                fifo_access.right->pop (value);
                right_array.push_back (value);
            }

            object_to_send.addMember ("audio_samples_right", choc::value::createArray (right_array));

            should_eval_js = true;
        }

        if (should_eval_js)
        {
            juce::String javascript = juce::String ("onReceiveAudioBuffer(") + choc::json::toString (object_to_send) + juce::String (")");
            this->chocWebView->evaluateJavascript (javascript.toStdString());
        }
    }
}
