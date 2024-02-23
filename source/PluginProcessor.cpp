/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout params;
    params.add (std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { "gain", 1 }, "Gain", 0.0f, 1.0f, 0.5f));
    return params;
}

//==============================================================================
PluginProcessor::PluginProcessor() :
#ifndef JucePlugin_PreferredChannelConfigurations
                                     AudioProcessor (BusesProperties()
    #if !JucePlugin_IsMidiEffect
        #if !JucePlugin_IsSynth
                          .withInput ("Input", juce::AudioChannelSet::stereo(), true)
        #endif
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
    #endif
                                             ),
#endif
                                     parameters (*this, nullptr, "PARAMETERS", createParameterLayout())
{
    mGainParameter = parameters.getRawParameterValue ("gain");
    mSineTonePhase = 0;
    mSmoothedGain.setCurrentAndTargetValue (*mGainParameter);
    // TODO: maybe register a callback here to change mSmoothedGain when mGainParameter changes
}

PluginProcessor::~PluginProcessor() = default;

//==============================================================================
void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    mSmoothedGain.reset (sampleRate, 0.01f);
    mSmoothedGain.setTargetValue (*mGainParameter);
}

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
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

    // TODO: is this sufficient for keeping smoothed gain up-to-date with the gain param?
    mSmoothedGain.setTargetValue (*mGainParameter);

    float* left_channel = buffer.getWritePointer (0);
    float* right_channel = buffer.getWritePointer (1);

    // simple sine tone generator
    float sine_hz = 180.f;
    auto phase_delta = static_cast<float> (sine_hz / getSampleRate());
    for (int sample_idx = 0; sample_idx < buffer.getNumSamples(); sample_idx++)
    {
        auto sine_out = static_cast<float> (std::sin (mSineTonePhase * 2 * M_PI));

        // Increment phase
        mSineTonePhase += phase_delta;
        // Wrap phase
        if (mSineTonePhase > 1.f)
        {
            mSineTonePhase -= 1.f;
        }
        left_channel[sample_idx] = sine_out;
        right_channel[sample_idx] = sine_out;
    }

    for (int sample_idx = 0; sample_idx < buffer.getNumSamples(); sample_idx++)
    {
        float gain_value = mSmoothedGain.getNextValue();
        left_channel[sample_idx] *= gain_value;
        right_channel[sample_idx] *= gain_value;
    }
}

//==============================================================================
const juce::String PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool PluginProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool PluginProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double PluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
        // so this should be at least 1, even if you're not really implementing programs.
}

int PluginProcessor::getCurrentProgram()
{
    return 0;
}

void PluginProcessor::setCurrentProgram (int index)
{
}

const juce::String PluginProcessor::getProgramName (int index)
{
    return {};
}

void PluginProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void PluginProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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
bool PluginProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor (*this, parameters);
}

//==============================================================================
void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState != nullptr)
        if (xmlState->hasTagName (parameters.state.getType()))
            parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}
