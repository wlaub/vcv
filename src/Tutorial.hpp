#include "rack.hpp"
#include "dsp/digital.hpp"

using namespace rack;


extern Plugin *plugin;


void center(Widget* thing, int x = 1, int y = 1);


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
struct PrometheusWidget : ModuleWidget
{
    PrometheusWidget();
};
