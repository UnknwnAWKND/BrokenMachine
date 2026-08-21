#pragma once
#include <JuceHeader.h>

class Saturator
{
public:
    struct Settings
    {
        float drive = 0.18f;
        float bias = 0.0f;
        float texture = 0.38f;
        float sag = 0.20f;
        float diCharacter = 0.28f;
        float react = 0.34f;
        float broken = 0.18f;
        float portaTrim = 0.44f;
        float portaChannel = 0.56f;
        float portaBassDb = -2.0f;
        float portaTrebleDb = 2.0f;
    };

    void prepare (double sampleRate, int numChannels, int maxBlockSize);
    void reset();
    void process (juce::AudioBuffer<float>& buffer, const Settings& settings, float envelope);

private:
    static float softClip (float x, float grain) noexcept;

    double sr = 44100.0;
    double processingRate = 88200.0;
    std::unique_ptr<juce::dsp::Oversampling<float>> oversampling;

    std::vector<float> sagState;
    std::vector<float> bassState;
    std::vector<float> trebleState;
    std::vector<float> hfState;
};
