#include "Tutorial.hpp"
#define BITL 16

struct Prometheus : Module {
	enum ParamIds {
        DEPTH_PARAM,
        MODE_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		DEPTH_INPUT,
        TAPS_INPUT, 
        GATE_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
        DIGI_OUTPUT,
        ANLG_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
        BIT_LIGHT,
		NUM_LIGHTS=BIT_LIGHT+BITL
	};


    SchmittTrigger gateTrigger;

    Label* testLabel;

    int buffer = 0;

	Prometheus() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;

	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};


unsigned char reverse(unsigned char input)
{
    static unsigned char lookup[16] = {0,8,4,12,2,10,6,14,1,9,5,13,2,11,7,15};
    return (lookup[input&0xf]<<4)|lookup[input >>4];

}

void Prometheus::step() {
	// Implement a simple sine oscillator
	//float deltaTime = 1.0 / engineGetSampleRate();

	// Compute the frequency from the pitch parameter and input
    int depth;
    if(inputs[DEPTH_INPUT].active)
        depth = cv_to_depth(inputs[DEPTH_INPUT].value);
    else
        depth = params[DEPTH_PARAM].value;

    if(gateTrigger.process(inputs[GATE_INPUT].value))
    {
        int taps = cv_to_num(inputs[TAPS_INPUT].value, depth);

        int mask = 0;
        for(int i = 0; i < depth; ++i)
        {
            mask<<=1;
            mask|=1;
        }

        buffer &= mask;
        taps &= mask;

        int inv = params[MODE_PARAM].value;

        int lsb = buffer &1;
        lsb ^= inv;
        buffer >>=1;
        if(lsb)
        {
            buffer ^= taps;
        }
    

        int temp = buffer;
        for(int i = 0; i < BITL; ++i)
        {
            lights[BIT_LIGHT+i].value = (i < depth) ? (temp&1) : 0;
            temp >>=1;
        }

        outputs[DIGI_OUTPUT].value = buffer;
        outputs[ANLG_OUTPUT].value = buffer&1;  
    }
}


PrometheusWidget::PrometheusWidget() {
	Prometheus *module = new Prometheus();
	setModule(module);
	box.size = Vec(6 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/Prometheus.svg")));
		addChild(panel);
	}

	addChild(createScrew<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createScrew<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createScrew<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));


    addParam(createParam<RoundSmallBlackSnapKnob>(
        Vec(46, 51), module, Prometheus::DEPTH_PARAM,
        1, 16, 12
        ));

    addInput(createInput<PJ301MPort>(
        Vec(17.5, 52.5), module, Prometheus::DEPTH_INPUT
        ));

    addInput(createInput<PJ301MPort>(
        Vec(17.5, 112.5), module, Prometheus::TAPS_INPUT
        ));

    addInput(createInput<PJ301MPort>(
        Vec(17.5, 142.5), module, Prometheus::GATE_INPUT
        ));

    addParam(createParam<CKSS>(
        Vec(22.5, 175), module, Prometheus::MODE_PARAM,
        0,1,1
        ));


    for(int i = 0; i < 8; ++i)
    {

        auto* llight = createLight<MediumLight<BlueLight>>(
            Vec(30, 207.5+15*i), module, Prometheus::BIT_LIGHT+i
            );
        center(llight);
        addChild(llight);

        auto* rlight = createLight<MediumLight<BlueLight>>(
            Vec(60, 207.5+15*i), module, Prometheus::BIT_LIGHT+i+8
            );
        center(rlight);
        addChild(rlight);
    }
 
    addOutput(createOutput<PJ301MPort>(
        Vec(17.5, 322.5), module, Prometheus::DIGI_OUTPUT
        ));

    addOutput(createOutput<PJ301MPort>(
        Vec(47.5, 322.5), module, Prometheus::ANLG_OUTPUT
        ));


    auto* label = new Label();
    label->box.pos=Vec(0, 30);
    label->text = "";
    addChild(label); 
    module->testLabel = label;



}
