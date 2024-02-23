/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"

#include "PluginProcessor.h"
#include <memory>

//==============================================================================
PluginEditor::PluginEditor (PluginProcessor& parent, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&parent), audioProcessor (parent), valueTreeState (vts)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (500, 300);
}

PluginEditor::~PluginEditor() = default;

//==============================================================================
void PluginEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::black);

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Hey man it's your friend Henry", getLocalBounds(), juce::Justification::centred, 1);

    gainSlider.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    gainSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    gainSlider.setBounds (0, 0, getWidth(), getHeight());
    addAndMakeVisible (gainSlider);

    gainAttachment = std::make_unique<SliderAttachment> (valueTreeState, "gain", gainSlider);
}

void PluginEditor::resized()
{
}
