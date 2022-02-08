#include "Tia.hpp"

#define N 7
#define IDX(row, col) row*N+col

struct TiaIExpander : Module {
    enum ParamId {
       PARAMS_LEN
    };
    enum InputId {
        SIGNAL_INPUT,
        INPUTS_LEN = SIGNAL_INPUT + 7*N
    };
    enum OutputId {
        POLY_OUTPUT,
        OUTPUTS_LEN = POLY_OUTPUT + N
    };
    enum LightId {
        LIGHTS_LEN
    };


    TiaIExpander() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

        for(int row = 0; row < N; ++row)
        {
            for(int col = 0; col < 7; ++col)
            {
                configInput(SIGNAL_INPUT+IDX(row, col), "Source");
            }

            configOutput(POLY_OUTPUT+row, "Crossfader Outputs (Polyphonic)");
        }

    }

    void process(const ProcessArgs& args) override {
        Expander* expander = 0;
        if(rightExpander.module) expander = &rightExpander;
        else if(leftExpander.module) expander = &leftExpander;
        if(!expander) return;

        if(expander->module->model != modelTiaI) return;

        TiaMessage* message = reinterpret_cast<TiaMessage*>(expander->module->leftExpander.consumerMessage);

        for(int row = 0; row < N; ++row)
        {
            if(outputs[POLY_OUTPUT+row].active)
            {
                for(int i = 0; i < 7; ++i)
                {

                    double fade = message->faders[i];

                    double bot = 0;
                    double top = 0;
                    if(message->top_select[i] < 7)
                    {
                        double top_gain = message->gains[message->top_select[i]];
                        top = inputs[SIGNAL_INPUT+IDX(row, message->top_select[i])].getVoltage() * top_gain;
                    }
                    if(message->bot_select[i] < 7)
                    {
                        double bot_gain = message->gains[message->bot_select[i]];
                        bot = inputs[SIGNAL_INPUT+IDX(row, message->bot_select[i])].getVoltage() * bot_gain;
                    }

                    double mix = top*fade + bot*(1-fade);
                    outputs[POLY_OUTPUT+row].setVoltage(mix, i);
     
                }
                outputs[POLY_OUTPUT+row].setChannels(7);
            }
        }


    }

};


struct TiaIExpanderWidget : ModuleWidget {



    TiaIExpanderWidget(TiaIExpander* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/TiaExpander.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH*2.5, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 3.5 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH*2.5, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 3.5 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));


        #define GRID(x,y) 15.24*(x-0.5), 15.24*(y)+3.28

        for(int row = 0; row < N; ++row)
        {
            for(int col = 0; col < 7; ++col)
            {

                addInput(createInputCentered<PJ301MPort>(
                        mm2px(Vec(GRID(col+1, row+1))), module, TiaIExpander::SIGNAL_INPUT+IDX(row,col)));
         
            }

            addOutput(createOutputCentered<PJ301MPort>(
                    mm2px(Vec(GRID(8,row+1))), module, TiaIExpander::POLY_OUTPUT+row));
        }
    }
};


Model* modelTiaIExpander = createModel<TiaIExpander, TiaIExpanderWidget>("TiaIExpander");
