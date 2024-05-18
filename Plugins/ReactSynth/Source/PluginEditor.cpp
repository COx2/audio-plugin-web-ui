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

        return choc::ui::WebView::Options::Resource (memory_block.toString().toStdString(), mime_type);
    }
}

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p)
    , processorRef (p)
    , valueTreeState (p.getAPVTS())
    , misWebViewBundle (WebView::WebViewBundle_zip, WebView::WebViewBundle_zipSize, false)
{
    juce::ignoreUnused (processorRef);

    zipWebViewBundle = std::make_unique<juce::ZipFile> (misWebViewBundle);
    
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

    options.fetchResource = [this] (const std::string& path)
        -> std::optional<choc::ui::WebView::Options::Resource>
    {
        if (this->zipWebViewBundle.get() == nullptr)
        {
            return std::nullopt;
        }

        return getResourceFromZipFile (*this->zipWebViewBundle, path);
    };

    chocWebView = std::make_unique<choc::ui::WebView> (options);

    auto web_view_callback_on_midi_note_on =
        [safe_this = juce::Component::SafePointer (this)] (const choc::value::ValueView& args)
        -> choc::value::Value
    {
        if (safe_this.getComponent() == nullptr)
        {
            return choc::value::Value (-1);
        }

        const auto choc_json_string = choc::json::toString (args);
        //juce::Logger::outputDebugString ("note_on: " + choc_json_string);

        const auto json = juce::JSON::parse (choc_json_string);

        const auto midi_channel = json.getArray()->getReference(0);
        const auto midi_note_number = json.getArray()->getReference (1);

        if (! safe_this->midiKeyboardStatePtr.expired())
        {
            safe_this->midiKeyboardStatePtr.lock()->noteOn ((int) midi_channel, (int) midi_note_number, 0.6f);
        }

        return choc::value::Value (0);
    };

    auto web_view_callback_on_midi_note_off =
        [safe_this = juce::Component::SafePointer (this)] (const choc::value::ValueView& args)
        -> choc::value::Value
    {
        if (safe_this.getComponent() == nullptr)
        {
            return choc::value::Value (-1);
        }

        const auto choc_json_string = choc::json::toString (args);
        //juce::Logger::outputDebugString ("note_off: " + choc_json_string);

        const auto json = juce::JSON::parse (choc_json_string);

        const auto midi_channel = json.getArray()->getReference (0);
        const auto midi_note_number = json.getArray()->getReference (1);

        if (! safe_this->midiKeyboardStatePtr.expired())
        {
            safe_this->midiKeyboardStatePtr.lock()->noteOff ((int) midi_channel, (int) midi_note_number, 0.0f);
        }

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
        //juce::Logger::outputDebugString(choc_json_string);

        const auto juce_json = juce::JSON::parse (choc_json_string);
        //if (false)
        //{
        //    juce::Logger::outputDebugString(juce_json[0]["sliderName"]);
        //    juce::Logger::outputDebugString(juce_json[0]["sliderValue"]);
        //    juce::Logger::outputDebugString(juce_json[0]["sliderRangeMin"]);
        //    juce::Logger::outputDebugString(juce_json[0]["sliderRangeMax"]);
        //}

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

    auto web_view_callback_get_parameter_value =
        [safe_this = juce::Component::SafePointer (this)] (const choc::value::ValueView& args)
        -> choc::value::Value
    {
        if (safe_this.getComponent() == nullptr)
        {
            return choc::value::Value (-1);
        }

        const auto choc_json_string = choc::json::toString (args);
        //juce::Logger::outputDebugString(choc_json_string);

        const auto juce_json = juce::JSON::parse (choc_json_string);
        if (juce_json[0]["parameterName"] == "gain")
        {

        }

        // Should fix range convert.
        const auto value_normalized = safe_this->valueTreeState.getParameter ("gain")->getNormalisableRange().convertFrom0to1 (safe_this->valueTreeState.getParameter ("gain")->getValue());

        return choc::value::Value (value_normalized);
    };

    auto web_view_callback_on_initial_update =
        [safe_this = juce::Component::SafePointer (this)] (const choc::value::ValueView& args)
        -> choc::value::Value
    {
        if (safe_this.getComponent() == nullptr)
        {
            return choc::value::Value (-1);
        }

        const auto choc_json_string = choc::json::toString (args);
        juce::Logger::outputDebugString (choc_json_string);

        return choc::value::Value (0);
    };

    chocWebView->bind ("onMidiNoteOn", web_view_callback_on_midi_note_on);
    chocWebView->bind ("onMidiNoteOff", web_view_callback_on_midi_note_off);
    chocWebView->bind ("onSliderChanged", web_view_callback_on_sliider_changed);
    chocWebView->bind ("getParameterValue", web_view_callback_get_parameter_value);
    chocWebView->bind ("onInitialUpdate", web_view_callback_on_initial_update);

    //const auto script = R"(
    //// Utility functions.
    //function mapRange(value, fromMin, fromMax, toMin, toMax) {
    //    const clampedValue = Math.min(Math.max(value, fromMin), fromMax);
    //    const normalizedValue = (clampedValue - fromMin) / (fromMax - fromMin);
    //    const mappedValue = normalizedValue * (toMax - toMin) + toMin;
    //    return mappedValue;
    //}

    //// Event callback from JUCE.
    //function onParameterChanged(jsonData) {
    //    // This is the function that will be called from C++
    //    console.log('JSON from C++: onParameterChanged - ', jsonData);

    //    // Access the 'type' property in the JSON data
    //    const parameterName = jsonData.parameterName;

    //    // Update the HTML label with the message
    //    if(parameterName === "gain")
    //    {
    //        console.log('gainSlider: ');
    //    }
    //}

    //// Invoke C++ callback for requesting initial update.
    //onInitialUpdate();
    //)";

    //chocWebView->addInitScript (script);

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
    setSize (960, 480);
    setResizable(true, true);

    //startTimerHz (30);

    midiKeyboardStatePtr = processorRef.getMidiKeyboardState();
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
