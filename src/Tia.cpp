#include "TechTechTechnologies.hpp"


struct TiaI : Module {
    enum ParamId {
        SET_TOP0_PARAM,
        SET_TOP1_PARAM,
        SET_TOP2_PARAM,
        SET_TOP3_PARAM,
        SET_TOP4_PARAM,
        SET_TOP5_PARAM,
        SET_TOP6_PARAM,
        SET_TOP_ALL_PARAM,
        SET_BOT0_PARAM,
        SET_BOT1_PARAM,
        SET_BOT2_PARAM,
        SET_BOT3_PARAM,
        SET_BOT4_PARAM,
        SET_BOT5_PARAM,
        SET_BOT6_PARAM,
        SET_BOT_ALL_PARAM,
        SELECT0_PARAM,
        SELECT1_PARAM,
        SELECT2_PARAM,
        SELECT3_PARAM,
        SELECT4_PARAM,
        SELECT5_PARAM,
        SELECT6_PARAM,
        SELECT_NONE_PARAM,
        GAIN0_PARAM,
        GAIN1_PARAM,
        GAIN2_PARAM,
        GAIN3_PARAM,
        GAIN4_PARAM,
        GAIN5_PARAM,
        GAIN6_PARAM,
        GLOBAL_GAIN_PARAM,
        FADE_RANGE_PARAM,
        PARAMS_LEN
    };
    enum InputId {
        SIGNAL0_INPUT,
        SIGNAL1_INPUT,
        SIGNAL2_INPUT,
        SIGNAL3_INPUT,
        SIGNAL4_INPUT,
        SIGNAL5_INPUT,
        SIGNAL6_INPUT,
        XFADE0_INPUT,
        XFADE1_INPUT,
        XFADE2_INPUT,
        XFADE3_INPUT,
        XFADE4_INPUT,
        XFADE5_INPUT,
        XFADE6_INPUT,
        POLY_XFADE_INPUT,
        INPUTS_LEN
    };
    enum OutputId {
        POLY_OUTPUT,
        OUTPUTS_LEN
    };
    enum LightId {
        SELECT_TOP0_LIGHT,
        SELECT_BOT0_LIGHT = SELECT_TOP0_LIGHT+21,
        FADE_TOP0_LIGHT = SELECT_BOT0_LIGHT+21,
        FADE_BOT0_LIGHT = FADE_TOP0_LIGHT+7,
        SELECT0_LIGHT = FADE_BOT0_LIGHT+7,
        SELECT_NONE_LIGHT = SELECT0_LIGHT+7,
        LIGHTS_LEN
    };

    int select = 7;
    int top_select[7] = {0,1,2,3,4,5,6};
    int bot_select[7] = {7,7,7,7,7,7,7};

    int top_select_pending[7] = {0,1,2,3,4,5,6};
    int bot_select_pending[7] = {7,7,7,7,7,7,7};

    float blink_counter = 0;
    int blink = 0;

    TiaI() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

        for(int i = 0; i < 7; ++i)
        {
            configParam(SET_TOP0_PARAM+i, 0.f, 1.f, 0.f, "Assign xfade top");
            configParam(SET_BOT0_PARAM+i, 0.f, 1.f, 0.f, "Assign xfade bottom");
            configParam(SELECT0_PARAM+i, 0.f, 1.f, 0.f, "Select assignment source");
            configInput(SIGNAL0_INPUT+i, "Source");
            configInput(XFADE0_INPUT+i, "Crossfade CV");
            configParam(GAIN0_PARAM+i, 0.f, 1.f, 1.f, "Source Gain");
        }

        configParam(SET_TOP_ALL_PARAM, 0.f, 1.f, 0.f, "Assign all xfade top");
        configParam(SET_BOT_ALL_PARAM, 0.f, 1.f, 0.f, "Assign all xfade bottom");


        configParam(SELECT_NONE_PARAM, 0.f, 1.f, 0.f, "Select 0 V");
        configSwitch(FADE_RANGE_PARAM, 0.f, 1.f, 0.f, "Crossfade CV Range", {"0 - 5 V", "+/- 5 V"});

        configParam(GLOBAL_GAIN_PARAM, 0.f, 1.f, 1.f, "Global Source Gain");

        configInput(POLY_XFADE_INPUT, "Crossfade CV (Polyphonic)");

        configOutput(POLY_OUTPUT, "Crossfader Outputs (Polyphonic)");
    }

    void process(const ProcessArgs& args) override {

        /*Handle select buttons*/

        for(int i = 0; i < 8; ++i)
        {
            if(params[SELECT0_PARAM+i].getValue() == 1)
            {
                select = i;
            }
        }

        bool set_all_top = params[SET_TOP_ALL_PARAM].getValue() == 1;
        bool set_all_bot = params[SET_BOT_ALL_PARAM].getValue() == 1;
        for(int i = 0; i < 7; ++i)
        {
            if(set_all_top || params[SET_TOP0_PARAM+i].getValue() == 1)
            {
                top_select_pending[i] = select;
            }
            if(set_all_bot || params[SET_BOT0_PARAM+i].getValue() == 1)
            {
                bot_select_pending[i] = select;
            }
        }

        /*Compute crossfade ratios*/
        double faders[7];

        int fade_range = params[FADE_RANGE_PARAM].getValue();
        for(int i = 0; i < 7; ++i)
        {
            double fade = inputs[XFADE0_INPUT+i].getVoltage();
            fade += inputs[POLY_XFADE_INPUT].getPolyVoltage(i);
            fade /= 5;
            if(fade_range == 1) //bipolar 5 V
            {
                fade = clamp(fade, -1.f, 1.f);
                fade = (fade+1)/2;
            }
            else if(fade_range == 0) //0-5 V
            {
                fade = clamp(fade, 0.f, 1.f);
            }
            faders[i] = fade; 
        }

        /*Handle pending selections*/
        //TODO: It would be nice to have it optionally apply instantly
        for(int i = 0; i < 7; ++i)
        {
            if(faders[i] < 0.0001)
            {
                top_select[i] = top_select_pending[i];
            }
            if(faders[i] > 0.9999)
            {
                bot_select[i] = bot_select_pending[i];
            }
        }

        /*Do crossfading*/

        double global_gain = params[GLOBAL_GAIN_PARAM].getValue();
        outputs[POLY_OUTPUT].setChannels(7);

        for(int i = 0; i < 7; ++i)
        {
            double fade = faders[i];

            double bot = 0;
            double top = 0;
            if(top_select[i] < 7)
            {
                double top_gain = params[GAIN0_PARAM+top_select[i]].getValue();
                top = inputs[SIGNAL0_INPUT+top_select[i]].getVoltage() * top_gain * global_gain;
            }
            if(bot_select[i] < 7)
            {
                double bot_gain = params[GAIN0_PARAM+bot_select[i]].getValue();
                bot = inputs[SIGNAL0_INPUT+bot_select[i]].getVoltage() * bot_gain * global_gain;
            }

            double mix = top*fade + bot*(1-fade);
            outputs[POLY_OUTPUT].setVoltage(mix, i);
            lights[FADE_TOP0_LIGHT+i].setBrightness(fade);
            lights[FADE_BOT0_LIGHT+i].setBrightness(1-fade);

        }

        /*Process selection lights*/
        blink_counter += args.sampleTime;
        if(blink_counter > 0.5)
        {
            blink_counter -= 0.5;
            blink = 1-blink;
        }

        for(int i = 0; i < 8; ++i)
        {
            if(i == select)
            {
               lights[SELECT0_LIGHT+i].setBrightness(1);
            }
            else
            {
               lights[SELECT0_LIGHT+i].setBrightness(0);
            }
        }

        for(int i = 0; i < 7; ++i)
        {
            float value = 0;
            float tvalue = 0;
            int color = 1;
            if(bot_select[i] == select)
            {
                value = 1;
            }
            if(bot_select[i] != bot_select_pending[i])
            {
                value = blink;
                if(bot_select_pending[i] != select)
                {
                    color = 2;
                }
            }

            for(int j = 0; j < 3; ++j)
            {
                tvalue = 0;
                if(j == color) tvalue = value;
                lights[SELECT_BOT0_LIGHT+3*i+j].setBrightness(tvalue);
            }

            color = 1;
            value = 0;
            if(top_select[i] == select)
            {
                value = 1;
            }
            if(top_select[i] != top_select_pending[i])
            {
                value = blink;
                if(top_select_pending[i] != select)
                {
                    color = 2;
                }
 
            }

            for(int j = 0; j < 3; ++j)
            {
                tvalue = 0;
                if(j == color) tvalue = value;
                lights[SELECT_TOP0_LIGHT+3*i+j].setBrightness(tvalue);
            }

        }

    }

    json_t* dataToJson() override {
        json_t* rootJ = json_object();

        json_object_set_new(rootJ, "select", json_integer(select));
        
        json_t* ts = json_array();
        json_t* bs = json_array();

        for(int i = 0; i < 7; ++i)
        {
            json_array_append_new(ts, json_integer(top_select[i]));
            json_array_append_new(bs, json_integer(bot_select[i]));
        }

        json_object_set_new(rootJ, "top_select", ts);
        json_object_set_new(rootJ, "bot_select", bs);


        return rootJ;

    } 

    void dataFromJson(json_t* rootJ) override {

        json_t* temp;

        temp = json_object_get(rootJ, "select");
        if(temp) select = json_integer_value(temp);

        json_t *ts, *bs;

        ts = json_object_get(rootJ, "top_select");
        bs = json_object_get(rootJ, "bot_select");

        for(int i = 0; i < 7; ++i)
        {
            int val;
            if(ts) 
            {
                val = json_integer_value(json_array_get(ts, i));
                top_select[i] = val;
                top_select_pending[i] = val;
            }
            if(bs) 
            {
                val = json_integer_value(json_array_get(bs, i));
                bot_select[i] = val;
                bot_select_pending[i] = val;
            }
        }

    } 

};


struct TiaIWidget : ModuleWidget {
    TiaIWidget(TiaI* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/Tia.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));


        #define GRID(x,y) 15.24*(x-0.5), 15.24*(y)+3.28

        for(int i = 0; i < 7; ++i)
        {
            addParam(createParamCentered<RoundBlackKnob>(
                    mm2px(Vec(GRID(1, 6))), module, TiaI::GAIN0_PARAM+i));

            addParam(createParamCentered<LEDBezel>(
                    mm2px(Vec(GRID(i+1, 1))), module, TiaI::SET_TOP0_PARAM+i));
            addParam(createParamCentered<LEDBezel>(
                    mm2px(Vec(GRID(i+1, 2))), module, TiaI::SET_BOT0_PARAM+i));

            addChild(createLightCentered<MediumLight<RedLight>>(
                    mm2px(Vec(GRID(i+1, 1.375))), module, TiaI::FADE_TOP0_LIGHT+i));
            addChild(createLightCentered<MediumLight<RedLight>>(
                    mm2px(Vec(GRID(i+1, 1.625))), module, TiaI::FADE_BOT0_LIGHT+i));
     

            addChild(createLightCentered<LEDBezelLight<RedGreenBlueLight>>(
                    mm2px(Vec(GRID(i+1, 1))), module, TiaI::SELECT_TOP0_LIGHT+3*i));
            addChild(createLightCentered<LEDBezelLight<RedGreenBlueLight>>(
                    mm2px(Vec(GRID(i+1, 2))), module, TiaI::SELECT_BOT0_LIGHT+3*i));

            addParam(createParamCentered<LEDBezel>(
                    mm2px(Vec(GRID(i+1, 4))), module, TiaI::SELECT0_PARAM+i));
            addChild(createLightCentered<LEDBezelLight<GreenLight>>(
                    mm2px(Vec(GRID(i+1, 4))), module, TiaI::SELECT0_LIGHT+i));
     

            addInput(createInputCentered<PJ301MPort>(
                    mm2px(Vec(GRID(i+1, 5))), module, TiaI::XFADE0_INPUT+i));

            addParam(createParamCentered<RoundBlackKnob>(
                    mm2px(Vec(GRID(i+1, 6))), module, TiaI::GAIN0_PARAM+i));


            addInput(createInputCentered<PJ301MPort>(
                    mm2px(Vec(GRID(i+1, 7))), module, TiaI::SIGNAL0_INPUT+i));
     
        }


        addParam(createParamCentered<LEDBezel>(
                mm2px(Vec(GRID(8,1))), module, TiaI::SET_TOP_ALL_PARAM));
        addParam(createParamCentered<LEDBezel>(
                mm2px(Vec(GRID(8,2))), module, TiaI::SET_BOT_ALL_PARAM));

        addParam(createParamCentered<CKSS>(
                mm2px(Vec(GRID(8,3))), module, TiaI::FADE_RANGE_PARAM));

        addParam(createParamCentered<LEDBezel>(
                mm2px(Vec(GRID(8,4))), module, TiaI::SELECT_NONE_PARAM));

        addChild(createLightCentered<LEDBezelLight<GreenLight>>(
                mm2px(Vec(GRID(8,4))), module, TiaI::SELECT_NONE_LIGHT));
 

        addInput(createInputCentered<PJ301MPort>(
                mm2px(Vec(GRID(8,5))), module, TiaI::POLY_XFADE_INPUT));
 

        addParam(createParamCentered<RoundBlackKnob>(
                mm2px(Vec(GRID(8,6))), module, TiaI::GLOBAL_GAIN_PARAM));


        addOutput(createOutputCentered<PJ301MPort>(
                mm2px(Vec(GRID(8,7))), module, TiaI::POLY_OUTPUT));
 
    }
};


Model* modelTiaI = createModel<TiaI, TiaIWidget>("TiaI");
