#pragma once

#include <JuceHeader.h>
#include "Parameters/Parameters.h"
#include "DSP/EnvelopeFollower.h"
#include "DSP/Compressor.h"
#include "DSP/Saturator.h"
#include "DSP/WowFlutter.h"
#include "DSP/MicroDouble.h"
#include "DSP/ToneEngine.h"
#include "DSP/SpeakerModel.h"
#include "DSP/SpatialProcessor.h"
#include "DSP/SafetyLimiter.h"

class BrokenMachineAudioProcessor : public juce::AudioProcessor
{
public:
    BrokenMachineAudioProcessor();
    ~BrokenMachineAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 6.5; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() noexcept { return apvts; }
    const juce::AudioProcessorValueTreeState& getAPVTS() const noexcept { return apvts; }

    float getInputMeter() const noexcept { return inputMeter.load(); }
    float getOutputMeter() const noexcept { return outputMeter.load(); }
    float getGainReductionMeter() const noexcept { return compressor.getGainReductionDb(); }
    float getDriftMeter() const noexcept { return wowFlutter.getCurrentDrift(); }

    const juce::StringArray& getFactoryPresetNames() const noexcept { return presetNames; }
    void loadFactoryPreset (int index);
    void mutate();

private:
    float value (const char* id) const noexcept;
    bool boolValue (const char* id) const noexcept;
    void setParam (const char* id, float plainValue);
    void updateMeter (std::atomic<float>& meter, float newPeak) noexcept;

    juce::AudioProcessorValueTreeState apvts;

    EnvelopeFollower envelopeFollower;
    ReactiveCompressor compressor;
    Saturator saturator;
    WowFlutter wowFlutter;
    MicroDouble microDouble;
    ToneEngine toneEngine;
    SpeakerModel speakerModel;
    SpatialProcessor spatial;
    SafetyLimiter safetyLimiter;

    juce::AudioBuffer<float> dryBuffer;
    int preparedBlockSize = 0;
    int preparedChannels = 2;
    double currentSampleRate = 44100.0;

    std::vector<float> inputHpX1;
    std::vector<float> inputHpY1;

    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> inputGain;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> outputGain;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> wetMix;

    std::atomic<float> inputMeter { 0.0f };
    std::atomic<float> outputMeter { 0.0f };

    juce::StringArray presetNames;
    juce::Random mutationRandom;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BrokenMachineAudioProcessor)
};
