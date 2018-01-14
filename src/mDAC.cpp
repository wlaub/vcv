#include "TechTechTechnologies.hpp"
#define BITL 16
#define NOUT 8

struct mDAC : Module {
	enum ParamIds {
        DEPTH_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		NUM_INPUTS
	};
	enum OutputIds {
		ANLG_OUTPUT,
        DEPTH_OUTPUT=ANLG_OUTPUT+NOUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

    Label* testLabel;

    TextField inputs[NOUT];

	mDAC() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;

	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};


void mDAC::step() {
	// Implement a simple sine oscillator
	//float deltaTime = 1.0 / engineGetSampleRate();

	// Compute the frequency from the pitch parameter and input

    int depth = params[DEPTH_PARAM].value;

    int binVal = 0;

    for(int i = 0; i < NOUT; ++i)
    {
//        outputs[ANLG_OUTPUT+i].value = num_to_cv(binVal, depth);
//        outputs[DEPTH_OUTPUT].value = depth_to_cv(depth);
 
    }

//    char tstr[256];
//    sprintf(tstr, "0x%04x\n%i");
//    if(testLabel)
//        testLabel->text = temp;

}


mDACWidget::mDACWidget() {
	mDAC *module = new mDAC();
	setModule(module);
	box.size = Vec(6 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/mDAC.svg")));
		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createScrew<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

    addParam(createParam<RoundSmallBlackSnapKnob>(
        Vec(12.5, 35), module, mDAC::DEPTH_PARAM,
        1, 16, 8
        ));

    addOutput(createOutput<PJ301MPort>(
        Vec(52.5, 37.5), module, mDAC::DEPTH_OUTPUT
        ));


    for(int i = 0; i < NOUT; ++i)
    {

        float yoff = i*35+85;
        addOutput(createOutput<PJ301MPort>(
            Vec(57.5, yoff+2.5), module, mDAC::ANLG_OUTPUT+i
            ));
        
        TextField* text = &(module->inputs[i]);
        text->box.pos = Vec(5, yoff+5);
        text->box.size = Vec(50, 20);
        addChild(text);

    }


    Label* label = new Label();
    label->box.pos=Vec(30, 0);
    label->text = "";
    addChild(label); 
    module->testLabel = label;


}
