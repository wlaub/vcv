#include "TechTechTechnologies.hpp"
#define BITL 16
#define NSEQ 2

struct Vulcan : Module {
	enum ParamIds {
        DEPTH_PARAM,
        CLOCK_PARAM,
        ROUTE_PARAM,
        POS_PARAM = CLOCK_PARAM+NSEQ,
        WIDTH_PARAM=POS_PARAM + NSEQ,
        RATE_PARAM =WIDTH_PARAM+NSEQ,
        MODE_PARAM =RATE_PARAM +NSEQ,
		NUM_PARAMS = MODE_PARAM+NSEQ
	};
	enum InputIds {
		DEPTH_INPUT,
        CLOCK_INPUT,
	    POS_INPUT = CLOCK_INPUT+NSEQ,
        WIDTH_INPUT=POS_INPUT + NSEQ,
        RATE_INPUT =WIDTH_INPUT+NSEQ,
        MODE_INPUT =RATE_INPUT +NSEQ,
		NUM_INPUTS = MODE_INPUT+NSEQ
	};
	enum OutputIds {
        DEPTH_OUTPUT,
        POS_OUTPUT,
        TRIG_OUTPUT=POS_OUTPUT+NSEQ,
        XOR_OUTPUT=TRIG_OUTPUT+NSEQ,
        AND_OUTPUT,
        OR_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

    int ready = 0;

    SchmittTrigger clockTrigger[NSEQ];
    SchmittTrigger modeTrigger[NSEQ];

    Label* testLabel;

    unsigned short pos[NSEQ] = {0};

    float phase = 0;

	Vulcan() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;

	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};

void Vulcan::step() {
	float deltaTime = 1.0 / engineGetSampleRate();

    if(ready == 0) return;

    DEPTH_STEP

    for(int j = 0; j < NSEQ; ++j)
    { 
        if(clockTrigger[j].process(inputs[CLOCK_INPUT+j].value))
        {

        }
    }

}


VulcanWidget::VulcanWidget() {
	Vulcan *module = new Vulcan();
	setModule(module);
	box.size = Vec(18* RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/Vulcan.svg")));
		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createScrew<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));


    DEPTH_WIDGETS(17.5, 50, Vulcan)

    float xoff, yoff, gap;

    xoff = 165;
    yoff = 380-285-45;
    addOutput(createOutput<PJ301MPort>(
        Vec(xoff+2.5, yoff+2.5), module, Vulcan::AND_OUTPUT
        ));
    addOutput(createOutput<PJ301MPort>(
        Vec(xoff+32.5, yoff+2.5), module, Vulcan::XOR_OUTPUT
        ));
    addOutput(createOutput<PJ301MPort>(
        Vec(xoff+62.5, yoff+2.5), module, Vulcan::OR_OUTPUT
        ));



    addParam(createParam<CKSS>(
        Vec(128, 380-162.18-20.641), module, Vulcan::ROUTE_PARAM,
        0, 1, 1
        ));





    for(int j = 0; j < NSEQ; ++j)
    {
        xoff = 15+135*j;
        yoff = 380-232.5-30;
        gap = 37.5;

        PARAM_PAIR(xoff, yoff, Vulcan::CLOCK, 5, j)
        
        PARAM_PAIR(xoff, yoff+gap, Vulcan::POS, 0, j)

        PARAM_PAIR(xoff, yoff+gap*2, Vulcan::WIDTH, 10, j)

        addInput(createInput<PJ301MPort>(
            Vec(xoff+2.5, yoff+2.5+gap*3), module, Vulcan::RATE_INPUT+j
            ));
        addParam(createParam<RoundSmallBlackSnapKnob>(
            Vec(xoff+31, yoff+1+gap*3), module, Vulcan::RATE_PARAM+j,
            0, 16, 1
            ));


        addInput(createInput<PJ301MPort>(
            Vec(xoff+2.5, yoff+2.5+gap*3+35), module, Vulcan::MODE_INPUT+j
            ));
        addParam(createParam<CKSS>(
            Vec(xoff+38, yoff+4.68+gap*3+35), module, Vulcan::MODE_PARAM+j,
            0, 1, 1
            ));
    

        yoff = 380-30-45;

        addOutput(createOutput<PJ301MPort>(
            Vec(xoff+2.5, yoff+2.5), module, Vulcan::POS_OUTPUT+j
            ));

        addOutput(createOutput<PJ301MPort>(
            Vec(xoff+32.5, yoff+2.5), module, Vulcan::TRIG_OUTPUT+j
            ));

    }


    auto* label = new Label();
    label->box.pos=Vec(0, 30);
    label->text = "";
    addChild(label); 
    module->testLabel = label;

    module->ready = 1;

}
