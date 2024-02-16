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
    setSize (static_cast<int> (audioProcessor.getGainValue()
                               * static_cast<float> (getParentMonitorArea().getWidth())),
        300);
    setResizable (true, true);

    //  TODO: this crashes the host when I open the editor after changing the parameter in the host.
    //  audioProcessor.getGainParameter()->addListener (&gainListener);
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

    gainSlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    gainSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    gainSlider.setRange (0.0, 1.0, 0.01);
    gainSlider.setValue (audioProcessor.getGainValue());
    gainSlider.onValueChange = [this] { audioProcessor.setGainValue (static_cast<float> (gainSlider.getValue())); };
    gainSlider.setBounds (0, 0, getWidth(), getHeight());
    addAndMakeVisible (gainSlider);
}

void ApiCppWeek3PluginAudioProcessorEditor::resized()
{
    audioProcessor.setGainValue (getWidth() / static_cast<float> (getParentMonitorArea().getWidth()));
}
