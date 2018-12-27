#include "TechTechTechnologies.hpp"
#include <math.h>
#define BITL 20
#define N 1
#define NIN 3
#define NOUT 4
#define BUFL (1<<16)
#define PI 3.1415926536

struct Athena : Module {
    enum ParamIds {
        ENUMS(ANGLE_PARAM,N),
        ENUMS(RADIUS_PARAM, N),
        NUM_PARAMS
    };
    enum InputIds {
        ENUMS(SIGNAL_INPUT, N),
        NUM_INPUTS
    };
    enum OutputIds {
        ENUMS(LEFT_OUTPUT, N),
        ENUMS(RIGHT_OUTPUT, N),
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS
    };

    int ready = 0;


    Label* testLabel;


    Athena() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS) {}
    void step() override;

    // For more advanced Module features, read Rack's engine.hpp header file
    // - toJson, fromJson: serialization of internal data
    // - onSampleRateChange: event triggered by a change of sample rate
    // - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};

void Athena::step() {
    float deltaTime = 1.0 / engineGetSampleRate();

    if(ready == 0) return;

    //One big circular buffer
    //Push data into buffer


    for(int j = 0; j < N; ++j)
    {
        float ears = 0.1;
        float inval = inputs[SIGNAL_INPUT+j].value;
        float r = params[RADIUS_PARAM+j].value;
        float a = params[ANGLE_PARAM+j].value*PI*2;
        float oval[2]; //output val
        float rval[2]; //distance val
        float dval[2]; //delay val
        float gval[2]; //gain val

        for(int i = 0; i < 2; ++i)
        {
            float x = r*cos(a);
            float y = r*sin(a);
            float idx = i*2-1;
            float xoff = x+idx*ears;
            rval[i] = sqrt(y*y+xoff*xoff);
            

        }
        outputs[LEFT_OUTPUT+j].value=rval[0];
        outputs[RIGHT_OUTPUT+j].value=rval[1];
    }


     
//    char tstr[256];
//    sprintf(tstr, ": %f", pos[0]);
//    if(testLabel)
//        testLabel->text = tstr;
    
}

struct AthenaWidget : ModuleWidget
{
    AthenaWidget(Athena* module);
};


AthenaWidget::AthenaWidget(Athena* module) : ModuleWidget(module) {
    box.size = Vec(20* RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
    
    {
        SVGPanel *panel = new SVGPanel();
        panel->box.size = box.size;
        panel->setBackground(SVG::load(assetPlugin(plugin, "res/Athena.svg")));
        addChild(panel);
    }

    addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
    addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
    addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));


    float xoff, yoff;

    for(int j = 0; j < N; ++j)
    {
        xoff = 73.929*j;
        yoff = 311.307+10;

        INPORT(6.721+12.5+xoff, 380-(yoff), Athena, SIGNAL_INPUT, j)
        KNOB(xoff+44.186+12,380-(yoff), 0, 10, 1, Tiny, Athena, RADIUS_PARAM, j)
        KNOB(xoff+31+6.707,380-(233.164+29), 0, 1, 0, Huge, Athena, ANGLE_PARAM, j)
        OUTPORT(6.721+12.5+xoff, 380-(yoff)-20, Athena, LEFT_OUTPUT, j)
        OUTPORT(6.721+12.5+xoff, 380-(yoff)+20, Athena, RIGHT_OUTPUT, j)
    }
 
    auto* label = new Label();
    label->box.pos=Vec(30, 0);
    label->text = "";
    label->color = nvgRGB(0,0,0);
    addChild(label); 
    module->testLabel = label;

    module->ready = 1;

}

Model* modelAthena = Model::create<Athena, AthenaWidget>(
        "TechTech Technologies", "Athena", "Athena", 
        DELAY_TAG
        );


