#include "TechTechTechnologies.hpp"
#include <math.h>
#define BITL 20
#define N 4
#define NIN 3
#define NOUT 4
#define BUFL (1<<16)

struct Mneme : Module {
    enum ParamIds {
        ENUMS(DELAY_PARAM,N),
        ENUMS(DELAY_CV_PARAM, N),
        ENUMS(IN_CV_PARAM, N),
        ENUMS(OUT_CV_PARAM, N),
        ENUMS(FB_CV_PARAM, N*N),
        NUM_PARAMS
    };
    enum InputIds {
        ENUMS(DELAY_INPUT, N),
        ENUMS(SIGNAL_INPUT, N),
        NUM_INPUTS
    };
    enum OutputIds {
        ENUMS(SIGNAL_OUTPUT, N),
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS
    };

    int ready = 0;

    float pos[N+1];
    float vals[N+1];

    Label* testLabel;

    ttt::CircularBuffer* buffer;

    Mneme() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS) {}
    void step() override;

    // For more advanced Module features, read Rack's engine.hpp header file
    // - toJson, fromJson: serialization of internal data
    // - onSampleRateChange: event triggered by a change of sample rate
    // - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};

void Mneme::step() {
    float deltaTime = 1.0 / engineGetSampleRate();

    if(ready == 0) return;

    //One big circular buffer
    //Push data into buffer

    vals[0] = inputs[SIGNAL_INPUT].value*params[IN_CV_PARAM].value;
    pos[0] = 0;

    buffer->push(vals[0]);

    for(int j = 0; j < N; ++j)
    {
        pos[j+1] = (inputs[DELAY_INPUT+j].value*params[DELAY_CV_PARAM+j].value)*(BUFL-1)
               + params[DELAY_PARAM+j].value+pos[j];
        //maybe clip the length?
        //account for sample rate

        vals[j+1] = buffer->get_tap_floating(pos[j+1]);
    }

    for(int j = 0; j < N+1; ++j)
    {
        float accum = 0;

        int k = 0; //k tracks the source index
        //Calculated feedpack to point j where
        // j = 0 is the input to the first tap
        // j = N is the output of the Nth tap
        //
        for(int i = 0; i < N; ++i)
        {
            if(i==j) ++k;
            else
            {
                accum += vals[k]*params[FB_CV_PARAM+j*N+i].value;
            }
            ++k;
        }
        //Input j=0 is into tap 1
        //Input j=N-1 is fed intp tap N
        //j=0 was already done to acquire the input
        if(j != N && j!= 0)
            accum += inputs[SIGNAL_INPUT+j].value * params[IN_CV_PARAM+j].value;

        if(j==0)
        {
            buffer->add(0, accum);
        }
        if(j>0)
        {
            buffer->add_floating(pos[j], accum);
//            outputs[SIGNAL_OUTPUT+j-1].value = (accum+vals[j])*params[OUT_CV_PARAM+j-1].value;
            outputs[SIGNAL_OUTPUT+j-1].value = 
                buffer->get_tap_floating(pos[j])*params[OUT_CV_PARAM+j-1].value;
        }
    }
 
     
//    char tstr[256];
//    sprintf(tstr, ": %f", pos[0]);
//    if(testLabel)
//        testLabel->text = tstr;
    
}

struct MnemeWidget : ModuleWidget
{
    MnemeWidget(Mneme* module);
};


MnemeWidget::MnemeWidget(Mneme* module) : ModuleWidget(module) {
    box.size = Vec(20* RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
    
    {
        SVGPanel *panel = new SVGPanel();
        panel->box.size = box.size;
        panel->setBackground(SVG::load(assetPlugin(plugin, "res/Mneme.svg")));
        addChild(panel);
    }

    addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
    addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
    addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

    module->buffer = new ttt::CircularBuffer(BUFL);

    float xoff, yoff;

    for(int j = 0; j < N; ++j)
    {
        xoff = 73.929*j;
        yoff = 311.307+10;

        INPORT(6.721+12.5+xoff, 380-(yoff), Mneme, DELAY_INPUT, j)
        KNOB(xoff+44.186+12,380-(yoff), -1, 1, 0, Tiny, Mneme, DELAY_CV_PARAM, j)

        KNOB(xoff+31+6.707,380-(233.164+29), 1, BUFL-1, 10, Huge, Mneme, DELAY_PARAM, j)

        for(int i = 0; i < N; ++i)
        {

            yoff = 192.561+12.5 - 38.891*1;

            ParamWidget* param;
            if(i<j)
            {
                param = ParamWidget::create<LEDSliderRed>(
                    Vec(xoff + 13.413+10.63/2 + i*(50-10.63)/(N-1), 380-yoff),
                    module, Mneme::FB_CV_PARAM + N*j+i,
                    -1, 1, 0
                );
            }
            else
            {
                param = ParamWidget::create<LEDSliderBlue>(
                    Vec(xoff + 13.413+10.63/2 + i*(50-10.63)/(N-1), 380-yoff),
                    module, Mneme::FB_CV_PARAM + N*j+i,
                    -1, 1, 0
                );
            }
            center(param,1,1);
            addParam(param);
        }

        yoff = 192.561+12.5 - 38.891*(3);
        INPORT(6.721+12.5+xoff, 380-(yoff), Mneme, SIGNAL_INPUT, j)
        KNOB(xoff+44.186+12,380-(yoff), -1,1,0, Tiny, Mneme, IN_CV_PARAM, j)

        yoff = 192.561+12.5 - 38.891*(4);
        OUTPORT(6.721+12.5+xoff, 380-(yoff), Mneme, SIGNAL_OUTPUT, j)
        KNOB(xoff+44.186+12,380-(yoff), -1,1,0, Tiny, Mneme, OUT_CV_PARAM, j)

    }
 
    auto* label = new Label();
    label->box.pos=Vec(30, 0);
    label->text = "";
    label->color = nvgRGB(0,0,0);
    addChild(label); 
    module->testLabel = label;

    module->ready = 1;

}

Model* modelMneme = Model::create<Mneme, MnemeWidget>(
        "Mneme", 
        );


