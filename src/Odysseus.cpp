#include "TechTechTechnologies.hpp"
#define NOISE_LUT_LEN 8
#define NOISE_STD_DEV 4
#define CLAMP6011(val) clamp(val, -10.0f, 10.0f)
#define CLAMP2057(val) clamp(val, -10.0f, 10.0f)
#define INT_RATE 10*deltaTime
#define DETECT_THRESH .476
#define CLEAR_THRESH 2.5

struct Odysseus : Module {
	enum ParamIds {
        OFFSET_PARAM,
        OFFSET_CV_PARAM,
        NOISE_PARAM,
        NOISE_CV_PARAM,
        FREQ_PARAM,
        RATE_PARAM,
        DETECT_PARAM,
        FS_PARAM,   //Fast/slow switch
        SS_PARAM,   //slow/slower switch
        AC_PARAM,
        CLEAR_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
        OFFSET_INPUT,
        NOISE_INPUT,
        CLK_INPUT,
        CLEAR_INPUT,
        IN_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		OUT_OUTPUT,
        DETECT_OUTPUT,
		NUM_OUTPUTS
	};
	enum LightIds {
		NUM_LIGHTS
	};

    Label* testLabel;

    int ready = 0;

    float NOISE_LUT[NOISE_LUT_LEN][2] = {
        {-10, 1.5},
        {0, 1},
        {1, .935},
        {2, .858},
        {3, .742},
        {4, .542},
        {5, .0716},
        {5.05, 0},
    };

    float sh_val = 0;
    float integrator = 0;
    float shphase = 0;

    SchmittTrigger clearTrigger;
    SchmittTrigger clkTrigger;

	Odysseus() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
	void step() override;

	// For more advanced Module features, read Rack's engine.hpp header file
	// - toJson, fromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu

};

float curve_lut(float val, float lut[][2], int len)
{
    if(val <= lut[0][0]) return lut[0][1];
    if(val >= lut[len-1][0]) return lut[len-1][1];
    int i = 0;
    for (i = 0; val >= lut[i][0]; ++i);
    float* next = lut[i];
    float* prev = lut[i-1];

    float a = (val-prev[0])/(next[0]-prev[0]);
    return (1-a)*prev[1] + a*next[1];

}

float curve_pot(float a, float res, float rt=-1, float rb=-1, float rtx = 0, float rbx = 0)
{/*
    return divider ratio of a curved pot
    a is turn amount. left to right -> 0 to 1
    res is pot resistance
    rt is top parallel resistance. rt < 0 indicates none
    rb is bottom parallel resitance. rb < 0 indicates none
    rtx and rtb are series top and bottom resistors
    */

    float rta, rba;
    rba = a*res;
    rta = (1-a)*res;
    if(rt >= 0) rta = rt*rta/(rt+rta);
    if(rb >= 0) rba = rb*rba/(rb+rba);
    rta += rtx;
    rba += rbx;

    return rba/(rta+rba);
}

void Odysseus::step() {
	float deltaTime = 1.0 / engineGetSampleRate();

    if(ready == 0) return;

    //Offset level
    float off_cv = inputs[OFFSET_INPUT].value * 
        2*(curve_pot(params[OFFSET_CV_PARAM].value, 100, 9.31, 9.31)-.5);
    float off_knob = 
        10*2*(curve_pot(params[OFFSET_PARAM].value, 100, 6.98, 6.98, 3.09, 3.09)-.5);

    float off_val = CLAMP6011(off_cv + off_knob);

    //Noise level
    float noise_cv = inputs[NOISE_INPUT].value * 
        curve_pot(params[NOISE_CV_PARAM].value, 100, -1, 13.3);
    float noise_knob = 
        5*curve_pot(params[NOISE_PARAM].value, 100, -1, 10);

    float noise_level = 5.36-(noise_cv + noise_knob);
    float std_dev = NOISE_STD_DEV*curve_lut(noise_level, NOISE_LUT, NOISE_LUT_LEN);

    //Generate noise
   
    float noise_val = 0;
    if(std_dev > 0 )
    {
        noise_val = CLAMP6011(std_dev*randomNormal());
    }

    float sig_val = CLAMP6011(noise_val+off_val);

    //Sample and hold clock
    float freq = 1000;
    int ndiv = 1;

    if(params[FS_PARAM].value > 0.5) //fast mode = 64
    {
        ndiv = 64;
    }
    else
    {
        if(params[SS_PARAM].value > 0.5) //slow mode = 1024
        {
            ndiv = 1024;
        }
        else //slower mode = 16384
        {
            ndiv = 16384;
        }
    }

    float freq_knob = 1000*curve_pot(params[FREQ_PARAM].value, 100);
    freq_knob = freq_knob*3e3/(freq_knob+3e3);
    freq_knob += 49.9; 

    freq = 1e6 * 50 / (freq_knob*ndiv);

    shphase += freq*deltaTime;

    float clk_sig = 0;

    while(shphase >= 1)
    {
        shphase -= 1;
        clk_sig = 5;
    }


    if(inputs[CLK_INPUT].active)
    {
        clk_sig = inputs[CLK_INPUT].value;
    }
    else
    {
    }

    clkTrigger.process(clk_sig);

    //Sample and hold on signal

    if (clkTrigger.isHigh())
    {
        sh_val = sig_val;
    }

    //Rate attenuation
   
    float rate_knob = 
    (curve_pot(params[RATE_PARAM].value, 100, -1, 1.15));

    float int_in_val = sh_val*rate_knob;

    //Integrate

    integrator = CLAMP2057(integrator+int_in_val*INT_RATE);

    //Clear/autoclear etc

    float clear_sig= 0;

    if (params[AC_PARAM].value >0.5)
    {
        clear_sig += outputs[DETECT_OUTPUT].value;
    }

    clear_sig += 10*params[CLEAR_PARAM].value;
    clear_sig += inputs[CLEAR_INPUT].value;

    clearTrigger.process(clear_sig/CLEAR_THRESH);

    if(clearTrigger.isHigh())
    {
        integrator = 0;
    }

    float out = integrator;

    //Detect

    float detect_knob =
    (curve_pot(params[DETECT_PARAM].value, 100, -1, 6.98));
    
    if (abs(detect_knob*out) >= DETECT_THRESH)
    {
        outputs[DETECT_OUTPUT].value = 5;
    }
    else
    {
        outputs[DETECT_OUTPUT].value = 0;
    }

    outputs[OUT_OUTPUT].value = out;

    /*
    char tstr[256];
    sprintf(tstr, "freq: %f", freq);
    if(testLabel)
        testLabel->text = tstr;
    */
}

struct OdysseusWidget : ModuleWidget
{
    OdysseusWidget(Odysseus* module);
};


OdysseusWidget::OdysseusWidget(Odysseus* module) : ModuleWidget(module) {
	box.size = Vec(10 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(assetPlugin(plugin, "res/Odysseus.svg")));
		addChild(panel);
	}

	addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

    OUTPORT(12.883+12.5, 380-(33.337+12.5), Odysseus, DETECT_OUTPUT)
    OUTPORT(108.966+15, 380-(33.337+12.5), Odysseus, OUT_OUTPUT)
	
    INPORT(6.721+12.5, 380-(318.706+12.5), Odysseus, OFFSET_INPUT)
    INPORT(117.618+12.5, 380-(318.706+12.5), Odysseus, NOISE_INPUT)
    INPORT(62.171+12.5, 380-(207.812+12.5), Odysseus, CLK_INPUT)
    INPORT(62.171+12.5, 380-(133.833+12.5), Odysseus, CLEAR_INPUT)
    INPORT(62.171+12.5, 380-(33.337+12.5), Odysseus, IN_INPUT)

    KNOB(44.186+12,380-(319.206+12), 0, 1, .5, Tiny, Odysseus, OFFSET_CV_PARAM)
    KNOB(81.152+12,380-(319.206+12), 0, 1, 0, Tiny, Odysseus, NOISE_CV_PARAM)

    KNOB(6.706+31,380-(241.064+31), 0, 1, .5, Huge, Odysseus, OFFSET_PARAM)
    KNOB(80.635+31,380-(241.064+31), 0, 1, 0, Huge, Odysseus, NOISE_PARAM)

    KNOB(16.701+21.005,380-(154.949+21.005), 0, 1, 0, Large, Odysseus, FREQ_PARAM)
    KNOB(90.630+21.005,380-(154.949+21.005), 0, 1, 0, Large, Odysseus, RATE_PARAM)
    KNOB(53.666+21.005,380-(66.233+21.005), 0, 1, 0, Large, Odysseus, DETECT_PARAM)

    SWITCH(20.353+7, 380-(209.991+20.641/2), Odysseus, FS_PARAM, 0, 1, 0)
    SWITCH(41.055+7, 380-(209.991+20.641/2), Odysseus, SS_PARAM, 0, 1, 0)

    BUTTON(36.149+16.418/2, 380-(120.433+16.414/2), DWhiteLatch, Odysseus, AC_PARAM, 0, 1, 0)
    BUTTON(95.296+16.414/2, 380-(120.433+16.414/2), DWhite, Odysseus, CLEAR_PARAM, 0, 1, 0)

    Label* label = new Label();
    label->box.pos=Vec(30, 0);
    label->text = "";
    label->color = nvgRGB(0,0,0);
    addChild(label); 
    module->testLabel = label;

    module -> ready = 1;

}

Model* modelOdysseus = Model::create<Odysseus, OdysseusWidget>(
        "TechTech Technologies", "Odysseus", "Odysseus", 
        UTILITY_TAG, FUNCTION_GENERATOR_TAG, NOISE_TAG,
        SLEW_LIMITER_TAG, LFO_TAG 
        );


