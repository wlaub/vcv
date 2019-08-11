/* Declare vars */
float input_q_in[4];
float input_i_in[4];
float input_freq[4];
float input_mod_in[4];
float input_aux_i;
float input_aux_q;
float output_mod_out[4];
float output_q_out[4];
float output_i_out[4];
float param_in_gain[4];
float param_freq_offset[4];
float param_freq_atv[4];

/* Read params */

param_in_gain[0] = params[PARAM_IN_GAIN].value;
param_freq_offset[0] = params[PARAM_FREQ_OFFSET].value;
param_freq_atv[0] = params[PARAM_FREQ_ATV].value;
param_freq_offset[1] = params[PARAM_FREQ_OFFSET+1].value;
param_freq_atv[1] = params[PARAM_FREQ_ATV+1].value;
param_freq_offset[2] = params[PARAM_FREQ_OFFSET+2].value;
param_freq_atv[2] = params[PARAM_FREQ_ATV+2].value;
param_freq_offset[3] = params[PARAM_FREQ_OFFSET+3].value;
param_freq_atv[3] = params[PARAM_FREQ_ATV+3].value;
param_in_gain[1] = params[PARAM_IN_GAIN+1].value;
param_in_gain[2] = params[PARAM_IN_GAIN+2].value;
param_in_gain[3] = params[PARAM_IN_GAIN+3].value;

/* Read inputs */

input_q_in[0] = inputs[INPUT_Q_IN].value;
input_q_in[0] = clamp(input_q_in[0], -10.0f, 10.0f);
input_i_in[0] = inputs[INPUT_I_IN].value;
input_i_in[0] = clamp(input_i_in[0], -10.0f, 10.0f);
input_freq[0] = inputs[INPUT_FREQ].value;
if (!inputs[INPUT_FREQ].active) 
{
    input_freq[0] = 10;
}
input_freq[0] *= param_freq_atv[0];
input_freq[0] += 0+1*param_freq_offset[0];
input_freq[0] = clamp(input_freq[0], -10.0f, 10.0f);
input_mod_in[0] = inputs[INPUT_MOD_IN].value;
input_mod_in[0] = clamp(input_mod_in[0], -10.0f, 10.0f);
input_freq[1] = inputs[INPUT_FREQ+1].value;
if (!inputs[INPUT_FREQ+1].active) 
{
    input_freq[1] = 10;
}
input_freq[1] *= param_freq_atv[1];
input_freq[1] += 0+1*param_freq_offset[1];
input_freq[1] = clamp(input_freq[1], -10.0f, 10.0f);
input_mod_in[1] = inputs[INPUT_MOD_IN+1].value;
input_mod_in[1] = clamp(input_mod_in[1], -10.0f, 10.0f);
input_q_in[2] = inputs[INPUT_Q_IN+2].value;
input_q_in[2] = clamp(input_q_in[2], -10.0f, 10.0f);
input_i_in[2] = inputs[INPUT_I_IN+2].value;
input_i_in[2] = clamp(input_i_in[2], -10.0f, 10.0f);
input_freq[2] = inputs[INPUT_FREQ+2].value;
if (!inputs[INPUT_FREQ+2].active) 
{
    input_freq[2] = 10;
}
input_freq[2] *= param_freq_atv[2];
input_freq[2] += 0+1*param_freq_offset[2];
input_freq[2] = clamp(input_freq[2], -10.0f, 10.0f);
input_mod_in[2] = inputs[INPUT_MOD_IN+2].value;
input_mod_in[2] = clamp(input_mod_in[2], -10.0f, 10.0f);
input_freq[3] = inputs[INPUT_FREQ+3].value;
if (!inputs[INPUT_FREQ+3].active) 
{
    input_freq[3] = 10;
}
input_freq[3] *= param_freq_atv[3];
input_freq[3] += 0+1*param_freq_offset[3];
input_freq[3] = clamp(input_freq[3], -10.0f, 10.0f);
input_mod_in[3] = inputs[INPUT_MOD_IN+3].value;
input_mod_in[3] = clamp(input_mod_in[3], -10.0f, 10.0f);
input_aux_i = inputs[INPUT_AUX_I].value;
input_aux_i = clamp(input_aux_i, -10.0f, 10.0f);
input_aux_q = inputs[INPUT_AUX_Q].value;
input_aux_q = clamp(input_aux_q, -10.0f, 10.0f);
input_q_in[3] = inputs[INPUT_Q_IN+3].value;
input_q_in[3] = clamp(input_q_in[3], -10.0f, 10.0f);
input_i_in[3] = inputs[INPUT_I_IN+3].value;
input_i_in[3] = clamp(input_i_in[3], -10.0f, 10.0f);
input_q_in[1] = inputs[INPUT_Q_IN+1].value;
input_q_in[1] = clamp(input_q_in[1], -10.0f, 10.0f);
input_i_in[1] = inputs[INPUT_I_IN+1].value;
input_i_in[1] = clamp(input_i_in[1], -10.0f, 10.0f);
output_mod_out[0] = outputs[OUTPUT_MOD_OUT].value;
output_mod_out[1] = outputs[OUTPUT_MOD_OUT+1].value;
output_mod_out[2] = outputs[OUTPUT_MOD_OUT+2].value;
output_mod_out[3] = outputs[OUTPUT_MOD_OUT+3].value;
output_q_out[0] = outputs[OUTPUT_Q_OUT].value;
output_i_out[0] = outputs[OUTPUT_I_OUT].value;
output_q_out[2] = outputs[OUTPUT_Q_OUT+2].value;
output_i_out[2] = outputs[OUTPUT_I_OUT+2].value;
output_q_out[3] = outputs[OUTPUT_Q_OUT+3].value;
output_i_out[3] = outputs[OUTPUT_I_OUT+3].value;
output_q_out[1] = outputs[OUTPUT_Q_OUT+1].value;
output_i_out[1] = outputs[OUTPUT_I_OUT+1].value;