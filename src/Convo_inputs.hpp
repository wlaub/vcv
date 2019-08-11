/* Declare vars */
float input_kernel_in[2];
float input_clock_in[2];
float input_signal_in[2];
float output_conv_out[2];
float param_freq_offset[2];
float param_kernel_gain[2];
float param_window_length[2];
float param_window_rollof[2];
float param_runst[2];
float param_signal_in_atv[2];

/* Read params */

param_freq_offset[0] = params[PARAM_FREQ_OFFSET].value;
param_kernel_gain[0] = params[PARAM_KERNEL_GAIN].value;
param_window_length[0] = params[PARAM_WINDOW_LENGTH].value;
param_window_rollof[0] = params[PARAM_WINDOW_ROLLOF].value;
param_runst[0] = params[PARAM_RUNST].value;
param_signal_in_atv[0] = params[PARAM_SIGNAL_IN_ATV].value;
param_freq_offset[1] = params[PARAM_FREQ_OFFSET+1].value;
param_kernel_gain[1] = params[PARAM_KERNEL_GAIN+1].value;
param_window_length[1] = params[PARAM_WINDOW_LENGTH+1].value;
param_window_rollof[1] = params[PARAM_WINDOW_ROLLOF+1].value;
param_runst[1] = params[PARAM_RUNST+1].value;
param_signal_in_atv[1] = params[PARAM_SIGNAL_IN_ATV+1].value;

/* Read inputs */

input_kernel_in[0] = inputs[INPUT_KERNEL_IN].value;
if (!inputs[INPUT_KERNEL_IN].active) 
{
    input_kernel_in[0] = 10;
}
input_kernel_in[0] = clamp(input_kernel_in[0], -10.0f, 10.0f);
input_clock_in[0] = inputs[INPUT_CLOCK_IN].value;
input_clock_in[0] = clamp(input_clock_in[0], -10.0f, 10.0f);
input_signal_in[0] = inputs[INPUT_SIGNAL_IN].value;
input_signal_in[0] *= param_signal_in_atv[0];
input_signal_in[0] = clamp(input_signal_in[0], -10.0f, 10.0f);
input_kernel_in[1] = inputs[INPUT_KERNEL_IN+1].value;
if (!inputs[INPUT_KERNEL_IN+1].active) 
{
    input_kernel_in[1] = 10;
}
input_kernel_in[1] = clamp(input_kernel_in[1], -10.0f, 10.0f);
input_clock_in[1] = inputs[INPUT_CLOCK_IN+1].value;
input_clock_in[1] = clamp(input_clock_in[1], -10.0f, 10.0f);
input_signal_in[1] = inputs[INPUT_SIGNAL_IN+1].value;
input_signal_in[1] *= param_signal_in_atv[1];
input_signal_in[1] = clamp(input_signal_in[1], -10.0f, 10.0f);
output_conv_out[0] = outputs[OUTPUT_CONV_OUT].value;
output_conv_out[1] = outputs[OUTPUT_CONV_OUT+1].value;