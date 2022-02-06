#include "TechTechTechnologies.hpp"
#include <numeric>

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

    double phase_accumulator = 0;

    CobaltI() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
        configParam(START_PARAM, 1.f, 7.f, 1.f, "Starting Subharmonic");
        configParam(LENGTH_PARAM, 1.f, 7.f, 1.f, "Sequence Length");
        configParam(PW_PARAM, 0.f, 1.f, 0.5f, "Pulse Width");
        configParam(PHASE_PARAM, 0.f, 1.f, 0.75f, "Starting Phase (Cycles)");

        configParam(FREQ_PARAM, 1e-3, 600.f, 10.f, "Combined Waveform Period (s)");
        configParam(SCALE_PARAM, 0.f, 10.f, 5.f, "Scale");
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

        double period = params[FREQ_PARAM].getValue();

        if(inputs[VOCT_INPUT].active)
        {
            period /= pow(2, inputs[VOCT_INPUT].getVoltage());
        }

        //TODO: reset button
        phase_accumulator += deltaTime/period;
        if(phase_accumulator > 1)
        {
            phase_accumulator -= 1;
        }

        //TODO: the other outputs
        //TODO: scale and offset
        outputs[SINE_OUTPUT].setChannels(length);
        for(int idx = start; idx < start+length; ++idx)
        {
            double relphase = phase_accumulator*total_period/idx;
            relphase += params[PHASE_PARAM].getValue();

            if(outputs[SINE_OUTPUT].active)
            {
                double x = sin(6.28*relphase);
                outputs[SINE_OUTPUT].setVoltage(x, idx-start);
            }

        }

    }
};


struct CobaltIWidget : ModuleWidget {
    CobaltIWidget(CobaltI* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/Cobalt.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        #define GRID(x,y) 15.24*(x-0.5), 15.24*(y)+3.28

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
