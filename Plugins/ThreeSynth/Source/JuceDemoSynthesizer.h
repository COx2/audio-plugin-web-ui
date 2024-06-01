#pragma once

#include <juce_audio_basics/juce_audio_basics.h>

//==============================================================================
class JuceDemoSynthesizer
    : public juce::Synthesiser
{
public:
    //==============================================================================
    JuceDemoSynthesizer();
    ~JuceDemoSynthesizer() override;

    //==============================================================================
    void initialise();

private:

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JuceDemoSynthesizer)
};