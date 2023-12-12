#pragma once

#include "PluginProcessor.h"
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_data_structures/juce_data_structures.h>
#include <gui/choc_WebView.h>

//==============================================================================
class AudioPluginAudioProcessorEditor final
    : public juce::AudioProcessorEditor
    , juce::Value::Listener
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    virtual void valueChanged (juce::Value& value) override;
    
    juce::Value valueParameterGain;
    juce::Value valueParameterInvertPhase;
    
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;

    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
    
    juce::AudioProcessorValueTreeState& valueTreeState;
 
    juce::Label gainLabel;
    juce::Slider gainSlider;
    std::unique_ptr<SliderAttachment> gainAttachment;
 
    juce::ToggleButton invertButton;
    std::unique_ptr<ButtonAttachment> invertAttachment;
    
    std::unique_ptr<choc::ui::WebView> chocWebView;
#if JUCE_WINDOWS
    std::unique_ptr<juce::HWNDComponent> juceHwndView;
#elif JUCE_MAC
    std::unique_ptr<juce::NSViewComponent> juceNsView;
#elif JUCE_LINUX
    std::unique_ptr<juce::XEmbedComponent> juceXEmbedView;
#endif

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};
