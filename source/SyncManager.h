//
// Created by Henry Ives on 3/15/24.
//

#pragma once

#include "Fraction.h"
#include <juce_audio_processors/juce_audio_processors.h>

struct TimeDivisionInfo
{
    juce::String timeDivisionString;
    Fraction barMultiplier;
};

static const std::vector<TimeDivisionInfo> TIME_DIVISIONS {
    { "1/128", { 1, 128 } },
    { "1/64T", { 1, 96 } },
    { "1/128D", { 3, 256 } },
    { "1/64", { 1, 64 } },
    { "1/32T", { 1, 48 } },
    { "1/64D", { 3, 128 } },
    { "1/32", { 1, 32 } },
    { "1/16T", { 1, 24 } },
    { "1/32D", { 3, 64 } },
    { "1/16", { 1, 16 } },
    { "1/8T", { 1, 12 } },
    { "1/16D", { 3, 32 } },
    { "1/8", { 1, 8 } },
    { "1/4T", { 1, 6 } },
    { "1/8D", { 3, 16 } },
    { "1/4", { 1, 4 } },
    { "1/2T", { 1, 3 } },
    { "1/4D", { 3, 8 } },
    { "1/2", { 1, 2 } },
    { "1/1T", { 2, 3 } },
    { "1/2D", { 3, 4 } },
    { "1 Bar", { 1, 1 } },
    { "1.5 Bars", { 3, 2 } },
    { "2 Bars", { 2, 1 } },
    { "3 Bars", { 3, 1 } },
    { "4 Bars", { 4, 1 } },
    { "6 Bars", { 6, 1 } },
    { "8 Bars", { 8, 1 } },
    { "12 Bars", { 12, 1 } },
    { "16 Bars", { 16, 1 } },
    { "32 Bars", { 32, 1 } },
};

static const int QUARTER_NOTE_INDEX = 15;

class SyncManager
{
public:
    SyncManager (juce::AudioProcessor* ap);

private:
    void updateCurrentPositionInfo();
    float getTimeDivisionSamples (u_long inDivision) const;
    juce::AudioProcessor* mProcessor = nullptr;
    double mSampleRate = 44100;
    juce::AudioPlayHead* mPlayHead = nullptr;
    /** Get playhead BPM. Defaults to 60 if no playhead. **/
    double mBPM = 60;
};
