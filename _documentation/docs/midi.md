# **MIDI Implementation**

## MIDI Input

### MIDI Rx Messages

| Message Type | Description |
| --- | --- |
| Clock | Receives 24 PPQ MIDI clock to sync with Sequencer and other clock-dependent systems |
| Transport Start | Starts the Sequencer <br>(same function as pressing the **_Play switch_** when Sequencer is OFF) |
| Transport Stop | Stops the Sequencer <br>(same function as pressing the **_Play switch_** when Sequencer is ON) |
| Program Change | Loads the selected Preset* <br>(limited to number of Presets on Iosefka) |
| CC (Continuous Controller) | Changes the specified parameter on the device* <br>(See [MIDI CC (Continuous Controller) chart](#midi-cc-continuous-controller-chart))
| Pitch Bend | Adjusts the oscillator pitch according to the amount of bend applied* |

*_Iosefka must be set to the same MIDI Channel as the transmitting device for these messages to be received_

### MIDI CC (Continuous Controller) chart

| **CC Number** | **Parameter**                           |
| ------------- | --------------------------------------- |
| 1             | Mod Wheel                               |
| 2             | Breath Control                          |
| 10            | Pitch (Oscillator 1)                    |
| 11            | Octave (Oscillator 1)                   |
| 12            | X Wave (Oscillator 1)                   |
| 13            | Y Wave (Oscillator 1)                   |
| 14            | Pitch (Oscillator 2)                    |
| 15            | Octave (Oscillator 2)                   |
| 16            | X Wave (Oscillator 2)                   |
| 17            | Y Wave (Oscillator 2)                   |
| 5             | Portamento                              |
| 18            | Phase Modulation                        |
| 19            | Oscillator Sync                         |
| 20            | Mix 1 (Oscillator 1 Mixer Level)        |
| 21            | Mix 2 (Oscillator 2 Mixer Level)        |
| 22            | Mix Noise (Noise Generator Mixer Level) |
| 74            | Filter Cutoff                           |
| 71            | Filter Resonance                        |
| 72            | Filter Type                             |
| 73            | Filter FM                               |
| 76            | Filter Tracking                         |
| 75            | Filter Envelope Level                   |
| 25            | Attack (Envelope 1)                     |
| 26            | Decay (Envelope 1)                      |
| 27            | Sustain (Envelope 1)                    |
| 28            | Release (Envelope 1)                    |
| 29            | Modulation Level (Envelope 1)           |
| 30            | Attack (Envelope 2)                     |
| 31            | Decay (Envelope 2)                      |
| 32            | Sustain (Envelope 2)                    |
| 33            | Release (Envelope 2)                    |
| 34            | Modulation Level (Envelope 2)           |
| 35            | Attack (Envelope 3)                     |
| 36            | Decay (Envelope 3)                      |
| 37            | Sustain (Envelope 3)                    |
| 38            | Release (Envelope 3)                    |
| 39            | Modulation Level (Envelope 3)           |
| 40            | Rate (LFO 1)                            |
| 41            | Shape (LFO 1)                           |
| 42            | Phase (LFO 1)                           |
| 43            | Reset/Sync (LFO 1)                      |
| 44            | Modulation Level (LFO 1)                |
| 45            | Rate (LFO 2)                            |
| 46            | Shape (LFO 2)                           |
| 47            | Phase (LFO 2)                           |
| 48            | Reset/Sync (LFO 2)                      |
| 49            | Modulation Level (LFO 2)                |
| 50            | Rate (LFO 3)                            |
| 51            | Shape (LFO 3)                           |
| 52            | Phase (LFO 3)                           |
| 53            | Reset/Sync (LFO 3)                      |
| 54            | Modulation Level (LFO 3)                |
| 55            | Modulation Destination (Envelope 1)     |
| 56            | Modulation Destination (Envelope 2)     |
| 57            | Modulation Destination (Envelope 3)     |
| 58            | Modulation Destination (LFO 1)          |
| 59            | Modulation Destination (LFO 2)          |
| 60            | Modulation Destination (LFO 3)          |
| 61            | Delay Rate                              |
| 62            | Delay Feedback                          |
| 63            | Delay Level                             |
| 64            | Delay Type                              |
| 65            | Delay Input Level                       |
| 85            | Bitrate Reduction                       |
| 86            | Samplerate Reduction                    |
| 87            | VCA level (Drone mode on above 1)       |
| 88            | Loop (Envelope 1)                       |
| 89            | Loop (Envelope 2)                       |
| 90            | Loop (Envelope 3)                       |
