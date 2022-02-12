#include "Cobalt.hpp"
#include <numeric>



struct Sixty : Module {
    enum ParamId {
        TRI_PARAM,
        RC_PARAM, //Raised Cosine
        DECAY_PARAM,
        TRAP_PARAM,
        CLK_PARAM,
        PARAMS_LEN
    };
    enum InputId {
        EXP_INPUT,
        INVERT_INPUT,
        INPUTS_LEN
    };
    enum OutputId {
        TRI_OUTPUT,
        RC_OUTPUT,
        DECAY_OUTPUT,
        TRAP_OUTPUT,
        EXP_OUTPUT,
        INVERT_OUTPUT,
        CLK_OUTPUT,
        OUTPUTS_LEN
    };
    enum LightId {
        LIGHTS_LEN
    };


    Sixty() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
        configParam(TRI_PARAM, 0.f, 1.f, 0.5f, "Symmetry");
        configParam(RC_PARAM, 0.f, 1.f, 0.5f, "Rolloff");
        configParam(DECAY_PARAM, 0.f, 1.f, 0.5f, "Decay Rate");
        configParam(TRAP_PARAM, 0.f, 1.f, 0.5f, "Slope");
        configParam(CLK_PARAM, 1.f, 8.f, 1.f, "Clock Subdivisions");

        configInput(EXP_INPUT, "Exponential Converter");
        configInput(INVERT_INPUT, "1-X Converter");

        configOutput(TRI_OUTPUT, "Asymmetric Triangle");
        configOutput(RC_OUTPUT, "Raise Cosine");
        configOutput(DECAY_OUTPUT, "RC Decay");
        configOutput(TRAP_OUTPUT, "Trapezoidal");
        configOutput(EXP_OUTPUT, "Exponential Converter");
        configOutput(INVERT_OUTPUT, "1-X Converter");
        configOutput(CLK_OUTPUT, "Clock Subdivision");
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

        /* Generate Waveforms*/
/*
        double scale = params[SCALE_PARAM].getValue()/2;
        double offset = params[OFFSET_PARAM].getValue();
        double outer_scale = 1;
        if(normalize)
        {
            outer_scale = 1.f/length;
        }

        out_message.scale = scale;
        out_message.offset = offset;
        out_message.outer_scale = outer_scale;
        out_message.length = length;

        double phases[MAX_LENGTH];
        for(int i = 0; i < length; ++i)
        {
            int idx = i + start;
            double trash;
            double x;
            double relphase = phase_accumulator*total_period/idx;
            relphase += params[PHASE_PARAM].getValue();
            relphase = modf(relphase, &trash);
            phases[i] = relphase;
            out_message.phases[i] = relphase;
        }

        outputs[SINE_OUTPUT].setChannels(length);
        outputs[RAMP_OUTPUT].setChannels(length);
        outputs[TRIANGLE_OUTPUT].setChannels(length);
        outputs[SQUARE_OUTPUT].setChannels(length);
        for(int i = 0; i < length; ++i)
        {
            double x;
            double relphase = phases[i];

            if(outputs[SINE_OUTPUT].active)
            {
                x = (sin(2*M_PI*relphase)*scale+offset)*outer_scale;
                outputs[SINE_OUTPUT].setVoltage(x, i);
            }
            if(outputs[RAMP_OUTPUT].active)
            {
                x = (ramp(relphase)*scale+offset)*outer_scale;
                outputs[RAMP_OUTPUT].setVoltage(x, i);
            }
            if(outputs[TRIANGLE_OUTPUT].active)
            {
                x = (triangle(relphase)*scale+offset)*outer_scale;
                outputs[TRIANGLE_OUTPUT].setVoltage(x, i);
            }
            if(outputs[SQUARE_OUTPUT].active)
            {
                double pw = params[PW_PARAM].getValue();
                x = (square(relphase, pw)*scale+offset)*outer_scale;
                outputs[SQUARE_OUTPUT].setVoltage(x, i);
            }
        }
*/

    }



};

#define GRIDX(x) 15.24*(x-0.5)
#define GRIDY(y) 15.24*(y)+3.28
#define GRID(x,y) GRIDX(x), GRIDY(y)

struct SixtyWidget : ModuleWidget {

    SixtyWidget(Sixty* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/Sixty.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(createParamCentered<RoundBlackKnob>(
            mm2px(Vec(GRID(1,1))), module, Sixty::TRI_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(
            mm2px(Vec(GRID(1,2))), module, Sixty::DECAY_PARAM));

        addParam(createParamCentered<RoundBlackKnob>(
            mm2px(Vec(GRID(1,3))), module, Sixty::RC_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(
            mm2px(Vec(GRID(1,4))), module, Sixty::TRAP_PARAM));

        addOutput(createOutputCentered<PJ301MPort>(
            mm2px(Vec(GRID(2,1))), module, Sixty::TRI_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(
            mm2px(Vec(GRID(2,2))), module, Sixty::DECAY_OUTPUT));

        addOutput(createOutputCentered<PJ301MPort>(
            mm2px(Vec(GRID(2,3))), module, Sixty::RC_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(
            mm2px(Vec(GRID(2,4))), module, Sixty::TRAP_OUTPUT));


        addInput(createInputCentered<PJ301MPort>(
            mm2px(Vec(GRID(1,5))), module, Sixty::EXP_INPUT));
        addInput(createInputCentered<PJ301MPort>(
            mm2px(Vec(GRID(1,6))), module, Sixty::INVERT_INPUT));

        addOutput(createOutputCentered<PJ301MPort>(
            mm2px(Vec(GRID(2,5))), module, Sixty::EXP_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(
            mm2px(Vec(GRID(2,6))), module, Sixty::INVERT_OUTPUT));

        addParam(createParamCentered<RoundBlackSnapKnob>(
            mm2px(Vec(GRID(1,7))), module, Sixty::CLK_PARAM));
        addOutput(createOutputCentered<PJ301MPort>(
            mm2px(Vec(GRID(2,7))), module, Sixty::CLK_OUTPUT));
    }
};


Model* modelSixty = createModel<Sixty, SixtyWidget>("Sixty");
