#pragma once

#include <JuceHeader.h>

namespace Params
{
    namespace ID
    {
        inline constexpr auto inputDb        = "inputDb";
        inline constexpr auto diCharacter    = "diCharacter";
        inline constexpr auto compress       = "compress";
        inline constexpr auto punch          = "punch";
        inline constexpr auto crush          = "crush";
        inline constexpr auto threshold      = "threshold";
        inline constexpr auto ratio          = "ratio";
        inline constexpr auto attackMs       = "attackMs";
        inline constexpr auto releaseMs      = "releaseMs";
        inline constexpr auto makeupDb       = "makeupDb";
        inline constexpr auto kneeDb         = "kneeDb";

        inline constexpr auto drive          = "drive";
        inline constexpr auto bias           = "bias";
        inline constexpr auto texture        = "texture";
        inline constexpr auto sag            = "sag";
        inline constexpr auto portaTrim      = "portaTrim";
        inline constexpr auto portaChannel   = "portaChannel";
        inline constexpr auto portaBassDb    = "portaBassDb";
        inline constexpr auto portaTrebleDb  = "portaTrebleDb";

        inline constexpr auto wow            = "wow";
        inline constexpr auto flutter        = "flutter";
        inline constexpr auto drift          = "drift";
        inline constexpr auto damage         = "damage";
        inline constexpr auto doubleMix      = "doubleMix";
        inline constexpr auto detuneCents    = "detuneCents";
        inline constexpr auto offsetMs       = "offsetMs";
        inline constexpr auto spread         = "spread";
        inline constexpr auto driftLink      = "driftLink";

        inline constexpr auto lowCutHz       = "lowCutHz";
        inline constexpr auto highCutHz      = "highCutHz";
        inline constexpr auto body           = "body";
        inline constexpr auto bodyFreqHz     = "bodyFreqHz";
        inline constexpr auto bite           = "bite";
        inline constexpr auto biteFreqHz     = "biteFreqHz";
        inline constexpr auto resonance      = "resonance";
        inline constexpr auto formant        = "formant";
        inline constexpr auto speaker        = "speaker";
        inline constexpr auto size           = "size";

        inline constexpr auto space          = "space";
        inline constexpr auto slapMs         = "slapMs";
        inline constexpr auto feedback       = "feedback";
        inline constexpr auto width          = "width";
        inline constexpr auto darkness       = "darkness";
        inline constexpr auto smear          = "smear";
        inline constexpr auto reverbDrive    = "reverbDrive";
        inline constexpr auto reverbGate     = "reverbGate";
        inline constexpr auto reverbDuck     = "reverbDuck";

        // Macro IDs are kept for backward state compatibility, but the v0.2 UI gives them clearer names.
        inline constexpr auto react          = "react";       // PRESSURE
        inline constexpr auto broken         = "broken";      // BREAKUP
        inline constexpr auto brightness     = "brightness";  // FOCUS
        inline constexpr auto bloom          = "bloom";       // BLOOM
        inline constexpr auto weird          = "weird";       // WARP

        inline constexpr auto mix            = "mix";
        inline constexpr auto outputDb       = "outputDb";
        inline constexpr auto autoGain       = "autoGain";
        inline constexpr auto safe           = "safe";
    }

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
}
