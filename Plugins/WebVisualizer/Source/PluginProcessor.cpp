#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
{
    parameters = std::make_unique<juce::AudioProcessorValueTreeState>(*this, nullptr, juce::Identifier ("APVTSTutorial"), createParameterLayout());
    
    phaseParameter = parameters->getRawParameterValue ("invertPhase");
    gainParameter  = parameters->getRawParameterValue ("gain");

    audioFFIO_Left.reset (4800, 0.0f);
    audioFFIO_Right.reset (4800, 0.0f);
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::ignoreUnused (sampleRate, samplesPerBlock);
    
    auto phase = *phaseParameter < 0.5f ? 1.0f : -1.0f;
    previousGain = *gainParameter * phase;

    audioFFIO_Left.reset (65536, 0.0f);
    audioFFIO_Right.reset (65536, 0.0f);

    sineOsc_Left.setFrequency (880, sampleRate);
    sineOsc_Left.resetPhase();

    sineOsc_Right.setFrequency (1320, sampleRate);
    sineOsc_Right.resetPhase();
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.

    audioFFIO_Left.reset (65536, 0.0f);
    audioFFIO_Right.reset (65536, 0.0f);
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    auto phase = *phaseParameter < 0.5f ? 1.0f : -1.0f;
    auto currentGain = *gainParameter * phase;

    //for (int sample_idx = 0; sample_idx < buffer.getNumSamples(); sample_idx++)
    //{
    //    if (buffer.getNumChannels() > 0)
    //    {
    //        buffer.getWritePointer (0)[sample_idx] = sineOsc_Left.getSample();
    //    }
    //    if (buffer.getNumChannels() > 1)
    //    {
    //        buffer.getWritePointer (1)[sample_idx] = sineOsc_Right.getSample();
    //    }
    //}

    if (juce::approximatelyEqual (currentGain, previousGain))
    {
        buffer.applyGain (currentGain);
    }
    else
    {
        buffer.applyGainRamp (0, buffer.getNumSamples(), previousGain, currentGain);
        previousGain = currentGain;
    }

    for (int sample_idx = 0; sample_idx < buffer.getNumSamples(); sample_idx++)
    {
        if (buffer.getNumChannels() > 0)
        {
            audioFFIO_Left.push (buffer.getReadPointer (0)[sample_idx]);
        }
        if (buffer.getNumChannels() > 1)
        {
            audioFFIO_Right.push (buffer.getReadPointer (1)[sample_idx]);
        }
    }

}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
    
    auto state = parameters->copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
    
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (parameters->state.getType()))
            parameters->replaceState (juce::ValueTree::fromXml (*xmlState));
}

AudioPluginAudioProcessor::StereoAudioFIFOAccess AudioPluginAudioProcessor::getStereoAudioFIFOAccess()
{
    return { &audioFFIO_Left, &audioFFIO_Right };
}

juce::AudioProcessorValueTreeState::ParameterLayout AudioPluginAudioProcessor::createParameterLayout()
{
    // It will move should not const.
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> parameters;
    parameters.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ "gain", 1 },
          "Gain",
          juce::NormalisableRange<float>(0.0f, 1.0f),
          0.8f));
    parameters.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{ "invertPhase", 1 },
           "Invert Phase",
            false));
    return juce::AudioProcessorValueTreeState::ParameterLayout{ parameters.begin(), parameters.end() };
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}
