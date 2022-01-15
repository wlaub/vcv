
#include "TechTechTechnologies.hpp"

#define PRINT_SEARCH //printf
#define MAX_CHANNELS 16
#define GLITCH_MASK 0xffff

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

    struct ttt::Biquad** filters;

    int filt_order = 2;

    float out_value[MAX_CHANNELS] = {0};

    double x0[MAX_CHANNELS] = {0};
    double y0[MAX_CHANNELS] = {0};

    float glitch_light = 0;
    float taps_light = 0;
    unsigned short last_taps = 0;

    unsigned short shift_register[MAX_CHANNELS] = {0};
    unsigned short actual_length = 1;
    float clock_phase[MAX_CHANNELS] = {0};
    float glitch_phase= 0;

    unsigned int glitch_waiting = 0; //Flags

    unsigned char* raw_index[2];
    unsigned short get_taps(unsigned short length, unsigned short param0, unsigned short param1, unsigned char order);
    unsigned short get_actual_length(unsigned short length);

    dsp::SchmittTrigger clkTrigger, glitchTrigger, glitchGateTrigger;

    Prometheus2()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        /* +CONFIGS */
        #include "Prometheus2_configs.hpp"
        /* -CONFIGS */

        #include "prom_filter.hpp"

        filters = new struct ttt::Biquad*[filt_order];

        for(int i = 0; i < filt_order; ++i)
        {
            filters[i] = new struct ttt::Biquad(MAX_CHANNELS, sos[i]);
        }

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

    void process(const ProcessArgs& args) override {

        float deltaTime = args.sampleTime;

        /*  +INPUT_PROCESSING */
        #include "Prometheus2_inputs.hpp"
        /*  -INPUT_PROCESSING */

        //Process glitch input

        if(glitchTrigger.process(param_glitch_button))
        {
            glitch_phase = 0;
            glitch_waiting = GLITCH_MASK;
        }

        if(glitchGateTrigger.process(input_glitch_gate))
        {
            glitch_waiting = GLITCH_MASK;
        }

        if(param_glitch_button == 1)
        {
            double glitch_period = 1/param_glitch_rate;

            glitch_phase += deltaTime;

            if(glitch_phase > glitch_period)
            {
                glitch_phase -= glitch_period;
                glitch_waiting = GLITCH_MASK;
            }
        }

        //Glitch light indicator timing

        glitch_light *= .9995;
        if(glitch_waiting == GLITCH_MASK)
        {
            glitch_light = 1;
        }
        light_light_left = glitch_light;
        light_light_right = glitch_light;

        //Compute tap configuration parameters

        float length_value = param_length_offset+2+param_length_fine+input_length_cv;
        
        unsigned short length = 4096/pow(2,length_value);
    //        if(length <2) length = 2;
        if(length >= 4096) length = 4095;

        float param0_value = clamp(
                +param_param_0_coarse
                +input_param_0_cv,
                0.0f,1.0f);
        float param1_value = clamp(
                param_param_0_fine,
                0.0f, 1.0f);

        unsigned short param0 = 65535*param0_value;
        unsigned short param1 = 65535*param1_value;

        unsigned short taps;
        taps = get_taps(length, param0, param1, 0);
        actual_length = get_actual_length(length);

        //Taps change indicator

        if(taps != last_taps)
        {
            last_taps = taps;
            taps_light = 1- taps_light;
        }

        light_light_center = taps_light;

        //Run lfsr's

        int ext_tick = clkTrigger.process(input_ext_clk);   

        int channels = inputs[INPUT_VOCT].getChannels();
        if(channels > MAX_CHANNELS)
        {
            channels = MAX_CHANNELS;
        }
        if(channels == 0)
        {
            channels = 1;
        }

        for(int c = 0; c < channels; ++c)
        {
            int tick = 0;

            if(!inputs[INPUT_EXT_CLK].active)
            {
                double freq = inputs[INPUT_VOCT].getVoltage(c)
                    +param_pitch_coarse+param_pitch_fine;
                freq = 261.626*pow(2, freq);

                float period = 1/freq;
                float locked_period = period;

                if(param_freq_lock == 1)
                {
                    locked_period = period/actual_length;
                }

                if(deltaTime > locked_period)
                {
                    clock_phase[c] = locked_period;
                }
                else
                {
                    clock_phase[c] += deltaTime;
                }

                if(clock_phase[c] >= locked_period)
                {
                    clock_phase[c] -= locked_period;
                    tick = 1;
                }
            }

            if(tick || ext_tick)
            {
                int self_mask = 1 << c;
                if(glitch_waiting&self_mask)
                {
                    glitch_waiting &= ~self_mask;
                    taps = 4095; //TODO: Change this
                }

                unsigned short feedback = 1^__builtin_popcount(taps&shift_register[c]);

                shift_register[c] <<= 1;
                shift_register[c] |= (feedback&1);

                out_value[c] = 10*(feedback&1)-5;

            }

            double dc_coupled = out_value[c];
            double ac_coupled = out_value[c];

            for(int i = 0; i < filt_order; ++i)
            {
                ac_coupled = filters[i]->step(ac_coupled, c);
            }

            /*
            ac_coupled = dc_coupled-.1*y0;;
            x0 = dc_coupled;
            y0 = ac_coupled; 
        */
            if(param_bias_control == 0)
            {
                output_out = dc_coupled+5;
            }
            else
            {
                output_out = ac_coupled;
            }
            outputs[OUTPUT_OUT].setVoltage(output_out, c);
        }
        outputs[OUTPUT_OUT].setChannels(channels);
        /*  +OUTPUT_PROCESSING */
        #include "Prometheus2_outputs.hpp"
        /*  -OUTPUT_PROCESSING */



    }



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

