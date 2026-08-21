# Broken Machine v0.2

An original Windows VST3 guitar processor built around touch-sensitive direct-preamp overload, distorted/gated reverb, early-digital movement, low-tuned clarity and expressive dynamics.

It is inspired by the broader experimental guitar-production vocabulary associated with Mk.gee, but it is **not affiliated with, endorsed by, or an exact clone of Mk.gee or any proprietary hardware**.

## FL Studio — simplest install

1. Download the latest `BrokenMachine-v0.2-Windows-VST3` build artifact.
2. Extract it.
3. Copy the entire `Broken Machine.vst3` folder to:
   `C:\Program Files\Common Files\VST3\`
4. In FL Studio open **Options → Manage plugins**.
5. Click **Find installed plugins**.
6. Search for **Broken Machine** and load it on a Mixer insert.

No JUCE, Visual Studio or CMake installation is required when using the prebuilt artifact.

## Main controls

- **INPUT** — hit the processor harder/softer.
- **PRESSURE** — how strongly your playing dynamics push compression, preamp and ambience behavior.
- **BLOOM** — moves the effect toward a delayed, swelling tail or toward a tighter front-loaded response.
- **BREAKUP** — coordinates 424-style overload, wet-path distortion and texture.
- **WARP** — irregular pitch/formant/modeler-like instability.
- **FOCUS** — coordinated low-end tightening and upper-mid clarity.
- **SPACE** — distorted/gated reverb send amount.
- **OUTPUT** — final level.

Use **DEEP EDIT** for the individual compressor, 424 Trim/Channel/EQ, distorted reverb, modulation, tone and speaker controls.

## Why v0.2 is different

v0.1 was a generic serial chain with a short-delay “space” effect. v0.2 rebuilds the architecture around a parallel dry + distorted/gated reverb branch feeding a two-stage, oversampled 424-inspired preamp. It also uses a less flattening linked compressor, subtler random movement, a completely rebuilt preset library and a new eight-macro UI.

See [RESEARCH.md](RESEARCH.md) for the research evidence, confidence ratings, design decisions and limitations.

## Build from source

Requirements: Visual Studio 2022 with Desktop C++ workload and CMake 3.22+.

JUCE 9.0.1 is pinned and fetched automatically by CMake.

```bat
BUILD_WINDOWS.bat
```

Or:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --target BrokenMachine_VST3 --parallel
```

## Licensing note

JUCE has its own licensing terms. Review the current JUCE license before distributing or selling a closed-source binary.
