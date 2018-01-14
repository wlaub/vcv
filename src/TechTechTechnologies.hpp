#include "rack.hpp"
#include "dsp/digital.hpp"

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
    mDACWidget();
};
struct PrometheusWidget : ModuleWidget
{
    PrometheusWidget();
};
