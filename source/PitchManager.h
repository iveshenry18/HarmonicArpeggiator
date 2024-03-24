//
// Created by Henry Ives on 3/23/24.
//

#pragma once

#include "Fraction.h"
#include <juce_audio_processors/juce_audio_processors.h>

/**
 * Semitones -> Pitch Ratio
 * Taken from https://en.wikipedia.org/wiki/Five-limit_tuning
 */
static const std::unordered_map<int, Fraction> PITCH_RATIOS = {
    { 0, { 1, 1 } }, // Unison
    { 1, { 16, 15 } }, // m2
    { 2, { 9, 8 } }, // M2
    { 3, { 6, 5 } }, // m3
    { 4, { 5, 4 } }, // M3
    { 5, { 4, 3 } }, // P4
    { 6, { 45, 32 } }, // TT
    { 7, { 3, 2 } }, // P5
    { 8, { 8, 5 } }, // m6
    { 9, { 5, 3 } }, // M6
    { 10, { 9, 5 } }, // m7
    { 11, { 15, 8 } }, // M7
};

static const juce::String NOTE_NAMES[] { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

static juce::String getNoteFromMidiNumber (int midiNote)
{
    return NOTE_NAMES[midiNote % 12] + std::to_string ((midiNote / 12) - 1);
}

class PitchManager
{
};
