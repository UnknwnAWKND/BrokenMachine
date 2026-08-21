#include "SpatialProcessor.h"

void SpatialProcessor::prepare (double sampleRate, int numChannels, int maxBlockSize)
{
    sr = juce::jmax (1.0, sampleRate);
    const int channels = juce::jmax (1, numChannels);
    maxSamples = juce::jmax (16384, maxBlockSize);
    wetBuffer.setSize (channels, maxSamples, false, true, true);

    delaySize = static_cast<int> (std::ceil (sr * 0.180)) + 8;
    predelayLines.assign (static_cast<size_t> (channels), std::vector<float> (static_cast<size_t> (delaySize), 0.0f));
    writePositions.assign (static_cast<size_t> (channels), 0);
    hpX1.assign (static_cast<size_t> (channels), 0.0f);
    hpY1.assign (static_cast<size_t> (channels), 0.0f);
    lpState.assign (static_cast<size_t> (channels), 0.0f);

    reverb.setSampleRate (sr);
    reset();
}

void SpatialProcessor::reset()
{
    wetBuffer.clear();
    for (auto& line : predelayLines)
        std::fill (line.begin(), line.end(), 0.0f);
    std::fill (writePositions.begin(), writePositions.end(), 0);
    std::fill (hpX1.begin(), hpX1.end(), 0.0f);
    std::fill (hpY1.begin(), hpY1.end(), 0.0f);
    std::fill (lpState.begin(), lpState.end(), 0.0f);
    gateDetector = 0.0f;
    gateGain = 1.0f;
    reverb.reset();
}

float SpatialProcessor::readDelay (const std::vector<float>& line, int writeIndex, float delaySamples) const noexcept
{
    float read = static_cast<float> (writeIndex) - delaySamples;
    while (read < 0.0f) read += static_cast<float> (delaySize);
    while (read >= static_cast<float> (delaySize)) read -= static_cast<float> (delaySize);
    const int i0 = static_cast<int> (read);
    const int i1 = (i0 + 1) % delaySize;
    const float frac = read - static_cast<float> (i0);
    return line[static_cast<size_t> (i0)] + frac * (line[static_cast<size_t> (i1)] - line[static_cast<size_t> (i0)]);
}

void SpatialProcessor::process (juce::AudioBuffer<float>& buffer, const Settings& s)
{
    const int channels = juce::jmin (buffer.getNumChannels(), wetBuffer.getNumChannels());
    const int samples = buffer.getNumSamples();
    if (channels <= 0 || samples <= 0 || samples > maxSamples)
        return;

    const float bloom = juce::jlimit (-1.0f, 1.0f, s.bloom);
    const float positiveBloom = juce::jmax (0.0f, bloom);
    const float pressure = juce::jlimit (0.0f, 1.0f, s.pressure);
    const float breakup = juce::jlimit (0.0f, 1.0f, s.breakup);
    const float send = juce::jlimit (0.0f, 1.0f, s.space + positiveBloom * 0.12f);
    if (send <= 0.00001f)
        return;

    const float predelayMs = juce::jlimit (0.0f, 150.0f, s.predelayMs);
    const float predelaySamples = predelayMs * 0.001f * static_cast<float> (sr);
    const float width = juce::jlimit (0.0f, 1.0f, s.width);
    const float darkness = juce::jlimit (0.0f, 1.0f, s.darkness);
    const float diffusion = juce::jlimit (0.0f, 1.0f, s.diffusion);

    // Rainger-inspired behavior: keep the direct note intact, create a separate reverb send,
    // filter it, then distort/gate the wet return before it hits the downstream preamp.
    const float wetHpHz = 125.0f + darkness * 95.0f;
    const float wetLpHz = juce::jmap (darkness, 12500.0f, 3100.0f);
    const float hpA = std::exp (-juce::MathConstants<float>::twoPi * wetHpHz / static_cast<float> (sr));
    const float lpA = std::exp (-juce::MathConstants<float>::twoPi * wetLpHz / static_cast<float> (sr));

    for (int ch = 0; ch < channels; ++ch)
    {
        auto* wet = wetBuffer.getWritePointer (ch);
        auto& line = predelayLines[static_cast<size_t> (ch)];
        int& w = writePositions[static_cast<size_t> (ch)];
        float x1 = hpX1[static_cast<size_t> (ch)];
        float y1 = hpY1[static_cast<size_t> (ch)];

        for (int i = 0; i < samples; ++i)
        {
            const float x = buffer.getSample (ch, i);
            line[static_cast<size_t> (w)] = x;
            const float pre = predelayMs <= 0.01f ? x : readDelay (line, w, juce::jmax (1.0f, predelaySamples));
            w = (w + 1) % delaySize;

            const float hp = hpA * (y1 + pre - x1);
            x1 = pre;
            y1 = hp;
            wet[i] = hp;
        }

        hpX1[static_cast<size_t> (ch)] = x1;
        hpY1[static_cast<size_t> (ch)] = y1;
    }

    juce::Reverb::Parameters rp;
    rp.roomSize = juce::jlimit (0.05f, 0.96f, 0.20f + juce::jlimit (0.0f, 1.0f, s.decay) * 0.66f + positiveBloom * 0.08f);
    rp.damping = juce::jlimit (0.0f, 1.0f, 0.18f + darkness * 0.68f);
    rp.wetLevel = 1.0f;
    rp.dryLevel = 0.0f;
    rp.width = juce::jlimit (0.0f, 1.0f, 0.20f + width * 0.80f);
    rp.freezeMode = 0.0f;
    reverb.setParameters (rp);

    if (channels >= 2)
        reverb.processStereo (wetBuffer.getWritePointer (0), wetBuffer.getWritePointer (1), samples);
    else
        reverb.processMono (wetBuffer.getWritePointer (0), samples);

    const float wetDrive = juce::jlimit (0.0f, 1.0f, s.reverbDrive + breakup * 0.16f);
    const float driveGain = juce::Decibels::decibelsToGain (wetDrive * 24.0f + breakup * 5.0f);
    const float gateAmount = juce::jlimit (0.0f, 1.0f, s.reverbGate);
    const float duckAmount = juce::jlimit (0.0f, 1.0f, s.reverbDuck);
    const float gateThreshold = juce::jmap (gateAmount * gateAmount, 0.0020f, 0.105f);
    const float detectorAttack = std::exp (-1.0f / (0.0015f * static_cast<float> (sr)));
    const float detectorRelease = std::exp (-1.0f / (0.075f * static_cast<float> (sr)));
    const float gateAttack = std::exp (-1.0f / (0.0012f * static_cast<float> (sr)));
    const float gateReleaseMs = juce::jmap (gateAmount, 160.0f, 28.0f);
    const float gateRelease = std::exp (-1.0f / (0.001f * gateReleaseMs * static_cast<float> (sr)));

    for (int i = 0; i < samples; ++i)
    {
        float detector = 0.0f;
        for (int ch = 0; ch < channels; ++ch)
            detector = juce::jmax (detector, std::abs (buffer.getSample (ch, i)));

        const float dCoeff = detector > gateDetector ? detectorAttack : detectorRelease;
        gateDetector = dCoeff * gateDetector + (1.0f - dCoeff) * detector;

        float targetGate = 1.0f;
        if (gateAmount > 0.005f)
        {
            const float lo = gateThreshold * 0.72f;
            const float hi = gateThreshold * 1.35f;
            targetGate = juce::jlimit (0.0f, 1.0f, (gateDetector - lo) / juce::jmax (1.0e-5f, hi - lo));
            targetGate = targetGate * targetGate * (3.0f - 2.0f * targetGate);
        }

        const float gCoeff = targetGate > gateGain ? gateAttack : gateRelease;
        gateGain = gCoeff * gateGain + (1.0f - gCoeff) * targetGate;

        const float transient = juce::jlimit (0.0f, 1.0f, gateDetector * (4.0f + pressure * 3.0f));
        const float duck = 1.0f - duckAmount * transient * 0.74f;
        const float bloomLift = 1.0f + positiveBloom * (1.0f - transient) * 0.34f;

        for (int ch = 0; ch < channels; ++ch)
        {
            auto* wet = wetBuffer.getWritePointer (ch);
            float w = wet[i];
            float lp = lpState[static_cast<size_t> (ch)];
            lp = (1.0f - lpA) * w + lpA * lp;
            lpState[static_cast<size_t> (ch)] = lp;

            // Diffusion blends the raw digital tail toward the damped return before clipping.
            w = juce::jmap (diffusion, w, lp);
            const float clipped = std::tanh (w * driveGain) / juce::jmax (1.0f, 0.72f + wetDrive * 0.65f);
            const float wetOut = juce::jmap (wetDrive, w, clipped);

            const float dry = buffer.getSample (ch, i);
            buffer.setSample (ch, i, dry + wetOut * send * gateGain * duck * bloomLift * 0.82f);
        }
    }
}
