/* Declare vars */
float input_length_cv;
float input_param_1_cv;
float input_param_0_cv;
float input_voct;
float input_ext_clk;
float input_glitch_gate;
float input_freq_lock_gate;
float output_out;
float param_param_1_cv_atv;
float param_glitch_button;
float param_length_cv_atv;
float param_param_1_coarse;
float param_length_fine;
float param_param_order;
float param_param_0_coarse;
float param_length_offset;
float param_param_0_cv_atv;
float param_freq_lock;
float param_pitch_coarse;
float param_pitch_fine;
float param_bias_control;

/* Read params */

param_param_1_cv_atv = params[PARAM_PARAM_1_CV_ATV].value;
param_glitch_button = params[PARAM_GLITCH_BUTTON].value;
param_length_cv_atv = params[PARAM_LENGTH_CV_ATV].value;
param_param_1_coarse = params[PARAM_PARAM_1_COARSE].value;
param_length_fine = params[PARAM_LENGTH_FINE].value;
param_param_order = params[PARAM_PARAM_ORDER].value;
param_param_0_coarse = params[PARAM_PARAM_0_COARSE].value;
param_length_offset = params[PARAM_LENGTH_OFFSET].value;
param_param_0_cv_atv = params[PARAM_PARAM_0_CV_ATV].value;
param_freq_lock = params[PARAM_FREQ_LOCK].value;
param_pitch_coarse = params[PARAM_PITCH_COARSE].value;
param_pitch_fine = params[PARAM_PITCH_FINE].value;
param_bias_control = params[PARAM_BIAS_CONTROL].value;

/* Read inputs */

input_length_cv = inputs[INPUT_LENGTH_CV].value;
if (!inputs[INPUT_LENGTH_CV].active) 
{
    input_length_cv = 0;
}
input_length_cv *= param_length_cv_atv;
input_length_cv = clamp(input_length_cv, -10.0f, 10.0f);
input_param_1_cv = inputs[INPUT_PARAM_1_CV].value;
if (!inputs[INPUT_PARAM_1_CV].active) 
{
    input_param_1_cv = 0;
}
input_param_1_cv *= param_param_1_cv_atv;
input_param_1_cv = clamp(input_param_1_cv, -10.0f, 10.0f);
input_param_0_cv = inputs[INPUT_PARAM_0_CV].value;
if (!inputs[INPUT_PARAM_0_CV].active) 
{
    input_param_0_cv = 0;
}
input_param_0_cv *= param_param_0_cv_atv;
input_param_0_cv = clamp(input_param_0_cv, -10.0f, 10.0f);
input_voct = inputs[INPUT_VOCT].value;
input_voct = clamp(input_voct, -10.0f, 10.0f);
input_ext_clk = inputs[INPUT_EXT_CLK].value;
input_ext_clk = clamp(input_ext_clk, -10.0f, 10.0f);
input_glitch_gate = inputs[INPUT_GLITCH_GATE].value;
input_freq_lock_gate = inputs[INPUT_FREQ_LOCK_GATE].value;
input_freq_lock_gate = clamp(input_freq_lock_gate, -10.0f, 10.0f);
output_out = outputs[OUTPUT_OUT].value;