# Broken Machine

Current Windows VST3 development release: **v0.6.1**.

The verified source is reconstructed from the versioned payloads in `patches/` by the matching GitHub Actions workflow. v0.6.1 preserves all 87 v0.5.9 host parameter indices exactly.

Use `.github/workflows/build-v061.yml` to build, test, visually validate, and package the Windows x86-64 VST3. The workflow verifies the payload, parameter order, retired-DSP isolation, state migration, parallel-compression measurements/null, five reactive factory presets, ten 1050 × 800 interface states, PE architecture, metadata, and final install archive.

Research and the evidence-to-implementation audit are in [`docs/RESEARCH_AND_IMPLEMENTATION_v0.6.0.md`](docs/RESEARCH_AND_IMPLEMENTATION_v0.6.0.md).

The v0.6.1 interface, preset-browser, and five-sound factory-bank changes are documented in [`docs/IMPLEMENTATION_v0.6.1.md`](docs/IMPLEMENTATION_v0.6.1.md).
