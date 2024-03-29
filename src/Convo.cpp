
#include "TechTechTechnologies.hpp"
#define BUFL 128

struct Convo : Module {
    /* +ENUMS */
    #include "Convo_enums.hpp"
    /* -ENUMS */
    
    /* +TRIGGER_VARS */
    #include "Convo_vars.hpp"
    /* -TRIGGER_VARS */

    dsp::SchmittTrigger clock_trigger[2];

    int ready = 0;

    ttt::CircularBuffer* signal[2];
    ttt::CircularBuffer* kernel[2];

    float window[2][BUFL]={{0},{0}};

    float smpl_phase[2] = {0};
    float smpl_val[2] = {0};

   

    Convo() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        #include "Convo_paramconfig.hpp"       
    }

    void process(const ProcessArgs& args) override {
        float deltaTime = args.sampleTime;

        if(ready == 0) return;
        /*  +INPUT_PROCESSING */
        #include "Convo_inputs.hpp"
        /*  -INPUT_PROCESSING */

        for(int i = 0; i < 2; ++i)
        {
            //push signal buffer
            if(inputs[INPUT_SIGNAL_IN+i].active)
            {
                signal[i]->push(input_signal_in[i]);        
            }
            else
            {
                signal[i]->push(0);
            }

            //Process kernel clock and runst

            if(param_runst[i] == 1)
            {
                float smpl_voct = 1*param_freq_offset[i];
                float smpl_freq = 261.626f * powf(2.0f, smpl_voct);

                smpl_phase[i] += deltaTime*smpl_freq;
                int int_clock = 0;
                if(smpl_phase[i] > 1)
                {
                    smpl_phase[i] -= 1;
                    int_clock=1;
                }    

                if(inputs[INPUT_CLOCK_IN+i].active)
                {
                    auto gate = clock_trigger[i].process(input_clock_in[i]);
                    if(gate)
                    {
                        kernel[i]->push(input_kernel_in[i]*param_kernel_gain[i]/10.);
                    }
                }
                else
                {   //Process internal kernel clock
                    if(int_clock == 1)
                    {
                         kernel[i]->push(input_kernel_in[i]*param_kernel_gain[i]/10.);                   
                    }
                }
            }

            //Process window shape

            double result = 0;
            for(int j = 0; j < BUFL; ++j)
            {
               float w = .9/((abs(j*param_window_rollof[i]-BUFL*param_window_length[i])+1));
               //Compute convolution
               result += w*signal[i]->get_tap(j)*kernel[i]->get_tap(j);
            }

            output_conv_out[i] = result;

            //Write output

        }

        /*  +OUTPUT_PROCESSING */
        #include "Convo_outputs.hpp"
        /*  -OUTPUT_PROCESSING */



    }



    // For more advanced Module features, read Rack's engine.hpp header file
    // - dataToJson, dataFromJson: serialization of internal data
    // - onSampleRateChange: event triggered by a change of sample rate
    // - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};



struct ConvoWidget : ModuleWidget {
    ConvoWidget(Convo *module) {
		setModule(module);
        box.size = Vec(10.0 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

        {
            SvgPanel *panel = new SvgPanel();
            panel->box.size = box.size;
            panel->setBackground(SVG::load(asset::plugin(pluginInstance, "res/Convo.svg")));
            addChild(panel);
        }

        if(module)       
        {
            for(int i; i < 2; ++i)
            {
                module->signal[i] = new ttt::CircularBuffer(BUFL);
                module->kernel[i] = new ttt::CircularBuffer(BUFL);
            }
        }

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        /* +CONTROL INSTANTIATION */
        #include "Convo_panel.hpp"
        /* -CONTROL INSTANTIATION */
        if(module)
            module->ready = 1;
    }
};

#include "Convo_instance.hpp"

