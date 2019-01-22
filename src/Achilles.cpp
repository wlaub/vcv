
#include "TechTechTechnologies.hpp"
#define BUFL (1<<12)

struct Achilles : Module {
    /* +ENUMS */
    #include "Achilles_enums.hpp"
    /* -ENUMS */
    
    /* +TRIGGER_VARS */
    #include "Achilles_vars.hpp"
    /* -TRIGGER_VARS */

    SchmittTrigger gate_trigger;

    int ready = 0;

    ttt::CircularBuffer* delay;
    float delay_tap = 10;

    float env_timer = -1;
    float env_length = 10;

    float sh_phase[2] = {0};
    float sh_val[2] = {0};

    Achilles() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
    void step() override;

    // For more advanced Module features, read Rack's engine.hpp header file
    // - toJson, fromJson: serialization of internal data
    // - onSampleRateChange: event triggered by a change of sample rate
    // - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};


void Achilles::step() {
    float deltaTime = engineGetSampleTime();

    if(ready == 0) return;
    /*  +INPUT_PROCESSING */
    #include "Achilles_inputs.hpp"
    /*  -INPUT_PROCESSING */

    /* GATE PROCESSING */

    auto gate = gate_trigger.process(input_gate);

    if(gate)
    {
        env_timer = 0;
    }

    {//This should happen on a gate
        float voct;
        voct = input_voct + input_fm;
//        voct *= 1-input_match/100;

        float period = 1/(deltaTime*261.626f * powf(2.0f, voct));
        delay_tap = period;

        env_length = period*(1 + input_match/10);
    }



    /* ENVELOPE HANDLING */

    float env = 0; //TODO
    output_env = 0;
    if(! inputs[INPUT_EXT_ENV].active)
    {
        if(env_timer >= 0)
        {
            float skew = .5*(1+input_env[0]/10);
            float sym = input_env[3]/10;
            float beta = sym > 0 ? sym : -sym; // idk why abs floors...

            float shape = (1-beta)*input_env[1]/10;
            float alpha;

            if(env_timer < skew)
            {
                alpha = env_timer/skew;
                if(sym < 0)
                    shape -= beta;
                else
                    shape += beta;
            }
            else
            {
                alpha = 1-(env_timer-skew)/(1-skew);
                if(sym < 0)
                    shape += beta;
                else
                    shape -= beta;
            }

            output_noise[0] = beta;

            if(shape < -1)
            {
                shape = -1;
            }
            if(shape > 0)
            {
                shape*=(1+3*input_env[2]/5);
            }

            env=pow(alpha, 1+shape);

            env_timer += 1/env_length;
            if(env_timer >= 1)
            {
                env_timer = -1;
            }
        }
        env*=10;
        output_env = env;

    }
    else
    {
        env = input_ext_env;
    }


    /* NOISE GENERATION */

    float noise;
    float u;

    for(int i = 0; i < 2; ++ i)
    {
        float noise_aux = input_aux_noise[i];

        u = randomUniform()*2-1;

        if(input_noise_shape[i] < 0) //Logistic curve
        {
            float k = -2*input_noise_shape[i]/10;
            float L = 1/(.5 - 1/(1+exp(10*k)));

            noise = -(1/k)*log((L+2*u)/(L-2*u));
 
        }
        else //S&H
        {
            noise = u*10;

            //32kHz -> 2.04Hz
            float sh_voct = 7-1.4*input_noise_shape[i];
            float sh_freq = 261.626f * powf(2.0f, sh_voct);

            sh_phase[i] += deltaTime*sh_freq;
            if(sh_phase[i] >= 1)
            {
                sh_phase[i] -= 1;
                sh_val[i] = noise;
            }
            noise = sh_val[i];
        }

        //TODO: Filtering

        //TODO: Noise mix

        output_noise[i] = noise;
    }

    output_noise_out = output_noise[0];
    //TODO: generate noise mix here

    if(! inputs[INPUT_NOISE_INPUT].active)
    { //select internal noise source
        noise = output_noise_out;
    } 
    else
    {
        noise = input_noise_input;
    }

    noise *= env;
    noise *= input_noise_level;

    /* DELAY CLOCKING */
    float delay_clock = input_delay_clock;

    if (delay_clock > 2.5)
    { //measuring new delay length
    }
    else
    { //running normally
    }

    //Compute delay tap from inputs

    /* FEEDBACK FILTER */

    output_delay_out = delay->get_tap_floating(delay_tap);

    float filter_out = output_delay_out;

    /* FEEDBACK CALCULATION */
    
    float feedback = 0;

    if (inputs[INPUT_FEEDBACK].active)
    {
        feedback += input_feedback;
    }
    else
    {
        feedback += filter_out*param_feedback_atv; //TODO: Norm to 1 and use input_feedback
    }

    feedback += noise;
    output_delay_in = feedback;

    delay->push(feedback);


    /*  +OUTPUT_PROCESSING */
    #include "Achilles_outputs.hpp"
    /*  -OUTPUT_PROCESSING */



}


struct AchillesWidget : ModuleWidget {
    AchillesWidget(Achilles *module) : ModuleWidget(module) {
        box.size = Vec(27.0 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

        {
            SVGPanel *panel = new SVGPanel();
            panel->box.size = box.size;
            panel->setBackground(SVG::load(assetPlugin(plugin, "res/Achilles.svg")));
            addChild(panel);
        }

        module->delay = new ttt::CircularBuffer(BUFL);

        addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        /* +CONTROL INSTANTIATION */
        #include "Achilles_panel.hpp"
        /* -CONTROL INSTANTIATION */

        module->ready=1;
    }
};

#include "Achilles_instance.hpp"

