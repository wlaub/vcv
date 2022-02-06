#include "TechTechTechnologies.hpp"
#include <numeric>

#define MAX_LENGTH 7

struct CobaltI : Module {
    enum ParamId {
        START_PARAM,
        PW_PARAM,
        LENGTH_PARAM,
        FREQ_PARAM,
        SCALE_PARAM,
        PHASE_PARAM,
        OFFSET_PARAM,
        PARAMS_LEN
    };
    enum InputId {
        VOCT_INPUT,
        RESET_INPUT,
        INPUTS_LEN
    };
    enum OutputId {
        RAMP_OUTPUT,
        SQUARE_OUTPUT,
        SINE_OUTPUT,
        TRIANGLE_OUTPUT,
        OUTPUTS_LEN
    };
    enum LightId {
        LIGHTS_LEN
    };

    int start = -1;
    int length = -1;
    int total_period = -1;

    double period;
    double phase_accumulator = 0;

    int sequence_changed = 0;
    int panel_update = 0;

    dsp::SchmittTrigger reset_trigger;

    CobaltI() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
        configParam(START_PARAM, 1.f, 7.f, 1.f, "Starting Subharmonic");
        configParam(LENGTH_PARAM, 1.f, MAX_LENGTH, 1.f, "Sequence Length");
        configParam(PW_PARAM, 0.f, 1.f, 0.5f, "Pulse Width");
        configParam(PHASE_PARAM, 0.f, 1.f, 0.75f, "Starting Phase (Cycles)");

        configParam(FREQ_PARAM, 1e-3, 600.f, 10.f, "Combined Waveform Period (s)");
        configParam(SCALE_PARAM, 0.f, 10.f, 5.f, "Scale (peak-to-peak)");
        configParam(OFFSET_PARAM, 0.f, 5.f, 2.5f, "Offset");

        configInput(VOCT_INPUT, "v/oct");
        configInput(RESET_INPUT, "Reset");
        configOutput(RAMP_OUTPUT, "Ramp");
        configOutput(SQUARE_OUTPUT, "Square");
        configOutput(SINE_OUTPUT, "Sine");
        configOutput(TRIANGLE_OUTPUT, "Tiangle");
    }

    int gcd(int a, int b) 
    {
       if (b == 0)
       return a;
       return gcd(b, a % b);
    }

    int lcm(int a, int b) 
    {
        return (a*b)/gcd(a,b);
    }

    int get_sequence_period(int s, int l)
    {
        if(s == start and l == length)
        {
            return total_period;
        }
        sequence_changed = 1;
        start = s;
        length = l;

        if(length == 1)
        {
            return start;
        }

        int value = lcm(start, start+1);

        for(int i = start+1; i < start+length; ++i)
        {
            value = lcm(value, i);
        }
        return value;

    }

    double ramp(double p)
    {
        if(p < 0.5)
            return (p+0.5)*2-1;
        else
            return (p-0.5)*2-1;
    }

    double triangle(double p)
    {
        if(p < 0.25)
        {
            return p*4;
        }
        else if(p < 0.75)
        {
            return 1-4*(p-0.25);
        }
        else
        {
            return -1+(p-0.75)*4;
        }
    }

    double square(double p, double pw)
    {
        if(p < pw)
            return 1;
        else
            return -1;
    }

    void process(const ProcessArgs& args) override {
        //TODO: some kind of 
        //some kind of trouble
        //up ahead
        //it would be nice to
        //it would be nice to have some indicator of the fastest
        //configured frequency

        float deltaTime = args.sampleTime;       

        int s = params[START_PARAM].getValue();
        int l = params[LENGTH_PARAM].getValue();

        total_period = get_sequence_period(s,l);

        period = params[FREQ_PARAM].getValue();

        if(inputs[VOCT_INPUT].active)
        {
            period /= pow(2, inputs[VOCT_INPUT].getVoltage());
        }

        phase_accumulator += deltaTime/period;
        if(phase_accumulator > 1)
        {
            phase_accumulator -= 1;
        }

        //TODO: It would be nice to have the option to hold it in reset
        if(reset_trigger.process(inputs[RESET_INPUT].getVoltage()))
        {
            phase_accumulator = 0;
        }

        //TODO: A normalize option would be nice
        double scale = params[SCALE_PARAM].getValue()/2;
        double offset = params[OFFSET_PARAM].getValue();

        outputs[SINE_OUTPUT].setChannels(length);
        outputs[RAMP_OUTPUT].setChannels(length);
        outputs[TRIANGLE_OUTPUT].setChannels(length);
        outputs[SQUARE_OUTPUT].setChannels(length);
        for(int i = 0; i < length; ++i)
        {
            int idx = i + start;
            double trash;
            double x;
            double relphase = phase_accumulator*total_period/idx;
            relphase += params[PHASE_PARAM].getValue();
            relphase = modf(relphase, &trash);

            if(outputs[SINE_OUTPUT].active)
            {
                x = sin(6.28*relphase)*scale+offset;
                outputs[SINE_OUTPUT].setVoltage(x, i);
            }
            if(outputs[RAMP_OUTPUT].active)
            {
                x = ramp(relphase)*scale+offset;
                outputs[RAMP_OUTPUT].setVoltage(x, i);
            }
            if(outputs[TRIANGLE_OUTPUT].active)
            {
                x = triangle(relphase)*scale+offset;
                outputs[TRIANGLE_OUTPUT].setVoltage(x, i);
            }
            if(outputs[SQUARE_OUTPUT].active)
            {
                double pw = params[PW_PARAM].getValue();
                x = square(relphase, pw)*scale+offset;
                outputs[SQUARE_OUTPUT].setVoltage(x, i);
            }




        }

    }
};

/*
Copied from https://github.com/VCVRack/Rack/blob/05fa24a72bccf4023f5fb1b0fa7f1c26855c0926/src/ui/Label.cpp#L28
and modified to behave as expected
*/

struct AlignLabel : Label {

void draw(const DrawArgs& args) {
	// TODO
	// Custom font sizes do not work with right or center alignment
	float x;
	switch (alignment) {
		default:
		case LEFT_ALIGNMENT: {
			x = 0.0;
		} break;
		case RIGHT_ALIGNMENT: {
			x = -bndLabelWidth(args.vg, -1, text.c_str());
		} break;
		case CENTER_ALIGNMENT: {
			x = (-bndLabelWidth(args.vg, -1, text.c_str())) / 2.0;
		} break;
	}

	nvgTextLineHeight(args.vg, lineHeight);
	bndIconLabelValue(args.vg, x, 0.0, box.size.x, box.size.y, -1, color, BND_LEFT, fontSize, text.c_str(), NULL);
}

};

/* End of copied code */

#define GRIDX(x) 15.24*(x-0.5)
#define GRIDY(y) 15.24*(y)+3.28
#define GRID(x,y) GRIDX(x), GRIDY(y)
#define XPER(i) 15.24*(4*i/MAX_LENGTH -0.5)



struct CobaltIWidget : ModuleWidget {
    AlignLabel* period_labels[MAX_LENGTH];
    AlignLabel* index_labels[MAX_LENGTH];

    void set_period_label(int i, int idx, double value)
    {
        char text[256];

        std::string suffix = "s";
        if(value <= 1.f/20)
        {
            value = 1/value;
            suffix = "Hz";
            if(value >= 1000)
            {
                value/=1000;
                suffix = "kHz";
            }
        }

        if(value >= 10000)
            sprintf(text, "%.0f %s", value, suffix.c_str());
        else if(value >= 10)
            sprintf(text, "%.4g %s", value, suffix.c_str());
        else if (value >= 1)
            sprintf(text, "%.3g %s", value, suffix.c_str());
        else 
            sprintf(text, "%.2g %s", value, suffix.c_str());

        period_labels[i]->text = text;

        sprintf(text, "%i", idx);
        index_labels[i]->text = text;
    }

    void clear_label(int i)
    {
        period_labels[i]->text = "";
        index_labels[i]->text = "";
    }

    void step() override {
        ModuleWidget::step();
        if(!module) return;

        CobaltI* mod = ((CobaltI*) module);
        
        {
            for(int i = 0; i < MAX_LENGTH; ++i)
            {
                int idx = mod->start+i;
                if(i < mod->length)
                {
                    set_period_label(i, idx, mod->period/(mod->total_period/idx));
                }
                else
                {
                    clear_label(i);
                }
            }
        }

    } 

    CobaltIWidget(CobaltI* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/Cobalt.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));



        for(int i = 0; i < MAX_LENGTH; ++i)
        {
            float xpos = GRIDX(0.5+0.75+3.5f*(i%4)/(3));
            float ypos = 4-.125;
            if(i >= 4) ypos += 0.75;

            period_labels[i] = createWidget<AlignLabel>(
                mm2px(Vec(xpos, GRIDY(ypos+.25))));
            period_labels[i]->alignment = Label::CENTER_ALIGNMENT;
            addChild(period_labels[i]);

            index_labels[i] = createWidget<AlignLabel>(
                mm2px(Vec(xpos, GRIDY(ypos))));
            index_labels[i]->alignment = Label::CENTER_ALIGNMENT;
            addChild(index_labels[i]);

            set_period_label(i, i+1, 8888);
 
        }

        addParam(createParamCentered<RoundHugeBlackKnob>(
            mm2px(Vec(GRID(3,2))), module, CobaltI::FREQ_PARAM));

        addParam(createParamCentered<RoundBlackSnapKnob>(
            mm2px(Vec(GRID(1.5,1))), module, CobaltI::START_PARAM));
        addParam(createParamCentered<RoundBlackSnapKnob>(
            mm2px(Vec(GRID(1.5,2))), module, CobaltI::LENGTH_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(
            mm2px(Vec(GRID(1.5,3))), module, CobaltI::PHASE_PARAM));

        addParam(createParamCentered<RoundBlackKnob>(
            mm2px(Vec(GRID(4.5,1))), module, CobaltI::PW_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(
            mm2px(Vec(GRID(4.5,2))), module, CobaltI::SCALE_PARAM));

        addParam(createParamCentered<RoundBlackKnob>(
            mm2px(Vec(GRID(4.5,3))), module, CobaltI::OFFSET_PARAM));

        addInput(createInputCentered<PJ301MPort>(
            mm2px(Vec(GRID(1.5,6))), module, CobaltI::VOCT_INPUT));
        addInput(createInputCentered<PJ301MPort>(
            mm2px(Vec(GRID(1.5,7))), module, CobaltI::RESET_INPUT));

        addOutput(createOutputCentered<PJ301MPort>(
            mm2px(Vec(GRID(3.5,6))), module, CobaltI::RAMP_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(
            mm2px(Vec(GRID(4.5,6))), module, CobaltI::SQUARE_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(
            mm2px(Vec(GRID(3.5,7))), module, CobaltI::SINE_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(
            mm2px(Vec(GRID(4.5,7))), module, CobaltI::TRIANGLE_OUTPUT));
    }
};


Model* modelCobaltI = createModel<CobaltI, CobaltIWidget>("CobaltI");
