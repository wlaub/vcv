#include "TechTechTechnologies.hpp"
#include <patch.hpp>

struct MetadataMain : Module {
    enum ParamIds {
        SAVE_BUTTON_PARAM,
        TIME_BUTTON_PARAM,
        TIME_SOURCE_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        SAVE_IN_INPUT,
        TIME_IN_INPUT,
        NUM_INPUTS
    };
    enum OutputIds {
        NUM_OUTPUTS
    };
    enum LightIds {
        FILE_LIGHT_LIGHT,
        TRIGGER_LIGHT_LIGHT,
        OVERRIDE_LIGHT_LIGHT,
        SAVE_LIGHT,
        TIME_LIGHT,
        NUM_LIGHTS
    };

    int trigger_time = 0;
    int save_request = 0;

    dsp::BooleanTrigger save_button, time_button;
    dsp::SchmittTrigger save_gate, time_gate;

    dsp::PulseGenerator save_pulse, time_pulse, save_holdoff;

    MetadataMain() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(SAVE_BUTTON_PARAM, 0.f, 1.f, 0.f, "Save the patch now");
        configParam(TIME_BUTTON_PARAM, 0.f, 1.f, 0.f, "Set the trigger time");
        configParam(TIME_SOURCE_PARAM, 0.f, 2.f, 0.f, "Select the time source for the recording timestamp");
    }

    void process(const ProcessArgs& args) override {

        bool save_enable = !save_holdoff.process(args.sampleTime);

        if( (   save_button.process(params[SAVE_BUTTON_PARAM].getValue() > 0.f) 
            || save_gate.process(inputs[SAVE_IN_INPUT].getVoltage()) )
            && save_enable
            ) 
        {
            save_request = 1;

            save_holdoff.trigger(10);
            save_pulse.trigger(1e-1);

        }

        lights[SAVE_LIGHT].setBrightness(0);
        if(save_pulse.process(args.sampleTime))
        {
            lights[SAVE_LIGHT].setBrightness(1);
        }


        if(    time_button.process(params[TIME_BUTTON_PARAM].getValue() > 0.f) 
            || time_gate.process(inputs[TIME_IN_INPUT].getVoltage())) 
        {
            trigger_time = time(0);
            time_pulse.trigger(1e-1);
        }

        lights[TIME_LIGHT].setBrightness(0);
        if(time_pulse.process(args.sampleTime))
        {
            lights[TIME_LIGHT].setBrightness(1);
        }

        float source = params[MetadataMain::TIME_SOURCE_PARAM].getValue();

        lights[FILE_LIGHT_LIGHT].setBrightness(0);
        lights[TRIGGER_LIGHT_LIGHT].setBrightness(0);
        lights[OVERRIDE_LIGHT_LIGHT].setBrightness(0);
        switch(int(source))
        {
            case 0: 
                lights[FILE_LIGHT_LIGHT].setBrightness(1);
            break;
            case 1: 
                lights[TRIGGER_LIGHT_LIGHT].setBrightness(1);
            break;
            case 2:
                lights[OVERRIDE_LIGHT_LIGHT].setBrightness(1);
            break;
            default:
            break;
        }
 


    }
};


struct MetadataMainWidget : ModuleWidget {
    TextField* desc_field;
    TextField* tags_field;
    TextField* auth_field;
    TextField* time_field;

    Label* time_label;

    MetadataMainWidget(MetadataMain* module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/MetadataMain.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        float nudge = 1.5875;
        float bump = 3.175*3;

        addParam(createParamCentered<LEDBezel>(mm2px(Vec(6.35, 26.15-nudge)), module, MetadataMain::SAVE_BUTTON_PARAM));
        addParam(createParamCentered<LEDBezel>(mm2px(Vec(31.75, 26.15-nudge)), module, MetadataMain::TIME_BUTTON_PARAM));

        addChild(createLightCentered<LEDBezelLight<GreenLight>>(mm2px(Vec(6.35, 26.15-nudge)), module, MetadataMain::SAVE_LIGHT));
        addChild(createLightCentered<LEDBezelLight<GreenLight>>(mm2px(Vec(31.75, 26.15-nudge)), module, MetadataMain::TIME_LIGHT));

        addParam(createParamCentered<CKSSThree>(mm2px(Vec(66.675-bump, 28.267-nudge)), module, MetadataMain::TIME_SOURCE_PARAM));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(19.05, 26.15-nudge)), module, MetadataMain::SAVE_IN_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(44.45, 26.15-nudge)), module, MetadataMain::TIME_IN_INPUT));



        addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(73.025-bump, 24.562-nudge)), module, MetadataMain::OVERRIDE_LIGHT_LIGHT));
        addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(73.025-bump, 28.267-nudge)), module, MetadataMain::TRIGGER_LIGHT_LIGHT));
        addChild(createLightCentered<MediumLight<GreenLight>>(mm2px(Vec(73.025-bump, 31.971-nudge)), module, MetadataMain::FILE_LIGHT_LIGHT));


        desc_field = createWidget<TextField>(mm2px(Vec(3.175, 35.675)));
        desc_field->box.size = mm2px(Vec(95.25, 31.75));
        desc_field->multiline = true;
        addChild(desc_field);

        tags_field = createWidget<TextField>(mm2px(Vec(3.175, 73.775)));
        tags_field->box.size = mm2px(Vec(95.25, 19.05));
        addChild(tags_field);

        auth_field = createWidget<TextField>(mm2px(Vec(3.175, 99.175)));
        auth_field->box.size = mm2px(Vec(95.25, 6.35));
        addChild(auth_field);

        time_field = createWidget<TextField>(mm2px(Vec(3.175, 111.875)));
        time_field->box.size = mm2px(Vec(95.25, 6.35));
        addChild(time_field);

        time_label = createWidget<Label>(mm2px(Vec(0.75, 111.875+5)));
        addChild(time_label);

    }

    void step() override {
        ModuleWidget::step();
        if(!module) return;

        MetadataMain* mod = ((MetadataMain*) module);
        
        char timestring[256];
        time_t timeval = mod->trigger_time;

        strftime(timestring, 256, "%Y-%m-%d %H:%M:%S", localtime(&timeval));

        char labeltext[256];
        sprintf(labeltext, "Trigger Time: %s", timestring);

        time_label->text = labeltext;

        if(mod->save_request == 1)
        {
            mod->save_request = 0;
            //save the patch here
//            APP->patch->save(asset::autosavePath);
//            printf("Save patch triggered from metadata main.\n");
//            APP->window->screenshot(1);
            APP->patch->saveDialog();
        }



    }

    json_t* toJson() override {
        json_t* rootJ = ModuleWidget::toJson();

        MetadataMain* mod = (MetadataMain*) module;

        json_object_set_new(rootJ, "description", json_string(desc_field->text.c_str()));
        json_object_set_new(rootJ, "tags", json_string(tags_field->text.c_str()));
        json_object_set_new(rootJ, "authors", json_string(auth_field->text.c_str()));
        json_object_set_new(rootJ, "override_time", json_string(time_field->text.c_str()));

        json_object_set_new(rootJ, "trigger_time", json_integer(mod->trigger_time));

        json_t* time_source = 0;
        float source = mod->params[MetadataMain::TIME_SOURCE_PARAM].getValue();

        switch(int(source))
        {
            case 0: 
                time_source = json_string("file");
            break;
            case 1: 
                time_source = json_string("trigger");
            break;
            case 2:
                time_source = json_string("override");
            break;
            default:
                time_source = json_string("error");
            break;
        }
       
        json_object_set_new(rootJ, "time_source", time_source);

        return rootJ;

    }

    void fromJson(json_t* rootJ) override {
        ModuleWidget::fromJson(rootJ);

        
        json_t* textJ;

        textJ = json_object_get(rootJ, "description");
        if(textJ) desc_field->text = json_string_value(textJ);

        textJ = json_object_get(rootJ, "tags");
        if(textJ) tags_field->text = json_string_value(textJ);

        textJ = json_object_get(rootJ, "authors");
        if(textJ) auth_field->text = json_string_value(textJ);

        textJ = json_object_get(rootJ, "override_time");
        if(textJ) time_field->text = json_string_value(textJ);

        json_t* ttime = json_object_get(rootJ, "trigger_time");
        if(ttime) ((MetadataMain*)module)->trigger_time = json_integer_value(ttime);
        

    }


};

Model* modelMetadataMain = createModel<MetadataMain, MetadataMainWidget>("MetadataMain");
