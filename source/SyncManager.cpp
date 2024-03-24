//
// Created by Henry Ives on 3/15/24.
//

#include "SyncManager.h"

SyncManager::SyncManager (juce::AudioProcessor* ap) : mProcessor (ap)
{
}

void SyncManager::updateSampleRate()
{
    mSampleRate = mProcessor->getSampleRate();
}

void SyncManager::updateCurrentPositionInfo()
{
    mPlayHead = mProcessor->getPlayHead();
    if (mPlayHead != nullptr)
    {
        auto positionInfo = mPlayHead->getPosition();
        if (positionInfo.hasValue())
            mBPM = juce::jmax ((positionInfo->getBpm().orFallback (60)), 1.);
        else
            DBG ("No positionInfo value");
    }
}

float SyncManager::getTimeDivisionSamples (u_long inDivisionIndex) const
{
    float samplesPerBeat = (static_cast<float> ((60.f / mBPM) * mSampleRate));
    return TIME_DIVISIONS.at (inDivisionIndex).barMultiplier * (samplesPerBeat * 4);
}
