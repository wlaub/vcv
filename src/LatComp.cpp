#include "TechTechTechnologies.hpp"

struct LatComp : Module {
	enum ParamIds {
		NUM_PARAMS
	};
	enum InputIds {
		LOOP_IN_INPUT,
		DELAY_IN_0_INPUT,
		DELAY_IN_1_INPUT,
		DELAY_IN_2_INPUT,
		DELAY_IN_3_INPUT,
		DELAY_IN_4_INPUT,
		DELAY_IN_5_INPUT,
		DELAY_IN_6_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		LOOP_OUT_OUTPUT,
		DELAY_OUT_0_OUTPUT,
		DELAY_OUT_1_OUTPUT,
		DELAY_OUT_2_OUTPUT,
		DELAY_OUT_3_OUTPUT,
		DELAY_OUT_4_OUTPUT,
		DELAY_OUT_5_OUTPUT,
		PULSE_OUT_OUTPUT,
		TIME_OUT_OUTPUT,
		DELAY_OUT_6_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

    int waiting = 0;
    int wait_counter = 0;
    float current_avg = 0;

    float current_timeout = 0.2;
    float min_timeout = 0.2;
    float max_timeout = 10;   
 
    dsp::PulseGenerator loopback_pulse;

    ttt::CircularBuffer* data;

    Label* info_label;    

	LatComp() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        data = new ttt::CircularBuffer(64);
	}

	void process(const ProcessArgs& args) override {

        if(waiting == 0)
        {
            loopback_pulse.trigger(5e-3f);
            waiting = 1;
            wait_counter = 0;
        }
        else if (waiting == 1)
        {
            ++wait_counter;
            float duration = wait_counter*args.sampleTime;

            if(inputs[LOOP_IN_INPUT].getVoltage() > 5.f || duration > current_timeout)
            {

                data->push(duration);
                waiting = 2;

                float avg = 0;
                float max = 0;
                float min = 1000;
                int N = 8;
                for(int i = 0; i < N; ++i)
                {
                    float sample = data->get_tap(i);
                    avg += sample;
                    if (sample > max) max = sample;
                    if (sample < min) min = sample;
                }
                avg /= N; 
                current_avg = avg;

                if (duration > current_timeout)
                {
                    current_timeout += 1;
                    if (current_timeout >= max_timeout)
                    {
                        current_timeout = min_timeout;
                    }
                }
                else
                {
                    current_timeout = max*1.1;
                }
                

                if (info_label)
                {
                    char tstr[256];
                    sprintf(tstr, 
                    "Avg:\n %.2f\nMin:\n %.2f\nMax:\n %.2f\nN=%i\n%.4f", avg*1000, min*1000, max*1000, N, current_timeout);
                    info_label->text = tstr;
                }

            }

        }
        else if (waiting == 2)
        {
            if(inputs[LOOP_IN_INPUT].getVoltage() < 1.f)
            {
                waiting = 0;
            }
        }

        bool pulse = loopback_pulse.process(args.sampleTime);
        outputs[LOOP_OUT_OUTPUT].setVoltage(pulse? 7.f : 0.f);
        outputs[TIME_OUT_OUTPUT].setVoltage(current_avg);
/*
    if (info_label)
    {
        char tstr[256];
        sprintf(tstr, 
        "St: %i\n%f", waiting, args.sampleTime);
        info_label->text = tstr;
    }
*/

	}
};


struct LatCompWidget : ModuleWidget {
    Label* info_label;

	LatCompWidget(LatComp* module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/LatComp.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(19.05, 26.15)), module, LatComp::LOOP_IN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(31.75, 26.15)), module, LatComp::DELAY_IN_0_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(31.75, 38.85)), module, LatComp::DELAY_IN_1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(31.75, 51.55)), module, LatComp::DELAY_IN_2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(31.75, 64.25)), module, LatComp::DELAY_IN_3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(31.75, 76.95)), module, LatComp::DELAY_IN_4_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(31.75, 89.65)), module, LatComp::DELAY_IN_5_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(31.75, 102.35)), module, LatComp::DELAY_IN_6_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(6.35, 26.15)), module, LatComp::LOOP_OUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(44.45, 26.15)), module, LatComp::DELAY_OUT_0_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(44.45, 38.85)), module, LatComp::DELAY_OUT_1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(44.45, 51.55)), module, LatComp::DELAY_OUT_2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(44.45, 64.25)), module, LatComp::DELAY_OUT_3_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(44.45, 76.95)), module, LatComp::DELAY_OUT_4_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(44.45, 89.65)), module, LatComp::DELAY_OUT_5_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(6.35, 102.35)), module, LatComp::PULSE_OUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(19.05, 102.35)), module, LatComp::TIME_OUT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(44.45, 102.35)), module, LatComp::DELAY_OUT_6_OUTPUT));

		// mm2px(Vec(22.225, 63.5))
		info_label = createWidget<Label>(mm2px(Vec(3.175, 32.5)));
        info_label->box.size = mm2px(Vec(22.25, 63.5));
        info_label->text = "Lat (ms):\n ----.-- \nMin:\n ----.-- \n Max:\n ----.--\nN samples";
        addChild(info_label);
        if(module)
        {
            module->info_label = info_label;
        }
	}
};


Model* modelLatComp = createModel<LatComp, LatCompWidget>("LatComp");
