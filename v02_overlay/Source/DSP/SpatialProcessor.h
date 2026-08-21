#pragma once
#include <JuceHeader.h>

class SpatialProcessor
{
public:
    struct Settings
    {
        float space = 0.10f;
        float predelayMs = 22.0f;
        float decay = 0.34f;
        float width = 0.52f;
        float darkness = 0.46f;
        float diffusion = 0.42f;
        float bloom = 0.10f;
        float reverbDrive = 0.24f;
        float reverbGate = 0.22f;
        float reverbDuck = 0.18f;
        float pressure = 0.34f;
        float breakup = 0.18f;
    };

    void prepare (double sampleRate, int numChannels, int maxBlockSize);
    void reset();
    void process (juce::AudioBuffer<float>& buffer, const Settings& settings);

private:
    float readDelay (const std::vector<float>& line, int writeIndex, float delaySamples) const noexcept;

    double sr = 44100.0;
    int delaySize = 0;
    int maxSamples = 0;
    juce::AudioBuffer<float> wetBuffer;
    std::vector<std::vector<float>> predelayLines;
    std::vector<int> writePositions;
    std::vector<float> hpX1;
    std::vector<float> hpY1;
    std::vector<float> lpState;
    juce::Reverb reverb;
    float gateDetector = 0.0f;
    float gateGain = 1.0f;
};
