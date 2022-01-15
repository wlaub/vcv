#include "TechTechTechnologies.hpp"
#define BITL 16

struct DAC : Module {
	enum ParamIds {
		PITCH_PARAM,
        DEPTH_PARAM,
        GATE_INPUT,
        BITS_PARAM,
		NUM_PARAMS=BITS_PARAM+BITL
	};
	enum InputIds {
        DEPTH_INPUT,
		PITCH_INPUT,
        BITS_INPUT, 
		NUM_INPUTS=BITS_INPUT+BITL
	};
	enum OutputIds {
		ANLG_OUTPUT,
        DIGI_OUTPUT,
        DEPTH_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		BLINK_LIGHT,
        BITIND_LIGHT,
        BIT_LIGHT=BITIND_LIGHT+BITL,
		NUM_LIGHTS=BIT_LIGHT+BITL
	};

    SchmittTrigger bitTrigger[BITL];

    Label* valLabel;

	DAC()
    {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        DEPTH_CONFIGURE
        for(int x = 0; x < 4; ++x)
        {
            for(int y = 0; y < 4; ++y)
            {
                int i = x+y*4;
                configParam(BITS_PARAM+i, 0,1,0, "");
            }
        }
        valLabel = new Label();

    }
	void step() override;

	// For more advanced Module features, read Rack's engine.hpp header file
	// - dataToJson, dataFromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};


void DAC::step() {
	// Implement a simple sine oscillator
	//float deltaTime = 1.0 / engineGetSampleRate();

	// Compute the frequency from the pitch parameter and input

    DEPTH_STEP

    int binVal = 0;

    for(int i = 0; i < BITL; ++i)
    {
        int bitVal;
        float switchVal = params[BITS_PARAM+i].value;
        int nval = (switchVal > .5) ? 1 : 0;
        bitVal = nval;


        if(inputs[BITS_INPUT+i].active)
        {
            bitTrigger[i].process(inputs[BITS_INPUT+i].value);
            nval = bitTrigger[i].isHigh() ? 1:0;
            bitVal = nval;
        }

        lights[BIT_LIGHT+i].value = (i < depth) ? 1 : 0;
        lights[BITIND_LIGHT+i].value = bitVal;
        if(i < depth)
            binVal |= ((bitVal&1)<<i);
    }

    outputs[DIGI_OUTPUT].value = binVal;
    outputs[ANLG_OUTPUT].value = num_to_cv(binVal, depth);
    outputs[DEPTH_OUTPUT].value = depth_to_cv(depth);
    char temp[256];
    sprintf(temp, "0x%04x\n%i", binVal, binVal);
    if(valLabel)
        valLabel->text = temp;

}

struct DACWidget : ModuleWidget
{
    DACWidget(DAC* module);
};

DACWidget::DACWidget(DAC* module) {
    setModule(module);
    box.size = Vec(16 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(asset::plugin(pluginInstance, "res/DAC.svg")));
		addChild(panel);
	}

	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

    float r;
    float insx = 30;
    float insy = 60;
    float gap;    

    DEPTH_WIDGETS(7.5, 46.5, DAC)

    if(module)
    {
        Label* label = module->valLabel;
        label->box.pos=Vec(90, insy-15);
        label->text = "TESTTESTESTESTETETSDASDASDA";
        addChild(label); 
        module->valLabel = label;
    }

    insx = 200;
    gap = 4;
    auto *anlgOut = createOutput<PJ301MPort>(
        Vec(insx-gap/2, insy), module, DAC::DIGI_OUTPUT
        );
    center(anlgOut,2,1);

    auto *digiOut = createOutput<PJ301MPort>(
        Vec(insx+gap/2, insy), module, DAC::ANLG_OUTPUT
        );

    center(digiOut, 0, 1);

    

    addOutput(digiOut);
    addOutput(anlgOut);

    r = 60;
    insx = 20;
    insy = 120;

    gap = 4;

    for(int x = 0; x < 4; ++x)
    {
        for(int y = 0; y < 4; ++y)
        {
            int i = x+y*4;
            float xoff = insx+x*r;
            float yoff = insy+y*r;

            auto* jack = createInput<PJ301MPort>(
                Vec(xoff, yoff), module, DAC::BITS_INPUT+i
                );

            center(jack);

            auto* button = createParam<CKSS>(
                Vec(xoff+jack->box.size.x/2+gap, yoff), module, DAC::BITS_PARAM+i);

            center(button, 0);

            auto* light = createLight<MediumLight<BlueLight>>(
                Vec(xoff, yoff+jack->box.size.y/2+gap), module, DAC::BIT_LIGHT+i
                );
            
            center(light, 1, 0);
            addChild(light);

            auto* light2 = createLight<MediumLight<GreenLight>>(
                Vec(button->box.pos.x+button->box.size.x/2, yoff+jack->box.size.y/2+gap), module, DAC::BITIND_LIGHT+i
                );
            
            center(light2, 1, 0);
            addChild(light2);


            addInput(jack);
            addParam(button);

        }
    }

}

Model *modelDAC = createModel<DAC, DACWidget>(
    "DAC" 
    );


