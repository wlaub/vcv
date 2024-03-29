#include "TechTechTechnologies.hpp"
#define BITL 20
#define N 4
#define BUFL (10*48000)

typedef struct
{
    float data[BUFL];
    int head;
    int tail;
    float loc;
} cbuf;

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
		NUM_LIGHTS = LOOP_LIGHTS + N*BITL*3
	};

    int ready = 0;

    dsp::SchmittTrigger clockTrigger[N];
    dsp::SchmittTrigger gateTrigger[N];

    dsp::PulseGenerator trigPulse[N];

    Label* testLabel;

    cbuf buffer[N];

	Sisyphus() 
    {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        for(int j = 0; j < N; ++j)
        {
            configParam(LENGTH_PARAM+j, 0,10,10, "Buffer length");
            configParam(RATE_PARAM+j, 0, 10, 5, "Playback Rate");
            configParam(MODE_PARAM+j, 0,1,0, "Playback Mode");
        }

       
    }
    void process(const ProcessArgs& args) override {

        float deltaTime = args.sampleTime;

        if(ready == 0) return;

        for(int j = 0; j < N; ++j)
        { 
            gateTrigger[j].process(inputs[GATE_INPUT+j].value);


            //set rate base on rate inputs

            float rate;

            if(inputs[RATE_INPUT+j].active)
            {
                rate = inputs[RATE_INPUT+j].value;
            } 
            else
            {
                rate = params[RATE_PARAM+j].value; //0-2
            }

            rate = rate/5;

            float lenval;
            if(inputs[LENGTH_INPUT+j].active)
            {
                lenval = inputs[LENGTH_INPUT+j].value;
            } 
            else
            {
                lenval = params[LENGTH_PARAM+j].value; //0-2
            }

            //set buffer tail based on length inputs
            int length = (BUFL-1)*lenval/10;
            if(length > buffer[j].head)
            {
                buffer[j].tail = BUFL-1-(length-buffer[j].head);
            }
            else
            {
                buffer[j].tail = buffer[j].head-length;
            }

            if(gateTrigger[j].isHigh())
            {
                outputs[SIGNAL_OUTPUT+j].value = buffer[j].data[int(round(buffer[j].loc))];

                float prev = buffer[j].loc;           
                buffer[j].loc +=rate; //Change to rate

                if(buffer[j].loc > buffer[j].head && prev <= buffer[j].head)
                { //Loc crosses head
                    buffer[j].loc = buffer[j].tail+(buffer[j].loc-buffer[j].head);
                    trigPulse[j].trigger(.001);
                }
                if(buffer[j].loc >=BUFL)
                { //Loc is past buffer end
                    buffer[j].loc -=BUFL;
                }
            }
            else
            {
                buffer[j].data[buffer[j].head] = inputs[SIGNAL_INPUT+j].value;
                outputs[SIGNAL_OUTPUT+j].value = buffer[j].data[buffer[j].head];

                buffer[j].head +=1;
                if(buffer[j].head >=BUFL)
                    buffer[j].head = 0;
                buffer[j].loc = buffer[j].head;
            }



            if(trigPulse[j].process(deltaTime))
            {
                outputs[TRIG_OUTPUT+j].value = 10;
            }
            else
            {
                outputs[TRIG_OUTPUT+j].value = 0;
            }


            int lightidx;

            for(int i = 0; i < BITL; ++i)
            {
                for(int k = 0; k < 3; ++ k)
                {
                    lights[LOOP_LIGHTS+3*(j*BITL+i)+k].value = 0;
                }
            }
            lightidx = 3*floor(j*BITL+buffer[j].head*((BITL-1.0)/BUFL));
            lights[LOOP_LIGHTS+lightidx].value = 1;

            lightidx = 3*floor(j*BITL+buffer[j].tail*((BITL-1.0)/BUFL));
            lights[LOOP_LIGHTS+lightidx+1].value = 1;

            lightidx = 3*floor(j*BITL+buffer[j].loc*((BITL-1.0)/BUFL));
            lights[LOOP_LIGHTS+lightidx+2].value = 1;
     
        }

    }

	// For more advanced Module features, read Rack's engine.hpp header file
	// - dataToJson, dataFromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};



struct SisyphusWidget : ModuleWidget
{
    SisyphusWidget(Sisyphus* module);
};


SisyphusWidget::SisyphusWidget(Sisyphus* module) {
		setModule(module);
//	Sisyphus *module = new Sisyphus();
//	setModule(module);
	box.size = Vec(20* RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SvgPanel *panel = new SvgPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(asset::plugin(pluginInstance, "res/Sisyphus.svg")));
		addChild(panel);
	}

	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));


    float xoff, yoff;


    for(int j = 0; j < N; ++j)
    {
        xoff = 47.5;
        yoff = 380-287.5-25+j*75;
        
        addInput(createInput<PJ301MPort>(
            Vec(xoff, yoff),  module, Sisyphus::SIGNAL_INPUT+j
            ));
        addInput(createInput<PJ301MPort>(
            Vec(xoff, yoff+30),  module, Sisyphus::GATE_INPUT+j
            ));


        xoff = 90.2;
        addParam(createParam<RoundBlackKnob>(
            Vec(xoff, yoff-2.3), module, Sisyphus::LENGTH_PARAM+j
            ));
        addInput(createInput<PJ301MPort>(
            Vec(xoff+2.5, yoff+30),  module, Sisyphus::LENGTH_INPUT+j
            ));

        xoff += 45;
        addParam(createParam<RoundBlackKnob>(
            Vec(xoff, yoff-2.3), module, Sisyphus::RATE_PARAM+j
            ));
        addInput(createInput<PJ301MPort>(
            Vec(xoff+2.5, yoff+30), module, Sisyphus::RATE_INPUT+j
            ));

        xoff += 45;
        addParam(createParam<RoundBlackSnapKnob>(
            Vec(xoff, yoff-2.3), module, Sisyphus::MODE_PARAM+j
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
            addChild(createLight<SmallLight<RedGreenBlueLight>>(
                Vec(xoff, yoff+57.5+7.5/2), module, Sisyphus::LOOP_LIGHTS+3*(j*BITL+i)
                ));

            xoff+= gap;
           
        }


    }

    auto* label = new Label();
    label->box.pos=Vec(0, 30);
    label->text = "";
    addChild(label); 
    if(module)
    {
    module->testLabel = label;

    module->ready = 1;
    }
}

Model* modelSisyphus = createModel<Sisyphus, SisyphusWidget>(
        "Sisyphus"
        );


