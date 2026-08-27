# BROKEN MACHINE v0.6.1 implementation

v0.6.1 is a focused interface and factory-bank release built on the verified
v0.6.0 DSP architecture. It does not add Echo or change the 87 public host
parameter IDs/indices.

## Interface

- The opening size is 1050 x 800. Width remains unchanged.
- Every rotary control uses the same 86 x 91 component bounds as the original
  Input, Compression, and Tone controls.
- Card title rails are inset horizontally so the four corner screws sit only
  on the card plate.
- Input and Output no longer display redundant on/off lamps.
- Help starts disabled and is centered more deliberately in the header.
- Safe Out is in the Output title rail.
- Link L/R uses the same header-switch treatment as Help and Safe Out and sits
  to the right of Phantom.
- Thin, inset separators divide every knob row.
- Input and Output have identical card widths and centered control columns.
- Drive and Reverb share the remaining lower-row width equally.
- Input and Output meters include 0, -12, -24, -36, and -60 dB markings.

## Presets

- The header preset rail is wider and centered over Input Type, Signal Order,
  and Theme.
- Its save action is a floppy-disk icon beside the previous/next buttons.
- Opening PRESETS initially shows only two folders: Factory and User.
- Opening one folder replaces the folder view; both lists can never be open at
  once.
- Each folder has a Back control.
- Clicking outside the browser closes it.
- Delete User Preset appears only at the bottom of the User folder. It retains
  the protected exact-word confirmation step and never appears in Factory or
  the header.

## Five factory sounds

The former fifteen-preset bank is replaced by five original presets. They are
not song, artist, or hardware replicas. They apply the v0.6.0 research
principles: a stable direct center, speaker identity, chord-safe waveform
modeling, controlled low frequencies, additive density, and a distorted,
filtered, gated, and ducked wet field.

1. **Living Wire** — the finished default: speaker-colored, lightly doubled,
   centered, and increasingly textured under a harder pick.
2. **Pressure Break** — compact and restrained at a light touch, then strongly
   driven and sagging when hit.
3. **Glass Machine** — a chord-safe modeled texture around a readable direct
   attack.
4. **Driven Bloom** — a stable direct core surrounded by driven, gated, and
   ducked ambience.
5. **Low Phantom** — protected baritone lows, a large speaker voice, centered
   widening, doubling, and ghost movement.

All five intentionally use high Pick Sensitivity values. Their creative effect
amounts define the available transformation while the existing dynamic
detector keeps quiet playing cleaner and hard playing denser, dirtier, and more
spatial. They are deliberately varied rather than five combinations with every
effect enabled.

## Compatibility and validation

- All 87 v0.5.9 host parameter IDs and indices remain unchanged.
- Retired hidden parameters remain loadable for session compatibility and
  remain DSP-inert.
- User preset storage and v0.6.0 state migration are preserved.
- The Windows workflow compiles the VST3 plus DSP, processor, and visual
  validators.
- Processor validation checks the exact five-name bank and verifies that every
  factory preset produces a measurable quiet-to-hard response difference.
- Visual validation creates ten unique 1050 x 800 states plus 125% and 150%
  scale probes, and asserts uniform knob and balanced-card geometry.
