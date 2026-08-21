#include "CustomLookAndFeel.h"

namespace
{
    const juce::Colour bg       { 0xff0c0f0f };
    const juce::Colour panel    { 0xff151919 };
    const juce::Colour panelHi  { 0xff1c2120 };
    const juce::Colour border   { 0xff303735 };
    const juce::Colour text     { 0xffe7e2d8 };
    const juce::Colour muted    { 0xff868f8b };
    const juce::Colour accent   { 0xffa5c7b8 };
    const juce::Colour accentHi { 0xffc9e3d6 };
}

BrokenLookAndFeel::BrokenLookAndFeel()
{
    setColour (juce::Slider::textBoxTextColourId, text);
    setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0xff111515));
    setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    setColour (juce::Slider::textBoxHighlightColourId, accent.withAlpha (0.28f));

    setColour (juce::ComboBox::backgroundColourId, panel);
    setColour (juce::ComboBox::textColourId, text);
    setColour (juce::ComboBox::outlineColourId, border);
    setColour (juce::PopupMenu::backgroundColourId, panel);
    setColour (juce::PopupMenu::textColourId, text);
    setColour (juce::PopupMenu::highlightedBackgroundColourId, accent.withAlpha (0.18f));
    setColour (juce::PopupMenu::highlightedTextColourId, accentHi);

    setColour (juce::Label::textColourId, text);
    setColour (juce::TextButton::textColourOffId, text);
    setColour (juce::TextButton::textColourOnId, bg);
}

void BrokenLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                          float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                          juce::Slider& slider)
{
    auto bounds = juce::Rectangle<float> (static_cast<float> (x), static_cast<float> (y),
                                           static_cast<float> (width), static_cast<float> (height)).reduced (12.0f);
    const float diameter = juce::jmin (bounds.getWidth(), bounds.getHeight());
    bounds = juce::Rectangle<float> (diameter, diameter).withCentre (bounds.getCentre());
    const auto centre = bounds.getCentre();
    const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

    auto ringBounds = bounds.reduced (5.0f);
    juce::Path track;
    track.addCentredArc (centre.x, centre.y, ringBounds.getWidth() * 0.5f, ringBounds.getHeight() * 0.5f,
                         0.0f, rotaryStartAngle, rotaryEndAngle, true);
    g.setColour (border.withAlpha (0.78f));
    g.strokePath (track, juce::PathStrokeType (4.6f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    juce::Path valueArc;
    valueArc.addCentredArc (centre.x, centre.y, ringBounds.getWidth() * 0.5f, ringBounds.getHeight() * 0.5f,
                            0.0f, rotaryStartAngle, angle, true);
    g.setColour (slider.isMouseOverOrDragging() ? accentHi : accent);
    g.strokePath (valueArc, juce::PathStrokeType (4.6f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    auto face = bounds.reduced (12.0f);
    juce::ColourGradient grad (panelHi, face.getCentreX(), face.getY(), panel, face.getCentreX(), face.getBottom(), false);
    g.setGradientFill (grad);
    g.fillEllipse (face);
    g.setColour (border.withAlpha (0.72f));
    g.drawEllipse (face, 1.0f);

    const float radius = face.getWidth() * 0.5f;
    const auto pointerEnd = centre + juce::Point<float> (std::sin (angle), -std::cos (angle)) * (radius * 0.68f);
    g.setColour (text.withAlpha (0.95f));
    g.drawLine (centre.x, centre.y, pointerEnd.x, pointerEnd.y, 2.0f);
    g.setColour (accentHi);
    g.fillEllipse (pointerEnd.x - 2.4f, pointerEnd.y - 2.4f, 4.8f, 4.8f);
}

void BrokenLookAndFeel::drawButtonBackground (juce::Graphics& g, juce::Button& button,
                                              const juce::Colour&, bool highlighted, bool down)
{
    auto r = button.getLocalBounds().toFloat().reduced (0.5f);
    auto c = panel;
    if (highlighted) c = panelHi;
    if (down) c = accent.withAlpha (0.22f).overlaidWith (panelHi);
    if (button.getToggleState()) c = accent;

    g.setColour (c);
    g.fillRoundedRectangle (r, 7.0f);
    g.setColour (button.getToggleState() ? accentHi.withAlpha (0.50f) : border);
    g.drawRoundedRectangle (r, 7.0f, 1.0f);
}

void BrokenLookAndFeel::drawComboBox (juce::Graphics& g, int width, int height, bool isButtonDown,
                                      int, int, int, int, juce::ComboBox& box)
{
    auto r = juce::Rectangle<float> (0.5f, 0.5f, static_cast<float> (width - 1), static_cast<float> (height - 1));
    g.setColour (isButtonDown ? panelHi : panel);
    g.fillRoundedRectangle (r, 7.0f);
    g.setColour (box.hasKeyboardFocus (true) ? accent : border);
    g.drawRoundedRectangle (r, 7.0f, 1.0f);

    const float cx = static_cast<float> (width - 18);
    const float cy = static_cast<float> (height) * 0.5f;
    juce::Path arrow;
    arrow.startNewSubPath (cx - 4.0f, cy - 2.0f);
    arrow.lineTo (cx, cy + 2.0f);
    arrow.lineTo (cx + 4.0f, cy - 2.0f);
    g.setColour (muted);
    g.strokePath (arrow, juce::PathStrokeType (1.6f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

void BrokenLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& button, bool highlighted, bool)
{
    auto r = button.getLocalBounds().toFloat();
    const float pillW = 30.0f;
    const float pillH = 16.0f;
    auto pill = juce::Rectangle<float> (r.getX(), r.getCentreY() - pillH * 0.5f, pillW, pillH);
    g.setColour (button.getToggleState() ? accent.withAlpha (0.86f) : border);
    g.fillRoundedRectangle (pill, pillH * 0.5f);

    const float knob = 12.0f;
    const float knobX = button.getToggleState() ? pill.getRight() - knob - 2.0f : pill.getX() + 2.0f;
    g.setColour (button.getToggleState() ? bg : muted.brighter (0.25f));
    g.fillEllipse (knobX, pill.getCentreY() - knob * 0.5f, knob, knob);

    g.setColour (highlighted ? text : muted);
    g.setFont (juce::Font (juce::FontOptions (11.0f, juce::Font::bold)));
    g.drawText (button.getButtonText(), r.withTrimmedLeft (38.0f), juce::Justification::centredLeft, true);
}
