#include "TechTechTechnologies.hpp"

#include <osdialog.h>

#include <chrono>

#define EDGE_PULSE 1e-3
#define LOGIC_HIGH 5

struct DecayMeasurement 
{

    long timestamp = 0;
    float sample_period = 0;
    float duration = 0;
    float threshold_low = 0;
    float threshold_high = 0;
    float pulse_holdoff = 0;
    float pulse_length = 0;
    float pulse_spacing = 0;
    float bias_max = 0;
    float bias_min = 0;
    double bias_avg = 0;

    int valid = 1;

    int pulse_count;
    char custom_data[256] = {0};

    int equals(DecayMeasurement* other)
    {
        return timestamp == other->timestamp;
    }

    json_t* toJson()
    {
        json_t* rootJ = json_object();

        json_object_set_new(rootJ, "timestamp", json_integer(timestamp));
        json_object_set_new(rootJ, "valid", json_integer(valid));
        json_object_set_new(rootJ, "duration", json_real(duration));
        json_object_set_new(rootJ, "bias_min", json_real(bias_min));
        json_object_set_new(rootJ, "bias_max", json_real(bias_max));
        json_object_set_new(rootJ, "bias_avg", json_real(bias_avg));
        json_object_set_new(rootJ, "sample_period", json_real(sample_period));
        json_object_set_new(rootJ, "custom_data", json_string(custom_data));

        json_t* config = json_object();
        json_object_set_new(rootJ, "config", config);

        json_object_set_new(config, "threshold_low", json_real(threshold_low));
        json_object_set_new(config, "threshold_high", json_real(threshold_high));
        json_object_set_new(config, "pulse_count", json_integer(pulse_count));
        json_object_set_new(config, "pulse_length", json_real(pulse_length));
        json_object_set_new(config, "pulse_spacing", json_real(pulse_spacing));
        json_object_set_new(config, "pulse_holdoff", json_real(pulse_holdoff));

        return rootJ;
    }

    long get_json_int(json_t* rootJ, const char* key, int def=-1)
    {
        json_t* obj = json_object_get(rootJ, key);
        if(obj)
        {
            return json_integer_value(obj);
        }
        return def;
    }
    float get_json_float(json_t* rootJ, const char* key, float def=-1)
    {
        json_t* obj = json_object_get(rootJ, key);
        if(obj)
        {
            return json_real_value(obj);
        }
        return def;
    }


    void fromJson(json_t* rootJ)
    {

        json_t* config;
        json_t* obj;

        timestamp = get_json_int(rootJ, "timestamp");
        duration = get_json_float(rootJ, "duration");
        sample_period = get_json_float(rootJ, "sample_period");
        bias_max = get_json_float(rootJ, "bias_max");
        bias_min = get_json_float(rootJ, "bias_min");
        bias_avg = get_json_float(rootJ, "bias_avg");
        valid = get_json_int(rootJ, "valid");
 
        obj = json_object_get(rootJ, "custom_data");
        if(obj)
        {
            sprintf(custom_data, json_string_value(obj));
        }

        config = json_object_get(rootJ, "config");
        if(!config) return;
        
        pulse_count = get_json_int(config, "pulse_count");
        threshold_low = get_json_float(config, "threshold_low");
        threshold_high = get_json_float(config, "threshold_high");
        pulse_length = get_json_float(config, "pulse_length");
        pulse_holdoff = get_json_float(config, "pulse_holdoff");
        pulse_spacing = get_json_float(config, "pulse_spacing");

    }

};

struct DecayTimer : Module {
    enum ParamIds {
        THRESHOLD_PARAM,
        HYSTERESIS_PARAM,
        RESET_BUTTON_PARAM,
        MANUAL_TRIGGER_PARAM,
        HOLDOFF_PARAM,
        POST_PULSE_LENGTH_PARAM,
        POST_PULSE_COUNT_PARAM,
        POST_PULSE_RATE_PARAM,
        SAVE_BUTTON_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        RESET_IN_INPUT,
        CMP_IN_INPUT,
        GATE_IN_INPUT,
        BIAS_IN_INPUT,
        NUM_INPUTS
    };
    enum OutputIds {
        POST_PULSE_OUT_OUTPUT,
        OVER_OUT_OUTPUT,
        RISING_EDGE_OUT_OUTPUT,
        UNDER_OUT_OUTPUT,
        FALLING_EDGE_OUT_OUTPUT,
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS
    };

    enum PulseStates {
        PULSE_IDLE,
        PULSE_HOLDOFF,
        PULSE_ACTIVE,
        PULSE_SPACING
    };

    Label* meas_label;
    Label* config_label;
    TextField* data_field;

    dsp::PulseGenerator done_pulse;
    dsp::PulseGenerator done_pulse_holdoff;
    dsp::PulseGenerator done_pulse_spacing;

    dsp::PulseGenerator rising_pulse;
    dsp::PulseGenerator falling_pulse;

    dsp::SchmittTrigger manual_trigger;

    int prev_gate = 0;
    int gate_value = 0;

    long meas_count = 0;
    long total_meas = 0;
    long current_total_meas = 0;
    long current_meas_count = 0;

    long min_count = -1;
    long max_count = 0;

    int meas_samples = 0;
    int pulse_counter = 0;

    int label_counter = 0;

    int meas_finished = 0;
    int pulse_state = PULSE_IDLE;

    int prev_do_meas = 0;
    int reset_hold = 0;

    int prev_manual = 0;

    int prev_save = 0;
    int save_request = 0;

    struct DecayMeasurement* current_meas = 0;
    std::list<struct DecayMeasurement>* data_list;

    DecayTimer() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(THRESHOLD_PARAM, -12.f, 12.f, 0.f, "Comparator Threshold");
        configParam(HYSTERESIS_PARAM, -12.f, 12.f, 1.f, "Comparator Hysteresis");
        configParam(RESET_BUTTON_PARAM, 0.f, 1.f, 0.f, "Measurement Reset");
        configParam(MANUAL_TRIGGER_PARAM, 0.f, 1.f, 0.f, "Trigger Pulse");
        configParam(HOLDOFF_PARAM, 0.f, 10.f, 0.1f, "Pulse Holdoff");
        configParam(POST_PULSE_LENGTH_PARAM, 0.f, 1.f, 0.01f, "Pulse Duration");
        configParam(POST_PULSE_COUNT_PARAM, 0.f, 10.f, 0.f, "Pulse Count");
        configParam(POST_PULSE_RATE_PARAM, 0.f, 10.f, 0.1f, "Pulse Spacing");
        configParam(SAVE_BUTTON_PARAM, 0.f, 1.f, 0.f, "Save the current measurement to a json file");

        data_list = new std::list<struct DecayMeasurement>;
    }

    void process(const ProcessArgs& args) override {


        if(params[RESET_BUTTON_PARAM].getValue() != 0 || inputs[RESET_IN_INPUT].getVoltage() > 1.0f)
        {
            meas_samples = 0;
            meas_count = 0;
            total_meas = 0;
            current_total_meas = 0;
            pulse_state = PULSE_IDLE;
            reset_hold = 1;
            min_count = -1;
            max_count = 0;
            data_list->clear();
        }

        /* Comparator Section */

        float threshold_a = params[THRESHOLD_PARAM].getValue();
        float threshold_b = threshold_a + params[HYSTERESIS_PARAM].getValue();
        float threshold, threshold_low, threshold_high;

        if(threshold_a >= threshold_b)
        {
            threshold_low = threshold_b;
            threshold_high = threshold_a;
        }
        else
        {
             threshold_low = threshold_a;
             threshold_high = threshold_b;
        }

        if(gate_value == 0)
        {
            threshold = threshold_high;
        }
        else
        {
            threshold = threshold_low;
        }

        if(inputs[CMP_IN_INPUT].getVoltage() >= threshold)
        {
            gate_value = 1;
        }
        else
        {
            gate_value = 0;
        }

        if(prev_gate == 0 && gate_value == 1)
        {
            rising_pulse.trigger(EDGE_PULSE);
//            outputs[RISING_EDGE_OUT_OUTPUT];
        }
        if(prev_gate == 1 && gate_value == 0)
        {
            falling_pulse.trigger(EDGE_PULSE);
        }


        /* Comparator outputs */
        bool pulse;

        outputs[OVER_OUT_OUTPUT].setVoltage(LOGIC_HIGH*gate_value);
        outputs[UNDER_OUT_OUTPUT].setVoltage(LOGIC_HIGH*(1-gate_value));

        pulse = rising_pulse.process(args.sampleTime);
        outputs[RISING_EDGE_OUT_OUTPUT].setVoltage(pulse? LOGIC_HIGH : 0.f);

        pulse = falling_pulse.process(args.sampleTime);
        outputs[FALLING_EDGE_OUT_OUTPUT].setVoltage(pulse? LOGIC_HIGH : 0.f);
 
        /*Timer section*/

        int pulse_num = params[POST_PULSE_COUNT_PARAM].getValue() ;
        float pulse_length = params[POST_PULSE_LENGTH_PARAM].getValue();
        float pulse_spacing = params[POST_PULSE_RATE_PARAM].getValue();
        float pulse_holdoff = params[HOLDOFF_PARAM].getValue();

        int do_measurement = 0;
        if(inputs[GATE_IN_INPUT].active && inputs[GATE_IN_INPUT].getVoltage() > 1.f)
        {
            do_measurement = 1;
        }
        else
        {
            do_measurement = gate_value;
        }

        meas_finished = 0;
        if(do_measurement != 0 && reset_hold == 0)
        {
            double bias = inputs[BIAS_IN_INPUT].getVoltage();
            if(prev_do_meas == 0)
            {
                current_meas = new struct DecayMeasurement;
                current_meas->bias_min = bias;
                current_meas->bias_max = bias;
                current_meas->threshold_low = threshold_low;
                current_meas->threshold_high = threshold_high;
                current_meas->pulse_count = pulse_num;
                current_meas->pulse_holdoff = pulse_holdoff;
                current_meas->pulse_length = pulse_length;
                current_meas->pulse_spacing = pulse_spacing;
                current_meas->sample_period = args.sampleTime;
                current_meas->timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
                current_meas_count = 0;
            }
            //Technically, there have been 0 samples elapsed since starting
            //But this won't increment after the last sample
            ++current_meas_count;
            ++current_total_meas;
            if(bias < current_meas->bias_min) current_meas->bias_min = bias;
            if(bias > current_meas->bias_max) current_meas->bias_max = bias;
            current_meas->bias_avg += bias;

        }
        if(do_measurement == 0)
        {
            if(prev_do_meas != 0 && reset_hold == 0)
            {
//                int delta = current_total_meas - total_meas;
                int delta = current_meas_count ;
                if(delta > max_count)
                {
                    max_count = delta;
                }
                if(delta < min_count || min_count < 0)
                {
                    min_count = delta;
                }
                total_meas = current_total_meas;
                ++meas_count;

                current_meas->duration = delta*args.sampleTime;
                current_meas->bias_avg/=delta;
                sprintf(current_meas->custom_data, "%s", data_field->text.c_str());
                if(
                    current_meas->threshold_low != threshold_low ||
                    current_meas->threshold_high != threshold_high ||
                    current_meas->pulse_count != pulse_num ||
                    current_meas->pulse_holdoff != pulse_holdoff ||
                    current_meas->pulse_length != pulse_length ||
                    current_meas->pulse_spacing != pulse_spacing ||
                    current_meas->sample_period != args.sampleTime
                )
                {
                    current_meas->valid = 0;
                }

                data_list->push_back(*current_meas);

                meas_finished = 1;
            }
            reset_hold = 0;
        }

        /*Done Pulse section*/

        int manual = 0;
        if(params[MANUAL_TRIGGER_PARAM].getValue() != 0)
        {
            manual = 1;
        }

        if(meas_finished != 0 || (manual == 1 && prev_manual == 0))
        {
            done_pulse_holdoff.trigger(pulse_holdoff);
            pulse_state = PULSE_HOLDOFF;
        }

        if(pulse_num == 0 || do_measurement != 0)
        {
            pulse_state = PULSE_IDLE;
            done_pulse_holdoff.reset();
            done_pulse.reset();
            done_pulse_spacing.reset();
        }

        bool holdoff_value = done_pulse_holdoff.process(args.sampleTime);
        bool pulse_value = done_pulse.process(args.sampleTime);
        bool pause_value = done_pulse_spacing.process(args.sampleTime);

        if(pulse_state != PULSE_IDLE)
        {
            if(pulse_state == PULSE_HOLDOFF)
            {
                if(!holdoff_value)
                {
                    pulse_state = PULSE_ACTIVE;
                    done_pulse.trigger(pulse_length);
                }
            }
            else if(pulse_state == PULSE_ACTIVE)
            {
                if(!pulse_value)
                {
                    ++pulse_counter;
                    if(pulse_counter >= pulse_num)
                    {
                        pulse_state = PULSE_IDLE;
                    }
                    else
                    {
                        done_pulse_spacing.trigger(pulse_spacing);
                        pulse_state = PULSE_SPACING;
                    }
                }
            }
            else if(pulse_state == PULSE_SPACING)
            {
                if(!pause_value)
                {
                    done_pulse.trigger(pulse_length);
                    pulse_state = PULSE_ACTIVE;
                }
            }
        }
        else
        {
            pulse_counter = 0;
        }

        outputs[POST_PULSE_OUT_OUTPUT].setVoltage(pulse_value? LOGIC_HIGH : 0.f);

        /* Save Button */

        int save = params[SAVE_BUTTON_PARAM].getValue();
        if(save != 0 && prev_save == 0 && save_request == 0)
        {
            save_request = 1;
        }
 
        /* Finalization */

        prev_gate = gate_value;
        prev_do_meas = do_measurement;
        prev_manual = manual;
        prev_save = save;

        /* Labels */
        ++label_counter;
        if(label_counter == 4000)
        {
            label_counter = 0;
        }
        if(config_label && label_counter == 0)
        {
            char tstr[4096];
            sprintf( tstr, 
                "Comparator Thresholds: %.2f / %.2f V\nDone Pulse:\n  Count: %i Length: %.1f ms\n  Holdoff: %.1f ms Spacing: %.1f ms",
                threshold_low, threshold_high,
                pulse_num, pulse_length*1000, 
                pulse_holdoff*1000, pulse_spacing*1000
                
                );

            config_label->text = tstr;
        }

        if(meas_label && label_counter == 1)
        {
            float meas_time = 0;
            if(meas_count != 0)
            {
                meas_time = args.sampleTime * total_meas / meas_count;
            }
            float min_time = 0;
            float max_time = max_count*args.sampleTime;
            if(min_count >= 0)
            {
                min_time = min_count*args.sampleTime;
            }

            float bias_avg = 0;
            float bias_min = 0;
            float bias_max = 0;
            if(current_meas)
            {
                if(do_measurement)
                {
                    bias_avg = current_meas->bias_avg/(current_meas_count);
                }
                else
                {
                    bias_avg = current_meas->bias_avg;
                }
                bias_min = current_meas->bias_min;
                bias_max = current_meas->bias_max;
            }
 

            char tstr[4096];
            sprintf(tstr,
            "Average:\n%.4f ms\nMin:\n%.3f ms\nMax:\n%.3f ms\nMaxDev:\n%.1f ppm\nCount: %i\nBias: %f\n%f\n%f",
            meas_time*1000,
            min_time*1000,
            max_time*1000,
            (max_time-min_time)*1000000/meas_time,
            meas_count,
            bias_min,bias_avg,bias_max
            );
            meas_label->text = tstr;
        }

    }


    bool fields_ready = false;
//    TextField* data_field;
    TextField* filename_field;

    json_t* toJson() override {
        json_t* rootJ = json_object();
        if(!fields_ready) return rootJ;
        json_object_set_new(rootJ, "custom_data", json_string(data_field->text.c_str()));
        json_object_set_new(rootJ, "data_filename", json_string(filename_field->text.c_str()));

        return rootJ;
    }


    bool load_json = false;
    json_t* widget_json;
    void dataFromJson(json_t* rootJ) override
    {
        widget_json = rootJ;
        load_json = true;
    }

};


struct DecayTimerWidget : ModuleWidget {
    Label* config_label;
    Label* meas_label;

    Label* data_label;
    TextField* data_field;
    TextField* filename_field;

    void loadJson(json_t* rootJ){
        printf("Attempting load\n");
        json_t* textJ;

        textJ = json_object_get(rootJ, "custom_data");
        if(textJ) data_field->text = json_string_value(textJ);

        textJ = json_object_get(rootJ, "data_filename");
        if(textJ) filename_field->text = json_string_value(textJ);
    }

    void step() override {
        ModuleWidget::step();
        if(!module) return;

        DecayTimer* mod = ((DecayTimer*) module);

        if(!mod->fields_ready)
        {
            mod->data_field = data_field;
            mod->filename_field = filename_field;
            mod->fields_ready = true;
        }

        if(mod->load_json)
        {
            loadJson(mod->widget_json);
            mod->load_json = false;
        }
        
        if(mod->save_request == 1)
        {
            std::string filename = asset::user("timer_data");
            system::createDirectory(filename);
            filename += "/";
            filename += filename_field->text;
            filename += ".json";
            FILE* fp;
            
            json_t* rootJ;
     
            fp = std::fopen(filename.c_str(), "r");
            if(fp)
            {
                json_error_t error;
                rootJ = json_loadf(fp, 0, &error);
                if(!rootJ)
                {
                    std::string message = string::f("JSON parsing error at %s %d:%d %s", error.source, error.line, error.column, error.text);
                    osdialog_message(OSDIALOG_WARNING, OSDIALOG_OK, message.c_str());
                    return;
                }
            }
            else
            {
                rootJ = json_object();
            }

            json_t* arrayJ;

            arrayJ = json_object_get(rootJ, "data");
            if(!arrayJ)
            {
                arrayJ = json_array();
                json_object_set_new(rootJ, "data", arrayJ);
            }

            //Then merge the data array with the json data array

            std::list<struct DecayMeasurement> source_list;
            for(int i = 0; i < json_array_size(arrayJ); ++i)
            {
                struct DecayMeasurement* ref = new DecayMeasurement;
                ref->fromJson(json_array_get(arrayJ, i));
                source_list.push_back(*ref);
            }

            for(struct DecayMeasurement meas : *(mod->data_list))
            {
                json_t* entry = meas.toJson();
                int matched = 0;
                for(struct DecayMeasurement ref: source_list)
                {
                    if(meas.equals(&ref))
                    {
                        matched = 1;
                        break;
                    }
                }
                if(matched == 0)
                {
                    json_array_append(arrayJ, entry);
                }
            }

            FILE* file = std::fopen(filename.c_str(), "w");

            json_dumpf(rootJ, file, JSON_INDENT(4));

            std::fclose(file);
            printf("Saved timer data to %s\n", filename.c_str());

            mod->save_request = 0;

        }

    }



    DecayTimerWidget(DecayTimer* module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/DecayTimer.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(createParamCentered<LEDBezel>(mm2px(Vec(47.625, 36.733)), module, DecayTimer::MANUAL_TRIGGER_PARAM));
        addParam(createParamCentered<LEDBezel>(mm2px(Vec(47.625, 24.033)), module, DecayTimer::RESET_BUTTON_PARAM));
 

        addParam(createParamCentered<RoundTinyBlackKnob>(mm2px(Vec(9.525, 24.033)), module, DecayTimer::THRESHOLD_PARAM));
        addParam(createParamCentered<RoundTinyBlackKnob>(mm2px(Vec(22.225, 24.033)), module, DecayTimer::HYSTERESIS_PARAM));
       addParam(createParamCentered<RoundTinyBlackKnob>(mm2px(Vec(34.925, 49.433)), module, DecayTimer::HOLDOFF_PARAM));
        addParam(createParamCentered<RoundTinyBlackKnob>(mm2px(Vec(47.625, 49.433)), module, DecayTimer::POST_PULSE_LENGTH_PARAM));
        addParam(createParamCentered<RoundTinyBlackSnapKnob>(mm2px(Vec(34.925, 62.133)), module, DecayTimer::POST_PULSE_COUNT_PARAM));
        addParam(createParamCentered<RoundTinyBlackKnob>(mm2px(Vec(47.625, 62.133)), module, DecayTimer::POST_PULSE_RATE_PARAM));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(34.925, 24.033)), module, DecayTimer::RESET_IN_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.525, 36.733)), module, DecayTimer::CMP_IN_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.225, 36.733)), module, DecayTimer::GATE_IN_INPUT));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(34.925, 36.733)), module, DecayTimer::POST_PULSE_OUT_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(9.525, 49.433)), module, DecayTimer::OVER_OUT_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.225, 49.433)), module, DecayTimer::RISING_EDGE_OUT_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(9.525, 62.133)), module, DecayTimer::UNDER_OUT_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.225, 62.133)), module, DecayTimer::FALLING_EDGE_OUT_OUTPUT));

        // mm2px(Vec(41.275, 49.648))
//        addChild(createWidget<Widget>(mm2px(Vec(53.975, 20.858))));

        meas_label = createWidget<Label>(mm2px(Vec(53.975, 20.85)));
        meas_label->box.size = mm2px(Vec(41.275, 49.65));
        meas_label->text = "Lat (ms):\n ----.-- \nMin:\n ----.-- \n Max:\n ----.--\nN samples";
        addChild(meas_label);
        if(module)
        {
            module->meas_label = meas_label;
        }
    
        // mm2px(Vec(88.95, 42.003))
//        addChild(createWidget<Widget>(mm2px(Vec(6.3, 74.707))));

        config_label = createWidget<Label>(mm2px(Vec(6.3, 74.7)));
        config_label->box.size = mm2px(Vec(88.95, 21));
        config_label->text = "Lat (ms):\n ----.-- \nMin:\n ----.-- \n Max:\n ----.--\nN samples";
        addChild(config_label);
        if(module)
        {
            module->config_label = config_label;
        }

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(6.3*2, 95+4)), module, DecayTimer::BIAS_IN_INPUT));

        float ypos = 95;
        float xpos = 10;
        data_label = createWidget<Label>(mm2px(Vec(6.3+xpos, ypos)));
        data_label->box.size = mm2px(Vec(32, 6.35));
        data_label->text = "Custom Data:";
        addChild(data_label);

        data_field = createWidget<TextField>(mm2px(Vec(6.3+xpos+30, ypos)));
        data_field->box.size = mm2px(Vec(31.5, 6.35));
        data_field->multiline = false;
        addChild(data_field);
        if(module)
        {
            module->data_field = data_field;
        }

        filename_field = createWidget<TextField>(mm2px(Vec(6.3*3, ypos+10)));
        filename_field->box.size = mm2px(Vec(77, 6.35));
        filename_field->multiline = false;
        filename_field->text = "decaytimer_data";
        addChild(filename_field);

        addParam(createParamCentered<LEDBezel>(mm2px(Vec(6.3*2, ypos+10+6.35/2)), module, DecayTimer::SAVE_BUTTON_PARAM));
 

 

    }
};


Model* modelDecayTimer = createModel<DecayTimer, DecayTimerWidget>("DecayTimer");
