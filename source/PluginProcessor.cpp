/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ApiCppWeek3PluginAudioProcessor::ApiCppWeek3PluginAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor (BusesProperties()
    #if !JucePlugin_IsMidiEffect
        #if !JucePlugin_IsSynth
                          .withInput ("Input", juce::AudioChannelSet::stereo(), true)
        #endif
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
    #endif
      )
#endif
{
    addParameter (mGainParameter = new juce::AudioParameterFloat (
                      juce::ParameterID { "gain", 1 },
                      "Gain",
                      0.0f,
                      1.0f,
                      0.5f));
    mSmoothedGain.setCurrentAndTargetValue (0.5f);
}

ApiCppWeek3PluginAudioProcessor::~ApiCppWeek3PluginAudioProcessor()
{
}

//==============================================================================
void ApiCppWeek3PluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    mSmoothedGain.reset (sampleRate, 0.01f);
    mSmoothedGain.setTargetValue (mGainParameter->get());
}

void ApiCppWeek3PluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    mSmoothedGain.setTargetValue (mGainParameter->get());

    float* left_channel = buffer.getWritePointer (0);
    //    float* right_channel = buffer.getWritePointer (1);

    // simple sine tone generator
    float sine_hz = 180.f;
    float phase_delta = sine_hz / getSampleRate();
    for (int sample_idx = 0; sample_idx < buffer.getNumSamples(); sample_idx++)
    {
        float sine_out = std::sin (mSineTonePhase * 2 * M_PI);

        // Increment phase
        mSineTonePhase += phase_delta;
        // Wrap phase
        if (mSineTonePhase > 1.f)
        {
            mSineTonePhase -= 1.f;
        }
        left_channel[sample_idx] = sine_out;
        //        right_channel[sample_idx] = sine_out;
    }

    for (int sample_idx = 0; sample_idx > buffer.getNumSamples(); sample_idx++)
    {
        float gain_value = mSmoothedGain.getNextValue();
        left_channel[sample_idx] *= gain_value;
        //        right_channel[sample_idx] *= gain_value;
    }
}

juce::AudioParameterFloat* ApiCppWeek3PluginAudioProcessor::getGainParameter()
{
    return mGainParameter;
}

void ApiCppWeek3PluginAudioProcessor::setGainParameterValue (float inGain)
{
    DBG ("Setting gain value to " << inGain << ".");
    mGainParameter->setValueNotifyingHost (inGain);
    mSmoothedGain.setTargetValue (inGain);
}

float ApiCppWeek3PluginAudioProcessor::getGainParameterValue()
{
    return mGainParameter->get();
}

//==============================================================================
const juce::String ApiCppWeek3PluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ApiCppWeek3PluginAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool ApiCppWeek3PluginAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool ApiCppWeek3PluginAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double ApiCppWeek3PluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ApiCppWeek3PluginAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
        // so this should be at least 1, even if you're not really implementing programs.
}

int ApiCppWeek3PluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ApiCppWeek3PluginAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ApiCppWeek3PluginAudioProcessor::getProgramName (int index)
{
    return {};
}

void ApiCppWeek3PluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void ApiCppWeek3PluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ApiCppWeek3PluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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
        #if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
        #endif

    return true;
    #endif
}
#endif

//==============================================================================
bool ApiCppWeek3PluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ApiCppWeek3PluginAudioProcessor::createEditor()
{
    return new ApiCppWeek3PluginAudioProcessorEditor (*this);
}

//==============================================================================
void ApiCppWeek3PluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ApiCppWeek3PluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ApiCppWeek3PluginAudioProcessor();
}
