#include "TechTechTechnologies.hpp"

#define MAX_CHANNELS 16

struct MatI : Module {
    enum ParamId {
        PARAMS_LEN
    };
    enum InputId {
        POLY_INPUT,
        INPUTS_LEN
    };
    enum OutputId {
        POLY_OUTPUT,
        OUTPUTS_LEN
    };
    enum LightId {
        INPUT_LIGHT,
        FILTER_LIGHT = INPUT_LIGHT+MAX_CHANNELS,
        LIGHTS_LEN = FILTER_LIGHT+MAX_CHANNELS
    };

    struct ttt::Biquad** filters;
    int* filter_order;
    int filter_count = 0;

    MatI() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

        configInput(POLY_INPUT, "Filter");

        configOutput(POLY_OUTPUT, "Filter");
    }

    void process(const ProcessArgs& args) override {

        int input_count = 0;
        if(inputs[POLY_INPUT].active)
        {
            input_count = inputs[POLY_INPUT].getChannels();
        }

        /* Handle Indicators */

        for(int i = 0; i < MAX_CHANNELS; ++i)
        {

            float value = 0;
            if(i < input_count)
            {
                value = 1;
            }
            lights[INPUT_LIGHT+i].setBrightness(value);

            value = 0;
            if(i < filter_count)
            {
                value = 1;
            }
            lights[FILTER_LIGHT+i].setBrightness(value);

        }

        /* Do Filters */

        for(int i = 0; i < input_count; ++i)
        {
            double x = inputs[POLY_INPUT].getVoltage(i);
            if(i < filter_count)
            {
                for(int j = 0; j < filter_order[i]; ++j)
                {
                    x = filters[i][j].step(x, 0);
                }
            }

            outputs[POLY_OUTPUT].setVoltage(x, i);
        }
        outputs[POLY_OUTPUT].setChannels(input_count);

    }

    json_t* dataToJson() override {
        json_t* rootJ = json_object();


        return rootJ;

    } 

    void dataFromJson(json_t* rootJ) override {

    } 

};


struct MatIWidget : ModuleWidget {
    MatIWidget(MatI* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/MatI.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));


        #define GRID(x,y) 15.24*(x-0.5), 15.24*(y)+3.28

        for(int i = 0; i < MAX_CHANNELS; ++i)
        {
            float ydx = 1.75+i*4.5f/(MAX_CHANNELS-1);
            addChild(createLightCentered<MediumLight<OrangeLight>>(
                    mm2px(Vec(GRID(0.875, ydx))), module, MatI::INPUT_LIGHT+i));
            addChild(createLightCentered<MediumLight<OrangeLight>>(
                    mm2px(Vec(GRID(1.125, ydx))), module, MatI::FILTER_LIGHT+i));
            
        }

        addInput(createInputCentered<PJ301MPort>(
                mm2px(Vec(GRID(1,1))), module, MatI::POLY_INPUT));


        addOutput(createOutputCentered<PJ301MPort>(
                mm2px(Vec(GRID(1,7))), module, MatI::POLY_OUTPUT));
 
    }
};


Model* modelMatI = createModel<MatI, MatIWidget>("MatI");
