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
    p.reserve (56);

    p.push_back (fparam (ID::inputDb,     "Input",        { -24.0f, 24.0f, 0.01f }, 0.0f, "dB"));
    p.push_back (fparam (ID::diCharacter, "DI Character", { 0.0f, 1.0f, 0.0001f }, 0.28f));

    p.push_back (fparam (ID::compress,  "Compress",  { 0.0f, 1.0f, 0.0001f }, 0.30f));
    p.push_back (fparam (ID::punch,     "Punch",     { -1.0f, 1.0f, 0.0001f }, 0.18f));
    p.push_back (fparam (ID::crush,     "Crush",     { 0.0f, 1.0f, 0.0001f }, 0.06f));
    p.push_back (fparam (ID::threshold, "Threshold", { -60.0f, 0.0f, 0.01f }, -22.0f, "dB"));
    p.push_back (fparam (ID::ratio,     "Ratio",     { 1.0f, 20.0f, 0.01f }, 3.5f, ":1"));
    p.push_back (fparam (ID::attackMs,  "Attack",    skewedRange (0.2f, 120.0f, 12.0f), 14.0f, "ms"));
    p.push_back (fparam (ID::releaseMs, "Release",   skewedRange (15.0f, 1200.0f, 140.0f), 145.0f, "ms"));
    p.push_back (fparam (ID::makeupDb,  "Makeup",    { -12.0f, 12.0f, 0.01f }, 0.0f, "dB"));
    p.push_back (fparam (ID::kneeDb,    "Knee",      { 0.0f, 18.0f, 0.01f }, 6.0f, "dB"));

    p.push_back (fparam (ID::drive,         "Drive",       { 0.0f, 1.0f, 0.0001f }, 0.18f));
    p.push_back (fparam (ID::bias,          "Bias",        { -1.0f, 1.0f, 0.0001f }, 0.0f));
    p.push_back (fparam (ID::texture,       "Preamp Grain",{ 0.0f, 1.0f, 0.0001f }, 0.38f));
    p.push_back (fparam (ID::sag,           "Sag",         { 0.0f, 1.0f, 0.0001f }, 0.20f));
    p.push_back (fparam (ID::portaTrim,     "424 Trim",    { 0.0f, 1.0f, 0.0001f }, 0.44f));
    p.push_back (fparam (ID::portaChannel,  "424 Channel", { 0.0f, 1.0f, 0.0001f }, 0.56f));
    p.push_back (fparam (ID::portaBassDb,   "424 Bass",    { -12.0f, 12.0f, 0.01f }, -2.0f, "dB"));
    p.push_back (fparam (ID::portaTrebleDb, "424 Treble",  { -12.0f, 12.0f, 0.01f }, 2.0f, "dB"));

    p.push_back (fparam (ID::wow,     "Wow",     { 0.0f, 1.0f, 0.0001f }, 0.035f));
    p.push_back (fparam (ID::flutter, "Flutter", { 0.0f, 1.0f, 0.0001f }, 0.025f));
    p.push_back (fparam (ID::drift,   "Drift",   { 0.0f, 1.0f, 0.0001f }, 0.04f));
    p.push_back (fparam (ID::damage,  "Damage",  { 0.0f, 1.0f, 0.0001f }, 0.0f));

    p.push_back (fparam (ID::doubleMix,   "Double", { 0.0f, 1.0f, 0.0001f }, 0.05f));
    p.push_back (fparam (ID::detuneCents, "Detune", { -20.0f, 20.0f, 0.01f }, 4.0f, "ct"));
    p.push_back (fparam (ID::offsetMs,    "Offset", skewedRange (0.0f, 35.0f, 8.0f), 8.0f, "ms"));
    p.push_back (fparam (ID::spread,      "Spread", { 0.0f, 1.0f, 0.0001f }, 0.45f));
    p.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { ID::driftLink, 1 }, "Drift Link", true));

    p.push_back (fparam (ID::lowCutHz,   "Low Cut",   skewedRange (20.0f, 500.0f, 90.0f), 68.0f, "Hz"));
    p.push_back (fparam (ID::highCutHz,  "High Cut",  skewedRange (1000.0f, 20000.0f, 7000.0f), 10800.0f, "Hz"));
    p.push_back (fparam (ID::body,       "Body",      { 0.0f, 1.0f, 0.0001f }, 0.16f));
    p.push_back (fparam (ID::bodyFreqHz, "Body Freq", skewedRange (150.0f, 2500.0f, 650.0f), 680.0f, "Hz"));
    p.push_back (fparam (ID::bite,       "Bite",      { 0.0f, 1.0f, 0.0001f }, 0.08f));
    p.push_back (fparam (ID::biteFreqHz, "Bite Freq", skewedRange (1000.0f, 8000.0f, 2400.0f), 2350.0f, "Hz"));
    p.push_back (fparam (ID::resonance,  "Resonance", { 0.2f, 8.0f, 0.001f }, 0.9f));
    p.push_back (fparam (ID::formant,    "Modeler Color", { 0.0f, 1.0f, 0.0001f }, 0.0f));

    p.push_back (fparam (ID::speaker, "Speaker Color", { 0.0f, 1.0f, 0.0001f }, 0.08f));
    p.push_back (fparam (ID::size,    "Speaker Size",  { 0.0f, 1.0f, 0.0001f }, 0.58f));

    // The v0.2 spatial section is a parallel, filtered reverb branch with post-reverb drive/gating.
    p.push_back (fparam (ID::space,       "Space",        { 0.0f, 1.0f, 0.0001f }, 0.10f));
    p.push_back (fparam (ID::slapMs,      "Pre-delay",    skewedRange (0.0f, 150.0f, 24.0f), 22.0f, "ms"));
    p.push_back (fparam (ID::feedback,    "Decay",        { 0.0f, 1.0f, 0.0001f }, 0.34f));
    p.push_back (fparam (ID::width,       "Width",        { 0.0f, 1.0f, 0.0001f }, 0.52f));
    p.push_back (fparam (ID::darkness,    "Reverb Dark",  { 0.0f, 1.0f, 0.0001f }, 0.46f));
    p.push_back (fparam (ID::smear,       "Diffusion",    { 0.0f, 1.0f, 0.0001f }, 0.42f));
    p.push_back (fparam (ID::reverbDrive, "Reverb Drive", { 0.0f, 1.0f, 0.0001f }, 0.24f));
    p.push_back (fparam (ID::reverbGate,  "Reverb Gate",  { 0.0f, 1.0f, 0.0001f }, 0.22f));
    p.push_back (fparam (ID::reverbDuck,  "Reverb Duck",  { 0.0f, 1.0f, 0.0001f }, 0.18f));

    p.push_back (fparam (ID::react,      "Pressure", { 0.0f, 1.0f, 0.0001f }, 0.34f));
    p.push_back (fparam (ID::broken,     "Breakup",  { 0.0f, 1.0f, 0.0001f }, 0.18f));
    p.push_back (fparam (ID::brightness, "Focus",    { -1.0f, 1.0f, 0.0001f }, 0.0f));
    p.push_back (fparam (ID::bloom,      "Bloom",    { -1.0f, 1.0f, 0.0001f }, 0.10f));
    p.push_back (fparam (ID::weird,      "Warp",     { 0.0f, 1.0f, 0.0001f }, 0.06f));

    p.push_back (fparam (ID::mix,      "Mix",    { 0.0f, 1.0f, 0.0001f }, 1.0f));
    p.push_back (fparam (ID::outputDb, "Output", { -24.0f, 12.0f, 0.01f }, -1.0f, "dB"));
    p.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { ID::autoGain, 1 }, "Auto Level", true));
    p.push_back (std::make_unique<juce::AudioParameterBool> (juce::ParameterID { ID::safe, 1 }, "Safe Output", true));

    return { p.begin(), p.end() };
}
