#include "Cobalt.hpp"
#include "PngModule.hpp"
#include <numeric>

struct Sixty : PngModule {
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

    const double exp_a = 2;
    const double exp_max = exp(exp_a);
    const double exp_min = exp(-exp_a);

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

    double triangle(double p, double pw)
    {
        if(p < pw)
            return 2*p/pw-1;
        else
            return 1-2*(p-pw)/(1-pw);
    }

    double trap(double p, double pw)
    {
        double w = (1-pw)/2;
        double ll = 0.25-w/2;
        double lr = ll+w;
        double rl = 0.75-w/2;
        double rr = rl+w;
        if(p < ll)
        {
            return -1;
        }
        else if(p < lr)
        {
            return 2*(p-ll)/w-1;
        }
        else if(p < rl)
        {
            return 1;
        }
        else if(p < rr)
        {
            return 1-2*(p-rl)/w;
        }
        else
        {
            return -1;
        }
    }

    double rc(double p, double pw)
    {
        double w = (1-pw)/2;
        double ll = 0.25-w/2;
        double lr = ll+w;
        double rl = 0.75-w/2;
        double rr = rl+w;
        if(p < ll)
        {
            return -1;
        }
        else if(p < lr)
        {
            return -cos(M_PI*(p-ll)/w);
        }
        else if(p < rl)
        {
            return 1;
        }
        else if(p < rr)
        {
            return cos(M_PI*(p-rl)/w);
        }
        else
        {
            return -1;
        }
    }

    double decay(double p, double pw)
    {
        double a = -4*(1+2*pw);
        double o = exp(0.5*a);
        double s = 1-o;
        if(p < 0.5)
        {

            return 2*(1-exp(p*a))/s -1;
        }
        else
        {
            return 2*(exp((p-0.5)*a)-o)/s-1;
        }
    }

    void process(const ProcessArgs& args) override {



        Expander* expander = 0;
        if(leftExpander.module) expander = &leftExpander;
        else if(rightExpander.module) expander = &rightExpander;
        if(!expander) return;

        if(expander->module->model != modelCobaltI) return;

        CobaltMessage* message = reinterpret_cast<CobaltMessage*>(expander->module->leftExpander.consumerMessage);


        /* Generate Waveforms*/

        int length = message->length;

        double scale = message->scale;
        double offset= message->offset;
        double outer_scale = message->outer_scale;
        double* phases = message->phases;

        outputs[TRI_OUTPUT].setChannels(length);
        outputs[RC_OUTPUT].setChannels(length);
        outputs[DECAY_OUTPUT].setChannels(length);
        outputs[TRAP_OUTPUT].setChannels(length);
        for(int i = 0; i < length; ++i)
        {
            double x;
            double pw;
            double relphase = phases[i];

            if(outputs[TRI_OUTPUT].active)
            {
                pw = params[TRI_PARAM].getValue();
                x = (triangle(relphase, pw)*scale+offset)*outer_scale;
                outputs[TRI_OUTPUT].setVoltage(x, i);
            }
            if(outputs[RC_OUTPUT].active)
            {
                pw = params[RC_PARAM].getValue();
                x = (rc(relphase,pw)*scale+offset)*outer_scale;
                outputs[RC_OUTPUT].setVoltage(x, i);
            }
            if(outputs[TRAP_OUTPUT].active)
            {
                pw = params[TRAP_PARAM].getValue();
                x = (trap(relphase, pw)*scale+offset)*outer_scale;
                outputs[TRAP_OUTPUT].setVoltage(x, i);
            }
            if(outputs[DECAY_OUTPUT].active)
            {
                pw = params[DECAY_PARAM].getValue();
                x = (decay(relphase, pw)*scale+offset)*outer_scale;
                outputs[DECAY_OUTPUT].setVoltage(x, i);
            }
        }

        if(outputs[CLK_OUTPUT].active)
        {
            int harm = params[CLK_PARAM].getValue();
            int val = int(floor(phases[0]*2*harm))%2;
            outputs[CLK_OUTPUT].setVoltage(10*val);
        }

        if(outputs[INVERT_OUTPUT].active)
        {
            double off = 2*offset*outer_scale;
            int num = inputs[INVERT_INPUT].getChannels();

            outputs[INVERT_OUTPUT].setChannels(num);
            for(int i = 0; i < num; ++i)
            {
                double x = inputs[INVERT_INPUT].getVoltage(i);
                outputs[INVERT_OUTPUT].setVoltage(off-x, i);
            }

        }

        if(outputs[EXP_OUTPUT].active)
        {
            double off = 2*offset*outer_scale;
            int num = inputs[EXP_INPUT].getChannels();

            outputs[EXP_OUTPUT].setChannels(num);
            for(int i = 0; i < num; ++i)
            {
                double x = inputs[EXP_INPUT].getVoltage(i);
                x /= outer_scale;
                x -= offset;
                x /= scale;
                x = 2*(exp(exp_a*x)-exp_min)/(exp_max-exp_min)-1;
                outputs[EXP_OUTPUT].setVoltage((x*scale+offset)*outer_scale, i);
            }

        }



    }


};

#define GRIDX(x) 15.24*(x-0.5)
#define GRIDY(y) 15.24*(y)+3.28
#define GRID(x,y) GRIDX(x), GRIDY(y)

struct SixtyWidget : PngModuleWidget {

    void appendContextMenu(Menu* menu) override {
            Sixty* module = dynamic_cast<Sixty*>(this->module);

            menu->addChild(new MenuEntry);

            panel_select_menu(menu, module);

        }

    SixtyWidget(Sixty* module) {
        setModule(module);

        set_panels(
            "res/Sixty.svg",
            {
            {"Fancy", "res/sixty_a.png"},
            });

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
