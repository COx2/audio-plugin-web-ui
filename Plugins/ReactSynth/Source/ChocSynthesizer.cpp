#include "ChocSynthesizer.h"
#include <audio/choc_Oscillators.h>

namespace
{
//==============================================================================
class ChocSynthesizerSound
    : public juce::SynthesiserSound
{
public:
    enum class Waveform
    {
        kSine,
        kSaw,
        kSquare,
        kTriangle,
    };

    //==============================================================================
    ChocSynthesizerSound(const juce::BigInteger& supportMidiNotes, Waveform waveform = Waveform::kSine, juce::ADSR::Parameters ampEnvelople = juce::ADSR::Parameters())
        : supportMidiNotes(supportMidiNotes)
        , waveform(waveform)
        , paramsAmpEnvelope(ampEnvelople)
    {
    }

    ~ChocSynthesizerSound() override
    {
    }

    void setFrequencyAndSamplerate(float frequency, float samplerate)
    {
        sineOscillator.setFrequency(frequency, samplerate);
        sineOscillator.resetPhase();

        sawOscillator.setFrequency(frequency, samplerate);
        sawOscillator.resetPhase();

        squareOscillator.setFrequency(frequency, samplerate);
        squareOscillator.resetPhase();

        triangleOscillator.setFrequency(frequency, samplerate);
        triangleOscillator.resetPhase();
    }

    float getNextSample()
    {
        switch (waveform)
        {
        case ChocSynthesizerSound::Waveform::kSine:
            return sineOscillator.getSample();
            break;
        case ChocSynthesizerSound::Waveform::kSquare:
            return squareOscillator.getSample();
            break;
        case ChocSynthesizerSound::Waveform::kSaw:
            return sawOscillator.getSample();
            break;
        case ChocSynthesizerSound::Waveform::kTriangle:
            return triangleOscillator.getSample();
            break;
        default:
            break;
        }

        // Fallback
        return sineOscillator.getSample();
    }

    //==============================================================================
    void setEnvelopeParameters(juce::ADSR::Parameters parametersToUse) 
    { 
        paramsAmpEnvelope = parametersToUse;
    }

    const juce::ADSR::Parameters& getEnvelopeParameters() const 
    {
        return paramsAmpEnvelope; 
    }

    //==============================================================================
    bool appliesToNote(int midiNoteNumber) override
    {
        return supportMidiNotes[midiNoteNumber];
    }

    bool appliesToChannel(int midiChannel) override
    {
        return true;
    }

private:
    //==============================================================================
    juce::BigInteger supportMidiNotes;
    Waveform waveform;
    juce::ADSR::Parameters paramsAmpEnvelope;

    choc::oscillator::Sine<float> sineOscillator;
    choc::oscillator::Saw<float> sawOscillator;
    choc::oscillator::Triangle<float> triangleOscillator;
    choc::oscillator::Square<float> squareOscillator;

    JUCE_LEAK_DETECTOR(ChocSynthesizerSound)
};

//==============================================================================
class ChocSynthesizerVoice
    : public juce::SynthesiserVoice
{
public:
    //==============================================================================
    ChocSynthesizerVoice()
    {}

    ~ChocSynthesizerVoice() override
    {}

private:
    //==============================================================================
    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<const ChocSynthesizerSound*> (sound) != nullptr;
    }

    bool isVoiceActive() const override
    {
        return juce::SynthesiserVoice::isVoiceActive() || ampEnvelope.isActive();
    }

    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* soundPtr, int /*currentPitchWheelPosition*/) override
    {
        if (auto* sound = dynamic_cast<ChocSynthesizerSound*> (soundPtr))
        {
            // In JUCE API MidiNoteNumber MiddleC is C4
            const double frequency_of_A4 = 440.0;
            const double frequency_of_note = juce::MidiMessage::getMidiNoteInHertz(midiNoteNumber, frequency_of_A4);
            const double sample_rate = getSampleRate();
            gain_velocity = velocity;

            sound->setFrequencyAndSamplerate(frequency_of_note, sample_rate);

            ampEnvelope.setSampleRate(sample_rate);
            ampEnvelope.setParameters(sound->getEnvelopeParameters());
            ampEnvelope.noteOn();
        }
        else
        {
            jassertfalse; // this object can only play ChocSynthesizerSound!
        }
    }

    void stopNote(float /*velocity*/, bool allowTailOff) override
    {
        if (allowTailOff)
        {
            ampEnvelope.noteOff();
        }
        else
        {
            clearCurrentNote();
            ampEnvelope.reset();
        }
    }

    void pitchWheelMoved(int /*newValue*/) override {}
    void controllerMoved(int /*controllerNumber*/, int /*newValue*/) override {}

    //==============================================================================
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override
    {
        if (auto* playingSound = dynamic_cast<ChocSynthesizerSound*> (getCurrentlyPlayingSound().get()))
        {
            for (int sample_idx = 0; sample_idx < outputBuffer.getNumSamples(); sample_idx++)
            {
                auto value_to_out = playingSound->getNextSample();
                auto envelopeValue = ampEnvelope.getNextSample();
                value_to_out = value_to_out * envelopeValue * gain_velocity;

                for(int ch_idx = 0; ch_idx < outputBuffer.getNumChannels(); ch_idx++)
                {
                    outputBuffer.setSample (ch_idx, sample_idx, value_to_out);
                }
            }
        }
    }
    using juce::SynthesiserVoice::renderNextBlock;

    //==============================================================================
    juce::ADSR ampEnvelope;
    float gain_velocity;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChocSynthesizerVoice)
};
}

//==============================================================================
// ChocSynthesizer
//==============================================================================
ChocSynthesizer::ChocSynthesizer()
{
}

ChocSynthesizer::~ChocSynthesizer()
{
}

void ChocSynthesizer::setOscillatorType()
{
    juce::BigInteger support_midi_notes;
    support_midi_notes.setRange(0, 126, true);
    juce::SynthesiserSound::Ptr newSound = new ChocSynthesizerSound(support_midi_notes,
        ChocSynthesizerSound::Waveform::kSine,
        juce::ADSR::Parameters(0.01f, 0.01f, 1.0f, 0.01f));

    // prepare sounds
    soundPtr = newSound;
    this->clearSounds();
    this->addSound(soundPtr);

    // prepare voices
    this->clearVoices();
    for (int v_idx = 0; v_idx < support_midi_notes.countNumberOfSetBits(); v_idx++)
    {
        this->addVoice(new ChocSynthesizerVoice());
    }
}
