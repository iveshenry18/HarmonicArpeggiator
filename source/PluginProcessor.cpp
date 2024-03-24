/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "Fraction.h"
#include "PluginEditor.h"

//==============================================================================
PluginProcessor::PluginProcessor() :
#ifndef JucePlugin_PreferredChannelConfigurations
                                     AudioProcessor (BusesProperties()
    #if !JucePlugin_IsMidiEffect
        #if !JucePlugin_IsSynth
                                                         .withInput ("Input", juce::AudioChannelSet::stereo(), true)
        #endif
                                                         .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
    #endif
                                             ),
#endif
                                     syncManager (this)
{
    _constructValueTreeStates();

    mBasisNote = dynamic_cast<juce::AudioParameterInt*> (mValueTreeState->getParameter ("basis_note"));
    jassert (mBasisNote != nullptr);
    mTimeBaseMs = dynamic_cast<juce::AudioParameterInt*> (mValueTreeState->getParameter ("time_base_ms"));
    jassert (mTimeBaseMs != nullptr);
    mTimeBaseSync = dynamic_cast<juce::AudioParameterInt*> (mValueTreeState->getParameter ("time_base_sync"));
    jassert (mTimeBaseSync != nullptr);
    mSyncTime = dynamic_cast<juce::AudioParameterBool*> (mValueTreeState->getParameter ("sync_time"));
    jassert (mSyncTime != nullptr);
    mLearnBasis = dynamic_cast<juce::AudioParameterBool*> (mValueTreeState->getParameter ("learn_basis"));
    jassert (mLearnBasis != nullptr);
}

PluginProcessor::~PluginProcessor() = default;

//==============================================================================
void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    mSampleRate = sampleRate;
    mSamplesPerBlock = samplesPerBlock;
    mTimeInSamples = 0;
    syncManager.updateSampleRate();
}

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    int64_t bufferStartTimeSamples = getPlayHead()->getPosition()->getTimeInSamples().orFallback (mTimeInSamples);
    syncManager.updateCurrentPositionInfo();

    std::vector<juce::MidiMessage> notesToDelete;
    bool allNotesOff = false;

    // Populate heldMidiNotes and note to learn
    std::optional<juce::MidiMessageMetadata> latestMessage;
    for (auto message : midiMessages)
    {
        auto messageData = message.getMessage();
        auto absoluteSamplePosition = message.samplePosition + bufferStartTimeSamples;
        if (messageData.isNoteOn())
        {
            if (mLearnBasis->get() && (!latestMessage.has_value() || message.samplePosition > latestMessage.value().samplePosition))
            {
                latestMessage = message;
            }
            heldMidiNotes.insert ({ { messageData.getChannel(), messageData.getNoteNumber() }, { messageData, absoluteSamplePosition } });
        }
        else if (messageData.isNoteOff())
        {
            notesToDelete.push_back (messageData);
        }
        else if (messageData.isAllNotesOff())
        {
            allNotesOff = true;
        }
    }

    if (mLearnBasis->get() && latestMessage.has_value())
    {
        *mBasisNote = latestMessage.value().getMessage().getNoteNumber();
    }

    /**
     * Compute new retrigs.
     */
    for (auto& heldNote : heldMidiNotes)
    {
        int noteNumber = heldNote.first.second;
        int retrigTime = static_cast<int> (getRetrigTimeSamples (noteNumber));
        int64_t timeSinceLastTrig = heldNote.second.absoluteSamplePosition - bufferStartTimeSamples;

        int writeHead = (static_cast<int> (timeSinceLastTrig) % retrigTime) - retrigTime;

        while (writeHead <= mSamplesPerBlock)
        {
            // Edge case: on/off falls exactly on block boundary
            if (writeHead == mSamplesPerBlock)
            {
                midiMessages.addEvent (juce::MidiMessage::noteOff (heldNote.first.first, heldNote.first.second), writeHead - 1);
            }
            else if (writeHead == 0)
            {
                midiMessages.addEvent (heldNote.second.midiMessage, writeHead);
                heldNote.second.absoluteSamplePosition = writeHead + bufferStartTimeSamples;
            }
            // Normal case
            else if (writeHead > 0)
            {
                midiMessages.addEvent (juce::MidiMessage::noteOff (heldNote.first.first, heldNote.first.second), writeHead - 1);
                midiMessages.addEvent (heldNote.second.midiMessage, writeHead);
                heldNote.second.absoluteSamplePosition = writeHead + bufferStartTimeSamples;
            }
            writeHead += retrigTime;
        }
    }

    // Erase deleted notes
    for (const auto& message : notesToDelete)
    {
        heldMidiNotes.erase ({ message.getChannel(), message.getNoteNumber() });
    }
    if (allNotesOff)
    {
        heldMidiNotes.clear();
    }

    // Update current time (if host doesn't provide time)
    mTimeInSamples += mSamplesPerBlock;
}

//==============================================================================
const juce::String PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool PluginProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool PluginProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double PluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
        // so this should be at least 1, even if you're not really implementing programs.
}

int PluginProcessor::getCurrentProgram()
{
    return 0;
}

void PluginProcessor::setCurrentProgram (int index)
{
}

const juce::String PluginProcessor::getProgramName (int index)
{
    return {};
}

void PluginProcessor::changeProgramName (int index, const juce::String& newName)
{
}

void PluginProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
    #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

            // This checks if the input layout matches the output layout
        #if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
        #endif

    return true;
    #endif
}
#endif

//==============================================================================
bool PluginProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    auto editor = new PluginEditor (*this);
    mValueTreeState->addParameterListener ("sync_time", editor);
    return editor;
}

//==============================================================================
void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = mValueTreeState->copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));

    if (xmlState != nullptr)
        if (xmlState->hasTagName (mValueTreeState->state.getType()))
            mValueTreeState->replaceState (juce::ValueTree::fromXml (*xmlState));
}

float PluginProcessor::computeBasisNoteSamples() const
{
    if (mSyncTime->get())
    {
        return (syncManager.getTimeDivisionSamples (static_cast<u_long> (mTimeBaseSync->get())));
    }
    else
    {
        return static_cast<float> ((static_cast<float> (mTimeBaseMs->get()) / 1000.f) * mSampleRate);
    }
}

double PluginProcessor::getRetrigTimeSamples (int noteNumber) const
{
    int basisNote = mBasisNote->get();
    auto basisNoteSamples = computeBasisNoteSamples();

    int deltaWithBasis = noteNumber - basisNote;
    int octaveDelta = deltaWithBasis / 12;
    int pitchClassDelta = deltaWithBasis % 12;
    if (pitchClassDelta < 0)
    {
        octaveDelta -= 1;
        pitchClassDelta += 12;
    }
    Fraction pitchClassRatio = PITCH_RATIOS.at (pitchClassDelta);

    float pitchRatioFromBasis = pitchClassRatio * (static_cast<const float> (pow (2, octaveDelta)));

    return basisNoteSamples / pitchRatioFromBasis;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}
void PluginProcessor::_constructValueTreeStates()
{
    mValueTreeState.reset (new juce::AudioProcessorValueTreeState (*this, nullptr, juce::Identifier ("APiCppProjParams"),

        {
            std::make_unique<juce::AudioParameterInt> (juce::ParameterID ("basis_note", 1), // parameterID
                "Basis Note", // parameter name
                0, // minimum value
                127, // maximum value
                60, // default value
                juce::AudioParameterIntAttributes().withStringFromValueFunction ([] (auto v1, auto v2) {
                    return getNoteFromMidiNumber (v1);
                })),

            std::make_unique<juce::AudioParameterInt> (juce::ParameterID ("time_base_ms", 1), // parameterID
                "Time Base (ms)", // parameter name
                50, // minimum value
                5000, // maximum value
                500,
                juce::AudioParameterIntAttributes().withStringFromValueFunction ([] (auto v1, auto v2) {
                    return std::to_string (v1) + " ms";
                })), // default value
            std::make_unique<juce::AudioParameterInt> (juce::ParameterID ("time_base_sync", 1), // parameterID
                "Time Base (sync)", // parameter name
                0, // minimum value
                TIME_DIVISIONS.size() - 1, // maximum value
                QUARTER_NOTE_INDEX,
                juce::AudioParameterIntAttributes().withStringFromValueFunction ([] (auto v1, auto v2) {
                    return TIME_DIVISIONS.at (static_cast<unsigned long> (v1)).timeDivisionString;
                })),
            std::make_unique<juce::AudioParameterBool> (juce::ParameterID ("learn_basis", 1), "Learn Basis", false),
            std::make_unique<juce::AudioParameterBool> (juce::ParameterID ("sync_time", 1), "Sync Time", false),
        }));
}
