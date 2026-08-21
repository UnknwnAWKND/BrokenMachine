#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/CustomLookAndFeel.h"
#include "UI/Meter.h"

class BrokenMachineAudioProcessorEditor : public juce::AudioProcessorEditor,
                                          private juce::Timer
{
public:
    explicit BrokenMachineAudioProcessorEditor (BrokenMachineAudioProcessor&);
    ~BrokenMachineAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    struct Knob
    {
        Knob (juce::AudioProcessorValueTreeState& state,
              const juce::String& parameterID,
              const juce::String& title,
              bool isMacro);
        juce::Slider slider;
        juce::Label label;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    };

    void timerCallback() override;
    void setAdvancedVisible (bool shouldShow);
    void addKnob (std::vector<std::unique_ptr<Knob>>& destination,
                  const char* paramID,
                  const juce::String& title,
                  bool isMacro = false);

    BrokenMachineAudioProcessor& processor;
    BrokenLookAndFeel look;

    juce::Label titleLabel;
    juce::Label subtitleLabel;
    juce::Label presetCaption;
    juce::Label statusLabel;
    juce::ComboBox presetBox;
    juce::TextButton mutateButton { "VARIATION" };
    juce::TextButton advancedButton { "DEEP EDIT" };
    juce::ToggleButton autoGainButton { "AUTO LEVEL" };
    juce::ToggleButton safeButton { "SAFE OUT" };
    juce::ToggleButton driftLinkButton { "LINK DRIFT" };

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> autoGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> safeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> driftLinkAttachment;

    std::vector<std::unique_ptr<Knob>> mainKnobs;
    std::vector<std::unique_ptr<Knob>> advancedKnobs;

    LevelMeter inputMeter { LevelMeter::Kind::Level };
    LevelMeter outputMeter { LevelMeter::Kind::Level };
    LevelMeter grMeter { LevelMeter::Kind::Reduction };
    LevelMeter driftMeter { LevelMeter::Kind::Drift };
    juce::Label inputMeterLabel, outputMeterLabel, grMeterLabel, driftMeterLabel;

    bool showingAdvanced = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BrokenMachineAudioProcessorEditor)
};
