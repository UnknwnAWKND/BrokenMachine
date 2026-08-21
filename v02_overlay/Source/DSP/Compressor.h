#pragma once
#include <JuceHeader.h>

class ReactiveCompressor
{
public:
    struct Settings
    {
        float macro = 0.30f;
        float punch = 0.18f;
        float crush = 0.06f;
        float thresholdDb = -22.0f;
        float ratio = 3.5f;
        float attackMs = 14.0f;
        float releaseMs = 145.0f;
        float makeupDb = 0.0f;
        float kneeDb = 6.0f;
        float react = 0.34f;
        float bloom = 0.10f;
    };

    void prepare (double sampleRate, int numChannels);
    void reset();
    void process (juce::AudioBuffer<float>& buffer, const Settings& settings, float externalEnvelope);
    float getGainReductionDb() const noexcept { return gainReductionDb.load(); }

private:
    float computeGainDb (float inputDb, float thresholdDb, float ratio, float kneeDb) const noexcept;

    double sr = 44100.0;
    int preparedChannels = 2;
    float detectorEnv = 0.0f;
    float crushEnv = 0.0f;
    std::atomic<float> gainReductionDb { 0.0f };
};
