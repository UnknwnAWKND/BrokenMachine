# BROKEN MACHINE v0.6.0 — research, DSP audit, and implementation record

Research and implementation date: 2026-08-22. Baseline audited: `v0.5.9`, commit `1dc3172aba404b5285bfc673df30c892d3063af4`.

## Scope and evidence policy

This work investigates transferable musical and technical behaviours. It does not claim to reproduce a person's private rig, proprietary algorithms, recordings, or performance. Commercial-facing names remain original.

Evidence labels used below:

- **Confirmed — primary:** directly stated by the artist/collaborator, shown in a period-labelled first-party performance or demonstration, or specified by the manufacturer/manual.
- **Strong inference:** multiple clear visual or reputable technical leads agree, but no direct period-specific statement closes every gap.
- **Plausible/unconfirmed:** repeated in community or press coverage without a traceable primary source.
- **Contradicted/unsupported:** conflicts with primary documentation or is too weak to guide DSP.

Periods are kept separate. A [May 2022 Dijon touring-band gear check](https://www.fretboardjournal.com/video/gear-check-the-guitarists-in-dijons-touring-band/) is not treated as the 2024 album chain. Andrew Aged's 2024/25 touring setup is evidence about his own role in the live system, not automatic evidence about Mike Gordon's studio path.

## Findings and confidence audit

| Claim | Evidence | Confidence | Engineering conclusion |
|---|---|---:|---|
| The desired aesthetic values murky, compressed, “underwater” playback and productive limitation. | Gordon describes his formative low-quality listening and preference for making strong work with limited gear in the [2024 Dazed interview](https://www.dazeddigital.com/music/article/64551/1/mk-gee-michael-gordon-interview-autumn-2024-issue-dazed). | Confirmed — primary, as an aesthetic statement | Preserve controlled bandwidth, density, nonlinear staging, and imperfection as optional behaviours; do not indiscriminately degrade resolution. |
| Playing and production are inseparable: guitar can occupy percussive, vocal, and noise roles. | The same [Dazed interview](https://www.dazeddigital.com/music/article/64551/1/mk-gee-michael-gordon-interview-autumn-2024-issue-dazed) describes guitar behaving like a snare, drum, or second singer and emphasizes unusual arrangements/production choices. | Confirmed — primary | Prioritize pick-sensitive transitions, preserved attack, chord definition, and controllable decay instead of a static “tone match.” |
| A VG-8-family system can model guitar/pickup/amp/cab/mic objects, restructure harmonics, and pitch each string without ordinary note tracking. | [Roland's VG-8 product documentation](https://www.roland.com/uk/products/vg-8/) specifies VGM, HRM, a GK-2A input, real-time waveform conversion with no tracking delay, polyphonic pitch shift, EQ, delay, chorus, hexa-pan, and reverb. | Confirmed — manufacturer, for the device | BROKEN MACHINE's modeled layer should remain waveform-derived and zero-latency. A monophonic pitch tracker would be the wrong abstraction for complex chords. |
| Exact use of an original VG-8 versus VG-8EX in each Mk.gee context. | Units are visually and journalistically attributed, but no direct period-specific artist statement located names the exact revision. Roland documents both as closely related systems. | Strong inference for VG-8 family; exact revision unconfirmed | Do not depend on an EX-only feature or claim a circuit/algorithm clone. The current resonant modeled layer captures transferable behaviour only. |
| A GK-2A is a divided pickup with one sensing yoke per string, requiring placement and sensitivity calibration. | [Roland's official GK-2A owner's manual](http://lib.roland.co.jp/support/en/manuals/res/1811189/GK-2A_e4.pdf) describes the divided pickup, six yokes, installation near the bridge, and per-system sensitivity adjustment. | Confirmed — manufacturer, for the device | A normal mono/stereo plugin input cannot reconstruct six isolated strings. Input Type and Pick Sensitivity are honest whole-signal calibrations, not fake hexaphonic processing. |
| Andrew Aged used a GK-2A-equipped modified Charvel, blended three signals, covered synth/pad/acoustic/rock roles, and ran all-DI during the cited tour. | Direct collaborator interview in [Guitar World](https://www.guitarworld.com/artists/guitarists/andrew-aged-mkgee-interview). | Confirmed — primary, Andrew's rig | Multi-path blending and DI-compatible bandwidth are useful architectural evidence. It does not prove Gordon used the same guitar, pedals, or routing. |
| Rainger FX associates Mk.gee with Reverb-X and Echo-X. | [Rainger FX's artist page](https://www.raingerfx.com/artists/mkgee) lists both products. | Confirmed — manufacturer attribution; period/settings unspecified | Reverb-X behaviour can inform ambience interaction, but no exact knob settings or signal position are assumed. |
| Reverb-X always retains the original signal; distortion affects only reverb; its gate can move after reverb; its pressure controller diverts signal into a wet send with trails. | [Official Reverb-X product page](https://www.raingerfx.com/shop/p/reverb-x) and [official manual](https://static1.squarespace.com/static/636e500201d1fa72da31bfd4/t/637f32dffe0a2325cc77c9da/1669280480630/Reverb-X+Manual.pdf). | Confirmed — manufacturer | Preserve a clean direct path plus independently filtered/driven/gated wet return. Existing Reverb Drive, Gate, Duck, Delay, Decay, Width, Low Cut, and High Cut already govern this coherently. |
| The original TASCAM 424 Mk I accepts roughly -50 to -10 dBV at its mic/line inputs; Trim controls preamplification; mixer EQ shelves are 100 Hz and 10 kHz; mixer and cassette paths have materially different bandwidth/distortion. | Scanned original TEAC/TASCAM manual mirrored by [Tetrakan](https://tetrakansupermonobloc.home.blog/2019/08/13/tascam-424-service-manual-and-schematics/): input/Trim description, ±10 dB shelves, mixer response about 20 Hz–30 kHz, cassette response about 40 Hz–16 kHz at high speed/12.5 kHz normal, dbx Type II, and separate THD specifications. | Confirmed — original manual content; mirror provenance noted | “424 tone” is not one tape-saturation block. A driven input/mixer path, optional spectral protection, cassette transport, and dbx are different systems. Keep staged oversampled preamp behaviour; do not reintroduce wow, hiss, dbx pumping, or hidden tape coloration without evidence that the tape path is required. |
| Gordon definitely used the original 424 Mk I, through a particular channel/master configuration, on every recording/live setup. | Repeated visual/press/community attribution exists, but no direct artist/engineer source located here confirms model revision, full routing, tape engagement, or settings for each context. | Plausible to strong visual inference; exact configuration unconfirmed | Do not brand an exact 424 emulation or hard-code its EQ/noise. Existing Preamp Drive, Distortion, Grain, Sag, Drive Low Cut, and Drive High Cut expose the perceptually useful behaviours honestly. |
| Gordon's low-register setup has a single confirmed scale length, string gauge, pickup combination, tuning, capo, and pick across periods. | The [Dazed profile](https://www.dazeddigital.com/music/article/64551/1/mk-gee-michael-gordon-interview-autumn-2024-issue-dazed) identifies a Fender Jaguar reissue in the observed show, but the full setup changes and the requested details were not all confirmed by primary sources. | Jaguar in that context confirmed; detailed “baritone recipe” unconfirmed | Keep a Baritone input calibration and low-band protection, but do not encode one tuning/gauge/pick as fact. |
| Pandora literally delivered 12-bit audio, so a 12-bit reducer is necessary. | The Dazed narrative reports Gordon's recollection/description, but no service codec documentation here confirms literal 12-bit delivery. | Unsupported as a technical specification | No bitcrusher was added. “Underwater” is treated as an aesthetic clue, not a codec design requirement. |
| VG-8 HRM is conventional monophonic guitar-synth note tracking. | Roland explicitly describes transforming each string's actual waveform in real time without tracking delay. | Contradicted by manufacturer | No pitch tracker, MIDI conversion, or note-quantized synth was added. |

## Device systems, not slogans

### Roland VG-8 / GK-2A system

The divided pickup supplies separately sensed string waveforms to a processor that can model instrument components, restructure harmonics, and perform per-string pitch operations. The most transferable properties are immediate response, polyphonic chord preservation, a blendable modeled identity, and the ability to move between recognizable guitar and synthetic timbre. It does **not** justify pretending a normal stereo VST input is hexaphonic.

BROKEN MACHINE therefore retains its low-latency waveform-derived resonant Modeler. `Synth Layer` remains a blend, `Synth Color` moves filtering/resonances, and `Synth Motion` adds slow decorrelation. This is an original simplified behaviour, not COSM/VGM/HRM code or a named model.

### TASCAM 424 Mk I system

The manual separates at least four relevant behaviours:

1. Input sensitivity and preamplifier gain (`TRIM`).
2. Channel level/pan and the main mixer buses.
3. Broad 100 Hz/10 kHz shelving EQ.
4. The cassette recorder, whose speed, bandwidth, dbx noise reduction, distortion, and track/crosstalk behaviour differ from the mixer path.

That distinction matters. Driving a mic/line preamp is not the same as recording to cassette, and either can be used without the other. BROKEN MACHINE's two-stage oversampled Preamp Drive, harder post-preamp Distortion path, Grain, signal-dependent Sag, and pre-nonlinearity low/high protection cover the safe, audible interaction. Retired Input Trim, Bias, Wow, Instability, and hidden 424 controls remain registered only for old sessions and do not process audio.

### Rainger Reverb-X system

The manufacturer's topology is unusually relevant: direct audio remains, wet audio can be distorted, the gate can act after the reverb/distortion, and a pressure controller varies the send while trails continue. BROKEN MACHINE already has the correct high-level architecture: the Spatial Processor builds a wet-only predelay/reverb buffer, filters it, compresses/drives it with oversampling around the nonlinear return, applies gate/duck gain, and adds it to the stage input. `Pick Sensitivity` now supplies the only user-visible dynamic calibration.

## Representative tone taxonomy

These observations describe audible behaviours, not hidden gear claims. First-party performance links are supplied with representative entry points; exact balances vary through each performance.

| Behaviour | Representative performance | Transient/dynamics | Spectrum/stereo/decay | Equipment contribution vs performance |
|---|---|---|---|---|
| Compressed direct guitar | [“Are You Looking Up” live, opening/first verse](https://www.youtube.com/watch?v=z0pzzkp85-Q&t=18s) | Pick remains clear above a dense body; modest crest range | Mid-forward, centered core with a short surrounding field | Compression/DI can shape density; muting, register, chord voicing, and attack create the readable rhythm. |
| Clean-to-broken transition | [“Candy” live, early verse into stronger accents](https://www.youtube.com/watch?v=wtOu9QY9P64&t=20s) | Harder strokes produce edge rather than only more level | Upper-mid texture grows while low notes remain bounded | Nonlinear gain staging matters, but the transition is initiated by the player's intensity and timing. |
| Hard-edged modeled distortion | [“Dream Police” live, opening and later accents](https://www.youtube.com/watch?v=3bLYswnD8yk&t=12s) | Flattened peaks coexist with sharp attacks | Narrow/metallic resonant energy sits around a centered guitar | A modeled/resonant branch is plausible; voicing, muting, and arrangement prevent it becoming generic fuzz. |
| Blooming/gated reverb | [“Little Bit More” live, sustained spaces](https://www.youtube.com/watch?v=NncelKQ6Hvw&t=20s) | Wet energy rises around/after notes and clears between phrases | Filtered wide decay around a stable direct core | Wet-only drive/gate/duck recreate the interaction; phrase spacing makes the gate/bloom musical. |
| Wide but centered image / short doubling | [“Are You Looking Up” live, chorus](https://www.youtube.com/watch?v=z0pzzkp85-Q&t=92s) | Center attack stays stable | Low-level lateral energy widens without losing mono focus | Fixed micro-delay/detune can supply width; true double tracking and live room/FOH processing remain possible contributors. |
| Near-feedback sustain and noise bridge | [“Dream Police” live, later instrumental section](https://www.youtube.com/watch?v=3bLYswnD8yk&t=145s) | Sustains move from controlled tone toward unstable energy | Resonant upper bands and long spatial tails | Monitoring level, physical coupling, gesture, and arrangement cannot be replaced by an insert plugin. BROKEN MACHINE does not restore a hidden feedback resonator. |
| Baritone/lowered-register definition | [Jimmy Kimmel performance, low-register passages](https://www.youtube.com/watch?v=xrTm_6VpTTo&t=20s) | Firm pick front with compressed body | Deep fundamentals remain separate from distortion; subsonics are absent | Guitar setup/tuning and playing are primary. Input calibration, Drive Low Cut, Tone Low Cut, and the always-on 12 Hz blocker protect the processor. |
| Percussive guitar role | [SNL “ROCKMAN,” opening groove](https://www.youtube.com/watch?v=pULkm-3b_1M&t=17s) | Muted attacks function as rhythmic events | Short, bright, centered hits contrast with louder broken gestures | Pick, palm muting, subdivisions, and arrangement dominate; a compressor must not erase this distinction. |

The plugin cannot substitute for tuning, string tension, pickup placement, pick material, chord vocabulary, muting, timing, physical feedback, room/FOH processing, overdubs, edits, or the decisions of a player and mix engineer.

## v0.5.9 signal-path audit

The source—not labels or old notes—showed this path:

1. Smoothed Input gain and Input Type calibration (profile gain, high-pass, low-pass, low-protection coefficient).
2. Shared envelope, fast transient, and slow sustain detectors.
3. Plugin-level Dry tap after input/profile calibration.
4. Bypass-smoothed normal/parallel compressor.
5. Bypass-smoothed fixed micro-double and reverse-grain Ghost.
6. One of four real topologies using Preamp, Tone, Modeler, and Reverb (including the actual parallel rig).
7. Speaker model within the Tone bypass domain.
8. Two cascaded 12 Hz high-pass/DC-blocking poles on the processed path.
9. Hidden legacy calibration/return values (`DI Character`, transient/sustain sensitivity,
   diffusion, Wet Compress/Tone/Bias) plus Auto Level still affected audio despite no longer
   belonging to the approved one-page control surface.
10. Independent smoothed Dry and Wet dB gains, smoothed Output gain, then Safe Out limiter.

Important v0.5.9 gaps:

- Retired `diCharacter`, `transientSense`, `sustainSense`, `smear`, `wetCompress`,
  `wetTone`, and `wetBias` still calibrated input or Reverb DSP invisibly.
- Retired Auto Level still changed wet level and was visible in the header despite the approved design retiring it.
- Parallel Compression crossfaded **from** the normal branch **to** the aggressive branch. It did not retain the main branch and add density underneath; correlated branches plus the linear replacement law made much of the range less informative.
- The UI card order and proportions did not match the approved mockup, the browser TreeView was transparent, and only a single dark theme existed.

## Evidence-to-implementation table

| Research finding | Source | Confidence | Perceptual importance | v0.5.9 behaviour | Gap | v0.6.0 action / governing control | CPU | Latency | Compatibility risk | Preset migration risk | Verification |
|---|---|---:|---:|---|---|---|---:|---:|---:|---:|---|
| Effects must follow visible controls, not hidden legacy state. | [Dazed](https://www.dazeddigital.com/music/article/64551/1/mk-gee-michael-gordon-interview-autumn-2024-issue-dazed); collaborator emphasis on playing/gesture in [Guitar World](https://www.guitarworld.com/artists/guitarists/andrew-aged-mkgee-interview) | Primary | Very high | Visible `Input Type`/`Pick Sensitivity`/Reverb controls coexisted with seven hidden calibration/return parameters. | The same visible panel could sound different after old state loads. | Remove all hidden reads. `Input Type` alone calibrates input; `Pick Sensitivity` scales attack/sustain response; diffusion derives from visible Reverb/Decay; hidden wet compressor/tone/bias are neutral. | Negligible/slight reduction | None | Low | Old hidden values load but are inert | Static process-path audit; compiled low/normal/hot and sustained signals; max-difference null between all retired parameters at opposite extremes. |
| Dense systems retain a direct/core signal while adding a processed role. | Reverb-X [manual](https://static1.squarespace.com/static/636e500201d1fa72da31bfd4/t/637f32dffe0a2325cc77c9da/1669280480630/Reverb-X+Manual.pdf); Andrew's three-signal blend in [Guitar World](https://www.guitarworld.com/artists/guitarists/andrew-aged-mkgee-interview) | Primary for those systems | Very high | Parallel compressor linearly replaced main with crushed branch. | Control was not truly “main plus density”; upper range could feel like a timbre replacement. | Keep normal branch at every value and progressively add a fast/deep 16:1 branch. Existing `Parallel Comp` ID/range unchanged. | Tiny | None; sample-aligned split | Low | None | Exact zero null; peak/RMS/crest/GR at 0/25/60/100%; automation smoothing. |
| Wet distortion/gating should not dirty direct audio. | Reverb-X [product](https://www.raingerfx.com/shop/p/reverb-x) and [manual](https://static1.squarespace.com/static/636e500201d1fa72da31bfd4/t/637f32dffe0a2325cc77c9da/1669280480630/Reverb-X+Manual.pdf) | Manufacturer | High | Separate wet buffer already filtered, driven, gated/ducked, then added. | No architectural gap. | Preserve; clarify help. Existing Reverb amount/Drive/Gate/Duck/Low Cut/High Cut govern it. | Existing | Existing oversampling has no reported host latency | None | None | Source trace; zero-send transparency; impulse/tail inspection. |
| Waveform modeling is more appropriate than monophonic note tracking for complex chords. | [Roland VG-8](https://www.roland.com/uk/products/vg-8/) | Manufacturer | High | Resonant waveform-derived Modeler, no pitch tracker. | No high-value gap that justified proprietary-style complexity. | Preserve original Modeler; clarify `Synth Layer/Color/Motion` behaviour. | Existing | None | None | None | Chord signal and silence/DC tests. |
| Driven input/mixer and cassette transport are distinct behaviours. | Original 424 manual scan via [Tetrakan](https://tetrakansupermonobloc.home.blog/2019/08/13/tascam-424-service-manual-and-schematics/) | Primary manual content | High | Oversampled staged preamp, hard distortion, grain, sag, low/high protection; retired tape effects inert. | Old presets still set retired 424 fields, but process path no longer read them. | Preserve honest visible Drive controls; keep retired IDs inert; do not add hiss/wow/dbx/bit reduction. | Existing | Oversampler internal; no declared plugin latency | Low | None | Retired-ID process audit; exact zero Drive path. |
| Low-register definition requires nonlinear-band protection, not only a final high-pass. | Performance observations plus Input/GK calibration principles | Inference | High | Baritone profile, Drive Low Cut/High Cut, Tone filters, 12 Hz blocker. | Hidden detector sensitivity could overreact inconsistently. | Preserve all protection; make detector calibration visible-only. | Negligible | None | Low | None | 36 Hz/55 Hz multitone, hot transient, subsonic/DC tests. |
| Automatic gain correction is not an approved behaviour and hides cause/effect. | Product requirements; no research source supports this exact macro | N/A | High for trust | Hidden formula trimmed wet output from multiple controls. | Unexplained gain changes; retired control remained visible. | Remove Auto Level UI and DSP read. Keep its ID/index registered only for old sessions. | Slight reduction | None | Low | Old sessions may play louder where hidden trim formerly acted; this is the intentional retirement | Static absence check; gain sweeps; binary marker audit. |

## Parallel Compression redesign

### Root cause and topology

The v0.5.9 branch did receive the correct compressor input and was sample-aligned/channel-linked. Its aggressive detector used 1.3 ms attack, a shortened release, a low threshold, high ratio, and makeup. The main problem was the rejoin law:

`main + (crushed - main) * amount`

That is a replacement crossfade. At 100%, the normal compressor disappears. Because both branches have identical polarity and latency and share the same source, the low/mid range can also read mostly as a modest correlated level/timbre change rather than an added floor of density.

v0.6.0 uses:

- the unchanged normal compressor (visible Threshold, Ratio, Attack, Release),
- a channel-linked aggressive detector with 1.3 ms attack, release clamped to 35–650 ms, threshold at the lower of -32 dB or 14 dB below the normal threshold, 16:1 ratio, and 12 dB knee,
- progressive branch makeup from 7 to 10 dB,
- a nonlinear send `a * (0.30 + 0.42a)` so the first half remains controllable and the upper half becomes obvious,
- conservative correlated-sum normalization `1 / sqrt(1 + 0.55 * send²)`,
- smoothed branch amount and post Gain,
- a forced exact zero state so `Parallel Comp = 0` is sample-for-sample the normal path.

The combined compressor output continues through Phantom, the chosen Drive/Tone/Modeler/Reverb topology, speaker model, 12 Hz blocker, Dry/Wet, Output, and Safe Out.

The compiled 48 kHz test uses a three-second stereo guitar-like signal with repeated pick transients and sustained harmonics. The Windows artifact retains the raw CSV; the initial compiled validation produced:

| Parallel Comp | Peak dBFS | RMS dBFS | Crest dB | Normal GR dB | Density-branch GR dB |
|---:|---:|---:|---:|---:|---:|
| 0% | -2.733 | -25.364 | 22.631 | 10.172 | 32.044 |
| 25% | -2.729 | -25.152 | 22.422 | 10.172 | 32.044 |
| 60% | -2.724 | -24.753 | 22.029 | 10.172 | 32.044 |
| 100% | -2.718 | -24.373 | 21.656 | 10.172 | 32.044 |

The 0% output nulls against the independently implemented normal-compressor reference at
`0.0000000000` maximum absolute sample difference. From 0% to 100%, RMS rises by 0.991 dB
while peak changes only 0.015 dB, so the measurable change is density/crest reduction rather
than an automatic peak-level boost. CI requires finite results, a zero-path difference no
greater than `1e-7`, progressively increasing RMS at 25/60/100%, lower maximum crest factor,
and substantially deeper density-branch gain reduction.

## v0.6.0 final signal path

1. **Input:** smoothed Input gain → selected Input Type calibration (gain + bandwidth + low-protection profile) → visible-only Pick Sensitivity detectors. Retired `DI Character` state is inert.
2. **Dry tap:** copy the calibrated input for independent Dry level.
3. **Compression:** normal compressor plus optional additive density branch → post-branch Gain → bypass crossfade.
4. **Phantom:** fixed 7-cent/8 ms Micro Double and reverse-grain Ghost, each within the Phantom bypass crossfade; Link L/R governs double drift correlation.
5. **Selected topology:**
   - Preamp → Tone → Modeler → Reverb,
   - Reverb → Preamp → Tone → Modeler,
   - parallel Preamp/Tone plus wet-only Reverb delta → Modeler,
   - Modeler → Reverb → Preamp → Tone.
6. **Tone speaker stage:** speaker blend within Tone bypass.
7. **Safety filtering:** two cascaded 12 Hz processed-path DC/subsonic blocker poles.
8. **Output:** independent smoothed Dry and Wet dB gains → smoothed Output gain → Safe Out limiter.

No Auto Level, Input Trim, DI Character, Bias, Instability, Wow, Tremolo, Formant,
Resonance, hidden reverb-return control, retired feedback resonator, or hidden
transient/sustain parameter contributes audio. Their IDs remain loadable only for host/state compatibility.

## Interface implementation and verification contract

- Exact production opening size: 1050 × 700; the approved FL Studio startup-size reassertion remains.
- Header and cards follow the supplied composition: Input/Compression/Tone/Phantom above Drive/Reverb/Output.
- Link L/R is bottom-center beneath Phantom's 2 × 2 knobs.
- Vintage uses procedural aged-beige plates, dark rails, restrained brass, screws, recessed cards, hardware knobs, green lamps, and segmented meters. No flattened mockup asset is used.
- Night is the same geometry/hitboxes with neutral graphite/dark-gray plates and restrained texture/lighting.
- Theme is stored in a JUCE `PropertiesFile`, defaults safely to Vintage, is not an APVTS parameter, and is not saved/loaded by audio presets.
- The TreeView is explicitly opaque and receives a theme-specific rail color; the browser panel, popup styling, selection, context action, editors, and confirmations are themed.
- Slider readout Labels are custom-drawn as plain text. Their persistent background, border, and outline are transparent; only the active TextEditor receives a focused editing surface.
- Ordinary inactive zeros are blank/subdued. Threshold `0.0 dB` and unity Gain `0.00 dB` stay visible; Dry/Wet minimum displays `OFF`.
- Card bypass dims plate, rail/title treatment, lamp, labels, knobs, and values while leaving the power hitbox usable.

The visual validation executable renders nine deterministic 1050 × 700 PNGs: Vintage, Night, browser, User selection, bypass, inactive zeros, active knob-value editing, the same functional in-editor User context menu used by right-click, and protected delete confirmation. It also renders 125% Vintage and 150% Night raster-scaling probes from the same logical 1050 × 700 editor. CI checks uniqueness, file sizes, and dimensions before packaging.

## Compatibility and remaining uncertainty

- All 87 v0.5.9 parameter IDs retain their exact host indices. No parameter was appended for v0.6.0.
- Retired IDs remain deserializable so old sessions/automation lanes do not break, but they remain inaudible.
- Dry/Wet legacy migration, all seven card bypass states, protected User deletion, Factory protection, Drive/Reverb low-cut controls, and two-pole 12 Hz blocker remain.
- The exact Mk.gee VG revision, exact 424-family revision/routing, complete guitar setup, strings, tunings, picks, DI/converter chain, and period-specific settings remain unconfirmed. They were not encoded as facts.
- Physical acoustic feedback, six-string divided-pickup processing, true multi-amp/FOH routing, overdubs, resampling/editing, and a player's technique cannot be recreated from a normal stereo insert.
- Objective tests establish safety, compatibility, and the intended density mapping; final musical preference still requires a guitarist listening in FL Studio through their own input chain.

## Primary/technical source index

- [Dazed — direct Mk.gee interview, September 2024](https://www.dazeddigital.com/music/article/64551/1/mk-gee-michael-gordon-interview-autumn-2024-issue-dazed)
- [Guitar World — direct Andrew Aged interview, April 2025](https://www.guitarworld.com/artists/guitarists/andrew-aged-mkgee-interview)
- [Fretboard Journal — period-labelled May 2022 Dijon touring-band gear check](https://www.fretboardjournal.com/video/gear-check-the-guitarists-in-dijons-touring-band/)
- [Roland VG-8 official product/support page](https://www.roland.com/uk/products/vg-8/)
- [Roland VG-8 official owner's manual](https://static.roland.com/assets/media/pdf/VG-8_OM.pdf)
- [Roland GK-2A official owner's manual](http://lib.roland.co.jp/support/en/manuals/res/1811189/GK-2A_e4.pdf)
- [Rainger FX official Mk.gee artist page](https://www.raingerfx.com/artists/mkgee)
- [Rainger FX Reverb-X official product page](https://www.raingerfx.com/shop/p/reverb-x)
- [Rainger FX Reverb-X official manual](https://static1.squarespace.com/static/636e500201d1fa72da31bfd4/t/637f32dffe0a2325cc77c9da/1669280480630/Reverb-X+Manual.pdf)
- [Original TASCAM 424 Mk I manual/service-manual scan mirror and provenance](https://tetrakansupermonobloc.home.blog/2019/08/13/tascam-424-service-manual-and-schematics/)
- [Official live “Dream Police” video](https://www.youtube.com/watch?v=3bLYswnD8yk)
- [Official live “Are You Looking Up” video](https://www.youtube.com/watch?v=z0pzzkp85-Q)
- [Official live “Candy” video](https://www.youtube.com/watch?v=wtOu9QY9P64)
- [Official live “Little Bit More” video](https://www.youtube.com/watch?v=NncelKQ6Hvw)
