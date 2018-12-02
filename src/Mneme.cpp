#include "TechTechTechnologies.hpp"
#include <math.h>
#define BITL 20
#define N 4
#define NIN 3
#define NOUT 4
#define BUFL (1<<12)

typedef struct
{
    float data[BUFL];
    unsigned int head;
} cbuf;

float interpolate(float left, float right, float alpha)
{
    //Buffer interpolate function from left/right values and distance from
    //left index to value at the corresponding position
    return left*(1-alpha)+right*(alpha);
}
float reverse_interpolate(float val, float alpha, int dir)
{
    //Reverse interpolate from value at float position alpha from left index
    //to value that would appear at left or right index depending on dir
    if(dir == 0) //left
    {
        return val*(1-alpha);
    }
    else //right
    {
        return alpha*val;
    }
}

unsigned int buffer_get_index(cbuf* buffer, unsigned int pos)
{// printf("buffer_get_index\n");
    //Return the index of the given position in the past
//    printf("pos - %i\n", pos);
//    printf("head - %i\n", buffer->head);
   
    while(pos >= BUFL)
    {
        pos -= BUFL;
    }
    if(pos <= buffer->head)
    {
        return buffer->head - pos;
    }
    else
    {
        if(buffer -> head != BUFL-1)
            return (BUFL-pos)+buffer->head;
        else
            return 0;
        //pos = head + 1
        //BUFL-head-1+head = BUFL-1
        //
        //pos = BUFL-1
        //BUFL-BUFL+1=head+1
    }

}

#define BUF_DECODE(buffer, loc)\
    float alpha;\
    unsigned int left, right;\
    left = buffer_get_index(buffer, floor(loc));\
    right = left+1; if(right == BUFL) --right;\
    alpha = loc-floor(loc);\

int buffer_push(cbuf* buffer, float val)
{// printf("buffer_push\n");
    buffer->head +=1;
    if(buffer->head == BUFL) buffer->head = 0;
    buffer->data[buffer->head] = val;

    return 0;
}

float buffer_add(cbuf* buffer, int index, float val)
{
    //Add the given value at the given index; Clamp as needed
    buffer->data[index]  += val;
    buffer->data[index] = clamp(buffer->data[index], -10.f, 10.f);

    return buffer->data[index];
}

int buffer_insert(cbuf* buffer, float loc, float val)
{ 
    //Insert the value at position val into the buffer using reverse
    //interpolation
    BUF_DECODE(buffer, loc)

    buffer->data[left]  = reverse_interpolate(val, alpha, 0);
    buffer->data[right] = reverse_interpolate(val, alpha, 1);

    return 0;
}


int buffer_add_insert(cbuf* buffer, float loc, float val)
{// printf("buffer_add_insert\n");
    //Same as insert, but add instead of replace
    BUF_DECODE(buffer, loc)

    buffer_add(buffer, left, reverse_interpolate(val, alpha, 0));
//    buffer->data[left] += reverse_interpolate(val, alpha, 1);
//    buffer->data[left] = clamp(buffer->data[left], -10.f, 10.f);
    buffer_add(buffer, left, reverse_interpolate(val, alpha, 0));
//    buffer->data[right] += reverse_interpolate(val, alpha, 1);
//    buffer->data[right] = clamp(buffer->data[right], -10.f, 10.f);   
    //Do clipping

    return 0;
}

float buffer_get_tap(cbuf* buffer, float loc)
{// printf("buffer_get_tap\n");
    //Get the value at the given float index
    BUF_DECODE(buffer, loc)

    return interpolate(buffer->data[left], buffer->data[right], alpha);

}

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

    cbuf buffer;

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

    buffer_push(&buffer, vals[0]);

    for(int j = 0; j < N; ++j)
    {
        pos[j+1] = (inputs[DELAY_INPUT+j].value*params[DELAY_CV_PARAM+j].value)*(BUFL-1)
               + params[DELAY_PARAM+j].value+pos[j];
        //maybe clip the length?
        //account for sample rate

        vals[j+1] = buffer_get_tap(&buffer, pos[j+1]);
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
            buffer_add(&buffer, buffer.head, accum);
        }
        if(j>0)
        {
            buffer_add_insert(&buffer, pos[j], accum);
            outputs[SIGNAL_OUTPUT+j-1].value = (accum+vals[j])*params[OUT_CV_PARAM+j-1].value;
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
        "TechTech Technologies", "Mneme", "Mneme", 
        DELAY_TAG, OSCILLATOR_TAG
        );


