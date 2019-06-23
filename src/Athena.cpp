#include "TechTechTechnologies.hpp"
#include <math.h>
#define BITL 20
#define N 3
#define NIN 3
#define NOUT 4
#define BUFL (1<<16)
#define PI 3.1415926536

struct Athena : Module {
    enum ParamIds {
        ENUMS(ANGLE_PARAM,1),
        ENUMS(RADIUS_PARAM, 1),
        NUM_PARAMS
    };
    enum InputIds {
        ENUMS(SIGNAL_INPUT, N),
        NUM_INPUTS
    };
    enum OutputIds {
        ENUMS(LEFT_OUTPUT, 1),
        ENUMS(RIGHT_OUTPUT, 1),
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

    float inval = inputs[SIGNAL_INPUT].value;

    outputs[LEFT_OUTPUT].value = 0;
    outputs[RIGHT_OUTPUT].value = 0;
    
    float r = params[RADIUS_PARAM].value;
    float aref = params[ANGLE_PARAM].value*PI*2;
    for(int j = 0; j < N; ++j)
    {
        float ears = 0.9;

        float a = aref + j*2*PI/N;
        while(a > PI) a-=2*PI;
        while(a < -PI) a+= 2*PI;
        float oval[2]; //output val
        float rval[2]; //distance val
        float dval[2]; //delay val
        float gval[2]; //gain val

        for(int i = 0; i < 2; ++i)
        {
            float x = r*sin(a); //0 is forward
            float y = r*cos(a);
            float idx = i*2-1;

            float xoff = x-idx*ears;
            
            rval[i] = sqrt(y*y+xoff*xoff);
            float k = 0.5-idx*abs(a/(2*PI)); // 0 when max, 1 when min
            
            k=20/(1+k*19);

            gval[i] = 1/(1+rval[i]*k);

            oval[i] = inval*gval[i]/N;
        }
        outputs[LEFT_OUTPUT].value+=oval[0];
        outputs[RIGHT_OUTPUT].value+=oval[1];
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


    xoff = 0;
    yoff = 311.307+10;

    INPORT(6.721+12.5+xoff, 380-(yoff), Athena, SIGNAL_INPUT, 0)
    OUTPORT(6.721+12.5+xoff, 380-(yoff)-20, Athena, LEFT_OUTPUT, 0)
    OUTPORT(6.721+12.5+xoff, 380-(yoff)+20, Athena, RIGHT_OUTPUT, 0)

    KNOB(xoff+44.186+12,380-(yoff), 0, 10, 1, Tiny, Athena, RADIUS_PARAM, 0)
    KNOB(xoff+31+6.707,380-(233.164+29), -.5, .5, 0, Huge, Athena, ANGLE_PARAM, 0)


    for(int j = 0; j < N; ++j)
    {
        xoff = 73.929*j;
        yoff = 311.307+10;


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
        "Athena", 
        );


