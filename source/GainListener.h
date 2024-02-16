/*
  ==============================================================================

    GainListener.h
    Created: 10 Feb 2024 3:29:45pm
    Author:  Henry Ives

  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

class GainListener : public juce::AudioProcessorParameter::Listener
{
public:
    GainListener (juce::AudioProcessorEditor&);
    void parameterGestureChanged (int parameterIndex, bool gestureIsStarting) override;
    void parameterValueChanged (int parameterIndex, float newValue) override;

private:
    juce::AudioProcessorEditor& editor;
};
