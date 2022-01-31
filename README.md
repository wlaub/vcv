
# VCV 2.0 Port

* vcvmon needs updating to deal with new compressed patch files
  * patch repo sure is gonna get ugly

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

### Prometheus 

#### I

3x up-to 16-bit LFSRs using Galois implementation

* Taps inputs: Tap configuration as 0 ~ 10V CV, quantized to the selected bit depth
  * taps1 and taps2 are xor'd together if present
* Gate input: Ticks the LFSR on a rising edge
* Lights: displays the state of the LFSR register
* D output: The internal register value scaled to 0 ~ 10V
* A output: The 0th bit of the register scaled to -5 ~ 5V

### Achilles (WIP) (II?)

Karplus Strong Toolkit

### Pleiades (WIP) (Sequence format unstable)

Tree Sequencer - this module is in active development. Please don't count on it being stable. At the time of this writing, the latest version in develop is nto compatible with the version in master.

### Mneme (WIP)

Delays?

### Athena (NOT STARTED)

???


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

## WIP Modules

### Tiamat I

#### Tia I

Multiplexing Crossfader

7 inputs (or 0 V) can by routed to the top or bottom of 7 independent crossfaders. Crossfader position is controlled by CV, and the crossfader outputs are provided as a single polyphonic output from the module. Changes the to a crossfader's inputs only occur when the input is not contributing to the output.

The top inputs to each crossfader are selected when the crossfader CV is at its maximum (5 V).

The bottom inputs to each crossfader are selected when the crossfader CV is at its minimum (0 or -5 V).

Intended for use with Mat I.

TODO: Menu options to configure 
    crossfade range 
        0-5 V, +/-5 V, 0-10 V? +/-10 V?
    source selection transition mode
        Immediate
        what do you call it for the default mode?

TODO: It would be nice to be able to CV control the routing, but idk how

#### Mat I

TODO: Menu spec file location
TODO: Clean up helper functions to generate from scipy sos

Multichannel Fixed Biquad Filter Bank

Each channel of a polyphonic input is processed through a different cascaded biquad filter. The filters coefficients are loaded from a json file. Inputs that don't have a corresponding filter loaded will be passed unfiltered. Filters that don't have a corresponding input will process the last input channel.

A polyphonic output provides the outputs of each individual filter, while a monophonic output provides a mix of all of the filter outputs without scaling.

The left column of LEDs indicates the active input channels, and the right column of LEDs indicates the loaded filter channels.

The red LED at the bottom of the module lights when it cannot find a set of filters matching the current sample rate or when the filters are otherwise not valid.

The default filter configuration includes a low-pass filter on channel 1, band-pass filters on channels 2-6, and a high-pass filter on channel 7. Intended for use with Tia I.

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

See `res/mati_default.json` for the default filter specification.

### Achilles

Karplus Strong Toolkit (noise -> env -> delay -> filter)

### Pleiades

Unfinished Tree Sequencer

Deleted because the ui was a disaster, and it was not worth the effort to port to v2

### Mneme

Delays?

### Athena

???

## Future Modules

### Odysseus II

More channels, more outputs

### Output Delay Module

v/oct relative to 120 bpm, clock input, stereo, external feedback access
maybe part of Mneme? Maybe part of Athena?


