
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

Karplus Strong Toolkit (noise -> env -> delay -> filter)

### Pleiades (WIP) (Sequence format unstable)

Tree Sequencer

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

## WIP Modules

### Achilles

Karplus Strong Toolkit (noise -> env -> delay -> filter)

### Pleiades

Tree Sequencer

### Mneme

Delays?

### Athena

???

## Future Modules

### Odysseus II

More channels, more outputs

### Polyphemus II

More modular, better curves, (v/oct lpf)

### Output Delay Module

v/oct relative to 120 bpm, clock input, stereo, external feedback access
maybe part of Mneme? Maybe part of Athena?


