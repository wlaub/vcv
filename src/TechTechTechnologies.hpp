#include "rack0.hpp"
#include "dsp/digital.hpp"
#include "dsp/functions.hpp"
#include "DSP.hpp"
#include "window.hpp"

#include <GLFW/glfw3.h>

#define OUTPORT(x,y,modname,param, offset)\
    auto *param = Port::create<PJ301MPort>(\
        Vec(x,y), Port::OUTPUT, module, modname::param + offset\
        );\
    center(param,1,1);\
    addOutput(param);\
\

#define INPORT(x,y,modname,param, offset)\
    auto *param = Port::create<PJ301MPort>(\
        Vec(x,y), Port::INPUT, module, modname::param + offset\
        );\
    center(param,1,1);\
    addInput(param);\
\

#define KNOB(x,y,min, max, def, type, modname,param, offset)\
    auto *param = ParamWidget::create<Round ## type ## BlackKnob>(\
        Vec(x,y),module, modname::param + offset,\
        min, max, def\
        );\
    center(param,1,1);\
    addParam(param);\
\

#define SWITCH(x,y, modname, param, ...)\
auto* param = ParamWidget::create<CKSS>(\
    Vec(x,y), module, modname::param,\
    __VA_ARGS__\
    );\
center(param,1,1);\
addParam(param);\
\

#define BUTTON(x,y, type, modname, param, ...)\
auto* param = ParamWidget::create<type>(\
    Vec(x,y), module, modname::param,\
    __VA_ARGS__\
    );\
center(param,1,1);\
addParam(param);\
\


#define DEPTH_WIDGETS(x,y,modname)\
    addInput(Port::create<PJ301MPort>(\
        Vec(x, y+2.5), Port::INPUT, module, modname::DEPTH_INPUT\
        ));\
\
\
    addParam(ParamWidget::create<RoundBlackSnapKnob>(\
        Vec(x+27.5, y+1), module, modname::DEPTH_PARAM,\
        1, 16, 8\
        ));\
\
    addOutput(Port::create<PJ301MPort>(\
        Vec(x+60.25, y+2.5), Port::OUTPUT, module, modname::DEPTH_OUTPUT\
        ));\
\

#define DEPTH_STEP\
    int depth;\
    if(inputs[DEPTH_INPUT].active)\
        depth = cv_to_depth(inputs[DEPTH_INPUT].value);\
    else\
        depth = params[DEPTH_PARAM].value;\
\
    outputs[DEPTH_OUTPUT].value = depth_to_cv(depth);\
\


#define PARAM_PAIR(xoff, yoff, name, def, idx)\
    addInput(Port::create<PJ301MPort>(\
        Vec(xoff+2.5, yoff+2.5), Port::INPUT, module, name ## _INPUT+idx\
        ));\
    addParam(ParamWidget::create<RoundBlackKnob>(\
        Vec(xoff+30.15, yoff+.15), module, name ## _PARAM+idx,\
        0, 10, def\
        ));\
\


#define PARAM_SEL(VAR, NAME, IDX)\
            if(inputs[NAME ## _INPUT+IDX].active)\
            {\
                VAR = inputs[NAME ## _INPUT+IDX].value;\
            }\
            else\
            {\
                VAR = params[NAME ## _PARAM+IDX].value;\
            } \
 \


#define CV_ATV_PARAM(xoff, yoff, name, min, max, def, idx)\
    addInput(Port::create<PJ301MPort>(\
        Vec(xoff, yoff), Port::INPUT, module, name ## _INPUT+idx\
        ));\
\
    addParam(ParamWidget::create<RoundTinyBlackKnob>(\
        Vec(xoff+35, yoff+3.5), module, name ## CV_PARAM+idx,\
        -1,1,0\
        ));\
\
    addParam(ParamWidget::create<RoundLargeBlackKnob>(\
        Vec(xoff+62.65, yoff-6.35), module, name ## _PARAM+idx,\
        min,max,def\
        ));\

#define CV_ATV_VALUE(name, max, idx)\
params[name ## _PARAM+idx].value\
+ max*params[name ## CV_PARAM+idx].value*inputs[name ## _INPUT+idx].value/10;\
 


using namespace rack;


extern Plugin *pluginInstance;


void center(Widget* thing, int x = 1, int y = 1);
//Using 0-10V CV's per https://vcvrack.com/manual/VoltageStandards.html
int cv_to_depth(float cv, int max = 16);
int cv_to_num(float cv, int depth);
float depth_to_cv(int depth, int max=16);
float num_to_cv(int num, int depth);

double db_to_num(float val, float unity);
double split_log(float v, float m, float n);

struct NumField : TextField {
    void onTextChange() override;

    int outNum = 0;
};


struct RoundTinyBlackKnob : RoundBlackKnob {
    RoundTinyBlackKnob() {
        setSVG(SVG::load(assetPlugin(pluginInstance, "res/Components/RoundTinyBlackKnob.svg")));
    }
};
 
struct DWhite : SVGSwitch, MomentarySwitch {
    DWhite() {
        addFrame(SVG::load(assetPlugin(pluginInstance, "res/Components/1DWHT_0.svg")));
        addFrame(SVG::load(assetPlugin(pluginInstance, "res/Components/1DWHT_1.svg")));
    }
};

struct DWhiteLatch : SVGSwitch, ToggleSwitch {
    DWhiteLatch() {
        addFrame(SVG::load(assetPlugin(pluginInstance, "res/Components/1DWHT_0.svg")));
        addFrame(SVG::load(assetPlugin(pluginInstance, "res/Components/1DWHT_1.svg")));
    }
};


static const float KNOB_SENSITIVITY = 0.0015f;

struct EncoderController;

struct TTTEncoder : RoundBlackKnob {
    bool flip=false;
    bool lights_ready = false;
    bool spinning = false;

    float lastAngle = 0;

    NVGcolor color = nvgRGBAf(0,0,0,1);

    EncoderController* controller;

    TTTEncoder();
   
    ModuleLightWidget* lights[7];

    void configureLights();

    void setValue(float v);

    void onHoverKey(EventHoverKey &e) override;
    void onDragMove(EventDragMove &e) override;
    void reset() override;
    void draw(NVGcontext *vg) override;
    void step() override;
    void fromJson(json_t *rootJ);

};

struct ITTTEncoder : TTTEncoder {
    ITTTEncoder() {
        TTTEncoder();
        flip=true;
    }
};

struct EncoderController {
    unsigned char ngroup = 3;
    unsigned char values[3][7];
    unsigned char defaults[7];
    TTTEncoder* widget;
    unsigned char index = 0;
    unsigned char group = 0;

    int delta = 0;

    EncoderController(TTTEncoder* w, const unsigned char* defs)
    {
        widget = w;
        widget->controller = this;
        for (int i = 0; i < 7; ++i)
        {
            defaults[i] = defs[i];
        }
        clear();
        setIndex(0);
    }

    int process();

    void setColor(EncoderController* src);
    void setColor(float r,float g, float b);
    void setValues(unsigned char* v, unsigned char tgroup=255);

    int getValue();
    int getValue(int idx);

    void clear();
    void reset();

    void reset(unsigned char i)
    { //TODO: Should be called when the widget gets reset
        values[group][i] = defaults[i];
        update(0);
    }

    void setIndex(unsigned char idx, unsigned char tgroup=255);

    void update(int amount);
};


////////////////////
// module widgets
////////////////////
/*
struct MyModuleWidget : ModuleWidget {
	MyModuleWidget();
};
*/

extern Model* modelDAC;

extern Model* modelmDAC;

extern Model* modelPrometheus;

extern Model* modelVulcan;

extern Model* modelSisyphus;

extern Model* modelPolyphemus;

extern Model* modelOuroboros;

extern Model* modelOdysseus;

extern Model* modelMneme;

extern Model* modelAthena;

extern Model* modelAchilles;

extern Model* modelQMod;

extern Model* modelConvo;

extern Model* modelPleiades;
