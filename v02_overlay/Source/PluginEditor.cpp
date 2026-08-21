#include "PluginEditor.h"

namespace
{
    const juce::Colour bg       { 0xff0c0f0f };
    const juce::Colour header   { 0xff101413 };
    const juce::Colour panel    { 0xff141918 };
    const juce::Colour panelAlt { 0xff171c1b };
    const juce::Colour border   { 0xff2b3330 };
    const juce::Colour text     { 0xffe8e2d7 };
    const juce::Colour muted    { 0xff7e8984 };
    const juce::Colour accent   { 0xffa5c7b8 };
}

BrokenMachineAudioProcessorEditor::Knob::Knob (juce::AudioProcessorValueTreeState& state,
                                                const juce::String& parameterID,
                                                const juce::String& title,
                                                bool isMacro)
{
    slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, isMacro ? 74 : 66, 18);
    if (auto* parameter = state.getParameter (parameterID))
        slider.setDoubleClickReturnValue (true, parameter->convertFrom0to1 (parameter->getDefaultValue()));
    slider.setMouseDragSensitivity (isMacro ? 190 : 150);

    label.setText (title, juce::dontSendNotification);
    label.setJustificationType (juce::Justification::centred);
    label.setColour (juce::Label::textColourId, isMacro ? text : muted.brighter (0.12f));
    label.setFont (juce::Font (juce::FontOptions (isMacro ? 11.5f : 10.0f, juce::Font::bold)));

    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (state, parameterID, slider);
}

BrokenMachineAudioProcessorEditor::BrokenMachineAudioProcessorEditor (BrokenMachineAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setLookAndFeel (&look);
    setResizable (true, true);
    setResizeLimits (900, 580, 1500, 980);
    setSize (1080, 700);

    titleLabel.setText ("BROKEN MACHINE", juce::dontSendNotification);
    titleLabel.setFont (juce::Font (juce::FontOptions (25.0f, juce::Font::bold)));
    titleLabel.setColour (juce::Label::textColourId, text);
    addAndMakeVisible (titleLabel);

    subtitleLabel.setText ("DYNAMIC PREAMP / DISTORTED REVERB / UNSTABLE MODELING", juce::dontSendNotification);
    subtitleLabel.setFont (juce::Font (juce::FontOptions (9.5f, juce::Font::bold)));
    subtitleLabel.setColour (juce::Label::textColourId, muted);
    addAndMakeVisible (subtitleLabel);

    presetCaption.setText ("TONE", juce::dontSendNotification);
    presetCaption.setFont (juce::Font (juce::FontOptions (9.0f, juce::Font::bold)));
    presetCaption.setColour (juce::Label::textColourId, muted);
    addAndMakeVisible (presetCaption);

    statusLabel.setText ("v0.2  /  2x PREAMP OS  /  VST3", juce::dontSendNotification);
    statusLabel.setFont (juce::Font (juce::FontOptions (9.0f, juce::Font::bold)));
    statusLabel.setColour (juce::Label::textColourId, muted);
    statusLabel.setJustificationType (juce::Justification::centredLeft);
    addAndMakeVisible (statusLabel);

    presetBox.addItemList (processor.getFactoryPresetNames(), 1);
    presetBox.setSelectedItemIndex (0, juce::dontSendNotification);
    presetBox.onChange = [this]
    {
        const int index = presetBox.getSelectedItemIndex();
        if (index >= 0)
            processor.loadFactoryPreset (index);
    };
    addAndMakeVisible (presetBox);

    mutateButton.onClick = [this] { processor.mutate(); };
    addAndMakeVisible (mutateButton);

    advancedButton.setClickingTogglesState (true);
    advancedButton.onClick = [this] { setAdvancedVisible (advancedButton.getToggleState()); };
    addAndMakeVisible (advancedButton);

    autoGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (processor.getAPVTS(), Params::ID::autoGain, autoGainButton);
    safeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (processor.getAPVTS(), Params::ID::safe, safeButton);
    driftLinkAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (processor.getAPVTS(), Params::ID::driftLink, driftLinkButton);
    addAndMakeVisible (autoGainButton);
    addAndMakeVisible (safeButton);
    addAndMakeVisible (driftLinkButton);

    // Main page is intentionally eight controls. The macros coordinate the deeper DSP rather than exposing a wall of knobs.
    addKnob (mainKnobs, Params::ID::inputDb,    "INPUT",    true);
    addKnob (mainKnobs, Params::ID::react,      "PRESSURE", true);
    addKnob (mainKnobs, Params::ID::bloom,      "BLOOM",    true);
    addKnob (mainKnobs, Params::ID::broken,     "BREAKUP",  true);
    addKnob (mainKnobs, Params::ID::weird,      "WARP",     true);
    addKnob (mainKnobs, Params::ID::brightness, "FOCUS",    true);
    addKnob (mainKnobs, Params::ID::space,      "SPACE",    true);
    addKnob (mainKnobs, Params::ID::outputDb,   "OUTPUT",   true);

    const std::pair<const char*, const char*> adv[] = {
        { Params::ID::compress, "COMPRESS" }, { Params::ID::punch, "PUNCH" }, { Params::ID::crush, "CRUSH" },
        { Params::ID::threshold, "THRESH" }, { Params::ID::ratio, "RATIO" }, { Params::ID::attackMs, "ATTACK" },
        { Params::ID::releaseMs, "RELEASE" }, { Params::ID::makeupDb, "MAKEUP" },

        { Params::ID::portaTrim, "424 TRIM" }, { Params::ID::portaChannel, "424 CHANNEL" },
        { Params::ID::portaBassDb, "424 BASS" }, { Params::ID::portaTrebleDb, "424 TREBLE" },
        { Params::ID::drive, "DRIVE" }, { Params::ID::texture, "GRAIN" }, { Params::ID::sag, "SAG" }, { Params::ID::bias, "BIAS" },

        { Params::ID::wow, "WOW" }, { Params::ID::flutter, "FLUTTER" }, { Params::ID::drift, "DRIFT" }, { Params::ID::damage, "DAMAGE" },
        { Params::ID::doubleMix, "DOUBLE" }, { Params::ID::detuneCents, "DETUNE" }, { Params::ID::offsetMs, "OFFSET" }, { Params::ID::spread, "SPREAD" },

        { Params::ID::slapMs, "PRE-DELAY" }, { Params::ID::feedback, "DECAY" }, { Params::ID::reverbDrive, "VERB DRIVE" },
        { Params::ID::reverbGate, "VERB GATE" }, { Params::ID::reverbDuck, "VERB DUCK" }, { Params::ID::smear, "DIFFUSION" },
        { Params::ID::darkness, "VERB DARK" }, { Params::ID::width, "WIDTH" },

        { Params::ID::lowCutHz, "LOW CUT" }, { Params::ID::highCutHz, "HIGH CUT" }, { Params::ID::body, "BODY" },
        { Params::ID::bodyFreqHz, "BODY FREQ" }, { Params::ID::bite, "BITE" }, { Params::ID::biteFreqHz, "BITE FREQ" },
        { Params::ID::resonance, "RESONANCE" }, { Params::ID::formant, "MODELER" },
        { Params::ID::speaker, "SPEAKER" }, { Params::ID::size, "SPKR SIZE" }, { Params::ID::diCharacter, "DI COLOR" }, { Params::ID::mix, "MIX" }
    };
    for (const auto& [id, name] : adv)
        addKnob (advancedKnobs, id, name, false);

    auto setupMeterLabel = [this] (juce::Label& l, const juce::String& labelText)
    {
        l.setText (labelText, juce::dontSendNotification);
        l.setJustificationType (juce::Justification::centred);
        l.setColour (juce::Label::textColourId, muted);
        l.setFont (juce::Font (juce::FontOptions (8.5f, juce::Font::bold)));
        addAndMakeVisible (l);
    };

    setupMeterLabel (inputMeterLabel, "IN");
    setupMeterLabel (outputMeterLabel, "OUT");
    setupMeterLabel (grMeterLabel, "GR");
    setupMeterLabel (driftMeterLabel, "DRIFT");
    addAndMakeVisible (inputMeter); addAndMakeVisible (outputMeter); addAndMakeVisible (grMeter); addAndMakeVisible (driftMeter);

    setAdvancedVisible (false);
    startTimerHz (30);
}

BrokenMachineAudioProcessorEditor::~BrokenMachineAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

void BrokenMachineAudioProcessorEditor::addKnob (std::vector<std::unique_ptr<Knob>>& destination,
                                                  const char* paramID,
                                                  const juce::String& title,
                                                  bool isMacro)
{
    auto k = std::make_unique<Knob> (processor.getAPVTS(), paramID, title, isMacro);
    addAndMakeVisible (k->slider);
    addAndMakeVisible (k->label);
    destination.push_back (std::move (k));
}

void BrokenMachineAudioProcessorEditor::setAdvancedVisible (bool shouldShow)
{
    showingAdvanced = shouldShow;
    advancedButton.setButtonText (shouldShow ? "BACK" : "DEEP EDIT");

    for (auto& k : mainKnobs)
    {
        k->slider.setVisible (! shouldShow);
        k->label.setVisible (! shouldShow);
    }
    for (auto& k : advancedKnobs)
    {
        k->slider.setVisible (shouldShow);
        k->label.setVisible (shouldShow);
    }
    driftLinkButton.setVisible (shouldShow);
    resized();
    repaint();
}

void BrokenMachineAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (bg);

    auto bounds = getLocalBounds();
    auto head = bounds.removeFromTop (108).toFloat();
    g.setColour (header);
    g.fillRect (head);
    g.setColour (border);
    g.drawLine (0.0f, head.getBottom(), static_cast<float> (getWidth()), head.getBottom(), 1.0f);

    g.setColour (accent);
    g.fillRoundedRectangle (20.0f, 25.0f, 3.0f, 47.0f, 1.5f);

    bounds.removeFromBottom (38);
    auto content = bounds.reduced (20, 16);

    if (! showingAdvanced)
    {
        const int columns = 4;
        const int rows = 2;
        const int gap = 10;
        const int cellW = (content.getWidth() - gap * (columns - 1)) / columns;
        const int cellH = (content.getHeight() - gap * (rows - 1)) / rows;

        for (int row = 0; row < rows; ++row)
            for (int col = 0; col < columns; ++col)
            {
                auto card = juce::Rectangle<int> (content.getX() + col * (cellW + gap),
                                                   content.getY() + row * (cellH + gap),
                                                   cellW, cellH).toFloat();
                g.setColour ((row + col) % 2 == 0 ? panel : panelAlt);
                g.fillRoundedRectangle (card, 12.0f);
                g.setColour (border.withAlpha (0.82f));
                g.drawRoundedRectangle (card, 12.0f, 1.0f);
            }
    }
    else
    {
        auto card = content.toFloat();
        g.setColour (panel);
        g.fillRoundedRectangle (card, 12.0f);
        g.setColour (border.withAlpha (0.82f));
        g.drawRoundedRectangle (card, 12.0f, 1.0f);
    }

    auto footer = juce::Rectangle<float> (0.0f, static_cast<float> (getHeight() - 38), static_cast<float> (getWidth()), 38.0f);
    g.setColour (header.darker (0.08f));
    g.fillRect (footer);
    g.setColour (border.withAlpha (0.80f));
    g.drawLine (0.0f, footer.getY(), static_cast<float> (getWidth()), footer.getY(), 1.0f);
    g.setColour (muted);
    g.setFont (juce::Font (juce::FontOptions (9.5f, juce::Font::bold)));
    g.drawText (showingAdvanced
                    ? "DEEP EDIT  /  REVERB RETURN IS DISTORTED + GATED BEFORE THE 424-STYLE PREAMP"
                    : "PLAY  /  SOFT NOTES STAY OPEN — HARD NOTES PUSH THE PREAMP, REVERB RETURN, AND COMPRESSION",
                footer.reduced (20.0f, 0.0f), juce::Justification::centredLeft);
}

void BrokenMachineAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    auto head = area.removeFromTop (108).reduced (20, 12);

    auto brand = head.removeFromLeft (300);
    titleLabel.setBounds (brand.removeFromTop (39));
    subtitleLabel.setBounds (brand.removeFromTop (19));
    statusLabel.setBounds (brand.removeFromTop (18));

    auto meters = head.removeFromRight (160);
    auto placeMeter = [&meters] (LevelMeter& meter, juce::Label& label)
    {
        auto cell = meters.removeFromLeft (38);
        label.setBounds (cell.removeFromBottom (15));
        meter.setBounds (cell.reduced (10, 0));
    };
    placeMeter (inputMeter, inputMeterLabel);
    placeMeter (outputMeter, outputMeterLabel);
    placeMeter (grMeter, grMeterLabel);
    auto driftCell = meters;
    driftMeterLabel.setBounds (driftCell.removeFromBottom (15));
    driftMeter.setBounds (driftCell.reduced (4, 12));

    auto controls = head.reduced (8, 0);
    auto top = controls.removeFromTop (48);
    presetCaption.setBounds (top.removeFromLeft (42));
    presetBox.setBounds (top.removeFromLeft (238).reduced (0, 6));
    mutateButton.setBounds (top.removeFromLeft (96).reduced (7, 6));
    advancedButton.setBounds (top.removeFromLeft (108).reduced (7, 6));

    auto lower = controls.removeFromTop (34);
    autoGainButton.setBounds (lower.removeFromLeft (106));
    safeButton.setBounds (lower.removeFromLeft (92));
    driftLinkButton.setBounds (lower.removeFromLeft (104));

    area.removeFromBottom (38);
    auto content = area.reduced (20, 16);

    auto& knobs = showingAdvanced ? advancedKnobs : mainKnobs;
    const int columns = showingAdvanced ? 8 : 4;
    const int rows = static_cast<int> (std::ceil (static_cast<double> (knobs.size()) / static_cast<double> (columns)));
    const int gap = showingAdvanced ? 4 : 10;
    const int cellW = (content.getWidth() - gap * (columns - 1)) / columns;
    const int cellH = (content.getHeight() - gap * (juce::jmax (1, rows) - 1)) / juce::jmax (1, rows);

    for (size_t i = 0; i < knobs.size(); ++i)
    {
        const int col = static_cast<int> (i) % columns;
        const int row = static_cast<int> (i) / columns;
        auto cell = juce::Rectangle<int> (content.getX() + col * (cellW + gap),
                                          content.getY() + row * (cellH + gap),
                                          cellW, cellH).reduced (showingAdvanced ? 3 : 12);
        auto labelArea = cell.removeFromTop (showingAdvanced ? 17 : 22);
        knobs[i]->label.setBounds (labelArea);
        knobs[i]->slider.setBounds (cell.reduced (showingAdvanced ? 2 : 6));
    }
}

void BrokenMachineAudioProcessorEditor::timerCallback()
{
    inputMeter.setValue (processor.getInputMeter());
    outputMeter.setValue (processor.getOutputMeter());
    grMeter.setValue (processor.getGainReductionMeter());
    driftMeter.setValue (processor.getDriftMeter());
}
