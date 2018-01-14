#include "rack.hpp"
#include "dsp/digital.hpp"

#define DEPTH_WIDGETS(x,y,modname)\
    addInput(createInput<PJ301MPort>(\
        Vec(x, y+2.5), module, modname::DEPTH_INPUT\
        ));\
\
\
    addParam(createParam<RoundSmallBlackSnapKnob>(\
        Vec(x+27.5, y), module, modname::DEPTH_PARAM,\
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



using namespace rack;


extern Plugin *plugin;


void center(Widget* thing, int x = 1, int y = 1);
//Using 0-10V CV's per https://vcvrack.com/manual/VoltageStandards.html
int cv_to_depth(float cv, int max = 16);
int cv_to_num(float cv, int depth);
float depth_to_cv(int depth, int max=16);
float num_to_cv(int num, int depth);

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
    TextField** infields;
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
