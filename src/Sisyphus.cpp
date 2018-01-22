#include "TechTechTechnologies.hpp"
#define BITL 20
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
	box.size = Vec(20* RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

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


    float xoff, yoff;


    for(int j = 0; j < N; ++j)
    {
        xoff = 47.5;
        yoff = 380-287.5-25+j*75;

        addInput(createInput<PJ301MPort>(
            Vec(xoff, yoff), module, Sisyphus::SIGNAL_INPUT+j
            ));
        addInput(createInput<PJ301MPort>(
            Vec(xoff, yoff+30), module, Sisyphus::GATE_INPUT+j
            ));


        xoff = 90;
        addParam(createParam<RoundSmallBlackKnob>(
            Vec(xoff+1, yoff-1.5), module, Sisyphus::LENGTH_PARAM+j,
            0,10,10
            ));
        addInput(createInput<PJ301MPort>(
            Vec(xoff+2.5, yoff+30), module, Sisyphus::LENGTH_INPUT+j
            ));

        xoff += 45;
        addParam(createParam<RoundSmallBlackKnob>(
            Vec(xoff+1, yoff-1.5), module, Sisyphus::RATE_PARAM+j,
            0,10,5
            ));
        addInput(createInput<PJ301MPort>(
            Vec(xoff+2.5, yoff+30), module, Sisyphus::RATE_INPUT+j
            ));

        xoff += 45;
        addParam(createParam<RoundSmallBlackSnapKnob>(
            Vec(xoff+1, yoff-1.5), module, Sisyphus::MODE_PARAM+j,
            0,1,0
            ));
        addInput(createInput<PJ301MPort>(
            Vec(xoff+2.5, yoff+30), module, Sisyphus::MODE_INPUT+j
            ));


        xoff+=45;
        addOutput(createOutput<PJ301MPort>(
            Vec(xoff+2.5, yoff), module, Sisyphus::SIGNAL_OUTPUT+j
            ));
        addOutput(createOutput<PJ301MPort>(
            Vec(xoff+2.5, yoff+30), module, Sisyphus::TRIG_OUTPUT+j
            ));

        float l = 14*15;
        float w = 7.5;
        float gap = (l-w)/(BITL-1);

        xoff = 45;
        
        for(int i = 0; i < BITL; ++i)
        {
            addChild(createLight<SmallLight<GreenLight>>(
                Vec(xoff, yoff+57.5+7.5/2), module, Sisyphus::LOOP_LIGHTS+j*BITL+i
                ));

            xoff+= gap;
           
        }


    }

    auto* label = new Label();
    label->box.pos=Vec(0, 30);
    label->text = "";
    addChild(label); 
    module->testLabel = label;

    module->ready = 1;

}
