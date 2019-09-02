/* Declare vars */
float input_length_cv;
float input_param_0_cv;
float input_voct;
float input_ext_clk;
float input_glitch_gate;
float output_out;
float param_glitch_button;
float param_length_cv_atv;
float param_length_fine;
float param_param_0_coarse;
float param_length_offset;
float param_param_0_cv_atv;
float param_freq_lock;
float param_pitch_coarse;
float param_pitch_fine;
float param_bias_control;
float param_param_0_fine;
float param_glitch_rate;
float param_voct_atv;

/* Read params */

param_glitch_button = params[PARAM_GLITCH_BUTTON].value;
param_length_cv_atv = params[PARAM_LENGTH_CV_ATV].value;
param_length_fine = params[PARAM_LENGTH_FINE].value;
param_param_0_coarse = params[PARAM_PARAM_0_COARSE].value;
param_length_offset = params[PARAM_LENGTH_OFFSET].value;
param_param_0_cv_atv = params[PARAM_PARAM_0_CV_ATV].value;
param_freq_lock = params[PARAM_FREQ_LOCK].value;
param_pitch_coarse = params[PARAM_PITCH_COARSE].value;
param_pitch_fine = params[PARAM_PITCH_FINE].value;
param_bias_control = params[PARAM_BIAS_CONTROL].value;
param_param_0_fine = params[PARAM_PARAM_0_FINE].value;
param_glitch_rate = params[PARAM_GLITCH_RATE].value;
param_voct_atv = params[PARAM_VOCT_ATV].value;

/* Read inputs */

input_length_cv = inputs[INPUT_LENGTH_CV].value;
if (!inputs[INPUT_LENGTH_CV].active) 
{
    input_length_cv = 0;
}
input_length_cv *= param_length_cv_atv;
input_length_cv = clamp(input_length_cv, -10.0f, 10.0f);
input_param_0_cv = inputs[INPUT_PARAM_0_CV].value;
if (!inputs[INPUT_PARAM_0_CV].active) 
{
    input_param_0_cv = 0;
}
input_param_0_cv *= param_param_0_cv_atv;
input_param_0_cv = clamp(input_param_0_cv, -10.0f, 10.0f);
input_voct = inputs[INPUT_VOCT].value;
if (!inputs[INPUT_VOCT].active) 
{
    input_voct = 0;
}
input_voct *= param_voct_atv;
input_voct = clamp(input_voct, -10.0f, 10.0f);
input_ext_clk = inputs[INPUT_EXT_CLK].value;
input_ext_clk = clamp(input_ext_clk, -10.0f, 10.0f);
input_glitch_gate = inputs[INPUT_GLITCH_GATE].value;
output_out = outputs[OUTPUT_OUT].value;