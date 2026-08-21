#include "Compressor.h"

void ReactiveCompressor::prepare (double sampleRate, int numChannels)
{
    sr = juce::jmax (1.0, sampleRate);
    preparedChannels = juce::jmax (1, numChannels);
    reset();
}

void ReactiveCompressor::reset()
{
    detectorEnv = 0.0f;
    crushEnv = 0.0f;
    gainReductionDb.store (0.0f);
}

float ReactiveCompressor::computeGainDb (float inputDb, float thresholdDb, float ratio, float kneeDb) const noexcept
{
    const float x = inputDb - thresholdDb;
    if (kneeDb <= 0.001f)
        return x > 0.0f ? (thresholdDb + x / ratio) - inputDb : 0.0f;

    const float halfKnee = kneeDb * 0.5f;
    if (x < -halfKnee)
        return 0.0f;
    if (x > halfKnee)
        return (thresholdDb + x / ratio) - inputDb;

    const float y = x + halfKnee;
    return (1.0f / ratio - 1.0f) * y * y / (2.0f * kneeDb);
}

void ReactiveCompressor::process (juce::AudioBuffer<float>& buffer, const Settings& s, float externalEnvelope)
{
    const int channels = juce::jmin (buffer.getNumChannels(), preparedChannels);
    const int samples = buffer.getNumSamples();
    if (channels <= 0 || samples <= 0)
        return;

    const float macro = juce::jlimit (0.0f, 1.0f, s.macro);
    const float punch = juce::jlimit (-1.0f, 1.0f, s.punch);
    const float bloom = juce::jlimit (-1.0f, 1.0f, s.bloom);
    const float pressure = juce::jlimit (0.0f, 1.0f, s.react);
    const float ext = juce::jlimit (0.0f, 1.0f, externalEnvelope * 3.0f);

    // The macro intentionally compresses less brutally than v0.1. Hard picking is allowed
    // to poke through so the downstream Portastudio stage can be what actually explodes.
    const float threshold = s.thresholdDb - macro * 12.0f - pressure * ext * 2.0f;
    const float ratio = juce::jlimit (1.0f, 18.0f, s.ratio + macro * 5.0f);
    const float attackMs = juce::jlimit (0.2f, 160.0f,
                                         s.attackMs * (1.0f + 0.85f * juce::jmax (0.0f, punch))
                                                    * (1.0f - 0.35f * juce::jmax (0.0f, -punch)));
    const float releaseMs = juce::jlimit (12.0f, 1500.0f,
                                          s.releaseMs * (1.0f + 0.70f * juce::jmax (0.0f, bloom)));
    const float makeupDb = s.makeupDb + macro * 2.8f;

    const float aA = std::exp (-1.0f / (0.001f * attackMs * static_cast<float> (sr)));
    const float aR = std::exp (-1.0f / (0.001f * releaseMs * static_cast<float> (sr)));
    const float cA = std::exp (-1.0f / (0.001f * 1.3f * static_cast<float> (sr)));
    const float cR = std::exp (-1.0f / (0.001f * 105.0f * static_cast<float> (sr)));
    const float crushMix = juce::jlimit (0.0f, 0.78f, s.crush);

    float maxReduction = 0.0f;
    float env = detectorEnv;
    float cenv = crushEnv;

    for (int i = 0; i < samples; ++i)
    {
        float level = 0.0f;
        for (int ch = 0; ch < channels; ++ch)
            level = juce::jmax (level, std::abs (buffer.getSample (ch, i)));
        level += 1.0e-9f;

        const float coeff = level > env ? aA : aR;
        env = coeff * env + (1.0f - coeff) * level;
        const float inDb = juce::Decibels::gainToDecibels (env, -120.0f);
        const float gainDb = computeGainDb (inDb, threshold, ratio, s.kneeDb);
        maxReduction = juce::jmax (maxReduction, -gainDb);
        const float mainGain = juce::Decibels::decibelsToGain (gainDb + makeupDb);

        const float ccoeff = level > cenv ? cA : cR;
        cenv = ccoeff * cenv + (1.0f - ccoeff) * level;
        const float cinDb = juce::Decibels::gainToDecibels (cenv, -120.0f);
        const float crushGainDb = computeGainDb (cinDb, -34.0f - macro * 4.0f, 12.0f, 10.0f);
        const float parallelGain = juce::Decibels::decibelsToGain (crushGainDb + 8.5f);

        for (int ch = 0; ch < channels; ++ch)
        {
            const float x = buffer.getSample (ch, i);
            const float main = x * mainGain;
            const float crushed = x * parallelGain;
            buffer.setSample (ch, i, main + (crushed - main) * crushMix);
        }
    }

    detectorEnv = env;
    crushEnv = cenv;
    gainReductionDb.store (maxReduction);
}
