#include "ToneEngine.h"

void ToneEngine::Biquad::setPeaking (double sampleRate, float freq, float q, float gainDb) noexcept
{
    const float safeFreq = juce::jlimit (10.0f, static_cast<float> (sampleRate * 0.45), freq);
    const float safeQ = juce::jmax (0.1f, q);
    const float A = std::pow (10.0f, gainDb / 40.0f);
    const float w0 = juce::MathConstants<float>::twoPi * safeFreq / static_cast<float> (sampleRate);
    const float alpha = std::sin (w0) / (2.0f * safeQ);
    const float c = std::cos (w0);

    const float bb0 = 1.0f + alpha * A;
    const float bb1 = -2.0f * c;
    const float bb2 = 1.0f - alpha * A;
    const float aa0 = 1.0f + alpha / A;
    const float aa1 = -2.0f * c;
    const float aa2 = 1.0f - alpha / A;

    b0 = bb0 / aa0; b1 = bb1 / aa0; b2 = bb2 / aa0;
    a1 = aa1 / aa0; a2 = aa2 / aa0;
}

void ToneEngine::prepare (double sampleRate, int numChannels)
{
    sr = juce::jmax (1.0, sampleRate);
    states.resize (static_cast<size_t> (juce::jmax (1, numChannels)));
    reset();
}

void ToneEngine::reset()
{
    for (auto& s : states)
        s.reset();
}

void ToneEngine::process (juce::AudioBuffer<float>& buffer, const Settings& s, float envelope)
{
    const int channels = juce::jmin (buffer.getNumChannels(), static_cast<int> (states.size()));
    const float focus = juce::jlimit (-1.0f, 1.0f, s.brightness);
    const float weird = juce::jlimit (0.0f, 1.0f, s.weird);
    const float react = juce::jlimit (0.0f, 1.0f, s.react);
    const float env = juce::jlimit (0.0f, 1.5f, envelope * 2.5f);

    const float lowCut = juce::jlimit (20.0f, 500.0f, s.lowCutHz * std::pow (2.0f, juce::jmax (0.0f, focus) * 0.48f));
    const float highCut = juce::jlimit (800.0f, static_cast<float> (sr * 0.45), s.highCutHz * std::pow (2.0f, focus * 0.34f));
    const float hpA = std::exp (-juce::MathConstants<float>::twoPi * lowCut / static_cast<float> (sr));
    const float lpA = std::exp (-juce::MathConstants<float>::twoPi * highCut / static_cast<float> (sr));

    const float q = juce::jlimit (0.25f, 10.0f, s.resonance * (1.0f + weird * 0.8f));
    const float bodyFreq = juce::jlimit (120.0f, 3000.0f, s.bodyFreqHz * (1.0f + react * env * 0.09f));
    const float biteFreq = juce::jlimit (700.0f, 10000.0f, s.biteFreqHz * std::pow (2.0f, focus * 0.18f + react * env * 0.04f));
    const float bodyGain = s.body * 8.5f + weird * 1.2f;
    const float biteGain = s.bite * 7.0f + juce::jmax (0.0f, focus) * 2.0f;
    const float formantGain = juce::jlimit (0.0f, 1.0f, s.formant) * (3.8f + weird * 2.8f);

    for (int ch = 0; ch < channels; ++ch)
    {
        auto& st = states[static_cast<size_t> (ch)];
        st.body.setPeaking (sr, bodyFreq, q, bodyGain);
        st.bite.setPeaking (sr, biteFreq, juce::jmax (0.5f, q * 0.75f), biteGain);
        st.f1.setPeaking (sr, 510.0f * (1.0f + weird * 0.08f), 3.2f + q * 0.2f, formantGain);
        st.f2.setPeaking (sr, 1450.0f * (1.0f + react * env * 0.05f), 4.0f + q * 0.3f, formantGain * 0.8f);
        st.f3.setPeaking (sr, 2650.0f * (1.0f - weird * 0.06f), 4.5f + q * 0.25f, formantGain * 0.55f);

        auto* data = buffer.getWritePointer (ch);
        for (int i = 0; i < buffer.getNumSamples(); ++i)
        {
            const float x = data[i];
            const float hp = hpA * (st.hpY1 + x - st.hpX1);
            st.hpX1 = x;
            st.hpY1 = hp;

            float y = st.body.process (hp);
            y = st.bite.process (y);
            y = st.f1.process (y);
            y = st.f2.process (y);
            y = st.f3.process (y);

            st.lp = (1.0f - lpA) * y + lpA * st.lp;
            data[i] = st.lp;
        }
    }
}
