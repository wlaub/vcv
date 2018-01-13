
# TechTech Technologies VCV Plugins

Mostly relating to reconfigurable LFSRs

### Prometheus

3x up-to 16-bit LFSRs using Galois implementation

* Depth knob/input: Bit depth from 1 to 16
* Taps inputs: Tap configuration as 0 ~ 10V CV, quantized to the selected bit depth
  * tap1 and tap2 are xor'd together if present
* Gate input: Ticks the LFSR on a rising edge
* Lights: displays the state of the LFSR register
* D output: The internal register value scaled to 0 ~ 10V
* A output: The 0th bit of the register scaled to -5 ~ 5V

### DAC

Utility module for generating digital values

* Depth knob/output: Selects bit depth from 1 to 16 and produces it as a 0 ~ 10V CV output
* Bit grid:
  * Switch sets bit value
  * Input sets bit value, overriding switch value
  * Green light shows current bit value
  * Blue light shows whether bit is in the select bit depth
* A output: Selected value scaled to 0 ~ 10V
* D output: Selected value unscaled (0 ~ 2^depth -1)

Route the depth output to the Prometheus depth input, and the A output to a taps input.



