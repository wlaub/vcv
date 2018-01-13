#include "TechTechTechnologies.hpp"
#define BITL 16
#define NLFSR 3

struct Prometheus : Module {
	enum ParamIds {
        DEPTH_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		DEPTH_INPUT,
        TAPS_INPUT, 
        GATE_INPUT=TAPS_INPUT+NLFSR,
        INT_INPUT=GATE_INPUT+NLFSR,
		NUM_INPUTS=INT_INPUT+NLFSR
	};
	enum OutputIds {
        DIGI_OUTPUT,
        ANLG_OUTPUT=DIGI_OUTPUT+NLFSR,
		NUM_OUTPUTS=ANLG_OUTPUT+NLFSR
	};
	enum LightIds {
        BIT_LIGHT,
		NUM_LIGHTS=BIT_LIGHT+BITL*NLFSR
	};


    SchmittTrigger gateTrigger[NLFSR];
    

    Label* testLabel;

    unsigned short buffer[NLFSR];

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
	//float deltaTime = 1.0 / engineGetSampleRate();

    int depth;
    if(inputs[DEPTH_INPUT].active)
        depth = cv_to_depth(inputs[DEPTH_INPUT].value);
    else
        depth = params[DEPTH_PARAM].value;

    for(int j = 0; j < NLFSR; ++j)
    { 
        if(gateTrigger[j].process(inputs[GATE_INPUT+j].value))
        {
            int taps = 0;

            if(inputs[TAPS_INPUT+j].active)
                taps = cv_to_num(inputs[TAPS_INPUT+j].value, depth);

            if(inputs[INT_INPUT+j].active)
                taps ^= cv_to_num(inputs[INT_INPUT+j].value, depth);

            int mask = 0;
            for(int i = 0; i < depth; ++i)
            {
                mask<<=1;
                mask|=1;
            }

            taps &= mask;

            int lsb = buffer[j]&1;
            lsb ^= 1;
            buffer[j] >>=1;
            if(lsb)
            {
                buffer[j] ^= taps;
            }
        
            buffer[j] &= mask;

            int temp = buffer[j];
            for(int i = 0; i < BITL; ++i)
            {
                lights[BIT_LIGHT+i+j*BITL].value = temp&1;
                temp >>=1;
            }

            outputs[DIGI_OUTPUT+j].value = num_to_cv(buffer[j], depth);
            outputs[ANLG_OUTPUT+j].value = 10*(buffer[j]&1)-5; 

        }
    }

}


PrometheusWidget::PrometheusWidget() {
	Prometheus *module = new Prometheus();
	setModule(module);
	box.size = Vec(6 *NLFSR* RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

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
        Vec(46, 53.5), module, Prometheus::DEPTH_PARAM,
        1, 16, 12
        ));

    addInput(createInput<PJ301MPort>(
        Vec(17.5, 55), module, Prometheus::DEPTH_INPUT
        ));


    for(int j = 0; j < NLFSR; ++j)
    {
        float xoff = 90*j;

        addInput(createInput<PJ301MPort>(
            Vec(17.5+xoff, 112.5), module, Prometheus::TAPS_INPUT+j
            ));

        addInput(createInput<PJ301MPort>(
            Vec(17.5+xoff, 142.5), module, Prometheus::INT_INPUT+j
            ));

        addInput(createInput<PJ301MPort>(
            Vec(17.5+xoff, 172.5), module, Prometheus::GATE_INPUT+j
            ));

        for(int i = 0; i < 8; ++i)
        {

            auto* llight = createLight<MediumLight<BlueLight>>(
                Vec(30+xoff, 207.5+15*i), module, Prometheus::BIT_LIGHT+i+j*BITL
                );
            center(llight);
            addChild(llight);

            auto* rlight = createLight<MediumLight<BlueLight>>(
                Vec(60+xoff, 207.5+15*i), module, Prometheus::BIT_LIGHT+i+8+j*BITL
                );
            center(rlight);
            addChild(rlight);
        }
     
        addOutput(createOutput<PJ301MPort>(
            Vec(17.5+xoff, 322.5), module, Prometheus::DIGI_OUTPUT+j
            ));

        addOutput(createOutput<PJ301MPort>(
            Vec(47.5+xoff, 322.5), module, Prometheus::ANLG_OUTPUT+j
            ));
    }

    auto* label = new Label();
    label->box.pos=Vec(0, 30);
    label->text = "";
    addChild(label); 
    module->testLabel = label;



}
