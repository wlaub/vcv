#include "TechTechTechnologies.hpp"
#define BITL 16
#define NSEQ 2

struct Vulcan : Module {
	enum ParamIds {
        DEPTH_PARAM,
        ROUTE_PARAM,
        CLOCK_PARAM,
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

    dsp::SchmittTrigger clockTrigger[NSEQ];
    dsp::SchmittTrigger modeTrigger[NSEQ];

    dsp::PulseGenerator trigPulse[NSEQ];

    Label* testLabel;

    int pos[NSEQ] = {0};

    float phase[2] = {0};
    int dir[2] = {1};

	Vulcan()
    {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        DEPTH_CONFIGURE

        configParam(ROUTE_PARAM, 0,1,1, "Route");


        for(int j = 0; j < NSEQ; ++j)
        {
            configParam(CLOCK_PARAM+j, 0, 10, 5, "Clock");
            configParam(POS_PARAM+j, 0, 10, 0, "Box Position");
            configParam(WIDTH_PARAM+j, 0, 10, 10, "Box Width");

            configParam(RATE_PARAM+j, 0,16,1, "Step Size?");
            configParam(MODE_PARAM+j, 0,1,1, "Loop Mode?");

        }

       
    }

    void process(const ProcessArgs& args) override {

        float deltaTime = args.sampleTime;

        if(ready == 0) return;

        DEPTH_STEP

        int clockRoute = params[ROUTE_PARAM].value;


        for(int j = 0; j < NSEQ; ++j)
        { 

            //Run trigger output one-shot
            float trigOut = 0;
            if(trigPulse[j].process(deltaTime))
            {
                trigOut = 10;
            }

            //Check mode
            int mode = 0;
            if(inputs[MODE_INPUT+j].active)
            {
                modeTrigger[j].process(inputs[MODE_INPUT+j].value);
                if(modeTrigger[j].isHigh())
                {
                    mode = 1;
                }
            }
            else
            {
                mode = params[MODE_PARAM+j].value;
            }

            int tickp = 0;

            //Run internal clock
    /*
            float freq = powf(2,params[CLOCK_PARAM+j].value); //1 to 1024, default 32
            phase[j] += freq*deltaTime;
            if(phase[j] >= 32) //Default 1Hz at 32
            {
                phase[j] -= 32;
                //Actually tick from internal clock only if no external clock
                if(inputs[CLOCK_INPUT+j].active || (clockRoute && j==1))
                {
                }
                else
                {
                    tickp = 1;
                }
            }        
    */

            //Replace external clock w/ routed clock if necessary
            float clockVal;
            if (clockRoute && j == 1)
            {
                clockVal = outputs[TRIG_OUTPUT].value;
            }
            else
            {
                clockVal = inputs[CLOCK_INPUT+j].value;
            }
            
            //Do tick if there's a tick
            if(tickp == 1 || clockTrigger[j].process(clockVal))
            {
                int rate;
                float spos;
                float width;
            
                //determin positions
                PARAM_SEL(spos, POS, j)
                PARAM_SEL(width, WIDTH, j)

                int start = cv_to_num(spos, depth);
                int w = cv_to_num(width, depth);

                //determine rate
                if(inputs[RATE_INPUT].active)
                {
                    rate = cv_to_num(inputs[RATE_INPUT].value, depth);
                }
                else
                {
                    rate = round(params[RATE_PARAM].value);
                } 

                if(mode == 1)//Loop mode always goes forward
                {
                    dir[j] = 1;
                }

                pos[j] += rate*dir[j];

                //handle pos looping
                //valid positions are 0 ~ w-1
                if(pos[j] >= start+w || pos[j] < start) 
                {
                    if(w == 0) //If w = 0, nothing should be changing
                    {
                        pos[j] = start;
                    }
                    else
                    {
                        pos[j] -= start;
                        if(mode == 0) //bounce mode
                        {
                            int extra;
                            if(dir[j] == 1) //bounce back from end
                            {               //if pos = w, new pos should be (w-1)-1
                                extra = pos[j] - w + 2;
                            }
                            else //bounce forward from start
                            {    //if pos = -1, npos should be 2
                               extra = pos[j] -1;
                            }
                            pos[j] -= extra;
                            dir[j] *= -1;
                        }
                        else // loop mode
                        {
                            pos[j] = (pos[j]%w);
                        }
                        pos[j] +=start;
                    }
                    trigPulse[j].trigger(.001); 
                    trigOut = 10;
                }
                outputs[TRIG_OUTPUT+j].value = trigOut;

                outputs[POS_OUTPUT+j].value = num_to_cv(pos[j], depth);
            }
        }

        outputs[AND_OUTPUT].value = num_to_cv(pos[0]&pos[1], depth);
        outputs[XOR_OUTPUT].value = num_to_cv(pos[0]^pos[1], depth);
        outputs[OR_OUTPUT].value = num_to_cv(pos[0]|pos[1], depth);

    }



	// For more advanced Module features, read Rack's engine.hpp header file
	// - dataToJson, dataFromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};

struct VulcanWidget : ModuleWidget
{
    VulcanWidget(Vulcan* module);
};



VulcanWidget::VulcanWidget(Vulcan* module) {
		setModule(module);
	box.size = Vec(18* RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SvgPanel *panel = new SvgPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(asset::plugin(pluginInstance, "res/Vulcan.svg")));
		addChild(panel);
	}

	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));


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
        Vec(128, 380-162.18-20.641), module, Vulcan::ROUTE_PARAM
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
        addParam(createParam<RoundBlackSnapKnob>(
            Vec(xoff+30.15, yoff+.15+gap*3), module, Vulcan::RATE_PARAM+j
            ));


        addInput(createInput<PJ301MPort>(
            Vec(xoff+2.5, yoff+2.5+gap*3+35), module, Vulcan::MODE_INPUT+j
            ));
        addParam(createParam<CKSS>(
            Vec(xoff+38, yoff+4.68+gap*3+35), module, Vulcan::MODE_PARAM+j
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
    if(module)
    {
        module->testLabel = label;
        module->ready = 1;
    }
}

Model* modelVulcan = createModel<Vulcan, VulcanWidget>(
        "Vulcan"
        );


