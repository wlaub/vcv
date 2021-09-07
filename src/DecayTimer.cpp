#include "TechTechTechnologies.hpp"

#define EDGE_PULSE 1e-3
#define LOGIC_HIGH 5

struct DecayTimer : Module {
    enum ParamIds {
        THRESHOLD_PARAM,
        HYSTERESIS_PARAM,
        RESET_BUTTON_PARAM,
        MANUAL_TRIGGER_PARAM,
        HOLDOFF_PARAM,
        POST_PULSE_LENGTH_PARAM,
        POST_PULSE_COUNT_PARAM,
        POST_PULSE_RATE_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        RESET_IN_INPUT,
        CMP_IN_INPUT,
        GATE_IN_INPUT,
        NUM_INPUTS
    };
    enum OutputIds {
        POST_PULSE_OUT_OUTPUT,
        OVER_OUT_OUTPUT,
        RISING_EDGE_OUT_OUTPUT,
        UNDER_OUT_OUTPUT,
        FALLING_EDGE_OUT_OUTPUT,
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS
    };

    enum PulseStates {
        PULSE_IDLE,
        PULSE_HOLDOFF,
        PULSE_ACTIVE,
        PULSE_SPACING
    };

    Label* meas_label;
    Label* config_label;

    dsp::PulseGenerator done_pulse;
    dsp::PulseGenerator done_pulse_holdoff;
    dsp::PulseGenerator done_pulse_spacing;

    dsp::PulseGenerator rising_pulse;
    dsp::PulseGenerator falling_pulse;

    dsp::SchmittTrigger manual_trigger;

    int prev_gate = 0;
    int gate_value = 0;

    int meas_count = 0;
    int total_meas = 0;
    int current_total_meas = 0;

    int min_count = -1;
    int max_count = 0;

    int meas_samples = 0;
    int pulse_counter = 0;

    int label_counter = 0;

    int meas_finished = 0;
    int pulse_state = PULSE_IDLE;

    int prev_do_meas = 0;
    int reset_hold = 0;

    int prev_manual = 0;

    DecayTimer() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(THRESHOLD_PARAM, -12.f, 12.f, 0.f, "Comparator Threshold");
        configParam(HYSTERESIS_PARAM, -12.f, 12.f, 1.f, "Comparator Hysteresis");
        configParam(RESET_BUTTON_PARAM, 0.f, 1.f, 0.f, "Measurement Reset");
        configParam(MANUAL_TRIGGER_PARAM, 0.f, 1.f, 0.f, "Trigger Pulse");
        configParam(HOLDOFF_PARAM, 0.f, 10.f, 0.1f, "Pulse Holdoff");
        configParam(POST_PULSE_LENGTH_PARAM, 0.f, 1.f, 0.01f, "Pulse Duration");
        configParam(POST_PULSE_COUNT_PARAM, 0.f, 10.f, 0.f, "Pulse Count");
        configParam(POST_PULSE_RATE_PARAM, 0.f, 10.f, 0.1f, "Pulse Spacing");
    }

    void process(const ProcessArgs& args) override {


        if(params[RESET_BUTTON_PARAM].getValue() != 0 || inputs[RESET_IN_INPUT].getVoltage() > 1.0f)
        {
            meas_samples = 0;
            meas_count = 0;
            total_meas = 0;
            current_total_meas = 0;
            pulse_state = PULSE_IDLE;
            reset_hold = 1;
            min_count = -1;
            max_count = 0;
        }

        /* Comparator Section */

        float threshold_a = params[THRESHOLD_PARAM].getValue();
        float threshold_b = threshold_a + params[HYSTERESIS_PARAM].getValue();
        float threshold, threshold_low, threshold_high;

        if(threshold_a >= threshold_b)
        {
            threshold_low = threshold_b;
            threshold_high = threshold_a;
        }
        else
        {
             threshold_low = threshold_a;
             threshold_high = threshold_b;
        }

        if(gate_value == 0)
        {
            threshold = threshold_high;
        }
        else
        {
            threshold = threshold_low;
        }

        if(inputs[CMP_IN_INPUT].getVoltage() >= threshold)
        {
            gate_value = 1;
        }
        else
        {
            gate_value = 0;
        }

        if(prev_gate == 0 && gate_value == 1)
        {
            rising_pulse.trigger(EDGE_PULSE);
//            outputs[RISING_EDGE_OUT_OUTPUT];
        }
        if(prev_gate == 1 && gate_value == 0)
        {
            falling_pulse.trigger(EDGE_PULSE);
        }


        /* Comparator outputs */
        bool pulse;

        outputs[OVER_OUT_OUTPUT].setVoltage(LOGIC_HIGH*gate_value);
        outputs[UNDER_OUT_OUTPUT].setVoltage(LOGIC_HIGH*(1-gate_value));

        pulse = rising_pulse.process(args.sampleTime);
        outputs[RISING_EDGE_OUT_OUTPUT].setVoltage(pulse? LOGIC_HIGH : 0.f);

        pulse = falling_pulse.process(args.sampleTime);
        outputs[FALLING_EDGE_OUT_OUTPUT].setVoltage(pulse? LOGIC_HIGH : 0.f);
 
        /*Timer section*/

        int do_measurement = 0;
        if(inputs[GATE_IN_INPUT].active && inputs[GATE_IN_INPUT].getVoltage() > 1.f)
        {
            do_measurement = 1;
        }
        else
        {
            do_measurement = gate_value;
        }

        meas_finished = 0;
        if(do_measurement != 0 && reset_hold == 0)
        {
            ++current_total_meas;
        }
        if(do_measurement == 0)
        {
            if(prev_do_meas != 0 && reset_hold == 0)
            {
                int delta = current_total_meas - total_meas;
                if(delta > max_count)
                {
                    max_count = delta;
                }
                if(delta < min_count || min_count < 0)
                {
                    min_count = delta;
                }
                total_meas = current_total_meas;
                ++meas_count;
                meas_finished = 1;
            }
            reset_hold = 0;
        }

        /*Done Pulse section*/

        int pulse_num = params[POST_PULSE_COUNT_PARAM].getValue() ;
        float pulse_length = params[POST_PULSE_LENGTH_PARAM].getValue();
        float pulse_spacing = params[POST_PULSE_RATE_PARAM].getValue();
        float pulse_holdoff = params[HOLDOFF_PARAM].getValue();

        int manual = 0;
        if(params[MANUAL_TRIGGER_PARAM].getValue() != 0)
        {
            manual = 1;
        }

        if(meas_finished != 0 || (manual == 1 && prev_manual == 0))
        {
            done_pulse_holdoff.trigger(pulse_holdoff);
            pulse_state = PULSE_HOLDOFF;
        }

        if(pulse_num == 0 || do_measurement != 0)
        {
            pulse_state = PULSE_IDLE;
            done_pulse_holdoff.reset();
            done_pulse.reset();
            done_pulse_spacing.reset();
        }

        bool holdoff_value = done_pulse_holdoff.process(args.sampleTime);
        bool pulse_value = done_pulse.process(args.sampleTime);
        bool pause_value = done_pulse_spacing.process(args.sampleTime);

        if(pulse_state != PULSE_IDLE)
        {
            if(pulse_state == PULSE_HOLDOFF)
            {
                if(!holdoff_value)
                {
                    pulse_state = PULSE_ACTIVE;
                    done_pulse.trigger(pulse_length);
                }
            }
            else if(pulse_state == PULSE_ACTIVE)
            {
                if(!pulse_value)
                {
                    ++pulse_counter;
                    if(pulse_counter >= pulse_num)
                    {
                        pulse_state = PULSE_IDLE;
                    }
                    else
                    {
                        done_pulse_spacing.trigger(pulse_spacing);
                        pulse_state = PULSE_SPACING;
                    }
                }
            }
            else if(pulse_state == PULSE_SPACING)
            {
                if(!pause_value)
                {
                    done_pulse.trigger(pulse_length);
                    pulse_state = PULSE_ACTIVE;
                }
            }
        }
        else
        {
            pulse_counter = 0;
        }

        outputs[POST_PULSE_OUT_OUTPUT].setVoltage(pulse_value? LOGIC_HIGH : 0.f);
//        outputs[POST_PULSE_OUT_OUTPUT].setVoltage((pulse_state == PULSE_IDLE)? LOGIC_HIGH : 0.f);
//        outputs[POST_PULSE_OUT_OUTPUT].setVoltage(LOGIC_HIGH*reset_hold);
 
        /* Finalization */

        prev_gate = gate_value;
        prev_do_meas = do_measurement;
        prev_manual = manual;

        /* Labels */
        ++label_counter;
        if(label_counter == 4000)
        {
            label_counter = 0;
        }
        if(config_label && label_counter == 0)
        {
            char tstr[4096];
            sprintf( tstr, 
                "Comparator Thresholds: %.2f / %.2f V\nDone Pulse:\n  Count: %i Length: %.1f ms\n  Holdoff: %.1f ms Spacing: %.1f ms",
                threshold_low, threshold_high,
                pulse_num, pulse_length*1000, 
                pulse_holdoff*1000, pulse_spacing*1000
                
                );

            config_label->text = tstr;
        }

        if(meas_label && label_counter == 1)
        {
            float meas_time = 0;
            if(meas_count != 0)
            {
                meas_time = args.sampleTime * total_meas / meas_count;
            }
            float min_time = 0;
            float max_time = max_count*args.sampleTime;
            if(min_count >= 0)
            {
                min_time = min_count*args.sampleTime;
            }


            char tstr[4096];
            sprintf(tstr,
            "Average:\n%.4f ms\nMin:\n%.3f\nMax:\n%.3f\nMaxDev:\n%.3f\nCount: %i\n",
            meas_time*1000,
            min_time*1000,
            max_time*1000,
            (max_time-min_time)*1000,
            meas_count
            );
            meas_label->text = tstr;
        }



    }
};


struct DecayTimerWidget : ModuleWidget {
    Label* config_label;
    Label* meas_label;

    DecayTimerWidget(DecayTimer* module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/DecayTimer.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(createParamCentered<LEDBezel>(mm2px(Vec(47.625, 36.733)), module, DecayTimer::MANUAL_TRIGGER_PARAM));
        addParam(createParamCentered<LEDBezel>(mm2px(Vec(47.625, 24.033)), module, DecayTimer::RESET_BUTTON_PARAM));
 

        addParam(createParamCentered<RoundTinyBlackKnob>(mm2px(Vec(9.525, 24.033)), module, DecayTimer::THRESHOLD_PARAM));
        addParam(createParamCentered<RoundTinyBlackKnob>(mm2px(Vec(22.225, 24.033)), module, DecayTimer::HYSTERESIS_PARAM));
       addParam(createParamCentered<RoundTinyBlackKnob>(mm2px(Vec(34.925, 49.433)), module, DecayTimer::HOLDOFF_PARAM));
        addParam(createParamCentered<RoundTinyBlackKnob>(mm2px(Vec(47.625, 49.433)), module, DecayTimer::POST_PULSE_LENGTH_PARAM));
        addParam(createParamCentered<RoundTinyBlackSnapKnob>(mm2px(Vec(34.925, 62.133)), module, DecayTimer::POST_PULSE_COUNT_PARAM));
        addParam(createParamCentered<RoundTinyBlackKnob>(mm2px(Vec(47.625, 62.133)), module, DecayTimer::POST_PULSE_RATE_PARAM));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(34.925, 24.033)), module, DecayTimer::RESET_IN_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.525, 36.733)), module, DecayTimer::CMP_IN_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.225, 36.733)), module, DecayTimer::GATE_IN_INPUT));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(34.925, 36.733)), module, DecayTimer::POST_PULSE_OUT_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(9.525, 49.433)), module, DecayTimer::OVER_OUT_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.225, 49.433)), module, DecayTimer::RISING_EDGE_OUT_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(9.525, 62.133)), module, DecayTimer::UNDER_OUT_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.225, 62.133)), module, DecayTimer::FALLING_EDGE_OUT_OUTPUT));

        // mm2px(Vec(41.275, 49.648))
//        addChild(createWidget<Widget>(mm2px(Vec(53.975, 20.858))));

        meas_label = createWidget<Label>(mm2px(Vec(53.975, 20.85)));
        meas_label->box.size = mm2px(Vec(41.275, 49.65));
        meas_label->text = "Lat (ms):\n ----.-- \nMin:\n ----.-- \n Max:\n ----.--\nN samples";
        addChild(meas_label);
        if(module)
        {
            module->meas_label = meas_label;
        }
    
        // mm2px(Vec(88.95, 42.003))
//        addChild(createWidget<Widget>(mm2px(Vec(6.3, 74.707))));

        config_label = createWidget<Label>(mm2px(Vec(6.3, 74.7)));
        config_label->box.size = mm2px(Vec(88.95, 42));
        config_label->text = "Lat (ms):\n ----.-- \nMin:\n ----.-- \n Max:\n ----.--\nN samples";
        addChild(config_label);
        if(module)
        {
            module->config_label = config_label;
        }
 

    }
};


Model* modelDecayTimer = createModel<DecayTimer, DecayTimerWidget>("DecayTimer");
