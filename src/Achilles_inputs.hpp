/* Declare vars */
float input_env[4];
float input_match;
float input_voct;
float input_ext_env;
float input_noise_input;
float input_noise_level;
float input_fm;
float input_feedback;
float input_gate;
float input_lp_level;
float input_bp_level;
float input_hp_level;
float input_lp_shape;
float input_lp_bw;
float input_bp_shape;
float input_bp_bw;
float input_hp_shape;
float input_hp_bw;
float input_aux_noise[2];
float input_noise_mix;
float input_noise_shape[2];
float input_noise_bw[2];
float input_noise_slope[2];
float input_delay_clock;
float output_delay_out;
float output_env_gate;
float output_env;
float output_noise[2];
float output_noise_out;
float output_delay_in;
float param_env_atv[4];
float param_env_offset[4];
float param_feedback_atv;
float param_output_level;
float param_voct_offset;
float param_fm_atv;
float param_noise_level_atv;
float param_match_atv;
float param_noise_mix_offset;
float param_noise_mix_width;
float param_noise_curve_slope;
float param_lp_level_atv;
float param_bp_level_atv;
float param_hp_level_atv;
float param_lp_shape_atv;
float param_lp_bw_atv;
float param_lp_shape_offset;
float param_lp_bw_offset;
float param_bp_shape_atv;
float param_bp_bw_atv;
float param_bp_shape_offset;
float param_bp_bw_offset;
float param_hp_shape_atv;
float param_hp_bw_atv;
float param_hp_shape_offset;
float param_hp_bw_offset;
float param_noise_shape_atv[2];
float param_noise_atv[2];
float param_noise_bw_atv[2];
float param_noise_slope_atv[2];
float param_aux_noise_mix[2];

/* Read params */

param_env_atv[0] = params[PARAM_ENV_ATV].value;
param_env_offset[0] = params[PARAM_ENV_OFFSET].value;
param_env_offset[1] = params[PARAM_ENV_OFFSET+1].value;
param_env_atv[1] = params[PARAM_ENV_ATV+1].value;
param_env_atv[2] = params[PARAM_ENV_ATV+2].value;
param_env_offset[2] = params[PARAM_ENV_OFFSET+2].value;
param_env_offset[3] = params[PARAM_ENV_OFFSET+3].value;
param_env_atv[3] = params[PARAM_ENV_ATV+3].value;
param_feedback_atv = params[PARAM_FEEDBACK_ATV].value;
param_output_level = params[PARAM_OUTPUT_LEVEL].value;
param_voct_offset = params[PARAM_VOCT_OFFSET].value;
param_fm_atv = params[PARAM_FM_ATV].value;
param_noise_level_atv = params[PARAM_NOISE_LEVEL_ATV].value;
param_match_atv = params[PARAM_MATCH_ATV].value;
param_noise_mix_offset = params[PARAM_NOISE_MIX_OFFSET].value;
param_noise_mix_width = params[PARAM_NOISE_MIX_WIDTH].value;
param_noise_curve_slope = params[PARAM_NOISE_CURVE_SLOPE].value;
param_lp_level_atv = params[PARAM_LP_LEVEL_ATV].value;
param_bp_level_atv = params[PARAM_BP_LEVEL_ATV].value;
param_hp_level_atv = params[PARAM_HP_LEVEL_ATV].value;
param_lp_shape_atv = params[PARAM_LP_SHAPE_ATV].value;
param_lp_bw_atv = params[PARAM_LP_BW_ATV].value;
param_lp_shape_offset = params[PARAM_LP_SHAPE_OFFSET].value;
param_lp_bw_offset = params[PARAM_LP_BW_OFFSET].value;
param_bp_shape_atv = params[PARAM_BP_SHAPE_ATV].value;
param_bp_bw_atv = params[PARAM_BP_BW_ATV].value;
param_bp_shape_offset = params[PARAM_BP_SHAPE_OFFSET].value;
param_bp_bw_offset = params[PARAM_BP_BW_OFFSET].value;
param_hp_shape_atv = params[PARAM_HP_SHAPE_ATV].value;
param_hp_bw_atv = params[PARAM_HP_BW_ATV].value;
param_hp_shape_offset = params[PARAM_HP_SHAPE_OFFSET].value;
param_hp_bw_offset = params[PARAM_HP_BW_OFFSET].value;
param_noise_shape_atv[0] = params[PARAM_NOISE_SHAPE_ATV].value;
param_noise_atv[1] = params[PARAM_NOISE_ATV+1].value;
param_noise_atv[0] = params[PARAM_NOISE_ATV].value;
param_noise_shape_atv[1] = params[PARAM_NOISE_SHAPE_ATV+1].value;
param_noise_bw_atv[1] = params[PARAM_NOISE_BW_ATV+1].value;
param_noise_bw_atv[0] = params[PARAM_NOISE_BW_ATV].value;
param_noise_slope_atv[0] = params[PARAM_NOISE_SLOPE_ATV].value;
param_aux_noise_mix[1] = params[PARAM_AUX_NOISE_MIX+1].value;
param_aux_noise_mix[0] = params[PARAM_AUX_NOISE_MIX].value;
param_noise_slope_atv[1] = params[PARAM_NOISE_SLOPE_ATV+1].value;

/* Read inputs */

input_env[0] = inputs[INPUT_ENV].value;
if (!inputs[INPUT_ENV].active) 
{
    input_env[0] = 10;
}
input_env[0] *= param_env_atv[0];
input_env[0] += 0+10*param_env_offset[0];
input_env[0] = clamp(input_env[0], -10.0f, 10.0f);
input_env[3] = inputs[INPUT_ENV+3].value;
if (!inputs[INPUT_ENV+3].active) 
{
    input_env[3] = 10;
}
input_env[3] *= param_env_atv[3];
input_env[3] += 0+10*param_env_offset[3];
input_env[3] = clamp(input_env[3], -10.0f, 10.0f);
input_env[1] = inputs[INPUT_ENV+1].value;
if (!inputs[INPUT_ENV+1].active) 
{
    input_env[1] = 10;
}
input_env[1] *= param_env_atv[1];
input_env[1] += 0+10*param_env_offset[1];
input_env[1] = clamp(input_env[1], -10.0f, 10.0f);
input_env[2] = inputs[INPUT_ENV+2].value;
if (!inputs[INPUT_ENV+2].active) 
{
    input_env[2] = 10;
}
input_env[2] *= param_env_atv[2];
input_env[2] += 0+10*param_env_offset[2];
input_env[2] = clamp(input_env[2], 0.0f, 10.0f);
input_match = inputs[INPUT_MATCH].value;
if (!inputs[INPUT_MATCH].active) 
{
    input_match = 10;
}
input_match *= param_match_atv;
input_voct = inputs[INPUT_VOCT].value;
if (!inputs[INPUT_VOCT].active) 
{
    input_voct = 0;
}
input_voct += 0+1*param_voct_offset;
input_ext_env = inputs[INPUT_EXT_ENV].value;
input_noise_input = inputs[INPUT_NOISE_INPUT].value;
input_noise_level = inputs[INPUT_NOISE_LEVEL].value;
if (!inputs[INPUT_NOISE_LEVEL].active) 
{
    input_noise_level = 10;
}
input_noise_level *= param_noise_level_atv;
input_fm = inputs[INPUT_FM].value;
if (!inputs[INPUT_FM].active) 
{
    input_fm = 10;
}
input_fm *= param_fm_atv;
input_fm = max(input_fm, 0.0f);
input_feedback = inputs[INPUT_FEEDBACK].value;
if (!inputs[INPUT_FEEDBACK].active) 
{
    input_feedback = 10;
}
input_feedback *= param_feedback_atv;
input_feedback = clamp(input_feedback, -10.0f, 10.0f);
input_gate = inputs[INPUT_GATE].value;
input_lp_level = inputs[INPUT_LP_LEVEL].value;
if (!inputs[INPUT_LP_LEVEL].active) 
{
    input_lp_level = 10;
}
input_lp_level *= param_lp_level_atv;
input_lp_level = clamp(input_lp_level, -10.0f, 10.0f);
input_bp_level = inputs[INPUT_BP_LEVEL].value;
if (!inputs[INPUT_BP_LEVEL].active) 
{
    input_bp_level = 10;
}
input_bp_level *= param_bp_level_atv;
input_bp_level = clamp(input_bp_level, -10.0f, 10.0f);
input_hp_level = inputs[INPUT_HP_LEVEL].value;
if (!inputs[INPUT_HP_LEVEL].active) 
{
    input_hp_level = 10;
}
input_hp_level *= param_hp_level_atv;
input_hp_level = clamp(input_hp_level, -10.0f, 10.0f);
input_lp_shape = inputs[INPUT_LP_SHAPE].value;
if (!inputs[INPUT_LP_SHAPE].active) 
{
    input_lp_shape = 10;
}
input_lp_shape *= param_lp_shape_atv;
input_lp_shape += 0+10*param_lp_shape_offset;
input_lp_shape = clamp(input_lp_shape, -10.0f, 10.0f);
input_lp_bw = inputs[INPUT_LP_BW].value;
if (!inputs[INPUT_LP_BW].active) 
{
    input_lp_bw = 10;
}
input_lp_bw *= param_lp_bw_atv;
input_lp_bw += 0+10*param_lp_bw_offset;
input_lp_bw = clamp(input_lp_bw, 0.0f, 10.0f);
input_bp_shape = inputs[INPUT_BP_SHAPE].value;
if (!inputs[INPUT_BP_SHAPE].active) 
{
    input_bp_shape = 10;
}
input_bp_shape *= param_bp_shape_atv;
input_bp_shape += 0+10*param_bp_shape_offset;
input_bp_shape = clamp(input_bp_shape, -10.0f, 10.0f);
input_bp_bw = inputs[INPUT_BP_BW].value;
if (!inputs[INPUT_BP_BW].active) 
{
    input_bp_bw = 10;
}
input_bp_bw *= param_bp_bw_atv;
input_bp_bw += 0+10*param_bp_bw_offset;
input_bp_bw = clamp(input_bp_bw, 0.0f, 10.0f);
input_hp_shape = inputs[INPUT_HP_SHAPE].value;
if (!inputs[INPUT_HP_SHAPE].active) 
{
    input_hp_shape = 10;
}
input_hp_shape *= param_hp_shape_atv;
input_hp_shape += 0+10*param_hp_shape_offset;
input_hp_shape = clamp(input_hp_shape, -10.0f, 10.0f);
input_hp_bw = inputs[INPUT_HP_BW].value;
if (!inputs[INPUT_HP_BW].active) 
{
    input_hp_bw = 10;
}
input_hp_bw *= param_hp_bw_atv;
input_hp_bw += 0+10*param_hp_bw_offset;
input_hp_bw = clamp(input_hp_bw, 0.0f, 10.0f);
input_aux_noise[0] = inputs[INPUT_AUX_NOISE].value;
input_aux_noise[1] = inputs[INPUT_AUX_NOISE+1].value;
input_noise_mix = inputs[INPUT_NOISE_MIX].value;
input_noise_shape[0] = inputs[INPUT_NOISE_SHAPE].value;
if (!inputs[INPUT_NOISE_SHAPE].active) 
{
    input_noise_shape[0] = 10;
}
input_noise_shape[0] *= param_noise_shape_atv[0];
input_noise_bw[0] = inputs[INPUT_NOISE_BW].value;
if (!inputs[INPUT_NOISE_BW].active) 
{
    input_noise_bw[0] = 10;
}
input_noise_bw[0] *= param_noise_bw_atv[0];
input_noise_bw[0] = clamp(input_noise_bw[0], 0.0f, 10.0f);
input_noise_shape[1] = inputs[INPUT_NOISE_SHAPE+1].value;
if (!inputs[INPUT_NOISE_SHAPE+1].active) 
{
    input_noise_shape[1] = 10;
}
input_noise_shape[1] *= param_noise_shape_atv[1];
input_noise_bw[1] = inputs[INPUT_NOISE_BW+1].value;
if (!inputs[INPUT_NOISE_BW+1].active) 
{
    input_noise_bw[1] = 10;
}
input_noise_bw[1] *= param_noise_bw_atv[1];
input_noise_bw[1] = clamp(input_noise_bw[1], 0.0f, 10.0f);
input_noise_slope[1] = inputs[INPUT_NOISE_SLOPE+1].value;
if (!inputs[INPUT_NOISE_SLOPE+1].active) 
{
    input_noise_slope[1] = 10;
}
input_noise_slope[1] *= param_noise_slope_atv[1];
input_noise_slope[1] = clamp(input_noise_slope[1], -10.0f, 10.0f);
input_noise_slope[0] = inputs[INPUT_NOISE_SLOPE].value;
if (!inputs[INPUT_NOISE_SLOPE].active) 
{
    input_noise_slope[0] = 10;
}
input_noise_slope[0] *= param_noise_slope_atv[0];
input_noise_slope[0] = clamp(input_noise_slope[0], -10.0f, 10.0f);
input_delay_clock = inputs[INPUT_DELAY_CLOCK].value;
output_delay_out = outputs[OUTPUT_DELAY_OUT].value;
output_env_gate = outputs[OUTPUT_ENV_GATE].value;
output_env = outputs[OUTPUT_ENV].value;
output_noise[0] = outputs[OUTPUT_NOISE].value;
output_noise[1] = outputs[OUTPUT_NOISE+1].value;
output_noise_out = outputs[OUTPUT_NOISE_OUT].value;
output_delay_in = outputs[OUTPUT_DELAY_IN].value;