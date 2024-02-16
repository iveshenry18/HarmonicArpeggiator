/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"
#include "GainListener.h"
#include "PluginProcessor.h"

//==============================================================================
ApiCppWeek3PluginAudioProcessorEditor::ApiCppWeek3PluginAudioProcessorEditor (ApiCppWeek3PluginAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), gainListener (*this)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (audioProcessor.getGainValue() * getParentMonitorArea().getWidth(), 300);
    setResizable (true, true);

    //    audioProcessor.getGainParameter()->addListener (&gainListener);
}

ApiCppWeek3PluginAudioProcessorEditor::~ApiCppWeek3PluginAudioProcessorEditor() = default;

//==============================================================================
void ApiCppWeek3PluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::black);

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hey man it's your friend Henry", getLocalBounds(), juce::Justification::centred, 1);
}

void ApiCppWeek3PluginAudioProcessorEditor::resized()
{
    audioProcessor.setGainValue (getWidth() / static_cast<float> (getParentMonitorArea().getWidth()));
}
