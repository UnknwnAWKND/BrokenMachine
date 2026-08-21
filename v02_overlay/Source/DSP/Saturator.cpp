#include "Saturator.h"

void Saturator::prepare (double sampleRate, int numChannels, int maxBlockSize)
{
    sr = juce::jmax (1.0, sampleRate);
    const auto channels = static_cast<size_t> (juce::jmax (1, numChannels));

    // 2x oversampling meaningfully reduces aliasing from the two nonlinear preamp stages
    // without making live guitar monitoring unnecessarily expensive.
    oversampling = std::make_unique<juce::dsp::Oversampling<float>> (
        channels, 1, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, true, true);
    oversampling->initProcessing (static_cast<size_t> (juce::jmax (1, maxBlockSize)));
    processingRate = sr * oversampling->getOversamplingFactor();

    sagState.assign (channels, 0.0f);
    bassState.assign (channels, 0.0f);
    trebleState.assign (channels, 0.0f);
    hfState.assign (channels, 0.0f);
}

void Saturator::reset()
{
    if (oversampling)
        oversampling->reset();
    std::fill (sagState.begin(), sagState.end(), 0.0f);
    std::fill (bassState.begin(), bassState.end(), 0.0f);
    std::fill (trebleState.begin(), trebleState.end(), 0.0f);
    std::fill (hfState.begin(), hfState.end(), 0.0f);
}

float Saturator::softClip (float x, float grain) noexcept
{
    const float g = juce::jlimit (0.0f, 1.0f, grain);
    const float smooth = std::tanh (x);
    const float console = x / (1.0f + std::abs (x));
    const float harder = std::tanh (x * (1.0f + 1.6f * g));
    return juce::jmap (g, juce::jmap (g, smooth, console), harder);
}

void Saturator::process (juce::AudioBuffer<float>& buffer, const Settings& s, float envelope)
{
    if (! oversampling)
        return;

    juce::dsp::AudioBlock<float> baseBlock (buffer);
    auto upBlock = oversampling->processSamplesUp (baseBlock);

    const int channels = juce::jmin (static_cast<int> (upBlock.getNumChannels()), static_cast<int> (sagState.size()));
    const int samples = static_cast<int> (upBlock.getNumSamples());

    const float drive = juce::jlimit (0.0f, 1.0f, s.drive);
    const float trim = juce::jlimit (0.0f, 1.0f, s.portaTrim);
    const float channel = juce::jlimit (0.0f, 1.0f, s.portaChannel);
    const float texture = juce::jlimit (0.0f, 1.0f, s.texture);
    const float broken = juce::jlimit (0.0f, 1.0f, s.broken);
    const float react = juce::jlimit (0.0f, 1.0f, s.react);
    const float sagAmount = juce::jlimit (0.0f, 1.0f, s.sag);
    const float di = juce::jlimit (0.0f, 1.0f, s.diCharacter);
    const float env = juce::jlimit (0.0f, 1.0f, envelope * 3.0f);

    // Tascam-inspired multi-stage gain structure: trim -> shelving EQ -> channel stage.
    const float trimDb = -2.0f + trim * 30.0f + drive * 10.0f + broken * 7.0f + react * env * 5.0f;
    const float channelDb = -8.0f + channel * 24.0f + drive * 5.0f + broken * 4.0f;
    const float trimGain = juce::Decibels::decibelsToGain (trimDb);
    const float channelGain = juce::Decibels::decibelsToGain (channelDb);

    const float bassGain = juce::Decibels::decibelsToGain (juce::jlimit (-12.0f, 12.0f, s.portaBassDb));
    const float trebleGain = juce::Decibels::decibelsToGain (juce::jlimit (-12.0f, 12.0f, s.portaTrebleDb));
    const float bassA = std::exp (-juce::MathConstants<float>::twoPi * 100.0f / static_cast<float> (processingRate));
    const float trebleA = std::exp (-juce::MathConstants<float>::twoPi * 10000.0f / static_cast<float> (processingRate));
    const float hfCut = juce::jlimit (5000.0f, 19000.0f, 17000.0f - di * 4200.0f - broken * 2600.0f - texture * 1100.0f);
    const float hfA = std::exp (-juce::MathConstants<float>::twoPi * hfCut / static_cast<float> (processingRate));

    const float sagAttack = std::exp (-1.0f / (0.006f * static_cast<float> (processingRate)));
    const float sagRelease = std::exp (-1.0f / (0.160f * static_cast<float> (processingRate)));
    const float asym = juce::jlimit (-0.28f, 0.28f, s.bias * 0.18f + broken * 0.035f);

    for (int ch = 0; ch < channels; ++ch)
    {
        auto* data = upBlock.getChannelPointer (static_cast<size_t> (ch));
        float sag = sagState[static_cast<size_t> (ch)];
        float low = bassState[static_cast<size_t> (ch)];
        float highLp = trebleState[static_cast<size_t> (ch)];
        float hf = hfState[static_cast<size_t> (ch)];

        for (int i = 0; i < samples; ++i)
        {
            const float x = data[i];
            const float level = std::abs (x);
            const float sagCoeff = level > sag ? sagAttack : sagRelease;
            sag = sagCoeff * sag + (1.0f - sagCoeff) * level;

            const float headroomSag = 1.0f / (1.0f + sagAmount * sag * (2.0f + broken * 1.8f));
            float u = x * trimGain * headroomSag + asym;

            // First overloaded op-amp stage: softer at low grain, increasingly square/rubbery at high grain.
            float stage1 = softClip (u, texture * 0.72f + broken * 0.18f) - softClip (asym, texture);

            // 424-style broad shelves around 100 Hz and 10 kHz before the second gain stage.
            low = (1.0f - bassA) * stage1 + bassA * low;
            highLp = (1.0f - trebleA) * stage1 + trebleA * highLp;
            const float high = stage1 - highLp;
            float eq = stage1 + (bassGain - 1.0f) * low + (trebleGain - 1.0f) * high;

            // Second gain stage / channel fader overload. It is intentionally asymmetric and touch-sensitive.
            const float stage2In = eq * channelGain + asym * 0.35f;
            float y = softClip (stage2In, 0.18f + texture * 0.52f + broken * 0.25f);
            y -= softClip (asym * 0.35f, texture);

            // Cheap-recording-device bandwidth loss is level-independent; do not confuse it with cab simulation.
            hf = (1.0f - hfA) * y + hfA * hf;
            y = juce::jmap (di * 0.62f + broken * 0.18f, y, hf);

            // Level compensation keeps breakup changes about character rather than simple loudness wins.
            const float trimComp = juce::Decibels::decibelsToGain (-3.0f - drive * 2.5f - broken * 2.0f - juce::jmax (0.0f, channelDb) * 0.10f);
            data[i] = y * trimComp;
        }

        sagState[static_cast<size_t> (ch)] = sag;
        bassState[static_cast<size_t> (ch)] = low;
        trebleState[static_cast<size_t> (ch)] = highLp;
        hfState[static_cast<size_t> (ch)] = hf;
    }

    oversampling->processSamplesDown (baseBlock);
}
