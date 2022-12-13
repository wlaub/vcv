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


struct ButtonMode{
    enum Mode {
        TOGGLE_MODE,
        TRIGGER_MODE,
        PASS_MODE,
        PASS_TRIGGER_MODE,
        MODES_LEN,
    };

    int mode = TOGGLE_MODE;

    json_t* to_json()
    {
        return json_integer(mode);
    }

    void from_json(json_t* rootJ)
    {
        if(rootJ) mode = json_integer_value(rootJ);
    }



};

struct EdgeMode{
    enum Mode {
        RISING_MODE,
        FALLING_MODE,
        BOTH_MODE,
        MODES_LEN,
    };
    
    int mode = RISING_MODE;

    json_t* to_json()
    {
        return json_integer(mode);
    }

    void from_json(json_t* rootJ)
    {
        if(rootJ) mode = json_integer_value(rootJ);
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

    dsp::SchmittTrigger clk_trigger, nclk_trigger, en_trigger;
    dsp::PulseGenerator clk_pulse, vis_pulse;

    dsp::SchmittTrigger butt_trigger[N];
    struct GateState states[N];

    int enabled = 1;

    float blink_counter = 0;
    int blink = 0;

    struct ButtonMode button_mode;
    struct EdgeMode edge_mode;

    Once() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

        configButton(ENABLE_PARAM, "Enable");

        for(int i = 0; i < N; ++i)
        {
            configOutput(GATE_OUTPUT+i, "Gate");
            configButton(BUTTON_PARAM+i, "Activate");
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
/*
        blink_counter += deltaTime;
        if(blink_counter > 0.5)
        {
            blink_counter -= 0.5;
            blink = 1-blink;
        }
*/
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

        float clk_value = inputs[CLK_INPUT].getVoltage();
        int clk = clk_value > 0.5? 1:0;
        int rclk = clk_trigger.process(clk_value);
        int fclk = nclk_trigger.process(!clk_trigger.state);
        
        int clk_trigger_value = 0;
        if(edge_mode.mode != EdgeMode::FALLING_MODE)
        {
            clk_trigger_value |= rclk;
        }
        if(edge_mode.mode != EdgeMode::RISING_MODE)
        {
            clk_trigger_value |= fclk;
        }


        if(clk_trigger_value && enabled)
        {
            vis_pulse.trigger(0.1);

            clk_pulse.trigger();
            for(int i = 0; i < N; ++i)
            {
                if(states[i].pending == 1)
                {
                    states[i].pending = 0;
                    states[i].value = 1 - states[i].value;
                }
            }
        }

        int clk_pulse_value = clk_pulse.process(args.sampleTime);
        int vis_pulse_value = vis_pulse.process(args.sampleTime);

        for(int i = 0; i < N; ++i)
        {
            float out_val = 0;
            if(button_mode.mode == ButtonMode::TRIGGER_MODE)
            {
                if(states[i].value)
                {
                    out_val = clk_pulse_value;
                }
                if(clk_pulse_value == 0)
                {
                    states[i].value = 0;
                }
            }
            else if(button_mode.mode == ButtonMode::PASS_TRIGGER_MODE)
            {
                if(states[i].value)
                {
                    out_val = clk_pulse_value;
                }
            }
            else if(button_mode.mode == ButtonMode::PASS_MODE)
            {
                if(states[i].value)
                {
                    out_val = clk;
                }
            }
            else
            {
                out_val = states[i].value;
            }

            outputs[GATE_OUTPUT+i].setVoltage(out_val*5);
        }

        /* Handle lights */

        if(enabled)
        {
            if(button_mode.mode ==  ButtonMode::TOGGLE_MODE)
            {
                set_light(ENABLE_LIGHT, clk,1,clk);
            }
            else if(button_mode.mode ==  ButtonMode::TRIGGER_MODE)
            {
                set_light(ENABLE_LIGHT, vis_pulse_value, 1, vis_pulse_value);
            }
            else if(button_mode.mode ==  ButtonMode::PASS_MODE)
            {
                set_light(ENABLE_LIGHT, clk, clk, 1);
            }
            else if(button_mode.mode ==  ButtonMode::PASS_TRIGGER_MODE)
            {
                set_light(ENABLE_LIGHT, vis_pulse_value, vis_pulse_value, 1);
            }
 
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

        json_object_set_new(rootJ, "button_mode", button_mode.to_json());
        json_object_set_new(rootJ, "edge_mode", edge_mode.to_json());

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

        button_mode.from_json(json_object_get(rootJ, "button_mode"));
        edge_mode.from_json(json_object_get(rootJ, "edge_mode"));

    } 



};

#define GRIDX(x) 15.24*(x-0.5)
#define GRIDY(y) 15.24*(y)+3.28
#define GRID(x,y) GRIDX(x), GRIDY(y)

struct OnceWidget : PngWidget {

    void button_mode_menu(Menu* menu, Once* module)
    {
            menu->addChild(createMenuLabel("Button Mode"));

            struct ModeItem : MenuItem {
                Once* module;
                int mode;
                void onAction(const event::Action& e) override {
                    module->button_mode.mode = mode;
                }
            };

            std::string mode_names[ButtonMode::MODES_LEN];
            mode_names[ButtonMode::TOGGLE_MODE] = "Toggle Gate";
            mode_names[ButtonMode::TRIGGER_MODE] = "Pulse";
            mode_names[ButtonMode::PASS_MODE] = "Pass Clock";
            mode_names[ButtonMode::PASS_TRIGGER_MODE] = "Pulse on Clock";

            int mode_sequence[ButtonMode::MODES_LEN] = {ButtonMode::TOGGLE_MODE, ButtonMode::TRIGGER_MODE, ButtonMode::PASS_MODE, ButtonMode::PASS_TRIGGER_MODE};
 
            for(int i = 0; i < ButtonMode::MODES_LEN; ++i)
            {
                int idx = mode_sequence[i];
                ModeItem* item = createMenuItem<ModeItem>(mode_names[idx]);
                item->module = module;
                item->mode = idx;
                item->rightText = CHECKMARK(module->button_mode.mode == idx);
                menu->addChild(item);
            }
 
    }

    void edge_mode_menu(Menu* menu, Once* module)
    {
            menu->addChild(createMenuLabel("Clock Mode"));

            struct ModeItem : MenuItem {
                Once* module;
                int mode;
                void onAction(const event::Action& e) override {
                    module->edge_mode.mode = mode;
                }
            };

            std::string mode_names[EdgeMode::MODES_LEN];
            mode_names[EdgeMode::RISING_MODE] = "Rising Edge";
            mode_names[EdgeMode::FALLING_MODE] = "Falling Edge";
            mode_names[EdgeMode::BOTH_MODE] = "Both Edges";

            int mode_sequence[EdgeMode::MODES_LEN] = {EdgeMode::RISING_MODE, EdgeMode::FALLING_MODE, EdgeMode::BOTH_MODE};
 
            for(int i = 0; i < EdgeMode::MODES_LEN; ++i)
            {
                int idx = mode_sequence[i];
                ModeItem* item = createMenuItem<ModeItem>(mode_names[idx]);
                item->module = module;
                item->mode = idx;
                item->rightText = CHECKMARK(module->edge_mode.mode == idx);
                menu->addChild(item);
            }
 
    }



    void appendContextMenu(Menu* menu) override {
            Once* module = dynamic_cast<Once*>(this->module);

            menu->addChild(new MenuEntry);

            button_mode_menu(menu, module);
            edge_mode_menu(menu, module);
        }


    OnceWidget(Once* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/Once.svg")));

        png_path = "res/once_b.png";

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
