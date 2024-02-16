/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "GainListener.h"
#include "PluginProcessor.h"
#include <juce_audio_processors/juce_audio_processors.h>

//==============================================================================
/**
*/
class ApiCppWeek3PluginAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    ApiCppWeek3PluginAudioProcessorEditor (ApiCppWeek3PluginAudioProcessor&);
    ~ApiCppWeek3PluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ApiCppWeek3PluginAudioProcessor& audioProcessor;
    GainListener gainListener;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ApiCppWeek3PluginAudioProcessorEditor)
};
