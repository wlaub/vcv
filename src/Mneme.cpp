#include "TechTechTechnologies.hpp"
#define BITL 20
#define N 4
#define NIN 3
#define NOUT 4
#define BUFL (10*48000)

typedef struct
{
    float data[BUFL];
    int head;
    int tail;
    float loc;
} cbuf;

struct Mneme : Module {
	enum ParamIds {
        DELAY_PARAM,
        DELAY_CV_PARAM = DELAY_PARAM+N,
        IN_CV_PARAM = DELAY_CV_PARAM+N,
        NUM_PARAMS = IN_CV_PARAM+N*NIN
	};
	enum InputIds {
        DELAY_INPUT,
		SIGNAL_INPUT = DELAY_INPUT+N,
		NUM_INPUTS = SIGNAL_INPUT+N*NIN
	};
	enum OutputIds {
        SIGNAL_OUTPUT,
		NUM_OUTPUTS = SIGNAL_OUTPUT+N*NOUT
	};
	enum LightIds {
        LOOP_LIGHTS,
		NUM_LIGHTS = LOOP_LIGHTS + N*BITL*3
	};

    int ready = 0;

    SchmittTrigger clockTrigger[N];
    SchmittTrigger gateTrigger[N];

    PulseGenerator trigPulse[N];

    Label* testLabel;

    cbuf buffer[N];

	Mneme() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;

	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};

void Mneme::step() {
	float deltaTime = 1.0 / engineGetSampleRate();

    if(ready == 0) return;

    for(int j = 0; j < N; ++j)
    { 


    }

}

struct MnemeWidget : ModuleWidget
{
    MnemeWidget(Mneme* module);
};


MnemeWidget::MnemeWidget(Mneme* module) : ModuleWidget(module) {
//	Mneme *module = new Mneme();
//	setModule(module);
	box.size = Vec(40* RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

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
        KNOB(xoff+44.186+12,380-(yoff), 0, 1, .5, Tiny, Mneme, DELAY_CV_PARAM, j)

        KNOB(xoff+31+6.707,380-(233.164+29), 0, 1, .5, Huge, Mneme, DELAY_PARAM, j)


        for(int i = 0; i < NIN; ++i)
        {
            yoff = 192.561+12.5 - 38.891*i;
            INPORT(6.721+12.5+xoff, 380-(yoff), Mneme, SIGNAL_INPUT, j*i)
            KNOB(xoff+44.186+12,380-(yoff), 0, 1, .5, Tiny, Mneme, IN_CV_PARAM, j*i)
        }

        for(int i = 0; i < NOUT; i+=2)
        {
            yoff = 192.561+12.5 - 38.891*(NIN+i/2);
            OUTPORT(6.721+12.5+xoff, 380-(yoff), Mneme, SIGNAL_INPUT, j*i)
            OUTPORT(xoff+44.186+12,380-(yoff), Mneme, IN_CV_PARAM, j*i)
        }

    }

    auto* label = new Label();
    label->box.pos=Vec(0, 30);
    label->text = "";
    addChild(label); 
    module->testLabel = label;

    module->ready = 1;

}

Model* modelMneme = Model::create<Mneme, MnemeWidget>(
        "TechTech Technologies", "Mneme", "Mneme", 
        DELAY_TAG, OSCILLATOR_TAG
        );


