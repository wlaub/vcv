
#include "TechTechTechnologies.hpp"


struct QMod : Module {
    /* +ENUMS */
    #include "QMod_enums.hpp"
    /* -ENUMS */
    
    /* +TRIGGER_VARS */
    #include "QMod_vars.hpp"
    /* -TRIGGER_VARS */

    float phase[4] = {0,0,0,0};

    QMod() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
    void step() override;

    // For more advanced Module features, read Rack's engine.hpp header file
    // - toJson, fromJson: serialization of internal data
    // - onSampleRateChange: event triggered by a change of sample rate
    // - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};


void QMod::step() {
    float deltaTime = engineGetSampleTime();


    /*  +INPUT_PROCESSING */
    #include "QMod_inputs.hpp"
    /*  -INPUT_PROCESSING */

    float i_clk[4];
    float q_clk[4];

    for(int i = 0; i < 4; ++i)
    {
        float pitch = input_freq[i];
        float dp = 2*M_PI*261.626f * powf(2.0f, pitch);
        phase[i] += dp*deltaTime;
        if(phase[i] > 2*M_PI)
        {
            phase[i] -= 2*M_PI;
        }
    }

    //TODO: I/Q carrier source selection
    for(int i = 0; i < 4; ++i)
    {
        i_clk[i] = sin(phase[i]);
    }

    for(int i = 0; i < 4; ++i)
    {
        q_clk[i] = sin(phase[i]+3.14/2);       
    }

    //Modulation

    for(int i = 0; i < 4; ++i)    
    {
        input_q_in[i] *= param_in_gain[i];
        input_i_in[i] *= param_in_gain[i];
        output_mod_out[i] =   i_clk[i]*input_i_in[i]
                            + q_clk[i]*input_q_in[i];
    }

    //Demodulation

    //TODO: Select passthrough

    for(int i = 0; i < 4; ++i)
    {
        input_mod_in[i] *= param_in_gain[i];
        output_i_out[i] = input_mod_in[i] * i_clk[i];
        output_q_out[i] = input_mod_in[i] * q_clk[i];
    }

    /*  +OUTPUT_PROCESSING */
    #include "QMod_outputs.hpp"
    /*  -OUTPUT_PROCESSING */



}


struct QModWidget : ModuleWidget {
    QModWidget(QMod *module) : ModuleWidget(module) {
        box.size = Vec(18.0 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

        {
            SVGPanel *panel = new SVGPanel();
            panel->box.size = box.size;
            panel->setBackground(SVG::load(assetPlugin(plugin, "res/QMod.svg")));
            addChild(panel);
        }



        addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        /* +CONTROL INSTANTIATION */
        #include "QMod_panel.hpp"
        /* -CONTROL INSTANTIATION */
    }
};

#include "QMod_instance.hpp"

