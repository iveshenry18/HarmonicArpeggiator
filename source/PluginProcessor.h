/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include <utility>

using ChannelAndNoteNumber = std::pair<int, int>;

struct MidiWithStart
{
    juce::MidiMessage midiMessage;
    int64_t absoluteSamplePosition;

    MidiWithStart (juce::MidiMessage  message, int64_t sample) : midiMessage (std::move(message)), absoluteSamplePosition (sample) {}
};

//==============================================================================
/**
*/
class PluginProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    PluginProcessor();
    ~PluginProcessor() override;

    // AUDIO FUNCTIONS
    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    // PRESET / STATE FUNCTIONS
    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
#endif

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

private:
    juce::AudioProcessorValueTreeState parameters;

    double mSampleRate = 44100;
    // If the host doesn't provide transport info, we need to keep track of sample time ourselves
    int mSamplesPerBlock = 512;
    // Just so you know at 44.1kHz this will overflow if the program runs for 6.6 million years.
    // It also happens to be exactly how the playhead behaves in standalone mode
    int64_t mTimeInSamples = 0;

    /**
     * (channel, noteNumber) -> midiMessage
     */
    std::map<ChannelAndNoteNumber, MidiWithStart> heldMidiNotes;
    double getRetrigTimeSamples (int number) const;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};
