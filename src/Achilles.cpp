
#include "TechTechTechnologies.hpp"
#define BUFL (1<<12)

struct Achilles : Module {
    /* +ENUMS */
    #include "Achilles_enums.hpp"
    /* -ENUMS */
    
    /* +TRIGGER_VARS */
    #include "Achilles_vars.hpp"
    /* -TRIGGER_VARS */

    ttt::CircularBuffer* delay;
    float delay_tap = 10;

    Achilles() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
    void step() override;

    // For more advanced Module features, read Rack's engine.hpp header file
    // - toJson, fromJson: serialization of internal data
    // - onSampleRateChange: event triggered by a change of sample rate
    // - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};


void Achilles::step() {
    float deltaTime = engineGetSampleTime();


    /*  +INPUT_PROCESSING */
    #include "Achilles_inputs.hpp"
    /*  -INPUT_PROCESSING */

    /* GATE PROCESSING */

    /* ENVELOPE HANDLING */

    float env = input_gate; //TODO

    /* NOISE GENERATION */

    float noise;
    if(! inputs[INPUT_NOISE_INPUT].active)
    {
        noise = randomNormal()*10;
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

    {//This should happen on a gate
        float voct;
        voct = input_voct + input_fm;
        voct *= 1-input_match/100;

        float period = 1/(deltaTime*261.626f * powf(2.0f, voct));
        delay_tap = period;
    }

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
        feedback += filter_out*param_feedback_atv;
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
    }
};

#include "Achilles_instance.hpp"

