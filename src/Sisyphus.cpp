#include "TechTechTechnologies.hpp"
#define BITL 16
#define N 4

struct Sisyphus : Module {
	enum ParamIds {
        LENGTH_PARAM,
        RATE_PARAM = LENGTH_PARAM+N,
        MODE_PARAM = RATE_PARAM+N,
		NUM_PARAMS = MODE_PARAM+N
	};
	enum InputIds {
		SIGNAL_INPUT,
        GATE_INPUT = SIGNAL_INPUT+N,
        LENGTH_INPUT = GATE_INPUT+N,
        RATE_INPUT = LENGTH_INPUT+N,
        MODE_INPUT = RATE_INPUT+N,
		NUM_INPUTS = MODE_INPUT+N
	};
	enum OutputIds {
        SIGNAL_OUTPUT,
        TRIG_OUTPUT = SIGNAL_OUTPUT+N,
		NUM_OUTPUTS = TRIG_OUTPUT+N
	};
	enum LightIds {
        LOOP_LIGHTS,
		NUM_LIGHTS = LOOP_LIGHTS + N*BITL
	};

    int ready = 0;

    SchmittTrigger clockTrigger[N];
    SchmittTrigger gateTrigger[N];

    PulseGenerator trigPulse[N];

    Label* testLabel;

	Sisyphus() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;

	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};

void Sisyphus::step() {
	float deltaTime = 1.0 / engineGetSampleRate();

    if(ready == 0) return;

    for(int j = 0; j < N; ++j)
    { 
    }

}


SisyphusWidget::SisyphusWidget() {
	Sisyphus *module = new Sisyphus();
	setModule(module);
	box.size = Vec(18* RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/Sisyphus.svg")));
		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createScrew<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));


    float xoff, yoff, gap;

/*
    xoff = 165;
    yoff = 380-285-45;
    addOutput(createOutput<PJ301MPort>(
        Vec(xoff+2.5, yoff+2.5), module, Sisyphus::AND_OUTPUT
        ));
    addOutput(createOutput<PJ301MPort>(
        Vec(xoff+32.5, yoff+2.5), module, Sisyphus::XOR_OUTPUT
        ));
    addOutput(createOutput<PJ301MPort>(
        Vec(xoff+62.5, yoff+2.5), module, Sisyphus::OR_OUTPUT
        ));



    addParam(createParam<CKSS>(
        Vec(128, 380-162.18-20.641), module, Sisyphus::ROUTE_PARAM,
        0, 1, 1
        ));





    for(int j = 0; j < NSEQ; ++j)
    {
        xoff = 15+135*j;
        yoff = 380-232.5-30;
        gap = 37.5;

        PARAM_PAIR(xoff, yoff, Sisyphus::CLOCK, 5, j)
        
        PARAM_PAIR(xoff, yoff+gap, Sisyphus::POS, 0, j)

        PARAM_PAIR(xoff, yoff+gap*2, Sisyphus::WIDTH, 10, j)

        addInput(createInput<PJ301MPort>(
            Vec(xoff+2.5, yoff+2.5+gap*3), module, Sisyphus::RATE_INPUT+j
            ));
        addParam(createParam<RoundSmallBlackSnapKnob>(
            Vec(xoff+31, yoff+1+gap*3), module, Sisyphus::RATE_PARAM+j,
            0, 16, 1
            ));


        addInput(createInput<PJ301MPort>(
            Vec(xoff+2.5, yoff+2.5+gap*3+35), module, Sisyphus::MODE_INPUT+j
            ));
        addParam(createParam<CKSS>(
            Vec(xoff+38, yoff+4.68+gap*3+35), module, Sisyphus::MODE_PARAM+j,
            0, 1, 1
            ));
    

        yoff = 380-30-45;

        addOutput(createOutput<PJ301MPort>(
            Vec(xoff+2.5, yoff+2.5), module, Sisyphus::POS_OUTPUT+j
            ));

        addOutput(createOutput<PJ301MPort>(
            Vec(xoff+32.5, yoff+2.5), module, Sisyphus::TRIG_OUTPUT+j
            ));

    }
*/

    auto* label = new Label();
    label->box.pos=Vec(0, 30);
    label->text = "";
    addChild(label); 
    module->testLabel = label;

    module->ready = 1;

}
