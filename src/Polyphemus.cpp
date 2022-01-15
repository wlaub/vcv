#include "TechTechTechnologies.hpp"
#define N 3
#define NSIG 2
#define NFILT NSIG+2
#define STEPFREQ 10
#define TAPS 0x800017
#define MIN(A,B) ((A<B)? A : B)
#define MAX(A,B) ((A>B)? A : B)
#define CLIP(A, B, C) MIN(MAX(A,B),C)

typedef struct
{
    float data[NFILT][2] = {{0}};
    float r; //The filter radius
    float p; //The filter angle
    float a; //iir coefficient
    float b; //iir coefficient
    int head[NFILT] = {0};
} biquad;



struct Polyphemus : Module {
	enum ParamIds {
        NORM_PARAM,
        NORMCV_PARAM,
        STAB_PARAM,
        STABCV_PARAM,
        GAIN_PARAM,
        RADIUS_PARAM = GAIN_PARAM+N+1,
        ANGLE_PARAM = RADIUS_PARAM+N+1,
        RADIUSCV_PARAM = ANGLE_PARAM+N+1,
        ANGLECV_PARAM = RADIUSCV_PARAM+N+1,
		NUM_PARAMS = ANGLECV_PARAM+N+1
	};
	enum InputIds {
        NORM_INPUT,
        STAB_INPUT,
		SIGNAL_INPUT,
        RADIUS_INPUT = SIGNAL_INPUT+NSIG,
        ANGLE_INPUT = RADIUS_INPUT+N+1,
		NUM_INPUTS = ANGLE_INPUT+N+1
	};
	enum OutputIds {
        X_OUTPUT,
        Y_OUTPUT,
        SIGNAL_OUTPUT,
		NUM_OUTPUTS=SIGNAL_OUTPUT+NFILT
	};
	enum LightIds {
		NUM_LIGHTS
	};

    int ready = 0;

    float plot_idx = 0;
    Label* testLabel;

    biquad filters[N];

    float stepphase = 0;

	Polyphemus() 
    {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    
        configParam(GAIN_PARAM, 0,2,1, "Gain");

        for(int j = 0; j < N; ++j)
        {
            configParam(RADIUSCV_PARAM+j, -1,1,0, "Radius CV Gain");
            configParam(RADIUS_PARAM+j, -1,1,0, "Radius");

            configParam(ANGLECV_PARAM+j, -1,1,0, "Angle CV Gain");
            configParam(ANGLE_PARAM+j, 0,3.14,0, "Angle");
        }
        CV_ATV_CONFIGURE(-1,1,0, RADIUS, N)

        CV_ATV_CONFIGURE(0,3.14,0, ANGLE, N)

        CV_ATV_CONFIGURE(0,1,0,NORM, 0)

        CV_ATV_CONFIGURE(-1,1,0,STAB, 0)

       
    }

    void process(const ProcessArgs& args) override {
      float deltaTime = args.sampleTime;

        //TODO: Logarithmic controls
        //TODO: clipping control?
        //TODO: normalization gain control for r=1?

        if(ready == 0) return;

        float x, y;
        float r, a;
        float gain;

        double norm = CV_ATV_VALUE(NORM, 1, 0);
        float stab = CV_ATV_VALUE(STAB, 1, 0);
        float rglob = CV_ATV_VALUE(RADIUS, 1, N);
        float aglob = CV_ATV_VALUE(ANGLE, 3.14, N);

        norm = CLIP(0, norm, 1);

        norm = 1 - split_log(norm, 20, 80);

        float maxrad = 1 + stab*.001;

        gain = params[GAIN_PARAM].value;

        float g = 1;

        for(int j = 0; j < N; ++j)
        { 

            //retrieve pole params from inputs
            //radius is -1 ~ 1, angle is 0 ~ 3.14
            //inputs are 0 ~ 10 w/ attenuverters

            r = CV_ATV_VALUE(RADIUS, 1, j);
            a = CV_ATV_VALUE(ANGLE, 3.14, j);

            r += rglob;
            a += aglob;

            //clip to +/- 1
            r = CLIP(-maxrad, r, maxrad);
            a = CLIP(0, a, 6.28);

            if(j == floor(plot_idx))
            {
                outputs[X_OUTPUT].value = 6.67*r*cos(a);
                outputs[Y_OUTPUT].value = 10*r*sin(a);
            }

            filters[j].r = r;
            filters[j].p = a;
            //Set filter params from inputs

            float c = cos(a);

            filters[j].a = -2*r*c;
            filters[j].b = r*r;
        }
        plot_idx+=.001;
        if(plot_idx >= N) plot_idx -= N;

        //Compute the total inverse gain at each filter frequency 
        //and store the smallest value in g
        g = -1;
        for(int i = 0; i < N; ++i)
        {
            float p = filters[i].p;  //the angle of concern
            
            float tg = 1;
            for(int j = 0; j < N; ++j)
            {
                a = filters[j].a;
                float b = filters[j].b;
                float c = cos(p);
                float k = (a+2*b*c);
                tg *= (1-b)*(1-b)+2*(1-b)*c*k+k*k;

            }
            if(tg < g || g<0)
            {
                g = tg;
            }
        }
    /*    if(g == 0)
        {
            g = 10E-6;
        }*/

        g = sqrt(g);

        g = (1-norm)+norm*g;

        float fsig[NFILT];

        for(int i = 0; i < NSIG; ++i)
        {
            fsig[i] = inputs[SIGNAL_INPUT+i].value;
        }

        //fsig[NFILT-2] is slow square wave
        stepphase += deltaTime*STEPFREQ;
        if(stepphase >= 1) stepphase -= 1;
        fsig[NFILT-2] = (stepphase > 0.5) ?5:0; 


        //fsig[NFILT-1] is white noise
        fsig[NFILT-1] = 10*(rand()/float(RAND_MAX)-.5);


        //apply filter to value
        for(int i = 0; i < NFILT; ++i)
        {
            x = fsig[i]*g*gain;
            for(int j = 0; j <N; ++j)
            {
                y = x;
                y -= filters[j].a*filters[j].data[i][filters[j].head[i]];
                filters[j].head[i] ^= 1;
                y -= filters[j].b*filters[j].data[i][filters[j].head[i]];

                filters[j].data[i][filters[j].head[i]] = y;
                x = y;

            }
            float clip = 100;

            if(x > clip)
            {
                x = clip;
            }
            else if(x<-clip)
            {
                x = -clip;
            }

            outputs[SIGNAL_OUTPUT+i].value = x;
     
        }

    /*
        r = filters[0].r;
        a = filters[0].p;
                char tstr[256];
                sprintf(tstr, "%f, %f, %f", r, a, g);
    //            sprintf(tstr, "%f, %e", norm, g);
                if(testLabel)
                    testLabel->text = tstr;
    */

    }



	// For more advanced Module features, read Rack's engine.hpp header file
	// - dataToJson, dataFromJson: serialization of internal data
	// - onSampleRateChange: event triggered by a change of sample rate
	// - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};

/* IIR coefficients
A = 2*r*cos(theta)
B = r^2
*/

struct PolyphemusWidget : ModuleWidget
{
    PolyphemusWidget(Polyphemus* module);
};


PolyphemusWidget::PolyphemusWidget(Polyphemus* module) {
		setModule(module);
//	Polyphemus *module = new Polyphemus();
//	setModule(module);
	box.size = Vec(18* RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

	{
		SVGPanel *panel = new SVGPanel();
		panel->box.size = box.size;
		panel->setBackground(SVG::load(asset::plugin(pluginInstance, "res/Polyphemus.svg")));
		addChild(panel);
	}

	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
	addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
	addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));


    float xoff, yoff;

    xoff = 17.5;
    yoff = 380-302.5-20;

    float vspace = 14;

    addInput(createInput<PJ301MPort>(
        Vec(xoff, yoff-vspace), module, Polyphemus::SIGNAL_INPUT
        ));

    addInput(createInput<PJ301MPort>(
        Vec(xoff, yoff+vspace), module, Polyphemus::SIGNAL_INPUT+1
        ));


    addParam(createParam<RoundLargeBlackKnob>(
        Vec(xoff+28.15, yoff-6.35), module, Polyphemus::GAIN_PARAM
        ));



    xoff = 89;

    for(int i = 0; i < NFILT; i+=2)
    {
        addOutput(createOutput<PJ301MPort>(
            Vec(xoff, yoff-vspace), module, Polyphemus::SIGNAL_OUTPUT+i
            ));

        addOutput(createOutput<PJ301MPort>(
            Vec(xoff, yoff+vspace), module, Polyphemus::SIGNAL_OUTPUT+i+1
            ));
        xoff += 35;
    }

    xoff += 30;
    addOutput(createOutput<PJ301MPort>(
        Vec(xoff, yoff-vspace), module, Polyphemus::X_OUTPUT
        ));

    addOutput(createOutput<PJ301MPort>(
        Vec(xoff, yoff+vspace), module, Polyphemus::Y_OUTPUT
        ));




    for(int j = 0; j < N; ++j)
    {
        xoff = 17.5;
        yoff = 380-232-25+j*85;

        addInput(createInput<PJ301MPort>(
            Vec(xoff, yoff), module, Polyphemus::RADIUS_INPUT+j
            ));

        addParam(createParam<RoundTinyBlackKnob>(
            Vec(xoff+35, yoff+3.5), module, Polyphemus::RADIUSCV_PARAM+j
            ));
 
        addParam(createParam<RoundLargeBlackKnob>(
            Vec(xoff+62.65, yoff-13.85), module, Polyphemus::RADIUS_PARAM+j
            ));


        addInput(createInput<PJ301MPort>(
            Vec(xoff, yoff+28), module, Polyphemus::ANGLE_INPUT+j
            ));

        addParam(createParam<RoundTinyBlackKnob>(
            Vec(xoff+35, yoff+3.5+28), module, Polyphemus::ANGLECV_PARAM+j
            ));
 
        addParam(createParam<RoundLargeBlackKnob>(
            Vec(xoff+62.65, yoff-13.85+43), module, Polyphemus::ANGLE_PARAM+j
            ));



    }

    xoff = 152.5;
    yoff = 380-302.5-25;

    yoff += 53;

    CV_ATV_PARAM(xoff, yoff, Polyphemus::RADIUS, -1,1,0,N)

    yoff += 53;

    CV_ATV_PARAM(xoff, yoff, Polyphemus::ANGLE, 0,3.14,0,N)

    yoff += 53;

    CV_ATV_PARAM(xoff, yoff, Polyphemus::NORM, 0,1,0,0)

    yoff += 53;

    CV_ATV_PARAM(xoff, yoff, Polyphemus::STAB, -1,1,0,0)

    yoff += 53;


/*
    addInput(createInput<PJ301MPort>(
        Vec(xoff, yoff), module, Polyphemus::RADIUS_INPUT+j
        ));

    addParam(createParam<RoundTinyBlackKnob>(
        Vec(xoff+34, yoff+9), module, Polyphemus::RADIUSCV_PARAM+j,
        -1,1,0
        ));

    addParam(createParam<RoundBlackKnob>(
        Vec(xoff+62.5, yoff+6.5), module, Polyphemus::RADIUS_PARAM+j,
        -1,1,0
        ));
*/


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

Model* modelPolyphemus = createModel<Polyphemus, PolyphemusWidget>(
    "Polyphemus" 
    );

