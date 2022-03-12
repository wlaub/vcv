
# VCV 2.0 Port

* MetadataFiles screenshot function is still a little broken 
  * doesn't render plugs and cable opacity also seems bugged.
  * also doesn't clip panels to their bounds

# TechTech Technologies VCV Plugins

## Green Cathedral

### Odysseus

#### I

Ramp/random walk generator. This module is still in development. Current implementation is based on design values, but will be updated to reflect final hardware behavior after it's finished.

### Polyphemus

#### I

6-pole IIR filter

* 3 pole pairs defined by radius and angle
  * radius input from -1 to 1 with CV input. Clips to (-1,1).
  * angle input from 0 to 3.14 with CV input. Clips to (0,6.28).

### II (WIP)

IIR filter with v/oct and envelope inputs

The filter consists of a single 1-10'th order real pole with radius computed from cutoff frequency. The pole order can be selected using a knob.

The envelope scales the cutoff frequency by a piecewise linear curve with a configurable knee. 

k is the knee value from 0 to 1

wc is the base cutoff frequency from the v/oct and pitch knobs

p is the frequency gain above knee from 0 to 19

The mapping from envelope voltage to cutoff frequency is

| Envelope Voltage | Cutoff Frequency |
|----|---|
| 0 - 10\*k V   | 0 - wc |
| 10\*kV - 10 V | wc - wc\*(1+p) |

#### Controls

The envelope level knob attenuates the envelope voltage or sets the normal voltage of the envelope input.

The envelope bias knob applies a fixed bias offset to the envelope.

#### Polyphony

Each filter input/output supports polyphony up to 16 channels.

When on of the v/oct or envelope inputs is patched with a polyphonic input, the resulting cutoff frequencies are applied sequentially to the filter channels:
| v/oct/env input | filter cutoff source |
|-|-|
|0|0|
|1|1|
|2|2|
|X|2|
|X|2|

#### TODO

* Make knobs have nice units
  * Frequency for base pitch
  * Volts for bias offset and cutoff knee
* Make a version with just one pane and deprecate this one
  * An octave control and hp/lp switch might be nice (instead of some of the redundant filter ports)
  * Make a panel

### Prometheus II

LFSR oscillator presented in a more conventional VCO format. Better documentation pending completion of the hardware implementation.

[Hardware design documentation](https://techtech.technology/en/modular/prometheus-ii/)

### Achilles (WIP) (II?)

Karplus Strong Toolkit

### Mneme (WIP)

Delays?

### Athena (NOT STARTED)

???

### Tiamat I

7-Channel Chromatic Mixer

Consists of two modules that can be used independently. In combination, a single complete audio spectrum is constructed out of different bands selected from multiple input signals.

#### Tia I

Multiplexing Crossfader

Tia configures routing from its 7 signal inputs to the 7 channels of Mat's default filter configuration.

7 inputs (or 0 V) can by routed to the top or bottom of 7 independent crossfaders. Crossfader position is controlled by CV, and the crossfader outputs are provided as a single polyphonic output from the module.

Routing is configured by selecting a source input from the top row of buttons and then assigning it to one side of a crossfader using the bottom two rows of buttons.

In smooth routing change mode:
* The top inputs to each crossfader are selected when the crossfader CV is at its maximum value.
* The bottom inputs to each crossfader are selected when the crossfader CV is at its minimum value.

In immediate routing change mode, assignments are applied immediately on clicking the button.

Clicking a pending assignment a second time will apply the assignment immediately even in smooth mode.

The right click menu provides options for configuring the crossfader CV range (unipolar/bipolar 5 V/10 V) as well as routing transition mode. In smooth mode, the input to a crossfader changes only when it doesn't contribute to the output (i.e. the crossfader is set all the way in the other direction). In immediate mode, it changes as soon as the button is pressed.

The Nth crossfader selects from the Nth channel of its routed inputs, so polyphonic inputs  can be used to provide different sets of signals to each crossfader. For example, different waveform outputs from Cobalt I can be routed to Tia, and the Tia output signals will have the same frequencies as the inputs but can crossfade between different waveforms.

TODO: 
* It would be nice to be able to CV control the routing, but idk how

##### Tia I Expander

Provides additional sets of inputs and outputs controlled by an adjacent Tia I instance.

#### Mat I

Multichannel Fixed Biquad Filter Bank

Each channel of a polyphonic input is processed through a different cascaded biquad filter. The filters coefficients are loaded from a json file. Inputs that don't have a corresponding filter loaded will be passed unfiltered. Filters that don't have a corresponding input will process the last input channel.

A polyphonic output provides the outputs of each individual filter, while a monophonic output provides a mix of all of the filter outputs without scaling.

The left column of LEDs indicates the active input channels, and the right column of LEDs indicates the loaded filter channels.

The red LED at the bottom of the module lights when it cannot find a set of filters matching the current sample rate or when the filters are otherwise not valid.

The default filter configuration includes a low-pass filter on channel 1, band-pass filters on channels 2-6, and a high-pass filter on channel 7. 

The filter specification has the form

```
{
    "filters": [
        {
            "fs" : <sample_rate (sps)>,
            "channels": [
            //Channel 0 filter stages
                [
                    //Filter stage 0 coefficients
                    [
                        b0, b1, b2,
                        a0, a1, a2
                    ],
                    //Filter stage 1 coefficients
                    [
                        b0, b1, b2,
                        a0, a1, a2
                    ],
                ...
                ],
            ...
            ]
        }
    ]
}
```

Each individual filter specification must include a valid non-zero sample rate and an identical number of channels. The module will automatically select the set of filters with the best sample rate match to the current engine sample rate.

See [`res/mati_default.json`](res/mati_default.json) for the default filter specification, which was generated using [`dev-scripts/mat_filters.py`](dev-scripts/mat_filters.py).

[`dev-scripts/tiamat.py`](dev-scripts/tiamat.py) provides some helpers for generating Mat I filter specitications from filters designed using scipy.signal.

### Lachesis I

Counter utility module

Increments count and output CV on clock edges. Clock and reset inputs may be polyphonic. CV minimum, maximum, and increment values may be configured using knobs.
Menu options include
* Clock edge mode (rising, falling, both)
* CV overflow modes (clamp or wrap)
* Simultaneous edge handling (increment once or increment multiple)

## Other Modules

### Ouroboros

5 VCO's modulating each other in a loop

### Sisyphus

4 circular buffers with length and rate control

* Gate input: Controls buffer I/O
  * Low value records input to buffer
  * High value loops through buffer
* Length control: Sets buffer length from 0-10s
* Rate control: Sets buffer playback rate from 0x to 2x
* Mode control: Currently does nothing
* Trig output: generates a pulse when the buffer loops
* Lights: Display buffer information
  * Red: buffer end location
  * Green: buffer start location
  * Blue: buffer playback location

### QMod

4x Quadrature Modulator/Demodulator Pairs

## LFSR Modules

### Depth

These modules deal with digital numbers passes around as 0-10V CV's. The module bit depth is selected by the depth input at the top left. The knob selects bit depth from 1 to 16. The input selects the same range from a 0-10V CV. The output passes through the bit depth selected by either the knob or the input. A bit depth of N will map a 0-10V CV to the entire range of an N-bit number:

0 ~ 10V -> 0 ~ 2^N-1

### Prometheus

3x up-to 16-bit LFSRs using Galois implementation

* Taps inputs: Tap configuration as 0 ~ 10V CV, quantized to the selected bit depth
  * taps1 and taps2 are xor'd together if present
* Gate input: Ticks the LFSR on a rising edge
* Lights: displays the state of the LFSR register
* D output: The internal register value scaled to 0 ~ 10V
* A output: The 0th bit of the register scaled to -5 ~ 5V

### Vulcan

2x consecutive sequence generator with bitwise logic combinations.

* Start input: sets sequence starting value
* Width input: sets sequence length
* Rate input: sets increment size
  * Knob is 0-16 with default value of 1
* Loop/bounce input: selects looping mode when the sequence passes its final value
  * On loop, the sequence wraps back to the start
  * On bounce, the sequence bounces back and forth between the start and end
* Pos output: gives the current sequence position
* Trig output: pulses when the sequence reaches an end
* The middle switch routes axis 1 trig output to axis to clock input
* The top three outputs are bitwise combinations of the two axes

The original intent of this module was to produce a 2-dimensional point defined by the two axes and have it step horizontally and then vertically over a rectangular area. The 3 logic outputs combine the two axes to generate a single non-linear sequence.

### DAC

Utility module for generating digital values

* Bit grid:
  * Switch sets bit value
  * Input sets bit value, overriding switch value
  * Green light shows current bit value
  * Blue light shows whether bit is in the select bit depth
* A output: Selected value scaled to 0 ~ 10V
* D output: Selected value unscaled (0 ~ 2^depth -1)

Route the depth output to the Prometheus depth input, and the A output to a taps input.

### mDAC

Utility module providing 8 digital outputs generated from text boxes. Numbers may be either base 10 or 16.

* 8x outputs: 8x analog outputs that behave the same way as the DAC A output

## Utility Modules

### Latency Compensator

This module measures an unknown delay by sending a pulse and measuring the time before the pulse returns. It provides several delays of the same length for use in aligning internally generated signals with delayed signals. In my application, I use this module to compensate for the latency through an ES-8 (when it's working), using output 8 and input 4 as a dedicated loopback channel for the latency measurement.

### Metadata modules

These modules add descriptive metadata to the patch file for use by other programs. In particular I use these to provide information about recordings generated from VCV without having to leave the window.

The files metadata module, when configured for image files, can also take a screenshot of the entire patch. When used, the screenshot control locks out ofor 10 seconds to prevent accidental screenshot spamming. The program may hang for a couple seconds while taking a screenshot, and it's possible that there are modules that prevent the screenshot function from working correctly.

TODO:
* Metadata main needs a more usable description text field. Maybe something larger.

### Decay Timer

This is a collection of utilities for measuring decay time of an envelope and generating json measurement files.

## WIP Modules

### Once

Button press synchronizer.

Updates gate outputs from button presses only on clock edges. Maybe be rising, falling, or both edges.

When the enable switch isn't activated, the module won't update the outputs.

Button light colors:
* Unlit: Gate low
* Green: Pending high
* White: Gate high
* Blue: Pending low

Button Modes:
* Toggle Gate
  * Toggle the corresponding gate output on the next clock edge.
  * Enable button lights up green when clock low and white when clock high
* Pulse
  * Produce a single pulse on the gate output on the next clock edge.
  * Enable button lights up green and flashes white on clock edges
* Pass Clock
  * On the next edge, start passing the logic state of the clock to the corresponding gate output.
  * Enabled button lights up blue when clock is low and white when clock is high
* Pulse on Clock
  * Procue a pulse on the gate output on every clock edge.
  * Enabled button lights up blue and flashes white on clock edges

TODO:
* Refactor the code so that it doesn't suck
  * I would like to be able to reuse the logic for a multichannel version with different clocks on each button.
  * Or maybe update this version so a polyphonic clock input applies a different clock to each button
  * Right now the code is very, umm, terrible

### Cobalt I

Subharmonic sequence oscillator

Generates waveforms with periods that are consecutive integer multiples of a fundamental period. Controls are provided for

* Combined waveform period in seconds
* Starting multiple from 1 to 7
* Sequence length from 1 to 7
* Waveform starting/intersection phase
* Square wave pulse width
* Waveform scale and offset

The phase control allows control of the intersection point of the generated waveforms.

Consider using it to drive the crossfaders in Tia I.

TODO:
* Align square wave phase to other waveforms more better


#### -60

Expander adds a bunch of additional waveforms
* Asymmetric triangle wave with symmetry control (from rising ramp to triangle to falling ramp)
* Raised cosine wave with rolloff control (from cosine to square wave)
* Slew-limited square wave with slope control (from trangle wave to square wave)
* RC/exponential decay wave with decay rate control
* Exponential converter (for making waveforms peakier)
* 1-X converter (for reversing minimum and maximum values of waveforms)
* Subdivision clock (integer multiple of fastest output frequency)

TODO:
* Make expanders stackable (search through -60 instances to find Cobalt instance)
* Make exponential converter factor configurable

### Achilles

Karplus Strong Toolkit (noise -> env -> delay -> filter)

TODO:
* Add filter stage
  * And also compensate filter group delay in delay stage

### Mneme

Delays?

## Deprecated and Removed Modules

### Pleiades

Unfinished Tree Sequencer

Deleted because the ui was a disaster, and it was not worth the effort to port to v2

## Future Modules

Potential future modules and their desirements.

### Odysseus II

More channels, more outputs. Easier to dial in.

### Athena

???

### Output Delay Module

v/oct relative to 120 bpm, clock input, stereo, external feedback access
maybe part of Mneme? Maybe part of Athena?

### Unwound Timeloop

Open-loop delay-feedback simulated with multiple delays. Allows each "echo" to be processed independently. For example, it might take v/oct and envelope as an input, and the outputs could each drive a different oscillator/attenuation pair.

### QMOD II

A more versatile modulation toolkit with built-in filters.

### ??? Arturo I? Kromagg I? Earth I?

A nice faderbank with sliders that feel natural.

### Here are some numbers

| Time | Feedback | Cutoff |
|-|-|-|
| 1ms * (1E4)^(0.045 + x * 0.249), x = 0 -> 1 | 0.1395 | 7.32875 kHz LP |
| 2.5586 ms | 0.123 | 7.962143 kHz LP |
| 4.2658 ms | 0.0225 | 2 kHz HP |

+3.15 V



