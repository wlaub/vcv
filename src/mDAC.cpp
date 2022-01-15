#include "TechTechTechnologies.hpp"
#define BITL 16
#define NOUT 8

struct mDAC : Module {
	enum ParamIds {
        DEPTH_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
        DEPTH_INPUT,
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

    NumField** infields;

    int ready = 0;

	mDAC() 
    {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        DEPTH_CONFIGURE
    }
	void step() override;

	// For more advanced Module features, read Rack's engine.hpp header file
	// - dataToJson, dataFromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu

};


void mDAC::step() {
	// Implement a simple sine oscillator
	//float deltaTime = 1.0 / engineGetSampleRate();

	// Compute the frequency from the pitch parameter and input

    if(ready == 0) return;

    DEPTH_STEP

    for(int i = 0; i < NOUT; ++i)
    {
        int val = infields[i]->outNum;
        outputs[ANLG_OUTPUT+i].value = num_to_cv(val, depth);
    }

//    char tstr[256];
//    sprintf(tstr, "0x%04x\n%i");
//    if(testLabel)
//        testLabel->text = temp;

}

struct mDACWidget : ModuleWidget
{
    NumField** infields;
    mDACWidget(mDAC* module);
    void jsontag(char* result, int i);
//    json_t *dataToJson() override;
//    void dataFromJson(json_t *rootJ) override;
};




mDACWidget::mDACWidget(mDAC* module) {
		setModule(module);
	box.size = Vec(8 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(asset::plugin(pluginInstance, "res/mDAC.svg")));
		addChild(panel);
	}

	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));


    DEPTH_WIDGETS(17.5, 35, mDAC) 


    infields = new NumField*[NOUT];

    if(module)
        module->infields = infields;

    for(int i = 0; i < NOUT; ++i)
    {

        float yoff = i*35+85;
        addOutput(createOutput<PJ301MPort>(
            Vec(77.5, yoff+2.5), module, mDAC::ANLG_OUTPUT+i
            ));

        infields[i] = new NumField();
        NumField* text = infields[i];
        text->box.pos = Vec(15, yoff+5);
        text->box.size = Vec(50, 20);
        addChild(text);

    }

    Label* label = new Label();
    label->box.pos=Vec(30, 0);
    label->text = "";
    addChild(label); 
    if(module)
        module->testLabel = label;

    if(module)
        module -> ready = 1;

}

void mDACWidget::jsontag(char* result, int i)
{

    sprintf(result, "text%i", i);
}

/*
json_t* mDACWidget::dataToJson()
{
    json_t *rootJ = ModuleWidget::dataToJson();

    char tstr[256];

    for(int i = 0; i < NOUT; ++i)
    {
        jsontag(tstr, i);
        json_object_set_new(rootJ, tstr,
            json_string(infields[i]->text.c_str())
            );
    }

    return rootJ;
}


void mDACWidget::dataFromJson(json_t *rootJ)
{
    char tstr[256];
    ModuleWidget::dataFromJson(rootJ);

    for(int i = 0; i < NOUT; ++i)
    {
        jsontag(tstr, i);
        infields[i]->text = json_string_value(
            json_object_get(rootJ, tstr)
            );
        infields[i]->onTextChange();
    }

}
*/
Model* modelmDAC = createModel<mDAC, mDACWidget>(
        "mDAC"
        );


