#include "rack.hpp"
//#include "dsp/digital.hpp"
//#include "dsp/functions.hpp"
#include "DSP.hpp"
//#include "window.hpp"

#include <libgen.h>
#include <GLFW/glfw3.h>
#include <osdialog.h>

//
// What follows is misguided legacy macros. I am sorry.
//
//

#define OUTPORT(x,y,modname,param, offset)\
    addOutput(createOutputCentered<PJ301MPort>(\
        Vec(x,y), module, modname::param + offset\
        ));\
\

#define INPORT(x,y,modname,param, offset)\
    addInput(createInputCentered<PJ301MPort>(\
        Vec(x,y), module, modname::param + offset\
        ));\
\

#define KNOB(x,y,min, max, def, type, modname,param, offset)\
    addParam(createParamCentered<Round ## type ## BlackKnob>(\
        Vec(x,y),module, modname::param + offset\
        ));\
\

#define SWITCH(x,y, modname, param, ...)\
    addParam(createParamCentered<CKSS>(\
        Vec(x,y), module, modname::param\
        ));\
\

#define BUTTON(x,y, type, modname, param, ...)\
    addParam(createParamCentered<type>(\
        Vec(x,y), module, modname::param\
        ));\
\

#define DEPTH_CONFIGURE configParam(DEPTH_PARAM, 1, 16, 8, "Conversion bit depth");

#define DEPTH_WIDGETS(x,y,modname)\
    addInput(createInput<PJ301MPort>(\
        Vec(x, y+2.5), module, modname::DEPTH_INPUT\
        ));\
\
\
    addParam(createParam<RoundBlackSnapKnob>(\
        Vec(x+27.5, y+1), module, modname::DEPTH_PARAM));\
\
    addOutput(createOutput<PJ301MPort>(\
        Vec(x+60.25, y+2.5), module, modname::DEPTH_OUTPUT\
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
    addParam(createParam<RoundBlackKnob>(\
        Vec(xoff+30.15, yoff+.15), module, name ## _PARAM+idx\
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


#define CV_ATV_CONFIGURE(min, max, def, name, idx)\
configParam(name ## CV_PARAM + idx, -1,1,0, "CV Gain"); \
configParam(name ## _PARAM + idx, min, max, def, "Value"); \

#define CV_ATV_PARAM(xoff, yoff, name, min, max, def, idx)\
    addInput(createInput<PJ301MPort>(\
        Vec(xoff, yoff), module, name ## _INPUT+idx\
        ));\
\
    addParam(createParam<RoundTinyBlackKnob>(\
        Vec(xoff+35, yoff+3.5), module, name ## CV_PARAM+idx\
        ));\
\
    addParam(createParam<RoundLargeBlackKnob>(\
        Vec(xoff+62.65, yoff-6.35), module, name ## _PARAM+idx\
        ));\

#define CV_ATV_VALUE(name, max, idx)\
params[name ## _PARAM+idx].value\
+ max*params[name ## CV_PARAM+idx].value*inputs[name ## _INPUT+idx].value/10;\
 


using namespace rack;

template <typename TBase = GrayModuleLightWidget>
struct TOrangeLight : TBase {
    TOrangeLight() {
        this->addBaseColor(nvgRGBAf(1, .33, 0, 1));
    }
};
using OrangeLight = TOrangeLight<>;


//For handling dialog paths
struct PathMemory {
    std::string dir;
    std::string file;

    void update(const char* path);
    json_t* to_json();
    void from_json(json_t* rootJ);
    std::string get_path();

    char* file_dialog(osdialog_file_action action, osdialog_filters* filters);
    char* file_dialog(osdialog_file_action action, const char* defdir, const char* deffile, const char* dialog_filter_spec);
};


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
    void onSelectText(const event::SelectText &e) override;

    int outNum = 0;
};


struct RoundTinyBlackKnob : SvgKnob {
    RoundTinyBlackKnob() {
        setSvg(SVG::load(asset::plugin(pluginInstance, "res/Components/RoundTinyBlackKnob.svg")));
    }
};

struct RoundTinyBlackSnapKnob : SvgKnob {
    RoundTinyBlackSnapKnob() {
        setSvg(SVG::load(asset::plugin(pluginInstance, "res/Components/RoundTinyBlackKnob.svg")));
        snap = true;
        smooth = false;
    }
};
 
 
struct DWhite : SVGSwitch {
    bool momentary=true;
    DWhite() {
        addFrame(SVG::load(asset::plugin(pluginInstance, "res/Components/1DWHT_0.svg")));
        addFrame(SVG::load(asset::plugin(pluginInstance, "res/Components/1DWHT_1.svg")));
    }
};

struct DWhiteLatch : SVGSwitch {
    DWhiteLatch() {
        addFrame(SVG::load(asset::plugin(pluginInstance, "res/Components/1DWHT_0.svg")));
        addFrame(SVG::load(asset::plugin(pluginInstance, "res/Components/1DWHT_1.svg")));
    }
};


static const float KNOB_SENSITIVITY = 0.0015f;

struct PngWidget : ModuleWidget {

    int png_handle = 0;

    std::string png_path;

    void drawLayer(const DrawArgs& args, int layer) override
    {
        ModuleWidget::drawLayer(args, layer);

        //layers are -1 and 1
        if (layer != -1)
        {
            return ;
        }

        /*
        Based on https://community.vcvrack.com/t/is-it-possible-to-load-a-static-panel-image/16807/9
        */

        nvgSave(args.vg);
        nvgBeginPath(args.vg);
        if(png_handle == 0)
        {
            png_handle = nvgCreateImage(
                args.vg,
                asset::plugin(pluginInstance, png_path).c_str(),
                0
                );
        }
        float w = box.size.x;
        float h = box.size.y;
        NVGpaint png_paint = nvgImagePattern(args.vg, 0, 0, w,h, 0, png_handle, 1.0f);
        nvgRect(args.vg, 0, 0, w,h);
        nvgFillPaint(args.vg, png_paint);
        nvgFill(args.vg);
        nvgClosePath(args.vg);
        nvgRestore(args.vg);
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

extern Model* modelOdysseus;

extern Model* modelMneme;

extern Model* modelAchilles;

extern Model* modelQMod;

extern Model* modelConvo;

extern Model* modelPolyphemus2;

extern Model* modelPrometheus2;

extern Model* modelLatComp;

extern Model* modelMetadataMain;

extern Model* modelMetadataFiles;

extern Model* modelDecayTimer;

extern Model* modelTiaI;

extern Model* modelTiaIExpander;

extern Model* modelMatI;

extern Model* modelCobaltI;

extern Model* modelSixty;

extern Model* modelOnce;

extern Model* modelLachesisI;

