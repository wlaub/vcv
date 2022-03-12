#include "TechTechTechnologies.hpp"
#include <numeric>

#define MAX_CHANNELS 16

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



struct LachesisI : Module {
    enum ParamId {
        INC_PARAM,
        PARAMS_LEN
    };
    enum InputId {
        CLOCK_INPUT,
        RESET_INPUT,
        INPUTS_LEN
    };
    enum OutputId {
        CV_OUTPUT,
        OUTPUTS_LEN
    };
    enum LightId {
        LIGHTS_LEN
    };

    int count = 0;
    float accumulator = 0;

    int panel_update = 1;

    dsp::SchmittTrigger rclock_trigger[MAX_CHANNELS];
    dsp::SchmittTrigger fclock_trigger[MAX_CHANNELS];
    dsp::SchmittTrigger reset_trigger[MAX_CHANNELS];

    struct EdgeMode edge_mode;

    LachesisI() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
        configParam(INC_PARAM, -10.f, 10.f, 1.f, "CV Output Increment");

        configInput(CLOCK_INPUT, "Clock");
        configInput(RESET_INPUT, "Reset");
        configOutput(CV_OUTPUT, "Count CV");

    }

    void process(const ProcessArgs& args) override {

        float deltaTime = args.sampleTime;       

        /* Handle Reset */

        for(int c = 0; c < MAX_CHANNELS; ++c)
        {
            float reset_val = inputs[RESET_INPUT].getVoltage(c);
            if(reset_trigger[c].process(reset_val))
            {
                count = 0;
                accumulator = 0;
                panel_update = 1;
            }
        }

        /* Handle Count */

        int increment = 0;
        int rising_increment = 0;
        int falling_increment = 0;
        for(int c = 0; c < MAX_CHANNELS; ++c)
        {
            float clock_val = inputs[CLOCK_INPUT].getVoltage(c);
            float rclock = clock_val > 0.5 ? 10:0;
            float fclock = clock_val < 0.5? 10:0;
            if(rclock_trigger[c].process(rclock))
            {
                rising_increment = 1;
            }
            if(fclock_trigger[c].process(fclock))
            {
                falling_increment = 1;
            }
 
        }


        if(edge_mode.mode != EdgeMode::FALLING_MODE)
        {
            increment |= rising_increment;
        }
        if(edge_mode.mode != EdgeMode::RISING_MODE)
        {
            increment |= falling_increment;
        }


        if(increment != 0)
        {
            accumulator += increment * params[INC_PARAM].getValue();
            accumulator = clamp(accumulator, -10.f, 10.f);
            count += increment;
            panel_update = 1;
        }

        outputs[CV_OUTPUT].setVoltage(accumulator);

    }

    json_t* dataToJson() override {
        json_t* rootJ = json_object();

        json_object_set_new(rootJ, "count", json_integer(count));
        json_object_set_new(rootJ, "accumulator", json_real(accumulator));       

        json_object_set_new(rootJ, "edge_mode", edge_mode.to_json());

        return rootJ;

    } 

    void dataFromJson(json_t* rootJ) override {

        json_t* temp;

        temp = json_object_get(rootJ, "count");
        if(temp) count = json_integer_value(temp);

        temp = json_object_get(rootJ, "accumulator");
        if(temp) accumulator = json_real_value(temp);

        edge_mode.from_json(json_object_get(rootJ, "edge_mode"));

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
#define GRIDY(y) 15.24*(y-0.5)+3.28
#define GRID(x,y) GRIDX(x), GRIDY(y)
#define NLABELS 2

struct LachesisIWidget : ModuleWidget {
    AlignLabel* count_labels[NLABELS];
 

    void clear_label(int i)
    {
        count_labels[i]->text = "";
    }

    void clear_labels()
    {
        for(int i = 0; i < NLABELS; ++i)
        {
            clear_label(i);
        }
    }

    void edge_mode_menu(Menu* menu, LachesisI* module)
    {
            menu->addChild(createMenuLabel("Clock Mode"));

            struct ModeItem : MenuItem {
                LachesisI* module;
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
            LachesisI* module = dynamic_cast<LachesisI*>(this->module);

            menu->addChild(new MenuEntry);

            edge_mode_menu(menu, module);

        }


    void step() override {
        ModuleWidget::step();
        if(!module) return;

        LachesisI* mod = ((LachesisI*) module);
        
        if(mod->panel_update)
        {
            char count_str[16];
            sprintf(count_str, "%i", mod->count);

            for(int i = 0; i < NLABELS; ++i)
            {
                count_labels[i]->text = count_str;
            }

            mod->panel_update = 0;
        }

    } 

    LachesisIWidget(LachesisI* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/Lachesis.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));



        for(int i = 0; i < NLABELS; ++i)
        {
            float xpos = GRIDX(1);
            float ypos = 4  + (i*2-1)*(3);

            count_labels[i] = createWidget<AlignLabel>(
                mm2px(Vec(xpos, GRIDY(ypos+.25))));
            count_labels[i]->alignment = Label::CENTER_ALIGNMENT;
            count_labels[i]->color = nvgRGB(0,0,0);
            count_labels[i]->text="0";
            addChild(count_labels[i]);

 
        }

        addParam(createParamCentered<RoundBlackKnob>(
            mm2px(Vec(GRID(1, 5))), module, LachesisI::INC_PARAM));

        addInput(createInputCentered<PJ301MPort>(
            mm2px(Vec(GRID(1,3))), module, LachesisI::CLOCK_INPUT));
        addInput(createInputCentered<PJ301MPort>(
            mm2px(Vec(GRID(1,4))), module, LachesisI::RESET_INPUT));

        addOutput(createOutputCentered<PJ301MPort>(
            mm2px(Vec(GRID(1,6))), module, LachesisI::CV_OUTPUT));
    }
};


Model* modelLachesisI = createModel<LachesisI, LachesisIWidget>("LachesisI");
