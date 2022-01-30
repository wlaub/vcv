#include "TechTechTechnologies.hpp"

#include <osdialog.h>

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

    struct ttt::Biquad** filters[MAX_CHANNELS];
    int filter_order[MAX_CHANNELS];
    int filter_count = 0;

    MatI() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

        configInput(POLY_INPUT, "Filter");

        configOutput(POLY_OUTPUT, "Filter");

        load_default_filter();
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

        int max_count = std::max(input_count, filter_count);

        for(int i = 0; i < max_count; ++i)
        {
            double x = inputs[POLY_INPUT].getPolyVoltage(i);
            if(i < filter_count)
            {
                for(int j = 0; j < filter_order[i]; ++j)
                {
                    x = filters[i][j]->step(x, 0);
                }
            }

            outputs[POLY_OUTPUT].setVoltage(x, i);
        }
        outputs[POLY_OUTPUT].setChannels(max_count);

    }

    json_t* dataToJson() override {
        json_t* rootJ = json_object();

        if(filter_count > 0)
        {
            json_object_set_new(rootJ, "filters", dump_filters());
        }

        return rootJ;

    } 

    void dataFromJson(json_t* rootJ) override {
        json_t* filter_array = json_object_get(rootJ, "filters");
        if(filter_array == 0)
        {
            load_default_filter();
        }
        else
        {
            load_filters(filter_array);
        }

    } 

    void load_default_filter() {
        std::string filename = asset::plugin(pluginInstance, "res/mati_default.json");
        load_filter_from_file(filename);
    }

    void load_filter_from_file(std::string filename) {
        json_t* rootJ = 0;
        json_t* filter_array = 0;

        json_error_t error;

        rootJ = json_load_file(filename.c_str(), 0, &error);
        filter_array = json_object_get(rootJ, "filters");

        if(!validate_filter(filter_array))
        {
            char message[2048];
            sprintf(message, "%s is not a valid filter specification", filename.c_str());
            osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK, message);
        }
        else
        {
            load_filters(filter_array);
        }
    }

    json_t* dump_filters() {
        json_t* filter_array = json_array();

        for(int i = 0; i < filter_count; ++i)
        {
            json_t* slices = json_array();
            for(int j = 0; j < filter_order[i]; ++j)
            {
                json_t* sos = json_array();
                for(int k = 0; k < 3; ++k)
                {
                    json_array_append(sos, json_real(filters[i][j]->b[k]));
                }
                for(int k = 0; k < 3; ++k)
                {
                    json_array_append(sos, json_real(filters[i][j]->a[k]));
                }
                json_array_append(slices, sos);
            }
            json_array_append(filter_array, slices);
        }

        return filter_array;
    }

    bool validate_filter(json_t* filter_array) {

        if(!filter_array || !json_is_array(filter_array)) 
        {
            return false;
        }


        filter_count = json_array_size(filter_array);
 
        for(int i = 0; i < filter_count; ++i)
        {
            json_t* slices = json_array_get(filter_array, i);
            if(!json_is_array(slices))
            {
                return false;
            }

            int order = json_array_size(slices);
            for(int j = 0; j < order; ++j)
            {
                json_t* sos_array = json_array_get(slices, j);
                if(json_array_size(sos_array) != 6)
                {
                    return false;
                }

                for(int k = 0; k < 6; ++k)
                {
                    if(!json_is_number(json_array_get(sos_array, k)))
                    {
                        return false;
                    }
                }

            }

        }
        return true;
       
    }

    void load_filters(json_t* filter_array) {

        filter_count = json_array_size(filter_array);
 
        for(int i = 0; i < filter_count; ++i)
        {
            json_t* slices = json_array_get(filter_array, i);
            int order = json_array_size(slices);
            filter_order[i] = order;
            filters[i] = new ttt::Biquad*[order];
            for(int j = 0; j < order; ++j)
            {
                json_t* sos_array = json_array_get(slices, j);
                if(json_array_size(sos_array) != 6)
                {
                    printf("Warning: filter specification is malformed at filter %i slice %i\n", i, j);
                }

                double sos[6];
                for(int k = 0; k < 6; ++k)
                {
                    sos[k] = json_number_value(json_array_get(sos_array, k));
                }

                filters[i][j] = new struct ttt::Biquad(1, sos);
            }

        }


    }

};


struct MatIWidget : ModuleWidget {


    void appendContextMenu(Menu* menu) override {
            MatI* module = dynamic_cast<MatI*>(this->module);

            menu->addChild(new MenuEntry);

            struct LoadItem : MenuItem {
                MatI* module;
                std::string dir = asset::user("patches");
                void onAction(const event::Action& e) override {
                    char* path = osdialog_file(OSDIALOG_OPEN, 
                        dir.c_str(), NULL, osdialog_filters_parse("Filter Spec:json"));
                    module->load_filter_from_file(path);
                }
            };

            LoadItem* tItem = createMenuItem<LoadItem>("Load Filter Spec");
            tItem->module = module;
            menu->addChild(tItem); 

        }

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
