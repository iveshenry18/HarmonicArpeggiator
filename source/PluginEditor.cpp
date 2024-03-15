/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginEditor.h"

#include "PluginProcessor.h"
#include <memory>

//==============================================================================
PluginEditor::PluginEditor (PluginProcessor& parent)
    : AudioProcessorEditor (&parent), audioProcessor (parent)
{
    mBasisNoteSlider.setRange (0, 127);
    mBasisNoteSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, true, 100, 20);
    mBasisNoteSlider.setTitle("Basis Note");
    mBasisNoteSlider.setHelpText("This is the note that will play at the Time Base");
    mBasisNoteSlider.setSliderStyle (juce::Slider::SliderStyle::RotaryVerticalDrag);
    addAndMakeVisible (mBasisNoteSlider);
    mBasisNoteSliderAttachment.reset (new juce::AudioProcessorValueTreeState::SliderAttachment (
        audioProcessor.getVTS(),
        "basis_note",
        mBasisNoteSlider));

    mBasisNoteLabel.setText("Basis Note", juce::dontSendNotification);
    mBasisNoteLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(mBasisNoteLabel);

    mTimeBaseSlider.setRange (0, 5000);
    mTimeBaseSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, true, 100, 20);
    mTimeBaseSlider.setTitle("Time Base");
    mTimeBaseSlider.setHelpText("This is the time value of the basis note");
    mTimeBaseSlider.setSliderStyle (juce::Slider::SliderStyle::RotaryVerticalDrag);
    addAndMakeVisible (mTimeBaseSlider);
    mTimeBaseSliderAttachment.reset (new juce::AudioProcessorValueTreeState::SliderAttachment (
        audioProcessor.getVTS(),
        "time_base",
       mTimeBaseSlider));

    mTimeBaseLabel.setText("Time Base", juce::dontSendNotification);
    mTimeBaseLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(mTimeBaseLabel);

    mSyncTimeToggle.setButtonText("Sync");
    mSyncTimeToggle.setHelpText("Whether the Time Base should be an absolute value (ms) or a rhythmic value");
    addAndMakeVisible (mSyncTimeToggle);
    mSyncTimeToggleAttachment.reset (new juce::AudioProcessorValueTreeState::ButtonAttachment (
        audioProcessor.getVTS(),
        "sync_time",
        mSyncTimeToggle));

    mLearnBasisNoteToggle.setButtonText("Learn");
    mLearnBasisNoteToggle.setHelpText("Learn the Basis Note");
    addAndMakeVisible (mLearnBasisNoteToggle);
    mLearnBasisNoteToggleAttachment.reset (new juce::AudioProcessorValueTreeState::ButtonAttachment (
        audioProcessor.getVTS(),
        "learn_basis_note",
        mLearnBasisNoteToggle));



    setSize (500, 300);
}

PluginEditor::~PluginEditor() = default;

//==============================================================================
void PluginEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::grey);
}

void PluginEditor::resized()
{
    auto bounds = getLocalBounds();
    auto knob_area = bounds.removeFromTop(getHeight() * .7);
    auto label_area = bounds.removeFromTop(getHeight() * .3);

    mBasisNoteSlider.setBounds(knob_area.removeFromLeft(3*getWidth()/10));
    mLearnBasisNoteToggle.setBounds(knob_area.removeFromLeft(getWidth()/5).removeFromTop(knob_area.getHeight()/2));
    mTimeBaseSlider.setBounds(knob_area.removeFromLeft(3*getWidth()/10));
    mSyncTimeToggle.setBounds(knob_area.removeFromLeft(getWidth()/5).removeFromTop(knob_area.getHeight()/2));

    mBasisNoteLabel.setBounds(label_area.removeFromLeft(3*getWidth()/10));
    mTimeBaseLabel.setBounds(label_area.removeFromRight(getWidth()/2).removeFromLeft(3*getWidth()/10));
}
