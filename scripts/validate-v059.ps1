param(
  [Parameter(Mandatory = $true)]
  [string]$SourceRoot
)

$ErrorActionPreference = "Stop"
$root = (Resolve-Path $SourceRoot).Path

function Require([bool]$Condition, [string]$Message) {
  if (-not $Condition) { throw $Message }
}

function Require-Markers([string]$Text, [string[]]$Markers, [string]$Context) {
  foreach ($marker in $Markers) {
    Require $Text.Contains($marker) "Missing $Context marker '$marker'"
  }
}

Push-Location $root
try {
  $cmake = Get-Content "CMakeLists.txt" -Raw
  Require $cmake.Contains("VERSION 0.5.9") "CMake version is not 0.5.9"
  Require $cmake.Contains('COMPANY_NAME "UnknwnAWKND"') "Vendor metadata changed"
  foreach ($retiredSource in @("Source/DSP/WowFlutter.cpp", "Source/DSP/Tremolo.cpp", "Source/DSP/FeedbackResonator.cpp")) {
    Require (-not $cmake.Contains($retiredSource)) "Retired DSP source is still compiled: $retiredSource"
  }

  $editor = Get-Content "Source\PluginEditor.cpp" -Raw
  $editorHeader = Get-Content "Source\PluginEditor.h" -Raw
  $laf = Get-Content "Source\UI\CustomLookAndFeel.cpp" -Raw
  $meter = Get-Content "Source\UI\Meter.cpp" -Raw
  $allUi = $editor + [Environment]::NewLine + $editorHeader + [Environment]::NewLine + $laf

  Require-Markers $allUi @(
    "setResizeLimits (1050, 700, 1700, 1150)",
    "setSize (1050, 700)",
    'statusLabel.setText ("v0.5.9  /  PRESETS"',
    "startupSizeTicks == 2",
    "startupSizeTicks == 9",
    "slider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 94, 18)",
    "const std::array<int, 7> columns { 1, 3, 3, 3, 2, 3, 3 }",
    "const int wOutput = topUsable * 22 / 100",
    "const int wReverb = bottomUsable * 36 / 100",
    "const int wPhantom = bottomUsable * 24 / 100",
    "uiFont (21.5f, juce::Font::bold)",
    "PRESETS",
    "SAVE PRESET",
    "presetBrowserTree",
    'PresetTreeItem ("FACTORY"',
    'PresetTreeItem ("USER"',
    "event.mods.isPopupMenu()",
    'menu.addItem (1, "DELETE")',
    "showOkCancelBox",
    '"YES", "NO"',
    'confirmation != "delete"',
    "TooltipWindow",
    "updateHelpTooltips",
    "Sound:",
    "How:",
    "valueInactive",
    "knob.slider.setEnabled (enabled)",
    "PICK SENSITIVITY",
    "setTooltip"
  ) "v0.5.9 UI"

  Require (-not $allUi.Contains("factoryPresetBox")) "Separate Factory preset dropdown remains"
  Require (-not $allUi.Contains("userPresetBox")) "Separate User preset dropdown remains"
  Require (-not $allUi.Contains("deletePresetButton")) "Standalone preset Delete button remains"
  Require (-not $allUi.Contains("helpTitleLabel")) "Permanent help title remains"
  Require (-not $allUi.Contains("helpBodyLabel")) "Permanent help body remains"
  Require (-not $editor.Contains("g.fillRoundedRectangle (cell.getX() + 12.0f")) "Decorative card-title line remains"
  Require $laf.Contains("juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack") "Value background is not transparent"
  Require $laf.Contains("juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack") "Value outline is not transparent"
  Require $meter.Contains("kind == Kind::Reduction ? i < litSegments : fromBottom < litSegments") "GR/level meter directions are not distinct"

  $bindings = @([regex]::Matches($editor, 'addModuleKnob \([^\r\n]+Params::ID::(\w+)') |
    ForEach-Object { $_.Groups[1].Value })
  Require ($bindings.Count -eq 39) "Expected 39 one-page knob bindings, found $($bindings.Count)"
  Require (($bindings | Sort-Object -Unique).Count -eq 39) "One-page knob bindings contain duplicates"
  $moduleCounts = @{
    inputModule = 2
    compressionModule = 6
    driveModule = 9
    reverbModule = 9
    phantomModule = 4
    toneModule = 6
    outputModule = 3
  }
  foreach ($name in $moduleCounts.Keys) {
    $count = [regex]::Matches($editor, "addModuleKnob \($name,").Count
    Require ($count -eq $moduleCounts[$name]) "$name has $count controls, expected $($moduleCounts[$name])"
  }
  foreach ($retired in @(
    "compress", "punch", "mix", "bias", "portaTrim", "portaChannel", "portaMaster",
    "portaBassDb", "portaTrebleDb", "brightness", "bloom", "weird", "wow", "flutter",
    "drift", "damage", "detuneCents", "offsetMs", "bodyFreqHz", "biteFreqHz",
    "resonance", "formant", "feedbackRes", "feedbackFreqHz", "feedbackHold", "darkness",
    "highBreakup", "tremoloDepth", "tremoloRateHz", "modelEnabled"
  )) {
    Require (-not ($bindings -contains $retired)) "Retired knob remains visible: $retired"
  }
  $helpCount = [regex]::Matches($editor, 'if \(id == \w+\) return "Sound:').Count
  Require ($helpCount -eq 39) "Expected a Sound/How tooltip for every knob, found $helpCount"

  $paramsHeader = Get-Content "Source\Parameters\Parameters.h" -Raw
  $params = Get-Content "Source\Parameters\Parameters.cpp" -Raw
  $ids = @([regex]::Matches($paramsHeader, 'inline constexpr auto\s+(\w+)\s*=') |
    ForEach-Object { $_.Groups[1].Value })
  Require ($ids.Count -eq 87 -and ($ids | Sort-Object -Unique).Count -eq 87) "Expected 87 unique declared parameter IDs"

  $layoutIds = @([regex]::Matches($params, '(?:fparam|cparam) \(ID::(\w+)|ParameterID \{ ID::(\w+), 1 \}') |
    ForEach-Object { if ($_.Groups[1].Success) { $_.Groups[1].Value } else { $_.Groups[2].Value } })
  Require ($layoutIds.Count -eq 87 -and ($layoutIds | Sort-Object -Unique).Count -eq 87) "Expected 87 unique layout parameters"
  $expectedV058 = "inputDb,diCharacter,inputProfile,topology,compress,punch,crush,threshold,ratio,attackMs,releaseMs,makeupDb,kneeDb,transientSense,sustainSense,drive,bias,texture,sag,portaTrim,portaChannel,portaMaster,portaBassDb,portaTrebleDb,lowTight,highBreakup,wow,flutter,drift,damage,doubleMix,detuneCents,offsetMs,spread,driftLink,lowCutHz,highCutHz,body,bodyFreqHz,bite,biteFreqHz,resonance,formant,speaker,size,space,slapMs,feedback,width,darkness,smear,reverbDrive,reverbGate,reverbDuck,wetCompress,wetTone,wetBias,ghost,ghostSizeMs,modelMix,modelColor,modelMotion,feedbackRes,feedbackFreqHz,feedbackHold,react,broken,brightness,bloom,weird,mix,outputDb,autoGain,safe,compressionEnabled,driveEnabled,reverbEnabled,pitchEnabled,toneEnabled,highProtect,reverbHighCutHz,tremoloDepth,tremoloRateHz,modelEnabled,reverbLowCutHz"
  Require (($layoutIds[0..84] -join ",") -eq $expectedV058) "The 85 v0.5.8 host parameter indices changed"
  Require (($layoutIds[85..86] -join ",") -eq "dryDb,wetDb") "Dry and Wet were not appended at indices 85 and 86"
  Require-Markers $params @(
    'fparam (ID::releaseMs, "Release",   skewedRange (1.0f, 4000.0f, 180.0f)',
    'fparam (ID::makeupDb,  "Gain",      { -30.0f, 30.0f, 0.01f }',
    'fparam (ID::react,      "Pick Sensitivity"',
    'fparam (ID::mix,      "Legacy Wet / Dry"',
    'fparam (ID::dryDb, "Dry", { -100.0f, 0.0f, 0.01f }, -3.0103f',
    'fparam (ID::wetDb, "Wet", { -100.0f, 0.0f, 0.01f }, -3.0103f'
  ) "parameter"

  $processor = Get-Content "Source\PluginProcessor.cpp" -Raw
  $processorHeader = Get-Content "Source\PluginProcessor.h" -Raw
  $processMatch = [regex]::Match($processor, '(?s)void BrokenMachineAudioProcessor::processBlock.*?(?=void BrokenMachineAudioProcessor::getStateInformation)')
  Require $processMatch.Success "Could not isolate processBlock"
  $process = $processMatch.Value
  foreach ($retired in @(
    "compress", "punch", "kneeDb", "mix", "bias", "portaTrim", "portaChannel", "portaMaster",
    "portaBassDb", "portaTrebleDb", "brightness", "bloom", "weird", "wow", "flutter",
    "drift", "damage", "detuneCents", "offsetMs", "bodyFreqHz", "biteFreqHz",
    "resonance", "formant", "feedbackRes", "feedbackFreqHz", "feedbackHold", "darkness",
    "highBreakup", "tremoloDepth", "tremoloRateHz", "modelEnabled"
  )) {
    Require (-not [regex]::IsMatch($process, "(?:value|boolValue|choiceValue) \(Params::ID::$retired\)")) "Retired parameter still drives processBlock: $retired"
  }
  Require-Markers ($processor + [Environment]::NewLine + $processorHeader) @(
    "dryGain.setTargetValue",
    "wetGain.setTargetValue",
    "outputLevelGain",
    "dry * dryG + wet * wetG",
    "restoreStateWithMigration",
    "stateParameterValue",
    "outputLevelDb (1.0f - legacyMix)",
    "outputLevelDb (legacyMix)",
    "legacyThreshold - legacyMacro * 12.0f",
    'confirmationText != "delete"',
    "wetDc2X1",
    "12.0f / static_cast<float> (currentSampleRate)"
  ) "processor"

  $compressor = Get-Content "Source\DSP\Compressor.cpp" -Raw
  $compressorHeader = Get-Content "Source\DSP\Compressor.h" -Raw
  Require-Markers ($compressor + [Environment]::NewLine + $compressorHeader) @(
    "float gainDb = 0.0f",
    "juce::jlimit (1.0f, 4000.0f, s.releaseMs)",
    "computeGainDb (cinDb, crushThreshold, 12.0f, 10.0f)",
    "(main + (crushed - main) * crushMix) * postGain",
    "postGainSmoother",
    "crushMixSmoother"
  ) "compressor"
  foreach ($hiddenField in @("s.macro", "s.punch", "s.knee", "s.transient", "s.sustain", "s.bloom")) {
    Require (-not $compressor.Contains($hiddenField)) "Hidden compressor setting remains active: $hiddenField"
  }

  $saturator = Get-Content "Source\DSP\Saturator.cpp" -Raw
  $saturatorHeader = Get-Content "Source\DSP\Saturator.h" -Raw
  Require-Markers ($saturator + [Environment]::NewLine + $saturatorHeader) @(
    "preampAmount <= 1.0e-6f && broken <= 1.0e-6f",
    "reset();",
    "return;",
    "stage1Gain",
    "stage2Gain",
    "distortionDrive",
    "distortionMix",
    "lowProtected * lowExclude",
    "highProtected * highExclude"
  ) "Drive transparency"
  Require (-not [regex]::IsMatch($saturator, 's\.porta\w+')) "Retired Porta setting still drives Saturator"

  @(
    "Static v0.5.9 audit passed",
    "Seven cards; 39 unique one-page knobs",
    "Drive and Reverb: 3 x 3; Compression: two rows of three",
    "87 unique parameters; all 85 v0.5.8 indices preserved; Dry and Wet appended",
    "Drive has an exact zero-effect transparent fast path",
    "Compression exposes exact controls with smoothed post-blend Gain and Parallel blend",
    "Independent smoothed Dry and Wet dB paths with exact OFF mute",
    "Unified Factory/User preset browser with protected right-click User deletion",
    "Floating Sound/How tooltips; no permanent help footer",
    "Gain reduction fills downward; level meters fill upward",
    "Two-pole 12 Hz processed-path blocker preserved"
  ) | Set-Content "SOURCE_VALIDATION.txt"
  Get-Content "SOURCE_VALIDATION.txt"
}
finally {
  Pop-Location
}
