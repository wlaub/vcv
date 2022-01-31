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
        MIX_OUTPUT,
        OUTPUTS_LEN
    };
    enum LightId {
        ERROR_LIGHT,
        INPUT_LIGHT,
        FILTER_LIGHT = INPUT_LIGHT+MAX_CHANNELS,
        LIGHTS_LEN = FILTER_LIGHT+MAX_CHANNELS
    };

    //Index order is channel, stage
    struct ttt::Biquad*** filters;
    //Order is sample_rate_index, channel_stage
    struct ttt::Biquad**** all_filters;
    double* sample_rates;
    int sample_rate_index = 0;
    int sample_rate_count = 0;

    int filter_order[MAX_CHANNELS];
    int filter_count = 0;

    float sample_rate = 0;

    float sample_rate_error = 0;
    bool filters_valid = false;

    PathMemory pathmem;

    std::string default_filter_path = asset::plugin(pluginInstance, "res/mati_default.json");

    MatI() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

        configInput(POLY_INPUT, "Filter");

        configOutput(POLY_OUTPUT, "Polyphonic Filter");

        configOutput(MIX_OUTPUT, "Filter Mix");

        configBypass(POLY_INPUT, POLY_OUTPUT);

        load_default_filter();
    }

    void onSampleRateChange(const SampleRateChangeEvent& e) override {
        sample_rate = e.sampleRate;
        pick_filter();
	}

    void onReset(const ResetEvent& e) override
    {
        Module::onReset(e);
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

        if(!filters_valid || sample_rate_error > 1)
        {
            lights[ERROR_LIGHT].setBrightness(1);
        }
        else
        {
            lights[ERROR_LIGHT].setBrightness(0);
        }

        /* Do Filters */

        int max_count = std::max(input_count, filter_count);

        if(filters_valid)
        {
            double mix = 0;
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
                mix += x;
            }
            outputs[POLY_OUTPUT].setChannels(max_count);
            outputs[MIX_OUTPUT].setVoltage(mix);
        }

    }

    void pick_filter()
    {
        /*
        Find the filter that best matches the current sample rate
        */
        if(filter_count == 0)
        {
            return;
        }

        sample_rate = APP->engine->getSampleRate();

        int best_idx = 0;
        float best_delta = -1;
        for(int i = 0; i < sample_rate_count; ++i)
        {
            float delta = abs(sample_rates[i] - sample_rate);
            if(best_delta < 0 || delta < best_delta)
            {
                best_delta = delta;
                best_idx = i;
            }
        }

        sample_rate_error = best_delta;
        filters = all_filters[best_idx];
        if(sample_rate_error > 2)
        {
            printf("Warning: Mat I couldn't find a perfect filter match for sample rate %f. Closest match: %f. \n", sample_rate, sample_rates[best_idx]);
        }
    }

    json_t* dataToJson() override {
        json_t* rootJ = json_object();

        if(filter_count > 0)
        {
            json_object_set_new(rootJ, "filters", dump_filter_specs());
        }
        json_object_set_new(rootJ, "filter_file", pathmem.to_json());

        return rootJ;

    } 

    void dataFromJson(json_t* rootJ) override {
        json_t* spec_array = json_object_get(rootJ, "filters");
        if(!load_filter_specs(spec_array))
        {
            load_default_filter();
        }
        pathmem.from_json(json_object_get(rootJ, "filter_file"));
            
    } 



    json_t* dump_filter_specs()
    {
        json_t* spec_array = json_array();
        
        for(int i = 0; i < sample_rate_count; ++i)
        {
            json_t* spec = json_object();
            json_object_set_new(spec, "fs", json_real(sample_rates[i]));
            json_object_set_new(spec, "channels", dump_filters(all_filters[i]));
            json_array_append(spec_array, spec);
        }

        return spec_array;
    }

    json_t* dump_filters(struct ttt::Biquad*** tfilters) {
        json_t* filter_array = json_array();

        for(int i = 0; i < filter_count; ++i)
        {
            json_t* slices = json_array();
            for(int j = 0; j < filter_order[i]; ++j)
            {
                json_t* sos = json_array();
                for(int k = 0; k < 3; ++k)
                {
                    json_array_append(sos, json_real(tfilters[i][j]->b[k]));
                }
                for(int k = 0; k < 3; ++k)
                {
                    json_array_append(sos, json_real(tfilters[i][j]->a[k]));
                }
                json_array_append(slices, sos);
            }
            json_array_append(filter_array, slices);
        }

        return filter_array;
    }

    void load_default_filter() {
        load_filter_from_file(default_filter_path);
    }

    void load_filter_from_file(std::string filename) {
        json_t* rootJ = 0;
        json_t* filter_array = 0;

        json_error_t error;

        rootJ = json_load_file(filename.c_str(), 0, &error);
        json_t* spec_array = json_object_get(rootJ, "filters");

        load_filter_specs(spec_array);

    }



    bool load_filter_specs(json_t* spec_array)
    {
        int spec_count = json_array_size(spec_array);
        int filter_count = -1;

        if(!json_is_array(spec_array) || spec_count == 0)
        {
            osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK, "Not a valid filter specification: must contain a non-empty 'filters' array");
            return false;  
        }

        //Validate each filter specification first
        for(int i = 0; i < spec_count; ++i)
        {
            json_t* spec = json_array_get(spec_array, i);
            json_t* filter_array = json_object_get(spec, "channels");

            char message[2048];
            bool passed = true;

            //validate sample rate
            double fs = json_number_value(json_object_get(spec, "fs"));
            if(fs == 0)
            {
                sprintf(message, "A filter spec has invalid sample rate. fs must be specified and non-zero.");
                passed = false;
            }

            //validate channel count
            int tcount = json_array_size(filter_array);
            if(filter_count == -1)
            {
                filter_count = tcount;
            }
            if(tcount != filter_count)
            {
                sprintf(message, "All filter specs must include the same number of channels: %i != %i", filter_count, tcount);
                passed = false;
            }

            //validate structure
            bool valid_structure = validate_filters(filter_array);
            if(!valid_structure)
            {
                sprintf(message, "Invalid filter specification for fs = %d", fs);
                passed = false;
            }

            if(!passed)
            {
                osdialog_message(OSDIALOG_ERROR, OSDIALOG_OK, message);
                return false; 
            }

        }

        //If that passed, then load all the new filter specs
        filters_valid = false;

        sample_rate_count = spec_count;
        sample_rates = new double[spec_count];
        all_filters = new struct ttt::Biquad***[spec_count];

        for(int i = 0; i < spec_count; ++i)
        {
            json_t* spec = json_array_get(spec_array, i);
            json_t* filter_array = json_object_get(spec, "channels");
            double fs = json_number_value(json_object_get(spec, "fs"));
            sample_rates[i] = fs;
            all_filters[i] = new struct ttt::Biquad**[MAX_CHANNELS];

            load_filters(filter_array, all_filters[i]);
        }

        pick_filter();

        filters_valid = true;

        return true;

    }

    bool validate_filters(json_t* filter_array) 
    {
        /*
        Confirm that the given array channels array from a filter spec contains
        only valid sos.
        */

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

    void load_filters(json_t* filter_array, struct ttt::Biquad*** tfilters) {

        filter_count = json_array_size(filter_array);
 
        for(int i = 0; i < filter_count; ++i)
        {
            json_t* slices = json_array_get(filter_array, i);
            int order = json_array_size(slices);
            filter_order[i] = order;
            tfilters[i] = new ttt::Biquad*[order];
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

                tfilters[i][j] = new struct ttt::Biquad(1, sos);
            }

        }
    }

};


struct MatIWidget : ModuleWidget {


    void appendContextMenu(Menu* menu) override {
            MatI* module = dynamic_cast<MatI*>(this->module);

            menu->addChild(new MenuEntry);

            struct ReloadItem : MenuItem {
                MatI* module;
                void onAction(const event::Action& e) override {
                    std::string path = module->pathmem.get_path();

                    if(!path.empty())
                    {
                        module->load_filter_from_file(path.c_str());
                    }
                    else
                    {
                        module->load_default_filter();
                    }
                }
            };

            ReloadItem* reload_item = createMenuItem<ReloadItem>("Reload Filter");
            reload_item->module = module;
            menu->addChild(reload_item);

            struct LoadItem : MenuItem {
                MatI* module;
                void onAction(const event::Action& e) override {
                    char* path = module->pathmem.file_dialog(
                        OSDIALOG_OPEN, 
                        asset::user("patches").c_str(), NULL, 
                        "Filter Spec:json;All Files:py");

                    if(path)
                    {
                        module->load_filter_from_file(path);
                        free(path);
                    }
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

        #define DY (3.5f/(MAX_CHANNELS-1+2))

        for(int i = 0; i < MAX_CHANNELS; ++i)
        {
            float ydx = 1.75+i*DY;
            addChild(createLightCentered<SmallLight<OrangeLight>>(
                    mm2px(Vec(GRID(1-DY/2, ydx))), module, MatI::INPUT_LIGHT+i));
            addChild(createLightCentered<SmallLight<OrangeLight>>(
                    mm2px(Vec(GRID(1+DY/2, ydx))), module, MatI::FILTER_LIGHT+i));
            
        }
        addChild(createLightCentered<MediumLight<RedLight>>(
                mm2px(Vec(GRID(1, 5.33))), module, MatI::ERROR_LIGHT));
 

        addInput(createInputCentered<PJ301MPort>(
                mm2px(Vec(GRID(1,1))), module, MatI::POLY_INPUT));


        addOutput(createOutputCentered<PJ301MPort>(
                mm2px(Vec(GRID(1,6))), module, MatI::POLY_OUTPUT));

        addOutput(createOutputCentered<PJ301MPort>(
                mm2px(Vec(GRID(1,7))), module, MatI::MIX_OUTPUT));


 
    }
};


Model* modelMatI = createModel<MatI, MatIWidget>("MatI");
