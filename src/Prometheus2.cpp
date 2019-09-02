
#include "TechTechTechnologies.hpp"

#define PRINT_SEARCH //printf

typedef struct {
    unsigned short left;
    unsigned short _;
    unsigned short right;
    unsigned short value;

} IndexEntry;

struct Prometheus2 : Module {
    /* +ENUMS */
    #include "Prometheus2_enums.hpp"
    /* -ENUMS */
    
    /* +TRIGGER_VARS */
    #include "Prometheus2_vars.hpp"
    /* -TRIGGER_VARS */

    unsigned short shift_register = 0;
    unsigned short actual_length = 1;
    float clock_phase = 0;
    float glitch_phase= 0;

    unsigned int glitch_waiting = 0;

    unsigned char* raw_index[2];
    unsigned short get_taps(unsigned short length, unsigned short param0, unsigned short param1, unsigned char order);
    unsigned short get_actual_length(unsigned short length);

    SchmittTrigger clkTrigger, glitchTrigger, glitchGateTrigger;

    Prometheus2()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        /* +CONFIGS */
        #include "Prometheus2_configs.hpp"
        /* -CONFIGS */

        for(int i = 0; i < 2; ++i)
        {
            char base_filename[256];
            sprintf(base_filename, "res/lookup_inv_%i.taps",i);
            
            FILE* fp = fopen(asset::plugin(pluginInstance, base_filename).c_str(), "rb");
            if(fp != 0)
            {
                fseek(fp, 0, SEEK_END);
                unsigned int length = ftell(fp);
                rewind(fp);
               
                raw_index[i] = new unsigned char[length];
                fread(raw_index[i], 1, length, fp);

                fclose(fp);

                get_taps(1, 30000, 30000, i);
                get_taps(11, 30000, 30000, i);
                get_taps(4095, 30000, 30000, i);

            }
            else
            {
                printf("ERROR: Couldn't find required file %s\n", base_filename);
            }
        }

    }
    void step() override;

    // For more advanced Module features, read Rack's engine.hpp header file
    // - toJson, fromJson: serialization of internal data
    // - onSampleRateChange: event triggered by a change of sample rate
    // - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};

unsigned short Prometheus2::get_actual_length(unsigned short length)
{
    unsigned char* buffer = raw_index[0];
    unsigned int base_address = ((unsigned int*)(buffer))[length];


    unsigned short actual_size = *(unsigned short*)(buffer+base_address-2);

    return actual_size;
}

unsigned short Prometheus2::get_taps(unsigned short length, unsigned short param0, unsigned short param1, unsigned char order)
{
    unsigned char* buffer = raw_index[order];
    unsigned int base_address = ((unsigned int*)(buffer))[length];


    unsigned short actual_size = *(unsigned short*)(buffer+base_address-2);

PRINT_SEARCH("Length %i maps to actual length %i starting at address %i\n", length, actual_size, base_address);
    unsigned short left;
    unsigned short right;
    unsigned short index;



    left = 0;
    right = *(unsigned short*)(buffer+base_address+2);
    index = 0;

PRINT_SEARCH("There are %i options available here\n", right);

    //I know there is a better way to do this, but I don't have bw to care rn.
    IndexEntry* entry;
    if(param0 == 65535) param0 -= 1;
    if(param1 == 65535) param1 -= 1;

    while(1)
    {
        entry = (IndexEntry*)(buffer+base_address+index*4);
PRINT_SEARCH("%i ?? %i ?? %i\n", entry->left, param0, entry->right);
PRINT_SEARCH("=%i/%i/%i\n", left, index, right);

        if(param0 < entry->left)
        {
            right=index;
            index = (index+left)/2;
        }
        else if(param0 >= entry->right)
        {
            left=index;
            index = (index+right)/2;
        }
        else
        {
            break;
        }
    }

//    unsigned int sub_address = base_address+*(unsigned short*)(buffer+base_address+6+index0*4);
    base_address+=entry->value;
    left = 0;
    right = *(unsigned short*)(buffer+base_address+2);
    index = 0;
    unsigned int count = 0;

PRINT_SEARCH("  There are %i options available here\n", right);

    while(count < 100)
    {
        count += 1;
        entry = (IndexEntry*)(buffer+base_address+index*4);
PRINT_SEARCH("  %i ?? %i ?? %i\n", entry->left, param1, entry->right);
PRINT_SEARCH("  =%i/%i/%i\n", left, index, right);
        if(param1 < entry->left)
        {
            right = index;
            index = (index+left)/2;
        }
        else if(param1 >= entry->right && param1 != 65535)
        {
            left = index;
            index = (index+right)/2;
        }
        else
        {
PRINT_SEARCH("<----%x\n", entry->value);
            return entry->value;
        }
    }
    return 0;

    
}



void Prometheus2::step() {
    float deltaTime = engineGetSampleTime();

    /*  +INPUT_PROCESSING */
    #include "Prometheus2_inputs.hpp"
    /*  -INPUT_PROCESSING */



    if(glitchTrigger.process(param_glitch_button))
    {
        glitch_phase = 0;
        glitch_waiting = 1;
    }

    if(glitchGateTrigger.process(input_glitch_gate))
    {
        glitch_waiting = 1;
    }

    if(param_glitch_button == 1)
    {
        double glitch_period = 1/param_glitch_rate;

        glitch_phase += deltaTime;

        if(glitch_phase > glitch_period)
        {
            glitch_phase -= glitch_period;
            glitch_waiting = 1;
        }
    }

    int tick = 0;

    if(!inputs[INPUT_EXT_CLK].active)
    {
        double freq = input_voct+param_pitch_coarse+param_pitch_fine;
        freq = 261.626*pow(2, freq);

        float period = 1/freq;
        float locked_period = period;

        if(param_freq_lock == 1)
        {
            locked_period = period/actual_length;
        }

        if(deltaTime > locked_period)
        {
            clock_phase = locked_period;
        }
        else
        {
            clock_phase += deltaTime;
        }

        if(clock_phase >= locked_period)
        {
            clock_phase -= locked_period;
            tick = 1;
        }
    }
    else
    {
        tick = clkTrigger.process(input_ext_clk);
    }

    if(tick)
    {

        float length_value = param_length_offset+2+param_length_fine+input_length_cv;
        
        unsigned short length = pow(2,length_value);
//        if(length <2) length = 2;
        if(length >= 4096) length = 4095;

        float param0_value = clamp(
                0.1*param_param_0_fine
                +param_param_0_coarse
                +input_param_0_cv,
                0.0f,1.0f);
        float param1_value = .5;

        unsigned short param0 = 65535*param0_value;
        unsigned short param1 = 65535*param1_value;

        if(glitch_waiting == 1)
        {
            glitch_waiting = 0;
            length = 4095;
        }

        unsigned short taps;
        taps = get_taps(length, param0, param1, 0);
        actual_length = get_actual_length(length);

        unsigned short feedback = 1^__builtin_popcount(taps&shift_register);

        shift_register <<= 1;
        shift_register |= (feedback&1);

        output_out = (feedback&1);

    }


    /*  +OUTPUT_PROCESSING */
    #include "Prometheus2_outputs.hpp"
    /*  -OUTPUT_PROCESSING */



}


struct Prometheus2Widget : ModuleWidget {

    Prometheus2Widget(Prometheus2 *module) {
        setModule(module);
        box.size = Vec(8.0 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Prometheus2.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));



        /* +CONTROL INSTANTIATION */
        #include "Prometheus2_panel.hpp"
        /* -CONTROL INSTANTIATION */
    }
};

#include "Prometheus2_instance.hpp"

