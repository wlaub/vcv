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

    TextField infields[NOUT];

	mDAC() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;

	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
    void jsontag(char* result, int i)
    {

        sprintf(result, "text%i", i);
    }


    json_t* toJson() override
    {
        json_t *rootJ = json_object();

        char tstr[256];

        for(int i = 0; i < NOUT; ++i)
        {
            jsontag(tstr, i);
            json_object_set_new(rootJ, tstr,
                json_string(infields[i].text.c_str())
                );
        }

        return rootJ;
    }


    void fromJson(json_t *rootJ) override
    {
        char tstr[256];

        for(int i = 0; i < NOUT; ++i)
        {
            jsontag(tstr, i);
            infields[i].text = json_string_value(
                json_object_get(rootJ, tstr)
                );
        }

    }



};


void mDAC::step() {
	// Implement a simple sine oscillator
	//float deltaTime = 1.0 / engineGetSampleRate();

	// Compute the frequency from the pitch parameter and input

    int depth;
    if(inputs[DEPTH_INPUT].active)
        depth = cv_to_depth(inputs[DEPTH_INPUT].value);
    else
        depth = params[DEPTH_PARAM].value;



    for(int i = 0; i < NOUT; ++i)
    {
        int val;
        int res;
        const char* str = infields[i].text.c_str();
        res = sscanf(str, "0x%x", &val);
        if(res == 0)
            res = sscanf(str, "%i", &val);

        if(res != 0)
        {
            int mask = 0;
            for(int i = 0; i < depth; ++i)
            {
                mask<<=1;
                mask|=1;
            }
            val &= mask;

            outputs[ANLG_OUTPUT+i].value = num_to_cv(val, depth);
        }
 
    }

//    char tstr[256];
//    sprintf(tstr, "0x%04x\n%i");
//    if(testLabel)
//        testLabel->text = temp;

}


mDACWidget::mDACWidget() {
	mDAC *module = new mDAC();
	setModule(module);
	box.size = Vec(8 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

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


    addInput(createInput<PJ301MPort>(
        Vec(17.5, 37.5), module, mDAC::DEPTH_INPUT
        ));


    addParam(createParam<RoundSmallBlackSnapKnob>(
        Vec(45, 35), module, mDAC::DEPTH_PARAM,
        1, 16, 8
        ));

    addOutput(createOutput<PJ301MPort>(
        Vec(77.5, 37.5), module, mDAC::DEPTH_OUTPUT
        ));


    for(int i = 0; i < NOUT; ++i)
    {

        float yoff = i*35+85;
        addOutput(createOutput<PJ301MPort>(
            Vec(77.5, yoff+2.5), module, mDAC::ANLG_OUTPUT+i
            ));
        
        TextField* text = &(module->infields[i]);
        text->box.pos = Vec(15, yoff+5);
        text->box.size = Vec(50, 20);
        addChild(text);

    }


    Label* label = new Label();
    label->box.pos=Vec(30, 0);
    label->text = "";
    addChild(label); 
    module->testLabel = label;
}


