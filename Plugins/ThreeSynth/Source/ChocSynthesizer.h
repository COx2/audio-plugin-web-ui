#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

//==============================================================================
class ChocSynthesizer
    : public juce::Synthesiser
{
public:
    //==============================================================================
    ChocSynthesizer();
    ~ChocSynthesizer() override;

    //==============================================================================
    void setOscillatorType();

private:
    //==============================================================================
    juce::SynthesiserSound::Ptr soundPtr;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChocSynthesizer)
};