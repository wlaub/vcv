#include "rack.hpp"
#include "dsp/digital.hpp"

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
    addParam(ParamWidget::create<RoundSmallBlackKnob>(\
        Vec(xoff+31, yoff+1), module, name ## _PARAM+idx,\
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


extern Plugin *plugin;


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
        setSVG(SVG::load(assetPlugin(plugin, "res/RoundTinyBlackKnob.svg")));
    }
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




