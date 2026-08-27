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
  Require $cmake.Contains("VERSION 0.6.1") "CMake version is not 0.6.1"
  Require $cmake.Contains('COMPANY_NAME "UnknwnAWKND"') "Vendor metadata changed"
  Require-Markers $cmake @(
    "BrokenMachineDspValidation",
    "BrokenMachineProcessorValidation",
    "BrokenMachineVisualValidation"
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
    "constexpr int designWidth = 1050",
    "constexpr int designHeight = 800",
    "constexpr int uniformKnobWidth = 86",
    "constexpr int uniformKnobHeight = 91",
    "setResizeLimits (designWidth, designHeight, 1700, 1250)",
    "setSize (designWidth, designHeight)",
    'statusLabel.setText ("v0.6.1  /  PRESETS"',
    "hasUniformKnobSizesForTesting",
    "hasBalancedInputOutputLayoutForTesting",
    "hasCleanOverlayForTesting",
    "getModuleTitleRailBounds",
    "juce::roundToInt (20.0f * scale)",
    "for (int row = 1; row < rowCount; ++row)",
    "const int wOutput = wInput",
    "const int wDrive = remaining / 2",
    "const int wReverb = remaining - wDrive",
    "getModuleKnobArea (inputModule).getCentreX() == input.getCentreX()",
    "getModuleKnobArea (outputModule).getCentreX() == output.getCentreX()",
    'helpButton.setToggleState (false, juce::dontSendNotification)',
    "helpMode = false",
    'driftLinkButton.getProperties().set ("bmHeaderSwitch", true)',
    "module == phantomModule",
    "driftLinkButton.setBounds",
    "module == outputModule",
    "safeButton.setBounds",
    'savePresetButton.getProperties().set ("bmSaveIcon", true)',
    'getWithDefault ("bmSaveIcon", false)',
    "enum class PresetBrowserPage { Folders, Factory, User }",
    "showPresetBrowserPage (PresetBrowserPage::Folders)",
    "showPresetBrowserPage (PresetBrowserPage::Factory)",
    "showPresetBrowserPage (PresetBrowserPage::User)",
    'PresetTreeItem ("FACTORY", PresetTreeItem::Kind::Folder',
    'PresetTreeItem ("USER", PresetTreeItem::Kind::Folder',
    'presetBackButton { "< BACK" }',
    'deleteUserPresetButton { "DELETE USER PRESET" }',
    "presetBrowserPage == PresetBrowserPage::User",
    "presetBrowserPage != PresetBrowserPage::Folders",
    "void BrokenMachineAudioProcessorEditor::mouseDown",
    "getPresetBrowserBounds().contains (local)",
    "closePresetBrowser()",
    'meterLabel (inputMeterLabel, "IN dB")',
    'meterLabel (outputMeterLabel, "OUT dB")',
    "slider.updateText()",
    "drawLabel (juce::Graphics& g, juce::Label& label)",
    "presetBrowserTree.setOpaque (true)",
    'enum class Theme { Vintage, Night }'
  ) "v0.6.1 UI"

  Require (-not $allUi.Contains("setSize (1050, 700)")) "Obsolete 1050 x 700 opening remains"
  Require (-not $allUi.Contains("contextMenuDeleteButton")) "Floating preset-delete button remains"
  Require (-not $allUi.Contains("openPresetContextMenu")) "Old context-menu deletion state remains"
  Require (-not $allUi.Contains("autoGainButton")) "Auto Level UI member remains"
  Require (-not $allUi.Contains('"AUTO LEVEL"')) "Auto Level label remains"
  Require (-not $allUi.Contains("presetBrowserTree.setOpaque (false)")) "Preset browser is transparent"
  Require (-not $editor.Contains("safeButton.setBounds (r (")) "Safe Out still has a global header position"
  Require $laf.Contains("theme == Theme::Vintage") "Theme palette is not centralized"
  Require $laf.Contains("juce::Colour (0xff2c2d2e)") "Night does not use neutral dark-gray plates"
  Require-Markers $meter @(
    "drawDbScale",
    "for (const int db : { 0, -12, -24, -36, -60 })",
    "vintage ? vintageGreen : mint"
  ) "dB-scaled meter"

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

  $paramsHeader = Get-Content "Source\Parameters\Parameters.h" -Raw
  $params = Get-Content "Source\Parameters\Parameters.cpp" -Raw
  $ids = @([regex]::Matches($paramsHeader, 'inline constexpr auto\s+(\w+)\s*=') |
    ForEach-Object { $_.Groups[1].Value })
  Require ($ids.Count -eq 87 -and ($ids | Sort-Object -Unique).Count -eq 87) "Expected 87 unique declared IDs"
  $layoutIds = @([regex]::Matches($params, '(?:fparam|cparam) \(ID::(\w+)|ParameterID \{ ID::(\w+), 1 \}') |
    ForEach-Object { if ($_.Groups[1].Success) { $_.Groups[1].Value } else { $_.Groups[2].Value } })
  $expectedV059 = "inputDb,diCharacter,inputProfile,topology,compress,punch,crush,threshold,ratio,attackMs,releaseMs,makeupDb,kneeDb,transientSense,sustainSense,drive,bias,texture,sag,portaTrim,portaChannel,portaMaster,portaBassDb,portaTrebleDb,lowTight,highBreakup,wow,flutter,drift,damage,doubleMix,detuneCents,offsetMs,spread,driftLink,lowCutHz,highCutHz,body,bodyFreqHz,bite,biteFreqHz,resonance,formant,speaker,size,space,slapMs,feedback,width,darkness,smear,reverbDrive,reverbGate,reverbDuck,wetCompress,wetTone,wetBias,ghost,ghostSizeMs,modelMix,modelColor,modelMotion,feedbackRes,feedbackFreqHz,feedbackHold,react,broken,brightness,bloom,weird,mix,outputDb,autoGain,safe,compressionEnabled,driveEnabled,reverbEnabled,pitchEnabled,toneEnabled,highProtect,reverbHighCutHz,tremoloDepth,tremoloRateHz,modelEnabled,reverbLowCutHz,dryDb,wetDb"
  Require ($layoutIds.Count -eq 87 -and ($layoutIds -join ",") -eq $expectedV059) "The v0.5.9 host parameter order changed"

  $processor = Get-Content "Source\PluginProcessor.cpp" -Raw
  $processorHeader = Get-Content "Source\PluginProcessor.h" -Raw
  Require-Markers $processor @(
    '"Living Wire", "Pressure Break", "Glass Machine", "Driven Bloom", "Low Phantom"',
    "case 0: // Living Wire",
    "case 1: // Pressure Break",
    "case 2: // Glass Machine",
    "case 3: // Driven Bloom",
    "case 4: // Low Phantom",
    "setParam (Params::ID::react, 0.97f)",
    "setParam (Params::ID::reverbDrive, 0.78f)",
    "setParam (Params::ID::doubleMix, 0.38f)",
    "setParam (Params::ID::speaker, 0.73f)"
  ) "five reactive factory presets"
  Require ([regex]::Matches($processor, 'case [0-4]: // (?:Living Wire|Pressure Break|Glass Machine|Driven Bloom|Low Phantom)').Count -eq 5) "Factory preset switch is not exactly five cases"

  $processMatch = [regex]::Match($processor, '(?s)void BrokenMachineAudioProcessor::processBlock.*?(?=void BrokenMachineAudioProcessor::getStateInformation)')
  Require $processMatch.Success "Could not isolate processBlock"
  $process = $processMatch.Value
  foreach ($retired in @(
    "diCharacter", "compress", "punch", "kneeDb", "mix", "bias", "portaTrim", "portaChannel", "portaMaster",
    "portaBassDb", "portaTrebleDb", "brightness", "bloom", "weird", "wow", "flutter", "drift", "damage",
    "detuneCents", "offsetMs", "bodyFreqHz", "biteFreqHz", "resonance", "formant", "feedbackRes",
    "feedbackFreqHz", "feedbackHold", "darkness", "smear", "wetCompress", "wetTone", "wetBias",
    "highBreakup", "tremoloDepth", "tremoloRateHz", "modelEnabled", "transientSense", "sustainSense", "autoGain"
  )) {
    Require (-not [regex]::IsMatch($process, "(?:value|boolValue|choiceValue) \(Params::ID::$retired\)")) "Retired parameter drives processBlock: $retired"
  }
  Require-Markers ($processor + [Environment]::NewLine + $processorHeader) @(
    "const float detectorSensitivity = 0.16f + pressure * 0.84f",
    "dry * dryG + wet * wetG",
    "restoreStateWithMigration",
    "wetDc2X1"
  ) "processor compatibility"

  $compressor = Get-Content "Source\DSP\Compressor.cpp" -Raw
  Require-Markers $compressor @(
    "crushMixTarget <= 1.0e-7f",
    "main + crushed * parallelSend",
    "parallelGainReductionDb"
  ) "parallel compressor"

  $processorTest = Get-Content "Tests\ProcessorValidation.cpp" -Raw
  Require-Markers $processorTest @(
    "runtime_parameter_count,87",
    "factory_preset_count,5",
    "factory_quiet_to_hard_response,pass",
    "expectedFactory",
    "quietGainDb",
    "hardGainDb",
    "protected_user_preset_delete,pass",
    "retired_hidden_parameter_null_max_abs"
  ) "processor validation"
  $visualTest = Get-Content "Tests\VisualValidation.cpp" -Raw
  Require ([regex]::Matches($visualTest, 'render \("(?:0[1-9]|10)-').Count -eq 10) "Expected ten exact-size visual states"
  Require ([regex]::Matches($visualTest, 'render \("\d\d-').Count -eq 12) "Expected ten exact-size states plus two display-scaling probes"
  Require-Markers $visualTest @(
    "01-vintage.png", "02-night.png", "03-preset-folders.png", "04-factory-folder.png",
    "05-user-folder-night.png", "09-user-delete.png", "10-delete-confirmation.png",
    "11-vintage-125-percent.png", "12-night-150-percent.png", "1050, 800",
    "hasUniformKnobSizesForTesting", "hasBalancedInputOutputLayoutForTesting",
    "hasCleanOverlayForTesting", "Help must be off by default",
    "An underlying child component overlaps an active preset or modal panel"
  ) "visual validation"

  @(
    "Static v0.6.1 source audit passed",
    "87 v0.5.9 parameter IDs and indices preserved exactly",
    "Seven cards and 39 unique visible knobs",
    "Exact uniform knob bounds at the 1050 x 800 opening size",
    "Screw-safe title rails and subtle per-row dividers",
    "Input and Output equal-width centered geometry with dB scales",
    "Drive and Reverb equal-width geometry",
    "Safe Out in Output and header-style Link L/R in Phantom",
    "Help disabled by default",
    "Exclusive Factory/User folder navigation with Back and click-away dismissal",
    "Disk save icon and User-folder-only deletion control",
    "Five original reactive factory presets",
    "Vintage/Night themes and v0.6.0 DSP compatibility preserved",
    "Ten deterministic 1050 x 800 visual validation states"
  ) | Set-Content "SOURCE_VALIDATION.txt"
  Get-Content "SOURCE_VALIDATION.txt"
}
finally {
  Pop-Location
}
