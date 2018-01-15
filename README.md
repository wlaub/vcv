
# TechTech Technologies VCV Plugins

Mostly relating to reconfigurable LFSRs

### Depth

These modules deal with digital numbers passes around as 0-10V CV's. The module bit depth is selected by the depth input at the top left. The knob selects bit depth from 1 to 16. The input selects the same range from a 0-10V CV. The output passes through the bit depth selected by either the knob or the input. A bit depth of N will map a CV as follows:
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



