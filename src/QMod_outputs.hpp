
/* Write outputs */

outputs[OUTPUT_MOD_OUT].value = output_mod_out[0];
outputs[OUTPUT_MOD_OUT+1].value = output_mod_out[1];
outputs[OUTPUT_MOD_OUT+2].value = output_mod_out[2];
outputs[OUTPUT_MOD_OUT+3].value = output_mod_out[3];
output_q_out[0] = clamp(output_q_out[0], -10.0f, 10.0f);
outputs[OUTPUT_Q_OUT].value = output_q_out[0];
output_i_out[0] = clamp(output_i_out[0], -10.0f, 10.0f);
outputs[OUTPUT_I_OUT].value = output_i_out[0];
output_q_out[2] = clamp(output_q_out[2], -10.0f, 10.0f);
outputs[OUTPUT_Q_OUT+2].value = output_q_out[2];
output_i_out[2] = clamp(output_i_out[2], -10.0f, 10.0f);
outputs[OUTPUT_I_OUT+2].value = output_i_out[2];
output_q_out[3] = clamp(output_q_out[3], -10.0f, 10.0f);
outputs[OUTPUT_Q_OUT+3].value = output_q_out[3];
output_i_out[3] = clamp(output_i_out[3], -10.0f, 10.0f);
outputs[OUTPUT_I_OUT+3].value = output_i_out[3];
output_q_out[1] = clamp(output_q_out[1], -10.0f, 10.0f);
outputs[OUTPUT_Q_OUT+1].value = output_q_out[1];
output_i_out[1] = clamp(output_i_out[1], -10.0f, 10.0f);
outputs[OUTPUT_I_OUT+1].value = output_i_out[1];