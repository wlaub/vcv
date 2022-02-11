#include "TechTechTechnologies.hpp"
#include <numeric>

#define N 6

struct GateState {

    int pending = 0;
    int value = 0;

    json_t* to_json()
    {
        json_t* result = json_object();
        
        json_object_set_new(result, "pending", json_integer(pending));
        json_object_set_new(result, "value", json_integer(value));

        return result;
    }
    
    void from_json(json_t* rootJ)
    {
        if(rootJ == 0) return;

        json_t* temp;

        temp = json_object_get(rootJ, "pending");
        if(temp) pending = json_integer_value(temp);

        temp = json_object_get(rootJ, "value");
        if(temp) value = json_integer_value(temp);

    }

};

struct Once : Module {
    enum ParamId {
        ENABLE_PARAM,
        BUTTON_PARAM,
        PARAMS_LEN = BUTTON_PARAM+N
    };
    enum InputId {
        CLK_INPUT,
        INPUTS_LEN
    };
    enum OutputId {
        GATE_OUTPUT,
        OUTPUTS_LEN = GATE_OUTPUT+N
    };
    enum LightId {
        ENABLE_LIGHT,
        BUTTON_LIGHT = ENABLE_LIGHT+3,
        LIGHTS_LEN = BUTTON_LIGHT+N*3
    };

    dsp::SchmittTrigger clk_trigger, en_trigger;

    dsp::SchmittTrigger butt_trigger[N];
    struct GateState states[N];

    int enabled = 1;

    float blink_counter = 0;
    int blink = 0;

    Once() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

        configButton(ENABLE_PARAM, "Enable");

        for(int i = 0; i < N; ++i)
        {
            configOutput(GATE_OUTPUT, "Gate");
            configButton(BUTTON_PARAM, "Activate");
        }

        configInput(CLK_INPUT, "Clock");

    }

    void set_light(int index, float r, float g, float b)
    {
        lights[index].setBrightness(r);
        lights[index+1].setBrightness(g);
        lights[index+2].setBrightness(b);
    }

    void process(const ProcessArgs& args) override {

        float deltaTime = args.sampleTime;       

        blink_counter += deltaTime;
        if(blink_counter > 0.5)
        {
            blink_counter -= 0.5;
            blink = 1-blink;
        }

        /* Handle button presses */

        for(int i = 0; i < N; ++i)
        {
            if(butt_trigger[i].process(params[BUTTON_PARAM+i].getValue()))
            {
                ++states[i].pending;
                states[i].pending %= 2;
            }

        }

        if(en_trigger.process(params[ENABLE_PARAM].getValue()))
        {
            enabled = 1 - enabled;
        }

        /* Handle clock edges */

        if(clk_trigger.process(inputs[CLK_INPUT].getVoltage()) && enabled)
        {
            for(int i = 0; i < N; ++i)
            {
                if(states[i].pending == 1)
                {
                    states[i].pending = 0;
                    states[i].value = 1 - states[i].value;
                }
            }
        }

        for(int i = 0; i < N; ++i)
        {
            outputs[GATE_OUTPUT+i].setVoltage(states[i].value*5);
        }


        /* Handle lights */

        if(enabled)
        {
            set_light(ENABLE_LIGHT, 0,1,0);
        }
        else
        {
            set_light(ENABLE_LIGHT, 0,0,0);
        }

        for(int i = 0; i < N; ++i)
        {
            int idx = BUTTON_LIGHT+3*i;
            if(states[i].pending == 1)
            {
                if(states[i].value == 0)
                {
                    set_light(idx, 0, 1, 0);
                }
                else
                {
                    set_light(idx, 0, 0, 1);
                }
            }
            else
            {
                if(states[i].value == 1)
                {
                    set_light(idx, 1, 1, 1);
                }
                else
                {
                    set_light(idx, 0, 0, 0);
                }
 
            }
        }

    }

    json_t* dataToJson() override {
        json_t* rootJ = json_object();

        json_object_set_new(rootJ, "enabled", json_integer(enabled));

        json_t* array = json_array();
        json_object_set_new(rootJ, "states", array);

        for(int i = 0; i < N; ++i)
        {
            json_array_append(array, states[i].to_json());
        }

        return rootJ;

    } 

    void dataFromJson(json_t* rootJ) override {

        json_t* temp;

        temp = json_object_get(rootJ, "enabled");
        if(temp) enabled = json_integer_value(temp);

        json_t* array = json_object_get(rootJ, "states");
        if(array)
        {
            for(int i = 0; i < N; ++i)
            {
                temp = json_array_get(array, i);
                states[i].from_json(temp);
            }
        }

    } 



};

#define GRIDX(x) 15.24*(x-0.5)
#define GRIDY(y) 15.24*(y)+3.28
#define GRID(x,y) GRIDX(x), GRIDY(y)

struct OnceWidget : ModuleWidget {

    void appendContextMenu(Menu* menu) override {
            Once* module = dynamic_cast<Once*>(this->module);

            menu->addChild(new MenuEntry);
/*
            struct NormalizeItem : MenuItem {
                Once* module;
                void onAction(const event::Action& e) override {
                    module->normalize = 1-module->normalize;
                }
            };
            NormalizeItem* nitem = createMenuItem<NormalizeItem>("Normalize Outputs");
            nitem->module = module;
            nitem->rightText = CHECKMARK(module->normalize);
            menu->addChild(nitem);

            struct ShowLabelsItem : MenuItem {
                Once* module;
                OnceWidget* widget;
                void onAction(const event::Action& e) override {
                    module->show_labels = 1-module->show_labels;
                    if(!module->show_labels)
                    {
                        widget->clear_labels();
                    }
                }
            };
            ShowLabelsItem* slitem = createMenuItem<ShowLabelsItem>("Show Periods");
            slitem->module = module;
            slitem->widget = this;
            slitem->rightText = CHECKMARK(module->show_labels);
            menu->addChild(slitem);
*/
            /* Reset Modes*/
/*
            menu->addChild(createMenuLabel("Reset Mode"));
            
            struct ResetItem : MenuItem {
                Once* module;
                int mode;
                void onAction(const event::Action& e) override {
                    module->reset_mode = mode;
                }
            };

            std::string rmode_names[Once::RMODE_LEN];
            rmode_names[Once::RMODE_TRIG] = "Trigger";
            rmode_names[Once::RMODE_HOLD] = "Hold";
            rmode_names[Once::RMODE_ENABLE] = "Enable";

            int rmode_sequence[Once::RMODE_LEN] = {Once::RMODE_TRIG, Once::RMODE_HOLD, Once::RMODE_ENABLE};
 
            for(int i = 0; i < Once::RMODE_LEN; ++i)
            {
                int idx = rmode_sequence[i];
                ResetItem* reset_item = createMenuItem<ResetItem>(rmode_names[idx]);
                reset_item->module = module;
                reset_item->mode = idx;
                reset_item->rightText = CHECKMARK(module->reset_mode == idx);
                menu->addChild(reset_item);
            }
*/
        }


    OnceWidget(Once* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/Once.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));



        for(int i = 0; i < N; ++i)
        {
            addParam(createParamCentered<LEDBezel>(
                    mm2px(Vec(GRID(1, i+2))), module, Once::BUTTON_PARAM+i));

            addChild(createLightCentered<LEDBezelLight<RedGreenBlueLight>>(
                    mm2px(Vec(GRID(1, i+2))), module, Once::BUTTON_LIGHT+3*i));

            addOutput(createOutputCentered<PJ301MPort>(
                mm2px(Vec(GRID(2, i+2))), module, Once::GATE_OUTPUT+i));

        }

        addInput(createInputCentered<PJ301MPort>(
            mm2px(Vec(GRID(1,1))), module, Once::CLK_INPUT));

        addParam(createParamCentered<LEDBezel>(
                mm2px(Vec(GRID(2, 1))), module, Once::ENABLE_PARAM));

        addChild(createLightCentered<LEDBezelLight<RedGreenBlueLight>>(
                mm2px(Vec(GRID(2, 1))), module, Once::ENABLE_LIGHT));


    }
};


Model* modelOnce = createModel<Once, OnceWidget>("Once");
