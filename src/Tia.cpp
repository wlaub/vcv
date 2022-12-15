#include "Tia.hpp"
#include "PngModule.hpp"

struct TiaI : PngModule {
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

    dsp::SchmittTrigger top_triggers[8], bot_triggers[8];

    float blink_counter = 0;
    int blink = 0;

    double faders[7] = {0};
    double gains[7];

    TiaMessage out_message;
    TiaMessage in_message;

    enum rangeMode {
        UNI5,
        BIP5,
        UNI10,
        BIP10,
        RMODE_LEN        
    };

    int range_mode = UNI5;

    enum switchMode {
        SMOOTH,
        IMMEDIATE,
        SMODE_LEN
    };

    int switch_mode = SMOOTH;

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

        leftExpander.producerMessage = &out_message;
        rightExpander.producerMessage = &out_message;

        leftExpander.consumerMessage = &in_message;
        rightExpander.consumerMessage = &in_message;


    }

    void write_message(TiaMessage* target)
    {
        for(int i = 0; i < 7; ++i)
        {
            target->faders[i] = faders[i];
            target->top_select[i] = top_select[i];
            target->bot_select[i] = bot_select[i];
            target->gains[i] = gains[i];
        }
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

        bool top_set[8];
        bool bot_set[8];
        for(int i = 0; i < 8; ++i)
        {
            top_set[i] = top_triggers[i].process(params[SET_TOP0_PARAM+i].getValue());
            bot_set[i] = bot_triggers[i].process(params[SET_BOT0_PARAM+i].getValue());
        }

        bool set_all_top = top_set[7];
        bool set_all_bot = bot_set[7];
        for(int i = 0; i < 7; ++i)
        {
            if(set_all_top || top_set[i])
            {
                if(top_select_pending[i] == select)
                {
                    top_select[i] = select;
                }
                top_select_pending[i] = select;
            }
            if(set_all_bot || bot_set[i])
            {
                if(bot_select_pending[i] == select)
                {
                    bot_select[i] = select;
                }
                bot_select_pending[i] = select;
            }
        }

        /*Compute crossfade ratios*/

        for(int i = 0; i < 7; ++i)
        {
            double fade = inputs[XFADE0_INPUT+i].getVoltage();
            fade += inputs[POLY_XFADE_INPUT].getPolyVoltage(i);
            if(range_mode == UNI5) //0-5 V
            {
                fade /= 5;
                fade = clamp(fade, 0.f, 1.f);
            }
            else if(range_mode == BIP5) //bipolar 5 V
            {
                fade /= 5;
                fade = clamp(fade, -1.f, 1.f);
                fade = (fade+1)/2;
            }
            else if(range_mode == UNI10) //0-10 V
            {
                fade /= 10;
                fade = clamp(fade, 0.f, 1.f);
            }
            else if(range_mode == BIP10) //bipolar 10 V
            {
                fade /= 10;
                fade = clamp(fade, -1.f, 1.f);
                fade = (fade+1)/2;
            }
 
            faders[i] = fade; 
        }


        /*Handle pending selections*/
        if(switch_mode == SMOOTH)
        {
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
        }
        else if(switch_mode == IMMEDIATE)
        {
            for(int i = 0; i < 7; ++i)
            {
                top_select[i] = top_select_pending[i];
                bot_select[i] = bot_select_pending[i];
            }
            
        }

        /*Do crossfading*/

        double global_gain = params[GLOBAL_GAIN_PARAM].getValue();

        for(int i = 0; i < 7; ++i)
        {
            gains[i] = params[GAIN0_PARAM+i].getValue() * global_gain;
        }

        outputs[POLY_OUTPUT].setChannels(7);

        for(int i = 0; i < 7; ++i)
        {
            double fade = faders[i];

            double bot = 0;
            double top = 0;
            if(top_select[i] < 7)
            {
                double top_gain = gains[top_select[i]];//params[GAIN0_PARAM+top_select[i]].getValue();
                top = inputs[SIGNAL0_INPUT+top_select[i]].getPolyVoltage(i) * top_gain;
            }
            if(bot_select[i] < 7)
            {
                double bot_gain = gains[bot_select[i]]; //params[GAIN0_PARAM+bot_select[i]].getValue();
                bot = inputs[SIGNAL0_INPUT+bot_select[i]].getPolyVoltage(i) * bot_gain;
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
    
        /* Handle Expanders */

        if(leftExpander.module && leftExpander.module->model == modelTiaIExpander)
        {
            write_message(reinterpret_cast<TiaMessage*>(leftExpander.producerMessage));
            leftExpander.requestMessageFlip();
        }
        if(rightExpander.module && rightExpander.module->model == modelTiaIExpander)
        {
            write_message(reinterpret_cast<TiaMessage*>(rightExpander.producerMessage));
            rightExpander.requestMessageFlip();
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

        json_object_set_new(rootJ, "range_mode", json_integer(range_mode));
        json_object_set_new(rootJ, "switch_mode", json_integer(switch_mode));

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

        temp = json_object_get(rootJ, "range_mode");
        if(temp) range_mode = json_integer_value(temp);

        temp = json_object_get(rootJ, "switch_mode");
        if(temp) switch_mode = json_integer_value(temp);



    } 

};


struct TiaIWidget : PngModuleWidget {


    void appendContextMenu(Menu* menu) override {
            TiaI* module = dynamic_cast<TiaI*>(this->module);

            menu->addChild(new MenuEntry);

            panel_select_menu(menu, module);

            menu->addChild(createMenuLabel("Crossfade CV Range"));
            
            struct RangeItem : MenuItem {
                TiaI* module;
                int mode;
                void onAction(const event::Action& e) override {
                    module->range_mode = mode;
                }
            };

            std::string rmode_names[TiaI::RMODE_LEN];
            rmode_names[TiaI::UNI5] = "0-5 V";
            rmode_names[TiaI::UNI10] = "0-10 V";
            rmode_names[TiaI::BIP5] =  "±5 V";
            rmode_names[TiaI::BIP10] =  "±10 V";

            int rmode_sequence[TiaI::RMODE_LEN] = {TiaI::UNI5, TiaI::UNI10, TiaI::BIP5, TiaI::BIP10};
 
            for(int i = 0; i < TiaI::RMODE_LEN; ++i)
            {
                int idx = rmode_sequence[i];
                RangeItem* range_item = createMenuItem<RangeItem>(rmode_names[idx]);
                range_item->module = module;
                range_item->mode = idx;
                range_item->rightText = CHECKMARK(module->range_mode == idx);
                menu->addChild(range_item);
            }

            menu->addChild(createMenuLabel("Signal Routing Change Mode"));

            struct SwitchItem : MenuItem {
                TiaI* module;
                int mode;
                void onAction(const event::Action& e) override {
                    module->switch_mode = mode;
                }
            };

            std::string smode_names[TiaI::SMODE_LEN];
            smode_names[TiaI::IMMEDIATE] = "Immediate";
            smode_names[TiaI::SMOOTH] = "Smooth";

            int smode_sequence[TiaI::SMODE_LEN] = {TiaI::SMOOTH, TiaI::IMMEDIATE};

            for(int i = 0; i < TiaI::SMODE_LEN; ++i)
            {
                int idx = smode_sequence[i];
                SwitchItem* switch_item = createMenuItem<SwitchItem>(smode_names[idx]);
                switch_item->module = module;
                switch_item->mode = idx;
                switch_item->rightText = CHECKMARK(module->switch_mode == idx);
                menu->addChild(switch_item);
            }



        }


    TiaIWidget(TiaI* module) {
        setModule(module);

        set_panels(
            {
            {"Default", "res/Tia.svg"},
            {"Fancy", "res/tia_b.png"},
            {"Alt 1", "res/tia_a.png"}
            });



        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH*2.5, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 3.5 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH*2.5, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 3.5 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));


        #define GRID(x,y) 15.24*(x-0.5), 15.24*(y)+3.28

        #define YTOP 5
        #define YBOT 7
        #define YSEL 4
        #define YFADE 3
        #define YGAIN 2
        #define YIN 1

        for(int i = 0; i < 7; ++i)
        {

            addParam(createParamCentered<LEDBezel>(
                    mm2px(Vec(GRID(i+1, YTOP))), module, TiaI::SET_TOP0_PARAM+i));
            addParam(createParamCentered<LEDBezel>(
                    mm2px(Vec(GRID(i+1, YBOT))), module, TiaI::SET_BOT0_PARAM+i));

            addChild(createLightCentered<LEDBezelLight<RedGreenBlueLight>>(
                    mm2px(Vec(GRID(i+1, YTOP))), module, TiaI::SELECT_TOP0_LIGHT+3*i));
            addChild(createLightCentered<LEDBezelLight<RedGreenBlueLight>>(
                    mm2px(Vec(GRID(i+1, YBOT))), module, TiaI::SELECT_BOT0_LIGHT+3*i));

            //1.375, 1.625
            addChild(createLightCentered<LargeLight<OrangeLight>>(
                    mm2px(Vec(GRID(i+1, YTOP+0.67))), module, TiaI::FADE_TOP0_LIGHT+i));
            addChild(createLightCentered<LargeLight<OrangeLight>>(
                    mm2px(Vec(GRID(i+1, YBOT-0.67))), module, TiaI::FADE_BOT0_LIGHT+i));
     

            addParam(createParamCentered<LEDBezel>(
                    mm2px(Vec(GRID(i+1, YSEL))), module, TiaI::SELECT0_PARAM+i));
            addChild(createLightCentered<LEDBezelLight<GreenLight>>(
                    mm2px(Vec(GRID(i+1, YSEL))), module, TiaI::SELECT0_LIGHT+i));
     

            addInput(createInputCentered<PJ301MPort>(
                    mm2px(Vec(GRID(i+1, YFADE))), module, TiaI::XFADE0_INPUT+i));

            addParam(createParamCentered<RoundBlackKnob>(
                    mm2px(Vec(GRID(i+1, YGAIN))), module, TiaI::GAIN0_PARAM+i));


            addInput(createInputCentered<PJ301MPort>(
                    mm2px(Vec(GRID(i+1, YIN))), module, TiaI::SIGNAL0_INPUT+i));
     
        }


        addParam(createParamCentered<LEDBezel>(
                mm2px(Vec(GRID(8,YTOP))), module, TiaI::SET_TOP_ALL_PARAM));
        addParam(createParamCentered<LEDBezel>(
                mm2px(Vec(GRID(8,YBOT))), module, TiaI::SET_BOT_ALL_PARAM));

//        addParam(createParamCentered<CKSS>(
//                mm2px(Vec(GRID(8,3))), module, TiaI::FADE_RANGE_PARAM));

        addParam(createParamCentered<LEDBezel>(
                mm2px(Vec(GRID(8,YSEL))), module, TiaI::SELECT_NONE_PARAM));

        addChild(createLightCentered<LEDBezelLight<GreenLight>>(
                mm2px(Vec(GRID(8,YSEL))), module, TiaI::SELECT_NONE_LIGHT));
 

        addInput(createInputCentered<PJ301MPort>(
                mm2px(Vec(GRID(8,YFADE))), module, TiaI::POLY_XFADE_INPUT));
 

        addParam(createParamCentered<RoundBlackKnob>(
                mm2px(Vec(GRID(8,YGAIN))), module, TiaI::GLOBAL_GAIN_PARAM));


        addOutput(createOutputCentered<PJ301MPort>(
                mm2px(Vec(GRID(8,YIN))), module, TiaI::POLY_OUTPUT));
 
    }
};


Model* modelTiaI = createModel<TiaI, TiaIWidget>("TiaI");
