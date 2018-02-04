#include "rack.hpp"
#include "dsp/digital.hpp"

#define DEPTH_WIDGETS(x,y,modname)\
    addInput(createInput<PJ301MPort>(\
        Vec(x, y+2.5), module, modname::DEPTH_INPUT\
        ));\
\
\
    addParam(createParam<RoundSmallBlackSnapKnob>(\
        Vec(x+27.5, y+1), module, modname::DEPTH_PARAM,\
        1, 16, 8\
        ));\
\
    addOutput(createOutput<PJ301MPort>(\
        Vec(x+60, y+2.5), module, modname::DEPTH_OUTPUT\
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
    addInput(createInput<PJ301MPort>(\
        Vec(xoff+2.5, yoff+2.5), module, name ## _INPUT+idx\
        ));\
    addParam(createParam<RoundSmallBlackKnob>(\
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
    addInput(createInput<PJ301MPort>(\
        Vec(xoff, yoff), module, name ## _INPUT+idx\
        ));\
\
    addParam(createParam<RoundTinyBlackKnob>(\
        Vec(xoff+34, yoff+2.5), module, name ## CV_PARAM+idx,\
        -1,1,0\
        ));\
\
    addParam(createParam<RoundBlackKnob>(\
        Vec(xoff+62.5, yoff-6.5), module, name ## _PARAM+idx,\
        min,max,def\
        ));\


using namespace rack;


extern Plugin *plugin;


void center(Widget* thing, int x = 1, int y = 1);
//Using 0-10V CV's per https://vcvrack.com/manual/VoltageStandards.html
int cv_to_depth(float cv, int max = 16);
int cv_to_num(float cv, int depth);
float depth_to_cv(int depth, int max=16);
float num_to_cv(int num, int depth);

struct NumField : TextField {
    void onTextChange() override;

    int outNum = 0;
};


struct RoundTinyBlackKnob : RoundBlackKnob {
    RoundTinyBlackKnob() {
        box.size = Vec(20, 20);
    }
};
 


////////////////////
// module widgets
////////////////////

struct MyModuleWidget : ModuleWidget {
	MyModuleWidget();
};
struct DACWidget : ModuleWidget
{
    DACWidget();
};
struct mDACWidget : ModuleWidget
{
    NumField** infields;
    mDACWidget();
    void jsontag(char* result, int i);
    json_t *toJson() override;
    void fromJson(json_t *rootJ) override;
};
struct PrometheusWidget : ModuleWidget
{
    PrometheusWidget();
};
struct VulcanWidget : ModuleWidget
{
    VulcanWidget();
};
struct SisyphusWidget : ModuleWidget
{
    SisyphusWidget();
};
struct PolyphemusWidget : ModuleWidget
{
    PolyphemusWidget();
};



