
#include "TechTechTechnologies.hpp"


struct Prometheus2 : Module {
    /* +ENUMS */
    #include "Prometheus2_enums.hpp"
    /* -ENUMS */
    
    /* +TRIGGER_VARS */
    #include "Prometheus2_vars.hpp"
    /* -TRIGGER_VARS */

    unsigned char* raw_index[2];
    unsigned int* master_index[2];
    unsigned short* data_index[2];

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


void Prometheus2::step() {
    float deltaTime = engineGetSampleTime();

    /*  +INPUT_PROCESSING */
    #include "Prometheus2_inputs.hpp"
    /*  -INPUT_PROCESSING */



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

