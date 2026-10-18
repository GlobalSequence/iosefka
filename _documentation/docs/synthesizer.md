# **The Synthesizer**

## Digital Audio

Iosefka's sound generation begins in the Microcontroller, where the oscillators and noise generator are mixed together and then sent out of the DAC for analog processing.

### Oscillators

Iosefka has two identical morphing wavetable oscillators with 256 waveforms, spread out over a 16x16 grid, and can be controlled using the X Wave and Y Wave Parameters for each oscillator separately. A list of the wavetables can be viewed in the [Wavetable Chart](./supplemental.md#wavetable-chart) section of this document.

There are additional sound design options such as:

- Pitch modulation options:
  - Frequency modulation
  - Portamento (glide)
- Cross modulation options from Osc 1 to Osc 2:
  - Phase modulation
  - Oscillator sync

Each oscillator has additional controls for scale, tuning, and octave, allowing for a very wide range of sounds when used with or without the sound design options listed above.

**Parameters (for each oscillator):**

<div class="joplin-table-wrapper"><table><thead><tr><th><p><strong>Parameter name</strong></p></th><th><p><strong>Function</strong></p></th></tr><tr><th><p>Scale/Pitch</p></th><th><ul><li>Scale (normal operation): Tunes the oscillator by -1 octave to ~+1 octave by semitones</li><li>Pitch (holding shift and turning <strong>Potentiometer</strong>): Tunes the oscillator by -1 octave to ~+1 octave smoothly</li></ul><p>This is a <a href="./operation.md#bipolar-values">Bipolar value</a></p></th></tr><tr><th><p>Octave</p></th><th><p>Choosing one of four octaves</p></th></tr><tr><th><p>X Wave</p></th><th><p>Select a wavetable along the X axis</p></th></tr><tr><th><p>Y Wave</p></th><th><p>Select a wavetable along the Y axis</p></th></tr></thead></table></div>

**Parameters (shared by both oscillators):**

| **Parameter name**           | **Function**                                                                                      |
| ---------------------------- | ------------------------------------------------------------------------------------------------- |
| Portamento                   | Also known as 'glide' - the amount of time it takes for one note to slide to the next when played |
| Phase Mod (Phase Modulation) | Phase modulation where Osc 1 is modulator and Osc 2 is carrier                                    |
| Sync                         | Oscillator sync where Osc 2 resets whenever Osc 1 completes a cycle                               |
**Parameters (shared by both oscillators and noise generator)**

| Parameter name              | Function |
| --------------------------- | -------- |
| SRR (Sample-rate Reduction) |          |
| BRR (Bit-rate Reduction)    |          |
### Noise Generator

Along with the oscillators, there is a white noise generator.

**Parameters:**

none

### Mixer

Both oscillators and noise generator are fed into the mixer, which controls the level of each of the three, before leaving the digital domain for the filter.

**Parameters:**

| **Parameter name** | **Function** |
| --- | --- |
| Mix 1 | Level of Oscillator 1 |
| Mix 2 | Level of Oscillator 2 |
| Mix Noise | Level of Noise Generator |

### Digital Delay

The post-VCA output of Iosefka feeds back into a 12-bit ADC, sampled at 48khz, which is then used as a digital delay. The digital delay can be given extremely short (~3ms) for interesting sound sculpting, or much longer (~683ms) for a more traditional delay effect. The Stutter delay effect allows for (very brief) musical accompaniment, or (very brief) mock polyphony effects. 

The digital delay's audio is produced by a separate DAC and is then mixed back into the VCA output.

**Parameters:**

| **Parameter name**       | **Function**                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            |
| ------------------------ | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| D. Level (Delay Level)   | Volume of the delay                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     |
| D. Rate (Delay Rate)     | Amount of time between delay input and output.                                                                                                                                                                                                                                                                                                                                                                                                                                                                          |
| D. Fdbk (Delay Feedback) | - For Unclocked, Clocked, and Short delay  - Amount of delay fed back into itself<br>- For Stutter delay - Selects the amount of repeats before refilling the buffer and beginning again. At maximum, the effect repeats infinitely                                                                                                                                                                                                                                                                                     |
| D. Type (Delay Type)     | Select between the following:<br><br>- 00 - Unclocked delay between ~3ms to ~683ms<br>- 01 - Clocked delay - Multiple divisions of sequencer or MIDI clock. Note that the clocked delay cannot exceed the delay buffer length of ~683ms. Clocked intervals above this will be shorter, and not clock accurate.<br>- 02 - Short delay - ~3ms to ~95ms. See the [Fun with Short Delays](#fun-with-short-delays) section below for things you can do with this type  <br>- 03 - Stutter delay - Repeats a portion of audio |
| D. Input (Delay Input)   | - For Unclocked, Clocked, and Short delay  - Level of the delay input<br>- For Stutter delay - Disables the effect at 0, Enables the effect anywhere above 0                                                                                                                                                                                                                                                                                                                                                            |

#### Fun with Short Delays
You can build the following delay based effects using the short delay and different modulation types:

- **Flanger** - Set a very short delay time with high feedback and LFO on Delay Rate
- **Chorus** - Set a medium delay time with lower feedback and an LFO on the delay rate (set the LFO depth a bit lower)
- **Comb Filter/Karplus Strong-like effects** - Set the Delay Rate to C0, and assign the Note Modulation Source to Delay Rate. This will, by design, track the notes on the keyboard and play musically, but due to limitations in the delay feedback and the rubber banding effect of changing delay rates, it's not a 1:1 recreation of these effects.

## Analog Audio

### Filter

The heart of the analog filter is the Sound Semiconductor V2140 chip. It is an OTA based pole mixing filter producing a very clean and controlled sound. The filter cutoff has its own Parameter for key tracking, which the filter is capable of doing easily across 5+ octaves.

The filter is configurable between four types:

- 2-pole low pass
- 4-pole low pass
- 2-pole high pass
- 2-pole band pass

The resonance of this filter has been tweaked to get just-very-loud-enough to where one should find themselves find themselves satisfied in case they want to go a little overboard.

The filter output is then fed back into the filter cutoff CV path for audio-rate frequency modulation with a Parameter to control the intensity of the modulation applied. In moderate amounts, this adds an almost overdrive or distortion to the sound. In high amounts, and at high resonance, this becomes absolute audio chaos, if you're into that sort of thing. When the Parameter is turned all the way down, the frequency modulation is not applied and therefore not audible.

Envelope 2 is hard-wired to the filter cutoff, and there is a Parameter dedicated to bipolar envelope amount.

**Parameters:**

<div class="joplin-table-wrapper"><table><thead><tr><th><p><strong>Parameter name</strong></p></th><th><p><strong>Function</strong></p></th></tr><tr><th><p>Cutoff</p></th><th><p>The point at which frequencies begin to become attenuated</p></th></tr><tr><th><p>Resonance</p></th><th><p>The amount of emphasis given to the audio at the cutoff point</p></th></tr><tr><th><p>F. Env. Level (Filter Envelope Level)</p></th><th><p>How much of Envelope 2 is applied to the filter cutoff amount. This is a <a href="./operation.md#bipolar-values">Bipolar value</a></p><p>Note: Holding shift and turning the <strong><em>Potentiometer</em></strong> will snap this Parameter to 0x80. This is a quick way to turn the modulation 'off'</p></th></tr><tr><th><p>F. Type (Filter Type)</p></th><th><p>Selecting between the four filter types:</p><ul><li>2 Pole Low Pass</li><li>4 Pole Low Pass</li><li>2 Pole High Pass</li><li>2 Pole Band Pass</li></ul></th></tr><tr><th><p>F. FM (Filter Frequency Modulation)</p></th><th><p>The amount of audio rate modulation fed from the filter output back into the filter cutoff</p></th></tr><tr><th><p>F. Track (Filter Tracking)</p></th><th><p>The amount of note tracking applied to the filter. When turned up all the way, the filter can be played just like an oscillator.</p></th></tr></thead></table></div>

### Amplifier

The voltage controlled amplifier is based around the venerable LM13700 dual OTA chip. The amplifier provides a linear response with a 'vintage' sound.

It can be configured to a 'drone' mode where the amplifier is opened partially (allowing for amplitude modulation effects), or all the way, providing the full volume of the filter output.

**Parameters:**

| **Parameter name** | **Function** |
| --- | --- |
| VCA | Controls how much the filter output is attenuated. |

### External Input

External audio can be fed into the external input in the back of the synthesizer, allowing for line level signals to be processed by both the analog audio block (filter and VCA) and also the digital delay.

**Note**: The external audio will only be heard when the VCA is open, so to hear the external audio, the synthesizer must be actively playing notes, or the VCA must be in drone mode. To that end, to use Iosefka purely as an analog/digital processor, here are some suggestions:

- Put the VCA in [drone mode](./operation.md#just-let-it-drone-my-dude)
- Turn down Mix 1, Mix 2, and Mix noise
- Now you should be hearing only the external input playing, allowing you to process audio with the filter, amplitude modulation, and the delay effects

**Note**: The gain of the external input is fixed, so the audio level fed into the input must be adjusted externally.

**Parameters (none)**

## Modulation Sources

All synthesizer Parameters are modulatable using the modulation sources on Iosefka (and then some!). **Sequencer Layer** Parameters cannot be modulated via the mod sources, but the LFOs are syncable to the internal and external clock, so there's that!

Here is a list of the [Modulation Destinations](./supplemental.md#modulation-destinations).

### Envelopes

Three ADSR envelopes with responses from snappy during short durations to long arcing response times when turned up.

All three envelopes are freely assignable to any modulation destination. Additionally, Envelope 2 is hard-wired to the Filter Cutoff, and Envelope 3 is hard-wired to the VCA.

As an example, if Envelope 2's destination is set to Mix Noise, it shares the Attack, Decay, Sustain, and Release time of the filter envelope, but the envelope amount for Mix Noise is independent of the envelope amount for Filter Cutoff. The same applies for Envelope 3.

**Parameters:**

(These Parameters apply to all 3 Envelopes)

| **Parameter name** | **Function**                                                                                                                                                                                                                                                                                        |
| ------------------ | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Attack | Envelope onset time                                                                                                                                                                                                                                                                                 |
| Decay ( + Loop) | The amount of time from the end of the attack phase to settle into the sustain phase<br>Holding **_Shift switch_** and turning the **_Potentiometer_** will enable the Looping Envelope, causing the envelope to begin the Attack phase as soon as it finishes the Release phase.                   |
| Sustain | The volume of the note as it is held (following the attack and decay phase)                                                                                                                                                                                                                         |
| Release | The amount of time until the volume fades to 0 after a Note-Off event happens                                                                                                                                                                                                                       |
| \- Level + | Bipolar Parameter level control allowing for non-inverted or inverted LFO modulation. This is a [Bipolar value](./operation.md#bipolar-values)<br>Holding **_Shift switch_** and turning the **_Potentiometer_** will snap this Parameter to 0x80. This is a quick way to turn the modulation 'off' |

### LFOs (Low Frequency Oscillators)

Three LFOs with a variety of waveforms. The LFOs can all independently be synced to the sequencer clock, as well as reset during Note-On messages.

The LFO modulation itself is bipolar, meaning that the value of the Modulation Destination will swing above and below the Parameter value at an equal depth. For example, let's say the Portamento Parameter was turned up to "80" on the **_OLED Display_** and you applied the LFO to Portamento, turning the "- Level +" up to "C0". This means that the LFO is ranging from "80 - 40" to "80 + 40".

**Parameters:**

(These Parameters apply to all 3 LFOs)

| **Parameter name** | **Function**                                                                                                                                                                                                                                                                                 |
| ------------------ | -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Rate               | The speed of the LFO's oscillation. When the LFO is synced to the Sequencer Clock, this Parameter becomes a time-division of the clock                                                                                                                                                       |
| Shape              | The shape of the LFO wave: triangle, saw, square, sine, and sample-hold                                                                                                                                                                                                                      |
| Phase              | The offset of the wave, control where the wave begins and ends                                                                                                                                                                                                                               |
| Reset/Sync         | This Parameter controls whether LFO Reset and/or Sync are turned on. When Reset is on, the LFO restarts whenever a Note-On event happens                                                                                                                                                     |
| \- Level +         | Bipolar Parameter level control allowing for non-inverted or inverted LFO modulation. This is a [Bipolar value](./operation.md#bipolar-values)<br>Note: Holding shift and turning the **_Potentiometer_** will snap this Parameter to 0x80. This is a quick way to turn the modulation 'off' |

### Selecting Envelope and LFO modulation destinations

When in the **Envelope View** or **LFO View**, pressing the respective **_View switch_** repeatedly will cycle through the **Mode** for that mod source's destination.

For example, in the **Envelope View**, pressing the **Envelope _Key switch_** will highlight the number on the left hand side of the **OLED Display**, indicating which Envelope number's modulation destination is selectable.

You can then use the **_Data Rotary Encoder_** to select the [Modulation Destination](./supplemental.md#modulation-destinations).
#### Velocity

Velocity is a control of how much of a Parameter is applied depending on how 'hard' a note was hit. This effect emulates the way that a piano key is louder the harder it is pressed. Most modern keyboards have a velocity control. The sequencer also has a velocity Parameter.

Velocity controls two Parameters: The VCA, and one freely selectable Modulation Destination.

**Parameters:**

None

#### Mod Wheel

Along with Velocity controls, most modern MIDI controllers have a Mod Wheel. Mod wheel is only controllable via MIDI.

**Parameters:**

None

#### Aftertouch

Aftertouch allows you to control a Modulation Destination with the pressure applied to the key after it has been pressed. Consult your favorite MIDI controller's manual to learn more!

**Parameters:**

None

#### Note

The amount of modulation is set depending on which note is being played. The higher the note, the greater the modulation. This can be reversed with the "note mod" setting in the Settings menu.

**Parameters:**

- Note + or - (in the settings menu) controls the bipolar behavior of the Note Modulation Source. This is set in the Settings Menu:
  - Note +: controls the Modulation Destination positively, higher notes is a **higher** parameter value and vice versa.
  - Note -: controls the Modulation Destination negatively, higher notes is a **lower** parameter value and vice versa.

#### Breath Control

For those with MIDI wind controllers, breath control is a Modulation Source. As of the writing of this manual, I've never seen anyone use one in person, but I hope that changes some day!

#### Pitch Bend

Pitch Bend is a MIDI controllable effect which applies exclusively to the pitch of Oscillator 1 and Oscillator 2. You can adjust the Pitch Bend Depth from the **Settings View** with a range of 1 to 12 semitones.

**Parameters:**

None