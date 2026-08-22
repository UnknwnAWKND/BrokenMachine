# Broken Machine

Current Windows VST3 release: **v0.5.9**.

The verified source is reconstructed from the versioned payloads in `patches/` by the matching GitHub Actions workflow. The v0.5.9 source overlay preserves all 85 v0.5.8 host parameter indices and appends independent Dry and Wet parameters.

Use `.github/workflows/build-v059.yml` to build, validate, and package the Windows x86-64 VST3. The workflow verifies the payload, parameter order, state migration, UI/DSP invariants, PE architecture, plugin metadata, and final install archive before publishing the artifact.
