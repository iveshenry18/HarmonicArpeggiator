/*
  ==============================================================================

    GainListener.cpp
    Created: 10 Feb 2024 3:29:45pm
    Author:  Henry Ives

  ==============================================================================
*/

#include "GainListener.h"

GainListener::GainListener (juce::AudioProcessorEditor& editor) : editor (editor) {};

void GainListener::parameterValueChanged (int parameterIndex, float newValue)
{
    DBG ("Parameter " << parameterIndex << " changed to " << newValue);
    // This crashes the host!
    //    if (parameterIndex == 0)
    //    {
    //        auto newWidth = juce::roundToInt(newValue * (editor.getParentMonitorArea().getWidth()));
    //        editor.getPositioner()->applyNewBounds(juce::Rectangle<int>{
    //            editor.getX(),
    //            editor.getY(),
    //            newWidth,
    //            editor.getHeight()
    //        });
    //    }
}

void GainListener::parameterGestureChanged (int parameterIndex, bool gestureIsStarting) {};
