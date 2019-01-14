
/* Write outputs */

outputs[OUTPUT_DELAY_OUT].value = output_delay_out;
outputs[OUTPUT_ENV_GATE].value = output_env_gate;
outputs[OUTPUT_ENV].value = output_env;
if (!outputs[OUTPUT_NOISE].active) 
{
    output_noise[0] = 10;
}
output_noise[0] *= param_noise_atv[0];
outputs[OUTPUT_NOISE].value = output_noise[0];
if (!outputs[OUTPUT_NOISE+1].active) 
{
    output_noise[1] = 10;
}
output_noise[1] *= param_noise_atv[1];
outputs[OUTPUT_NOISE+1].value = output_noise[1];
outputs[OUTPUT_NOISE_OUT].value = output_noise_out;
outputs[OUTPUT_DELAY_IN].value = output_delay_in;