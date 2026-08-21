#include "Parameters.h"

namespace
{
    juce::NormalisableRange<float> skewedRange (float min, float max, float centre)
    {
        juce::NormalisableRange<float> r { min, max };
        r.setSkewForCentre (centre);
        return r;
    }

    std::unique_ptr<juce::AudioParameterFloat> fparam (const char* id,
                                                       const char* name,
                                                       juce::NormalisableRange<float> range,
                                                       float def,
                                                       const juce::String& label = {})
    {
        juce::AudioParameterFloatAttributes attrs;
        if (label.isNotEmpty())
            attrs = attrs.withLabel (label);
        return std::make_unique<juce::AudioParameterFloat> (juce::ParameterID { id, 1 }, name, range, def, attrs);
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout Params::createParameterLayout()
{
    using P = std::unique_ptr<juce::RangedAudioParameter>;
    std::vector<P> p;
    p.reserve (49);

    p.push_back (fparam (ID::inputDb,     "Input",       { -24.0f, 24.0f, 0.01f }, 0.0f, "dB"));
    p.push_back (fparam (ID::diCharacter, "DI Character",{ 0.0f, 1.0f, 0.0001f }, 0.20f));

    p.push_back (fparam (ID::compress,  "Compress", { 0.0f, 1.0f, 0.0001f }, 0.35f));
    p.push_back (fparam (ID::punch,     "Punch",    { -1.0f, 1.0f, 0.0001f }, 0.10f));
    p.push_back (fparam (ID::crush,     "Crush",    { 0.0f, 1.0f, 0.0001f }, 0.10f));
    p.push_back (fparam (ID::threshold, "Threshold",{ -60.0f, 0.0f, 0.01f }, -24.0f, "dB"));
    p.push_back (fparam (ID::ratio,     "Ratio",    { 1.0f, 20.0f, 0.01f }, 4.0f, ":1"));
    p.push_back (fparam (ID::attackMs,  "Attack",   skewedRange (0.1f, 100.0f, 10.0f), 12.0f, "ms"));
    p.push_back (fparam (ID::releaseMs, "Release",  skewedRange (10.0f, 1000.0f, 120.0f), 120.0f, "ms"));
    p.push_back (fparam (ID::makeupDb,  "Makeup",   { -12.0f, 24.0f, 0.01f }, 0.0f, "dB"));
    p.push_back (fparam (ID::kneeDb,    "Knee",     { 0.0f, 24.0f, 0.01f }, 6.0f, "dB"));

    p.push_back (fparam (ID::drive,   "Drive",   { 0.0f, 1.0f, 0.0001f }, 0.20f));
    p.push_back (fparam (ID::bias,    "Bias",    { -1.0f, 1.0f, 0.0001f }, 0.0f));
    p.push_back (fparam (ID::texture, "Texture", { 0.0f, 1.0f, 0.0001f }, 0.35f));
    p.push_back (fparam (ID::sag,     "Sag",     { 0.0f, 1.0f, 0.0001f }, 0.20f));

    p.push_back (fparam (ID::wow,     "Wow",     { 0.0f, 1.0f, 0.0001f }, 0.12f));
    p.push_back (fparam (ID::flutter, "Flutter", { 0.0f, 1.0f, 0.0001f }, 0.08f));
    p.push_back (fparam (ID::drift,   "Drift",   { 0.0f, 1.0f, 0.0001f }, 0.10f));
    p.push_back (fparam (ID::damage,  "Damage",  { 0.0f, 1.0f, 0.0001f }, 0.02f));

    p.push_back (fparam (ID::doubleMix,   "Double", { 0.0f, 1.0f, 0.0001f }, 0.10f));
    p.push_back (fparam (ID::detuneCents, "Detune", { -20.0f, 20.0f, 0.01f }, 5.0f, "ct"));
    p.push_back (fparam (ID::offsetMs,    "Offset", skewedRange (0.0f, 35.0f, 8.0f), 9.0f, "ms"));
    p.push_back (fparam (ID::spread,      "Spread", { 0.0f, 1.0f, 0.0001f }, 0.55f));
    p.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { ID::driftLink, 1 }, "Drift Link", true));

    p.push_back (fparam (ID::lowCutHz,   "Low Cut",  skewedRange (20.0f, 500.0f, 80.0f), 45.0f, "Hz"));
    p.push_back (fparam (ID::highCutHz,  "High Cut", skewedRange (1000.0f, 20000.0f, 7000.0f), 14500.0f, "Hz"));
    p.push_back (fparam (ID::body,       "Body",      { 0.0f, 1.0f, 0.0001f }, 0.20f));
    p.push_back (fparam (ID::bodyFreqHz, "Body Freq",skewedRange (150.0f, 2500.0f, 650.0f), 720.0f, "Hz"));
    p.push_back (fparam (ID::bite,       "Bite",      { 0.0f, 1.0f, 0.0001f }, 0.12f));
    p.push_back (fparam (ID::biteFreqHz, "Bite Freq",skewedRange (1000.0f, 8000.0f, 2600.0f), 2800.0f, "Hz"));
    p.push_back (fparam (ID::resonance,  "Resonance", { 0.2f, 8.0f, 0.001f }, 1.1f));
    p.push_back (fparam (ID::formant,    "Formant",   { 0.0f, 1.0f, 0.0001f }, 0.0f));

    p.push_back (fparam (ID::speaker, "Speaker", { 0.0f, 1.0f, 0.0001f }, 0.18f));
    p.push_back (fparam (ID::size,    "Size",    { 0.0f, 1.0f, 0.0001f }, 0.52f));

    p.push_back (fparam (ID::space,    "Space",    { 0.0f, 1.0f, 0.0001f }, 0.08f));
    p.push_back (fparam (ID::slapMs,   "Slap",     skewedRange (0.0f, 150.0f, 35.0f), 36.0f, "ms"));
    p.push_back (fparam (ID::feedback, "Feedback", { 0.0f, 0.85f, 0.0001f }, 0.18f));
    p.push_back (fparam (ID::width,    "Width",    { 0.0f, 1.0f, 0.0001f }, 0.55f));
    p.push_back (fparam (ID::darkness, "Darkness", { 0.0f, 1.0f, 0.0001f }, 0.35f));
    p.push_back (fparam (ID::smear,    "Smear",    { 0.0f, 1.0f, 0.0001f }, 0.12f));

    p.push_back (fparam (ID::react,      "React",      { 0.0f, 1.0f, 0.0001f }, 0.30f));
    p.push_back (fparam (ID::broken,     "Broken",     { 0.0f, 1.0f, 0.0001f }, 0.20f));
    p.push_back (fparam (ID::brightness, "Dark / Bright", { -1.0f, 1.0f, 0.0001f }, 0.0f));
    p.push_back (fparam (ID::bloom,      "Tight / Bloom", { -1.0f, 1.0f, 0.0001f }, 0.0f));
    p.push_back (fparam (ID::weird,      "Weird",      { 0.0f, 1.0f, 0.0001f }, 0.10f));

    p.push_back (fparam (ID::mix,      "Mix",    { 0.0f, 1.0f, 0.0001f }, 1.0f));
    p.push_back (fparam (ID::outputDb, "Output", { -24.0f, 12.0f, 0.01f }, 0.0f, "dB"));
    p.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { ID::autoGain, 1 }, "Auto Gain", true));
    p.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { ID::safe, 1 }, "Safe", true));

    return { p.begin(), p.end() };
}
