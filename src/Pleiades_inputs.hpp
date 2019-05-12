/* Declare vars */
float input_clock;
float input_analog;
float output_out[8];
float param_center;
float param_step[7];
float param_config[3];
float param_mode[7];

/* Read params */

param_center = params[PARAM_CENTER].value;
param_step[3] = params[PARAM_STEP+3].value;
param_step[4] = params[PARAM_STEP+4].value;
param_step[5] = params[PARAM_STEP+5].value;
param_step[6] = params[PARAM_STEP+6].value;
param_step[0] = params[PARAM_STEP].value;
param_step[1] = params[PARAM_STEP+1].value;
param_step[2] = params[PARAM_STEP+2].value;
param_mode[2] = params[PARAM_MODE+2].value;
param_config[0] = params[PARAM_CONFIG].value;
param_mode[4] = params[PARAM_MODE+4].value;
param_mode[3] = params[PARAM_MODE+3].value;
param_mode[5] = params[PARAM_MODE+5].value;
param_mode[1] = params[PARAM_MODE+1].value;
param_config[2] = params[PARAM_CONFIG+2].value;
param_mode[6] = params[PARAM_MODE+6].value;
param_mode[0] = params[PARAM_MODE].value;
param_config[1] = params[PARAM_CONFIG+1].value;

/* Read inputs */

input_clock = inputs[INPUT_CLOCK].value;
if (!inputs[INPUT_CLOCK].active) 
{
    input_clock = 0;
}
input_clock = clamp(input_clock, -10.0f, 10.0f);
input_analog = inputs[INPUT_ANALOG].value;
if (!inputs[INPUT_ANALOG].active) 
{
    input_analog = 0;
}
input_analog = clamp(input_analog, -10.0f, 10.0f);
output_out[3] = outputs[OUTPUT_OUT+3].value;
output_out[2] = outputs[OUTPUT_OUT+2].value;
output_out[1] = outputs[OUTPUT_OUT+1].value;
output_out[0] = outputs[OUTPUT_OUT].value;
output_out[4] = outputs[OUTPUT_OUT+4].value;
output_out[5] = outputs[OUTPUT_OUT+5].value;
output_out[6] = outputs[OUTPUT_OUT+6].value;
output_out[7] = outputs[OUTPUT_OUT+7].value;