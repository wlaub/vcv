#include "TechTechTechnologies.hpp"


struct Polyphemus2 : Module {
	enum ParamIds {
		BIAS0_PARAM,
		BIAS1_PARAM,
		BIAS2_PARAM,
		BIAS3_PARAM,
		ENVGAIN0_PARAM,
		ENVGAIN1_PARAM,
		ENVGAIN2_PARAM,
		ENVGAIN3_PARAM,
		ENVP0_PARAM,
		ENVP1_PARAM,
		ENVP2_PARAM,
		ENVP3_PARAM,
		GAIN0_PARAM,
		GAIN1_PARAM,
		GAIN2_PARAM,
		GAIN3_PARAM,
		KNEE0_PARAM,
		KNEE1_PARAM,
		KNEE2_PARAM,
		KNEE3_PARAM,
		VOCTP0_PARAM,
		VOCTP1_PARAM,
		VOCTP2_PARAM,
		VOCTP3_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		ENV0_INPUT,
		ENV1_INPUT,
		ENV2_INPUT,
		ENV3_INPUT,
		IN00_INPUT,
		IN01_INPUT,
		IN02_INPUT,
		IN03_INPUT,
		IN10_INPUT,
		IN11_INPUT,
		IN12_INPUT,
		IN13_INPUT,
		IN20_INPUT,
		IN21_INPUT,
		IN22_INPUT,
		IN23_INPUT,
		VOCT0_INPUT,
		VOCT1_INPUT,
		VOCT2_INPUT,
		VOCT3_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT00_OUTPUT,
		OUT01_OUTPUT,
		OUT02_OUTPUT,
		OUT03_OUTPUT,
		OUT10_OUTPUT,
		OUT11_OUTPUT,
		OUT12_OUTPUT,
		OUT13_OUTPUT,
		OUT20_OUTPUT,
		OUT21_OUTPUT,
		OUT22_OUTPUT,
		OUT23_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

    float buffer[4][3];

	Polyphemus2() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        for(int i = 0; i < 4; ++i)
        {
            configParam(BIAS0_PARAM+i, -1.f, 1.f, 0.f, "Envelope bias offset");
            configParam(ENVGAIN0_PARAM+i, 0.f, 1.f, 0.f, "Not Implemented");
            configParam(ENVP0_PARAM+i, 0.f, 1.f, 1.f, "Envelope level");
            configParam(GAIN0_PARAM+i, 0.f, 19.f, 9.f, "Frequency gain above knee");
            configParam(KNEE0_PARAM+i, 0.f, 1.f, 0.5f, "Cutoff frequency knee level");
            configParam(VOCTP0_PARAM+i, -3.f, 3.f, 0.f, "Base cutoff Frequency");
            for(int j = 0; j < 3; ++ j)
            {
                buffer[i][j] = 0;
            }
        }
	}

    float get_r(float wc, float kc=0.5)
    {
        //Kc must be 0-1 gain^2 at cutoff frequency 
        kc = 1/kc;
        float a = 1-kc; //a = c
        float b = 2*(kc-cos(wc));
        if(b > 2*a)
        {
            return (-b + sqrt(b*b-4*a*a))/(2*a);
        }
        else
        {
            return 0;
        }
    }

    float get_k(float w, float r)
    {
        return 1/sqrt(r*r-2*r*cos(w)+1);
    }


	void process(const ProcessArgs &args) override {

        for(int i = 0; i < 4; ++i)
        {
            float t = 1.f;
            float knee = params[KNEE0_PARAM+i].getValue();
            
            if(inputs[ENV0_INPUT+i].active)
            {
                t = inputs[ENV0_INPUT+i].getVoltage()/10.f;
            }
            t *= params[ENVP0_PARAM+i].getValue();
            t += params[BIAS0_PARAM+i].getValue();
            if(t <= knee) 
            {
                t /= knee;
            }
            else
            {
                t = 1+(t-knee)/(1-knee)*params[GAIN0_PARAM+i].getValue();
            }

            float pitch = params[VOCTP0_PARAM+i].getValue();
            float freq =  dsp::FREQ_C4 * std::pow(2.f, pitch) * t;
            float wc = 6.28*freq*args.sampleTime;
            wc = clamp(wc, 0.f, 3.14f);

            float r = get_r(wc);
            float k = get_k(wc, r);

            for(int j = 0; j < 3; ++j)
            {
                float x = inputs[IN00_INPUT+i+4*j].getVoltage();
                float y = k*x+r*buffer[i][j];
                buffer[i][j] = y;
                outputs[OUT00_OUTPUT+i+4*j].setVoltage(y);
            }

        }

	}
};



struct Polyphemus2Widget : ModuleWidget {
	Polyphemus2Widget(Polyphemus2 *module) {
		setModule(module);
		setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Polyphemus2.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(17.3, 377.635)), module, Polyphemus2::BIAS0_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(58.958, 377.635)), module, Polyphemus2::BIAS1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(100.616, 377.635)), module, Polyphemus2::BIAS2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(142.274, 377.635)), module, Polyphemus2::BIAS3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(32.003, 377.635)), module, Polyphemus2::ENVGAIN0_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(73.661, 377.635)), module, Polyphemus2::ENVGAIN1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(115.319, 377.635)), module, Polyphemus2::ENVGAIN2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(156.977, 377.635)), module, Polyphemus2::ENVGAIN3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(32.003, 362.933)), module, Polyphemus2::ENVP0_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(73.661, 362.933)), module, Polyphemus2::ENVP1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(115.319, 362.933)), module, Polyphemus2::ENVP2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(156.977, 362.933)), module, Polyphemus2::ENVP3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(32.003, 392.338)), module, Polyphemus2::GAIN0_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(73.661, 392.338)), module, Polyphemus2::GAIN1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(115.319, 392.338)), module, Polyphemus2::GAIN2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(156.977, 392.338)), module, Polyphemus2::GAIN3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(17.3, 392.338)), module, Polyphemus2::KNEE0_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(58.958, 392.338)), module, Polyphemus2::KNEE1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(100.616, 392.338)), module, Polyphemus2::KNEE2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(142.274, 392.338)), module, Polyphemus2::KNEE3_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(17.3, 362.933)), module, Polyphemus2::VOCTP0_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(58.958, 362.933)), module, Polyphemus2::VOCTP1_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(100.616, 362.933)), module, Polyphemus2::VOCTP2_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(142.274, 362.933)), module, Polyphemus2::VOCTP3_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(32.003, 348.23)), module, Polyphemus2::ENV0_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(73.661, 348.23)), module, Polyphemus2::ENV1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(115.319, 348.23)), module, Polyphemus2::ENV2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(156.977, 348.23)), module, Polyphemus2::ENV3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(17.3, 407.041)), module, Polyphemus2::IN00_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(58.958, 407.041)), module, Polyphemus2::IN01_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(100.616, 407.041)), module, Polyphemus2::IN02_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(142.274, 407.041)), module, Polyphemus2::IN03_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(17.3, 421.744)), module, Polyphemus2::IN10_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(58.958, 421.744)), module, Polyphemus2::IN11_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(100.616, 421.744)), module, Polyphemus2::IN12_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(142.274, 421.744)), module, Polyphemus2::IN13_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(17.3, 436.446)), module, Polyphemus2::IN20_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(58.958, 436.446)), module, Polyphemus2::IN21_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(100.616, 436.446)), module, Polyphemus2::IN22_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(142.274, 436.446)), module, Polyphemus2::IN23_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(17.3, 348.23)), module, Polyphemus2::VOCT0_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(58.958, 348.23)), module, Polyphemus2::VOCT1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(100.616, 348.23)), module, Polyphemus2::VOCT2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(142.274, 348.23)), module, Polyphemus2::VOCT3_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(32.003, 407.041)), module, Polyphemus2::OUT00_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(73.661, 407.041)), module, Polyphemus2::OUT01_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(115.319, 407.041)), module, Polyphemus2::OUT02_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(156.977, 407.041)), module, Polyphemus2::OUT03_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(32.003, 421.744)), module, Polyphemus2::OUT10_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(73.661, 421.744)), module, Polyphemus2::OUT11_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(115.319, 421.744)), module, Polyphemus2::OUT12_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(156.977, 421.744)), module, Polyphemus2::OUT13_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(-32.003, 436.446)), module, Polyphemus2::OUT20_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(-73.661, 436.446)), module, Polyphemus2::OUT21_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(-115.319, 436.446)), module, Polyphemus2::OUT22_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(-156.977, 436.446)), module, Polyphemus2::OUT23_OUTPUT));
	}

};


Model *modelPolyphemus2 = createModel<Polyphemus2, Polyphemus2Widget>("Polyphemus2");
