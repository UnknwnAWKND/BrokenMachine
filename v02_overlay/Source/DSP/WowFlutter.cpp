#include "WowFlutter.h"

void WowFlutter::prepare (double sampleRate, int, int numChannels)
{
    sr = juce::jmax (1.0, sampleRate);
    delaySize = static_cast<int> (std::ceil (sr * 0.080)) + 8;
    const int channels = juce::jmax (1, numChannels);
    delayLines.assign (static_cast<size_t> (channels), std::vector<float> (static_cast<size_t> (delaySize), 0.0f));
    writePositions.assign (static_cast<size_t> (channels), 0);
    rngStates.resize (static_cast<size_t> (channels));
    slowNoise.assign (static_cast<size_t> (channels), 0.0f);
    fastNoise.assign (static_cast<size_t> (channels), 0.0f);
    driftNoise.assign (static_cast<size_t> (channels), 0.0f);
    for (int ch = 0; ch < channels; ++ch)
        rngStates[static_cast<size_t> (ch)] = 0x9E3779B9u ^ static_cast<uint32_t> (0xA341316Cu * (ch + 1));
    reset();
}

void WowFlutter::reset()
{
    for (auto& line : delayLines)
        std::fill (line.begin(), line.end(), 0.0f);
    std::fill (writePositions.begin(), writePositions.end(), 0);
    std::fill (slowNoise.begin(), slowNoise.end(), 0.0f);
    std::fill (fastNoise.begin(), fastNoise.end(), 0.0f);
    std::fill (driftNoise.begin(), driftNoise.end(), 0.0f);
    wowPhase = 0.0f;
    flutterPhase = 0.0f;
    displayedDrift.store (0.0f);
}

float WowFlutter::randomBipolar (int channel) noexcept
{
    auto& x = rngStates[static_cast<size_t> (channel)];
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return static_cast<float> (x & 0x00FFFFFFu) / 8388607.5f - 1.0f;
}

float WowFlutter::readDelay (const std::vector<float>& line, int writeIndex, float delaySamples) const noexcept
{
    float read = static_cast<float> (writeIndex) - delaySamples;
    while (read < 0.0f)
        read += static_cast<float> (delaySize);
    while (read >= static_cast<float> (delaySize))
        read -= static_cast<float> (delaySize);

    const int i0 = static_cast<int> (read);
    const int i1 = (i0 + 1) % delaySize;
    const float frac = read - static_cast<float> (i0);
    return line[static_cast<size_t> (i0)] + frac * (line[static_cast<size_t> (i1)] - line[static_cast<size_t> (i0)]);
}

void WowFlutter::process (juce::AudioBuffer<float>& buffer, const Settings& s, float envelope)
{
    const int channels = juce::jmin (buffer.getNumChannels(), static_cast<int> (delayLines.size()));
    const float wowAmt = juce::jlimit (0.0f, 1.0f, s.wow);
    const float flutterAmt = juce::jlimit (0.0f, 1.0f, s.flutter);
    const float driftAmt = juce::jlimit (0.0f, 1.0f, s.drift);
    const float damageAmt = juce::jlimit (0.0f, 1.0f, s.damage);
    const float weird = juce::jlimit (0.0f, 1.0f, s.weird);
    const float react = juce::jlimit (0.0f, 1.0f, s.react);
    const float baseDelayMs = 6.2f + 1.6f * weird;
    const float effectMix = juce::jlimit (0.0f, 1.0f,
                                          wowAmt * 0.52f + flutterAmt * 0.42f
                                        + driftAmt * 0.38f + damageAmt * 0.28f
                                        + weird * 0.10f);

    if (effectMix <= 0.00001f)
        return;

    const float wowInc = juce::MathConstants<float>::twoPi * (0.18f + 0.11f * weird) / static_cast<float> (sr);
    const float flutterInc = juce::MathConstants<float>::twoPi * (5.2f + 2.7f * weird) / static_cast<float> (sr);

    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        wowPhase += wowInc;
        flutterPhase += flutterInc;
        if (wowPhase > juce::MathConstants<float>::twoPi) wowPhase -= juce::MathConstants<float>::twoPi;
        if (flutterPhase > juce::MathConstants<float>::twoPi) flutterPhase -= juce::MathConstants<float>::twoPi;

        float driftForDisplay = 0.0f;

        for (int ch = 0; ch < channels; ++ch)
        {
            const auto idx = static_cast<size_t> (ch);
            const float rnd = randomBipolar (ch);
            slowNoise[idx] += 0.00045f * (rnd - slowNoise[idx]);
            fastNoise[idx] += 0.032f * (rnd - fastNoise[idx]);
            driftNoise[idx] += 0.000045f * (rnd - driftNoise[idx]);

            const float chOffset = (ch == 0 ? -0.13f : 0.17f);
            const float organicWow = 0.55f * std::sin (wowPhase + chOffset) + 0.45f * slowNoise[idx];
            const float organicFlutter = 0.55f * std::sin (flutterPhase * (1.0f + 0.03f * ch) + chOffset) + 0.45f * fastNoise[idx];
            const float drift = driftNoise[idx] * 3.2f;
            driftForDisplay += drift;

            const float reactBoost = 1.0f + react * juce::jlimit (0.0f, 1.0f, envelope * 2.5f) * 0.65f;
            float modMs = (organicWow * wowAmt * 1.25f
                         + organicFlutter * flutterAmt * 0.18f
                         + drift * driftAmt * 1.9f) * reactBoost;
            modMs += weird * 0.35f * slowNoise[idx];

            const float delayMs = juce::jlimit (1.5f, 26.0f, baseDelayMs + modMs);
            const float delaySamples = delayMs * 0.001f * static_cast<float> (sr);

            auto& line = delayLines[idx];
            int& w = writePositions[idx];
            auto* data = buffer.getWritePointer (ch);
            const float dry = data[i];
            line[static_cast<size_t> (w)] = dry;
            float y = readDelay (line, w, delaySamples);

            const float dropoutNoise = randomBipolar (ch);
            const float dropout = 1.0f - damageAmt * juce::jmax (0.0f, dropoutNoise - (0.75f - damageAmt * 0.25f)) * 1.8f;
            const float ampFlutter = 1.0f + damageAmt * 0.06f * fastNoise[idx];
            y *= juce::jlimit (0.25f, 1.15f, dropout * ampFlutter);
            data[i] = dry + (y - dry) * effectMix;

            w = (w + 1) % delaySize;
        }

        if (channels > 0)
            displayedDrift.store (driftForDisplay / static_cast<float> (channels));
    }
}
