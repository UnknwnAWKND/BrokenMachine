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
  Require $cmake.Contains("VERSION 0.6.0") "CMake version is not 0.6.0"
  Require $cmake.Contains('COMPANY_NAME "UnknwnAWKND"') "Vendor metadata changed"
  Require-Markers $cmake @(
    "BrokenMachineDspValidation",
    "Tests/DspValidation.cpp",
    "BrokenMachineProcessorValidation",
    "Tests/ProcessorValidation.cpp",
    "BrokenMachineVisualValidation",
    "Tests/VisualValidation.cpp"
  ) "validation target"
  foreach ($retiredSource in @("Source/DSP/WowFlutter.cpp", "Source/DSP/Tremolo.cpp", "Source/DSP/FeedbackResonator.cpp")) {
    Require (-not $cmake.Contains($retiredSource)) "Retired DSP source is compiled: $retiredSource"
  }

  $editor = Get-Content "Source\PluginEditor.cpp" -Raw
  $editorHeader = Get-Content "Source\PluginEditor.h" -Raw
  $laf = Get-Content "Source\UI\CustomLookAndFeel.cpp" -Raw
  $lafHeader = Get-Content "Source\UI\CustomLookAndFeel.h" -Raw
  $meter = Get-Content "Source\UI\Meter.cpp" -Raw
  $allUi = $editor + [Environment]::NewLine + $editorHeader + [Environment]::NewLine + $laf + [Environment]::NewLine + $lafHeader

  Require-Markers $allUi @(
    "setResizeLimits (1050, 700, 1700, 1150)",
    "setSize (1050, 700)",
    'statusLabel.setText ("v0.6.0  /  PRESETS"',
    'enum class Theme { Vintage, Night }',
    'uiSettings->getValue ("editorTheme", "Vintage")',
    'uiSettings->setValue ("editorTheme"',
    'themeBox.addItemList ({ "VINTAGE", "NIGHT" }',
    "presetBrowserTree.setOpaque (true)",
    "drawLabel (juce::Graphics& g, juce::Label& label)",
    "dynamic_cast<juce::Slider*>",
    "slider.updateText()",
    "juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack",
    "juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack",
    "const int wInput = topUsable * 22 / 100",
    "const int wComp = topUsable * 28 / 100",
    "const int wTone = topUsable * 29 / 100",
    "const int wDrive = bottomUsable * 33 / 100",
    "const int wReverb = bottomUsable * 39 / 100",
    "const std::array<int, 7> columns { 1, 3, 3, 3, 2, 3, 1 }",
    "driftLinkButton.setBounds",
    "body.removeFromBottom",
    "LINK L/R",
    'PresetTreeItem ("FACTORY"',
    'PresetTreeItem ("USER"',
    "event.mods.isPopupMenu()",
    "openPresetContextMenu",
    'contextMenuDeleteButton { "DELETE USER PRESET" }',
    "showOkCancelBox",
    'confirmation != "delete"',
    "setVisualTestState",
    '"context-menu"',
    '"delete-confirmation"'
  ) "v0.6.0 UI"

  Require (-not $allUi.Contains("autoGainButton")) "Auto Level UI member remains"
  Require (-not $allUi.Contains('"AUTO LEVEL"')) "Auto Level label remains"
  Require (-not $allUi.Contains("presetBrowserTree.setOpaque (false)")) "Preset browser is transparent"
  Require (-not $allUi.Contains("factoryPresetBox")) "Separate Factory dropdown remains"
  Require (-not $allUi.Contains("userPresetBox")) "Separate User dropdown remains"
  Require $laf.Contains("theme == Theme::Vintage") "Theme palette is not centralized"
  Require $laf.Contains("juce::Colour (0xff2c2d2e)") "Night does not use neutral dark-gray plates"
  Require $meter.Contains("vintage ? vintageGreen : mint") "Meters are not theme-aware"

  $bindings = @([regex]::Matches($editor, 'addModuleKnob \([^\r\n]+Params::ID::(\w+)') |
    ForEach-Object { $_.Groups[1].Value })
  Require ($bindings.Count -eq 39) "Expected 39 one-page knob bindings, found $($bindings.Count)"
  Require (($bindings | Sort-Object -Unique).Count -eq 39) "One-page bindings contain duplicates"
  $moduleCounts = @{
    inputModule = 2; compressionModule = 6; driveModule = 9; reverbModule = 9;
    phantomModule = 4; toneModule = 6; outputModule = 3
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
    "highBreakup", "tremoloDepth", "tremoloRateHz", "modelEnabled", "autoGain"
  )) {
    Require (-not ($bindings -contains $retired)) "Retired knob remains visible: $retired"
  }
  $helpCount = [regex]::Matches($editor, 'if \(id == \w+\) return "Sound:').Count
  Require ($helpCount -eq 39) "Expected 39 knob Sound/How tooltips, found $helpCount"

  $paramsHeader = Get-Content "Source\Parameters\Parameters.h" -Raw
  $params = Get-Content "Source\Parameters\Parameters.cpp" -Raw
  $ids = @([regex]::Matches($paramsHeader, 'inline constexpr auto\s+(\w+)\s*=') |
    ForEach-Object { $_.Groups[1].Value })
  Require ($ids.Count -eq 87 -and ($ids | Sort-Object -Unique).Count -eq 87) "Expected 87 unique declared IDs"
  $layoutIds = @([regex]::Matches($params, '(?:fparam|cparam) \(ID::(\w+)|ParameterID \{ ID::(\w+), 1 \}') |
    ForEach-Object { if ($_.Groups[1].Success) { $_.Groups[1].Value } else { $_.Groups[2].Value } })
  Require ($layoutIds.Count -eq 87 -and ($layoutIds | Sort-Object -Unique).Count -eq 87) "Expected 87 unique layout parameters"
  $expectedV059 = "inputDb,diCharacter,inputProfile,topology,compress,punch,crush,threshold,ratio,attackMs,releaseMs,makeupDb,kneeDb,transientSense,sustainSense,drive,bias,texture,sag,portaTrim,portaChannel,portaMaster,portaBassDb,portaTrebleDb,lowTight,highBreakup,wow,flutter,drift,damage,doubleMix,detuneCents,offsetMs,spread,driftLink,lowCutHz,highCutHz,body,bodyFreqHz,bite,biteFreqHz,resonance,formant,speaker,size,space,slapMs,feedback,width,darkness,smear,reverbDrive,reverbGate,reverbDuck,wetCompress,wetTone,wetBias,ghost,ghostSizeMs,modelMix,modelColor,modelMotion,feedbackRes,feedbackFreqHz,feedbackHold,react,broken,brightness,bloom,weird,mix,outputDb,autoGain,safe,compressionEnabled,driveEnabled,reverbEnabled,pitchEnabled,toneEnabled,highProtect,reverbHighCutHz,tremoloDepth,tremoloRateHz,modelEnabled,reverbLowCutHz,dryDb,wetDb"
  Require (($layoutIds -join ",") -eq $expectedV059) "The v0.5.9 host parameter order changed"

  $processor = Get-Content "Source\PluginProcessor.cpp" -Raw
  $processorHeader = Get-Content "Source\PluginProcessor.h" -Raw
  $processMatch = [regex]::Match($processor, '(?s)void BrokenMachineAudioProcessor::processBlock.*?(?=void BrokenMachineAudioProcessor::getStateInformation)')
  Require $processMatch.Success "Could not isolate processBlock"
  $process = $processMatch.Value
  foreach ($retired in @(
    "diCharacter", "compress", "punch", "kneeDb", "mix", "bias", "portaTrim", "portaChannel", "portaMaster",
    "portaBassDb", "portaTrebleDb", "brightness", "bloom", "weird", "wow", "flutter",
    "drift", "damage", "detuneCents", "offsetMs", "bodyFreqHz", "biteFreqHz",
    "resonance", "formant", "feedbackRes", "feedbackFreqHz", "feedbackHold", "darkness", "smear",
    "wetCompress", "wetTone", "wetBias",
    "highBreakup", "tremoloDepth", "tremoloRateHz", "modelEnabled",
    "transientSense", "sustainSense", "autoGain"
  )) {
    Require (-not [regex]::IsMatch($process, "(?:value|boolValue|choiceValue) \(Params::ID::$retired\)")) "Retired parameter drives processBlock: $retired"
  }
  Require-Markers ($processor + [Environment]::NewLine + $processorHeader) @(
    "const float detectorSensitivity = 0.16f + pressure * 0.84f",
    "dryGain.setTargetValue",
    "wetGain.setTargetValue",
    "dry * dryG + wet * wetG",
    "restoreStateWithMigration",
    "wetDc2X1",
    "12.0f / static_cast<float> (currentSampleRate)"
  ) "processor"
  Require (-not $process.Contains("autoTrim")) "Hidden Auto Level trim remains"

  $compressor = Get-Content "Source\DSP\Compressor.cpp" -Raw
  $compressorHeader = Get-Content "Source\DSP\Compressor.h" -Raw
  Require-Markers ($compressor + [Environment]::NewLine + $compressorHeader) @(
    "computeGainDb (cinDb, crushThreshold, 16.0f, 12.0f)",
    "crushMixTarget <= 1.0e-7f",
    "crushMixSmoother.setCurrentAndTargetValue (0.0f)",
    "const float parallelSend = crushMix * (0.30f + 0.42f * crushMix)",
    "0.55f * parallelSend * parallelSend",
    "main + crushed * parallelSend",
    "parallelGainReductionDb"
  ) "parallel compressor"
  Require (-not $compressor.Contains("crushed - main")) "Replacement crossfade remains in Parallel Compression"

  $dspTest = Get-Content "Tests\DspValidation.cpp" -Raw
  Require-Markers $dspTest @(
    "zero_null_max_abs",
    "0.0f, 0.25f, 0.60f, 1.0f",
    "peak_dbfs,rms_dbfs,crest_db,normal_gr_db,parallel_gr_db",
    "maxNull <= 1.0e-7f",
    "progressiveDensity",
    "decisiveMaximum"
  ) "DSP validation"
  $processorTest = Get-Content "Tests\ProcessorValidation.cpp" -Raw
  Require-Markers $processorTest @(
    "runtime_parameter_count,87",
    "legacy_mix_migration,pass",
    "protected_user_preset_delete,pass",
    "retired_hidden_parameter_null_max_abs",
    'runScenario ("low_transient"',
    'runScenario ("normal_transient_automation"',
    'runScenario ("hot_transient"',
    'runScenario ("normal_sustained"'
  ) "processor validation"
  $visualTest = Get-Content "Tests\VisualValidation.cpp" -Raw
  Require ([regex]::Matches($visualTest, 'render \("0[1-9]-').Count -eq 9) "Expected nine exact-size rendered visual states"
  Require ([regex]::Matches($visualTest, 'render \("\d\d-').Count -eq 11) "Expected nine exact-size states plus two display-scaling probes"
  Require-Markers $visualTest @("01-vintage.png", "02-night.png", "03-preset-browser.png", "04-user-selected-night.png", "07-value-edit.png", "08-context-menu.png", "09-delete-confirmation.png", "10-vintage-125-percent.png", "11-night-150-percent.png", "1050, 700") "visual validation"

  @(
    "Static v0.6.0 source audit passed",
    "87 v0.5.9 parameter IDs and indices preserved exactly",
    "Seven cards and 39 unique visible knobs",
    "Mockup card order and proportional 1050 x 700 geometry",
    "Vintage/Night editor-only persistent theme",
    "Opaque themed preset browser and protected User deletion",
    "Plain-text slider values with theme-aware edit state",
    "Auto Level and all retired hidden DSP contributions removed",
    "Exact-zero additive Parallel Compression plus compiled measurement test",
    "Compiled parameter state preset automation and input-level tests",
    "Two-pole 12 Hz processed-path blocker preserved",
    "Nine deterministic 1050 x 700 visual validation states"
  ) | Set-Content "SOURCE_VALIDATION.txt"
  Get-Content "SOURCE_VALIDATION.txt"
}
finally {
  Pop-Location
}
