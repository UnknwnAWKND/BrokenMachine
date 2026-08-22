# Broken Machine

Current Windows VST3 development release: **v0.6.0**.

The verified source is reconstructed from the versioned payloads in `patches/` by the matching GitHub Actions workflow. v0.6.0 preserves all 87 v0.5.9 host parameter indices exactly.

Use `.github/workflows/build-v060.yml` to build, test, visually validate, and package the Windows x86-64 VST3. The workflow verifies the payload, parameter order, retired-DSP isolation, state migration, parallel-compression measurements/null, nine 1050 × 700 interface states, PE architecture, metadata, and final install archive.

Research and the evidence-to-implementation audit are in [`docs/RESEARCH_AND_IMPLEMENTATION_v0.6.0.md`](docs/RESEARCH_AND_IMPLEMENTATION_v0.6.0.md).
