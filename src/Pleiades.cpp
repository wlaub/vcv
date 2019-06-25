
#include "TechTechTechnologies.hpp"

#define DEPTH 6
#define DMAIN true
#define DSEQ false
#define DTEMP false

#define CENTER_STEP_INDEX 1

/*
#define SDPRINT(DSEQ, x, ...) \
#ifdef x \
printf(...);        \
#endif  \
*/

#define DPRINT(x, ...) if(x) printf(__VA_ARGS__);

const float MODE_COLORS[7][3] = {
    {0,0,1},
    {0,1,0},
    {0,1,1},
    {1,0,0},
    {1,1,0},
    {1,0,1},
    {1,1,1}
    };

struct Step
{
    // 0 : slew
    // 1 : primary microtone
    // 2 : primary tone
    // 3 : primary octave
    // 4 : trigger
    // 5 : secondary tone 
    // 6 : secondary octave
    unsigned char values[7]={0,0,0,3,0,0,6};
//    unsigned char values[7]={0,0,0,0,0,0,0};
    void setValue(int index, unsigned char val)
    {
        values[index] = val;
    }

    float getValue(Step* triggerStep, int index, int subindex, float* tones, float prevTone)
    {
        //index and subindex must be 1-7
        //tones is an array mapping tone number to value
        //default [x/7 for x in range(7)]
        //
        //Step is the matching step from the next higher sequence and provides
        //trigger_setting for trigger configuration
        //
        //index is the index of the current substep and is used to
        //determine whether step is high or low by index%trigger_setting == 0
        //Over the course of the step, each trigger setting yields the sequence
        // 0 : 1 1 1 1 1 1 1
        // 1 : 0 1 0 1 0 1 0
        // 2 : 0 1 0 0 1 0 0
        // 3 : 0 0 0 1 0 0 0
        // 4 : 0 0 1 0 0 0 0
        // 5 : 0 1 0 0 0 0 0
        // 6 : 1 0 0 0 0 0 0
        //
        //prevTone is the tone value from the end of the previous index,
        //maintained at the controller level.
        //
        //subindex is the sequence index 2 steps down or 0 if there isn't one
        //it is used to compute slew between trigger states. the slew setting
        //determines how many subindex steps it takes to reach the target value
        //for the current substep trigger states so that the alpha between old
        //value and new value is
        // 0 : 1.0  1.0  1.0  1.0  1.0  1.0  1.0
        // 1 : 0.5  1.0  1.0  1.0  1.0  1.0  1.0
        // 2 : 0.3  0.7  1.0  1.0  1.0  1.0  1.0
        // 3 : 0.2  0.5  0.8  1.0  1.0  1.0  1.0
        // 4 : 0.2  0.4  0.6  0.8  1.0  1.0  1.0
        // 5 : 0.2  0.3  0.5  0.7  0.8  1.0  1.0
        // 6 : 0.1  0.3  0.4  0.6  0.7  0.9  1.0
        //

        float tone_values[2];
        tone_values[0] = values[3]+tones[values[2]]+tones[values[1]]/7.0 -3;
        tone_values[1] = values[6]+tones[values[5]] - 1;

        unsigned char trigger = triggerStep->values[4];

        unsigned char tlast = trigger+1;
        unsigned char tfirst = 8-index;

        unsigned char triggerIndex = tfirst%tlast;

        DPRINT(DSEQ, "    %i%%%i = %i\n", tfirst, tlast, triggerIndex);

        DPRINT(DTEMP, "%i\n", triggerIndex);
        triggerIndex = ((triggerIndex == 0) ? 0 : 1);

        DPRINT(DSEQ, "    ");
        for(int i = 0; i < 7; ++i)
        {
            DPRINT(DSEQ, "%i ", values[i]);
        }
        DPRINT(DSEQ, "\n");
        DPRINT(DSEQ, "    Param: %i, %i, %f\n", index, subindex, prevTone);
        DPRINT(DSEQ, "    Tones: %f, %f, %i\n", tone_values[0], tone_values[1], triggerIndex);

        if(!std::isfinite(prevTone)) return tone_values[triggerIndex];

        float alpha = min(1.0,float(subindex)/(values[0]+1));

        DPRINT(DSEQ, "    %i/%i -> %f\n", subindex-1, values[0]+1, alpha);

        return tone_values[triggerIndex]*alpha+prevTone*(1-alpha);
    }
};

struct Address
{
//Deals with the mechanics of incrementing through the octal address space and
//extracting relevant addresses to compute the current step.
//
//DEPTH is the number of levels excluding root step, so there are DEPTH+1 total
//steps to add together at each point in the sequence
//
//In digits, index 0 is the lowest level (largest step size)
//
    unsigned char digits[DEPTH] = {1};

    Address()
    {
        for (int i = 0; i < DEPTH; ++i)
            digits[i] = 1;
    }

    int step(int depth_index, bool sync = false)
    {//Increments the step and returns flags showing which indices rolled over
     //Depth protects smaller indices from being incremented
        int result = 0;
        for (int i = 0; i < DEPTH-depth_index; ++i)
        {
            int revidx = DEPTH-1-i;
            if(false and sync and revidx > depth_index)
            {
                digits[revidx] = 8;
            }
            else
            {
                digits[revidx] += 1;
            }

            if(digits[revidx] == 8) 
            {
                digits[revidx] = 1;
                result |= (1<<revidx);
            }
            else
            {
                i = DEPTH;
            }
        }

        return result;
    }

    void print()
    {
        DPRINT(DSEQ, "0o%07o\n",get_address(DEPTH+1));
    }

    int get_address(int depth)
    { //Returns the address of the step at the given depth by masking digits
      //and reducing to an octal address using the masking scheme.
      //
      // digits  [ A, B, C, ... ]
      // depth 0 [ 0, 0, 0, ... ]
      // depth 1 [ A, 0, 0, ... ]
      // depth 2 [ A, B, 0, ... ]
      //
      //And so on.

        int result = 0;
        for (int i = 0; i < DEPTH; ++i)
        {
            if(i < depth)
            {
                result |= digits[i];
            }
            result <<= 3;
        }
        return result>>3;
    }

    int get_sub_address(int depth, unsigned char index)
    { //Returns the address of the step at the given depth by masking digits
      //and reducing to an octal address using the masking scheme.
      //
      // digits  [ A, B, C, ... ]
      // depth 0 [ 0, 0, 0, ... ]
      // depth 1 [ A, 0, 0, ... ]
      // depth 2 [ A, B, 0, ... ]
      //
      //And so on.

        int result = 0;
        for (int i = 0; i < DEPTH; ++i)
        {
            if(i < depth)
            {
                result |= digits[i];
            }
            else if(i == depth)
            {
                result |= index;
            }
            result <<= 3;
        }
        return result>>3;
    }



};

struct Sequence
{
    struct Step* steps;
    //struct Step steps[(1<<3*DEPTH)];
    
    float prevTone[DEPTH+1] = {0};
    float prevValue[DEPTH+1] = {0};

    int length = 1<<(3*DEPTH);

    Sequence()
    {
        steps = new struct Step[length];

        for (int i = 0; i < DEPTH; ++i)
        {
            prevTone[i] = 0;
            prevValue[i] = 0;
        }
//        steps[0].values[6] = 6;
    }

    void checkSteps()
    {
        for (int i = 0; i < length; ++i)
        {
            for (int j = 0; j < 7; ++j)
            {
                if(steps[i].values[j] > 7)
                {
                    printf("Error: %i-%i=%i", i, j, steps[i].values[j]);
                }
            }

        }


    }

    int step(int rolls)
    {
        for (int i = 0; i < DEPTH; ++i)
        {
            if (rolls & (1<<(i+1)))
            {
                prevTone[i] = prevValue[i];
                DPRINT(DSEQ, "Rolled %i : %f -> %f\n", i, prevValue[i], prevTone[i]);
            }
        } 

        return 0;
    }

    Step getStep(int index)
    {
        //TODO: Move step retrieval to here
        if (index >= length)
        {
            printf("SEQUENCE INDEX ERROR: %o >i= %o\n", index, length);
        }
        return steps[index];
    }

    float get_value(Address fromAdd, Sequence triggerSeq, float* tones)
    {
        float result = 0;


/*
        for (int i = 0; i < DEPTH; ++i)
        {
            printf("%i", fromAdd.digits[i]);
        }
        printf("\n");
*/

        fromAdd.print();

        for (int i = -1; i < DEPTH; ++i)
        {
            int index = 1;
            int subindex = 7;

            if(i < DEPTH-1)
                index = fromAdd.digits[i+1];
            if(i < DEPTH-2)
                subindex = fromAdd.digits[i+2];

            int addIdx = fromAdd.get_address(i+1);
            Step mainStep = getStep(addIdx);
            Step triggerStep = triggerSeq.getStep(addIdx);

            float prevToneLocal = NAN;
            if (i < DEPTH-1)
                prevToneLocal = prevTone[i+1];


            DPRINT(DSEQ, "0o%07o:\n", addIdx);
            float stepVal = mainStep.getValue(
                &triggerStep, index, subindex, 
                tones, prevToneLocal
                );
            prevValue[i+1] = stepVal;
            result += stepVal;
            DPRINT(DSEQ, "  %i: %f / %f\n", i, stepVal, prevToneLocal);
//    float getValue(Step triggerStep, int index, int subindex, float* tones, float prevTone)
 
        }
        DPRINT(DSEQ, "\n");
        return result;
    }

    void* toStr(const char* filename)
    {
        char* result;

        unsigned char size = 3;
        result = new char[length*size];

        for(int i = 0; i < length; ++i)
        {
            unsigned int value = 0;
            for(int j = 0; j < 7; ++j)
            {
                value <<=3;
                value |= steps[i].values[j]&0x07;
            }

/*            if(value != 0)
            {
                printf("<--- %o @ %i\n", value, i);
            }*/

            for(int j = 0; j < size; ++j)
            {
                result[size*i+j] = (value & 0xff);
                if((value & 0xff) != 0)
/*                {
                    printf("VALUE %x @ %i (%o) \n", result[size*i+j]&0xff, size*i+j, value);
                }*/
 
                value >>=8;

            }

        }
        printf("\nSAVE %i\n", length);

        std::string settingsFilename = assetLocal(filename);
        FILE *file = fopen(settingsFilename.c_str(), "wb");
        if (file) {
            fwrite(result, 1, length*size, file);
        }
        fclose(file);

        delete[] result;
    }

    void fromStr(const char* filename)
    {

        char* string;

        unsigned char size = 3;
        string = new char[length*size];


        std::string settingsFilename = assetLocal(filename);
        FILE *file = fopen(settingsFilename.c_str(), "rb");
        if (file) {
            fread(string, 1, length*size, file);
        }
        fclose(file);

        for(int i = 0; i < length; ++i)
        {
            unsigned int value = 0;
            for(int j = 0; j < size; ++j)
            {
                value <<= 8;
                value |= (string[size*i+size-1-j]&0xff);
                /*if(string[size*i+size-1-j]&0xff != 0)
                {
                    printf("VALUE %x @ %i (%o)\n", string[size*i+size-1-j]&0xff, size*i+size-1-j, value);
                }*/
 

            }
            /*
            if(value != 0)
            {
                 printf("---> %o @ %i\n", value, i);                   
            }*/

            for(int j = 0; j < 7; ++j)
            {
               /* if(value != 0)
                {
                    printf("-- %i\n", value&0x07);
                }*/
                steps[i].values[6-j] = value&0x07;
                value >>=3;
            }
       }

        delete[] string;

        //printf(string);
        printf("LOAD\n");
    }

};

struct Pleiades : Module {
    /* +ENUMS */
    #include "Pleiades_enums.hpp"
    /* -ENUMS */
    enum LightIds {
        ENUMS(LIGHT_PORT, 7*2),
        ENUMS(LIGHT_ADDRESS, 7*DEPTH),
        ENUMS(LIGHT_WRITE_ENABLE, 2),
        NUM_LIGHTS
    };
 

    /* +TRIGGER_VARS */
    #include "Pleiades_vars.hpp"
    /* -TRIGGER_VARS */

    struct Sequence sequences[7];
    int seq_idx = 0; //currently selected sequence

    struct Address address;
    int depth_idx = DEPTH-4;

    struct EncoderController* encoders[NUM_PARAMS];
    int encoder_delta[NUM_PARAMS];

    SchmittTrigger saveTrigger;
    SchmittTrigger loadTrigger;

    SchmittTrigger clockTrigger;
    int counter = 0;
    int clockCounter = 0;
    float basePeriod = 49;
    float scalePeriod = 1.0/49;
    float clockPeriod = 49;

    int outputCounter = 0;

    bool update_steps = false;
    bool ready = false;
    bool write_enable = false;
    char write_name[256];

    float tones[7] = {0, 1.0/7, 2.0/7, 3.0/7, 4.0/7, 5.0/7, 6.0/7};

    void updateStepKnobs();
    void updateCenterFromStep();

    void step() override;

    void mode0Callback(unsigned char new_value);

    TextField* seq_name;

    json_t *dataToJson() override;
    void dataFromJson(json_t *rootJ) override;
    

    Pleiades() 
    {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    
    }

   

    // For more advanced Module features, read Rack's engine.hpp header file
    // - dataToJson, dataFromJson: serialization of internal data
    // - onSampleRateChange: event triggered by a change of sample rate
    // - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};

json_t* Pleiades::dataToJson()
{
    json_t *rootJ = json_object();

    char tstr[256];
    char filename[256];

    json_object_set_new(rootJ, "seq_name",
        json_string(write_name)
        );

    json_object_set_new(rootJ, "format_id", json_integer(0));


    for(int i = 0; i < 7; ++i)
    {
        sprintf(tstr, "seq_%i", i);
        sprintf(filename, "PleiadesSeq_%s_%i.dat", write_name,i);
        if(write_enable)
        {
            sequences[i].toStr(filename);
        }

        json_object_set_new(rootJ, tstr,
            json_string(filename)
            );
    }

    return rootJ;
}


void Pleiades::dataFromJson(json_t *rootJ)
{
    char tstr[256];
    Module::dataFromJson(rootJ);

    int format_id = json_integer_value(json_object_get(rootJ, "format_id"));

    seq_name->setText (json_string_value(json_object_get(rootJ, "seq_name")));
    sprintf(write_name, seq_name->text.c_str());
//    seq_name->onTextChange();

    for(int i = 0; i < 7; ++i)
    {
        sprintf(tstr, "seq_%i", i);       
        sequences[i].fromStr(json_string_value(
            json_object_get(rootJ, tstr)
            ));
    }
    write_enable=true;

    updateStepKnobs();

}




void Pleiades::updateStepKnobs()
{
    for(int i = 0 ; i < 7; ++i)
    {
        int step_index = address.get_sub_address(depth_idx+1, i+1);
        unsigned char value_index = encoders[PARAM_MODE+1]->getValue();                   
        unsigned char* step_values = 
            sequences[seq_idx].steps[step_index].values;

        encoders[PARAM_STEP+i]->setValues(step_values);
        DPRINT(DMAIN, "STEP UPDATED %o %i %i\n", 
                step_index, value_index,
                step_values[value_index]
                );
    }

    int step_index = address.get_address(depth_idx+1);
    unsigned char* step_values = 
        sequences[seq_idx].steps[step_index].values;

    encoders[PARAM_CENTER]->setValues(step_values, 1);


}

void Pleiades::step() {
    if(!ready) return;

    /* Assume that in any given step, only one knob can be turned. Otherwise,
     * there can be race conditions when the meaning of a knob changes at the
     * same time as its value. This should substantially simplify knob meta
     * configuration and order of operations. All of the knob meta configuration
     * can now take place at the top, knowing that it won't cause conflicts with
     * the operation of the module, which comes second.
     * */

    /* TODO: (Functional)
     * Somehow seq_idx gets set when changing depth to -1...
     * Make knobs able to have reasonable default values depending on function
     * Sync causes invalid stepping behavior at lower depth
     * Implement note tuning
     * Implement internal clock control
     * Implement adaptive exc clock loop filter
     * 
     * Bulk editing functionality
     * Analog controls
     * 
     * 
     * 
     * */

    /* TODO: (UI)
     * Make address display not suck
     * Implement mouse wheel control of encoder knobs - impossible?
     *
     *
     * */

    float deltaTime = engineGetSampleTime();

    /*  +INPUT_PROCESSING */
    #include "Pleiades_inputs.hpp"
    /*  -INPUT_PROCESSING */

    int N = 7;

    bool there_are_updates = false;
    for(int i = 0; i < NUM_PARAMS; ++i)
    {
        if(i == PARAM_CONFIG+2) continue;
        if(i == PARAM_SAVE) continue;       
        if(i == PARAM_LOAD) continue;        
        encoder_delta[i] = encoders[i]->process(); 
        if(encoder_delta[i] != 0) there_are_updates = true;
    }

    if(saveTrigger.process(params[PARAM_SAVE].value))
    {
        sprintf(write_name, seq_name->text.c_str());
        write_enable = true;
    }
    if(saveTrigger.process(params[PARAM_LOAD].value))
    {
        //Force load here
        //Set write enable on success
    }
    //set write enable lights here
    lights[LIGHT_WRITE_ENABLE].value = write_enable?0:1;
    lights[LIGHT_WRITE_ENABLE+1].value = write_enable?0:1;

    /***************************/
    /* Knob Meta-Configuration */
    /***************************/

    if(there_are_updates)
    {
        DPRINT(DMAIN, "TICK\n");
    }

    //MODE 0
    
    //MODE 1 (Step knob function)
    if(encoder_delta[PARAM_MODE+1] != 0)
    {
        int function_index = encoders[PARAM_MODE+1]->getValue();

        DPRINT(DMAIN, "STEP FUNCTION CHANGED %i\n", function_index);
        for(int i = 0; i < 7; ++i) 
        {
            encoders[PARAM_STEP+i]->setIndex(function_index);
        }
        if(encoders[PARAM_MODE+5]->getValue() == CENTER_STEP_INDEX)
        {
            encoders[PARAM_CENTER]->setIndex(function_index, 1);
        }
    }
   
    //MODE 2

    //MODE 3 (Temporary depth control)
    
    if(encoder_delta[PARAM_MODE+3] != 0)
    {
        depth_idx += encoder_delta[PARAM_MODE+3];
        int center_value = encoders[PARAM_CENTER]->getValue(0);
        address.digits[depth_idx] = center_value+1;
        updateStepKnobs();

        DPRINT(DMAIN, "DEPTH INDEX CHANGED %i\n", depth_idx);
    }

    //MODE 4

    //MODE 5 (Center knob function)
    if(encoder_delta[PARAM_MODE+5] != 0)
    {
        if(encoders[PARAM_MODE+5]->getValue() == CENTER_STEP_INDEX)
        {
            encoders[PARAM_CENTER]->setIndex(
                encoders[PARAM_MODE+1]->getValue(), 1);
            encoders[PARAM_CENTER]->setColor(encoders[PARAM_MODE+1]);
        }
        else
        {
            encoders[PARAM_CENTER]->setIndex(
                encoders[PARAM_MODE+5]->getValue(), 0);
            encoders[PARAM_CENTER]->setColor(encoders[PARAM_MODE+5]);
        }
    }

    //MODE 6

    //STEP 0-6 (Sequence step configurations)
    for(int i = 0 ; i < 7; ++i)
    {
        if(encoder_delta[PARAM_STEP+i] != 0)
        {
            int step_index = address.get_sub_address(depth_idx+1, i+1);
            unsigned char value_index = encoders[PARAM_MODE+1]->getValue();
            sequences[seq_idx].steps[step_index].setValue(
                value_index,
                encoders[PARAM_STEP+i]->getValue()
                );

            unsigned char* step_values = 
                sequences[seq_idx].steps[step_index].values;
            DPRINT(DMAIN, "PARAM CHANGE %o %i %i\n", step_index, value_index, step_values[value_index]);
        }
    }

    //CENTER

    if(encoder_delta[PARAM_CENTER] != 0)
    {
        int center_value = encoders[PARAM_CENTER]->getValue();
        switch(encoders[PARAM_MODE+5]->getValue())
        {
            int step_index;
            unsigned char value_index;
            case 0: //Step select
                address.digits[depth_idx] = center_value+1;
                updateStepKnobs();
            break;
            case 1: //Root step control
            
                step_index = address.get_address(depth_idx+1);
                value_index = encoders[PARAM_MODE+1]->getValue();
                sequences[seq_idx].steps[step_index].setValue(
                    value_index,
                    center_value
                    );
            
            break;
            case 2:
            break;
            case 3:
            break;
            case 4: //Sequence Select
                seq_idx = center_value;
                updateStepKnobs();
            break;
            case 5:
            break;
            case 6:
            break;
            case 7:
            break;
        }
    }



    //CONFIG 0
    
    //CONFIG 1
    
    //CONFIG 2



    bool sync = false;
    if(inputs[INPUT_CLOCK].active)
    {
        if(encoders[PARAM_MODE+6]->getValue() == 0)
        {
            if(clockTrigger.process(input_clock))
            {
                output_out[7] = 10*(clockCounter-basePeriod)/basePeriod;
//                float loop_rate = 1-pow(10, -clockCounter/10000.);
                float loop_rate = 1-1/(clockCounter/100+1);
                loop_rate = 1;
                basePeriod = basePeriod*(1-loop_rate) + clockCounter*loop_rate;

                clockPeriod = basePeriod*scalePeriod;
                clockCounter = 0;
                counter = clockPeriod+1;
                sync = true;
            }
            else
            {
                ++clockCounter;
            }
        }
    }
    else
    {
        clockCounter = 0;
    }


    if (counter < clockPeriod)
    {
        ++counter;
    }
    else
    {
        counter = 0;

        //Step sequence
        int rolls = address.step(depth_idx+1, sync);
        for(int i = 0; i < N; ++ i)
        {
            sequences[i].step(rolls);
        }

        //Update address lights
        int prev = 1;
        for(int i = 0; i < DEPTH; ++i)
        {
            for (int j = 0; j < 7; ++j)
            {
                lights[LIGHT_ADDRESS+i*7+j].value= 
                    ( 
                     (j+1 <= address.digits[i] and j+1 >= prev) or
                     (j+1 >= address.digits[i] and j+1 <= prev)
                      ?.1:0);
                if(j+1 == address.digits[i])
                    lights[LIGHT_ADDRESS+i*7+j].value= 1;
           }
           prev =  address.digits[i];
        }


//        printf("%i\n", (unsigned char)(param_mode[4]));
        //output_out[7] = address.digits[encoders[PARAM_MODE+4]->getValue()]-1;

        //Update port lights
        for(int i = 0; i < N; ++i)
        {
            lights[LIGHT_PORT+i*2].value = (i == seq_idx?1:0); //Value target
            lights[LIGHT_PORT+i*2+1].value = ((i+1)%7 == seq_idx?1:0); //Trigger target
        }

        //Generate outputs
        for(int i = 0; i < N; ++ i)
        {
            DPRINT(DTEMP, "TSEQ: %i\n", (i+1)%7);
            float val = sequences[i].get_value(address, sequences[(i+1)%7], tones);
            output_out[i] = val;
            DPRINT(DSEQ, "=%f\n", val);
        }
        DPRINT(DSEQ, "\n");

    }


    /*  +OUTPUT_PROCESSING */
    #include "Pleiades_outputs.hpp"
    /*  -OUTPUT_PROCESSING */


}


struct PleiadesWidget : ModuleWidget {
    TextField* seq_name;  

    void addParam(ParamWidget *param) {

        params.push_back(param);
        addChild(param);

        if(!module) return;

        if(param->paramQuantity->paramId == Pleiades::PARAM_CONFIG+2) return;
        if(param->paramQuantity->paramId == Pleiades::PARAM_SAVE) return;
        if(param->paramQuantity->paramId == Pleiades::PARAM_LOAD) return;

        const unsigned char defs[7] = {0,0,0,0,0,0,0};
        ((Pleiades*)(module))->encoders[param->paramQuantity->paramId] = new EncoderController((TTTEncoder*)param, defs);
        ((TTTEncoder*)param)->configureLights();
    }

    PleiadesWidget(Pleiades *module) {
		setModule(module);
        box.size = Vec(22.0 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

        {
            SVGPanel *panel = new SVGPanel();
            panel->box.size = box.size;
            panel->setBackground(SVG::load(assetPlugin(pluginInstance, "res/Pleiades.svg")));
            addChild(panel);
        }



        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        

        /* +CONTROL INSTANTIATION */
        #include "Pleiades_panel.hpp"
        /* -CONTROL INSTANTIATION */
//        Vec(163.999995, 204.0), 
        if(module)
        {
            Rect cbox = module->encoders[Pleiades::PARAM_CENTER]->widget->box;


            //Address indicator lights
            float radius = 25-7;
            for (int i = 0; i < DEPTH; ++i)
            {
                radius += 7;
                if(i == 3) radius += 5;
                for (int j =0; j < 7; ++j)
                {   //i = depth,  j = idx
                    //id = depth*7 + index
                    float angle = (j+.5)*2*M_PI/7;
                    light = createLightCentered<SmallLight<GreenLight>>(
                        Vec(-radius*sin(angle)+cbox.pos.x+cbox.size.x/2,
                             radius*cos(angle)+cbox.pos.y+cbox.size.y/2), 
                        module, Pleiades::LIGHT_ADDRESS+i*7+j
                    );
                    if (i > 2)
                    {
                    ((ModuleLightWidget*)light)->baseColors[0] = (nvgRGBAf(
                            1,.5,0,
                            1)); 
                    }
                    addChild(light);
                }

            }

            for(int i = 0; i < 7; ++i)
            {
                float irad = 150;
                float angle = (i+.5)*2*M_PI/7;           
                light = createLightCentered<SmallLight<GreenRedLight>>(
                    Vec(-irad*sin(angle)+cbox.pos.x+cbox.size.x/2,
                         irad*cos(angle)+cbox.pos.y+cbox.size.y/2), 
                    module, Pleiades::LIGHT_PORT+i*2
                );
                addChild(light);        
            }

            for(int i = 0; i < 7; ++i)
            {
                module->encoders[Pleiades::PARAM_MODE+i]->setColor(
                        MODE_COLORS[i][0],
                        MODE_COLORS[i][1],
                        MODE_COLORS[i][2]
                        );
            }

            module->encoders[Pleiades::PARAM_CENTER]->setColor(
                module->encoders[Pleiades::PARAM_MODE+5]);
            for(int i = 0; i < 7; ++i)
            {
                module->encoders[Pleiades::PARAM_STEP+i]->setColor(
                    module->encoders[Pleiades::PARAM_MODE+1]);
            }

            module->updateStepKnobs();

            seq_name = new TextField();
            float w = 75;
    
            seq_name->box.pos = Vec(box.size.x/2-w/2, 5);
            seq_name->box.size = Vec(w, 20);
            addChild(seq_name);

            float dist = 15;

            param = createParam<LEDButton>(
                Vec(box.size.x/2+w/2+dist, 5),
                module,
                Pleiades::PARAM_SAVE, 0,1,0
                );

            center(param,1,0);
            addParam(param);

            light = createLightCentered<SmallLight<RedLight>>(
                Vec(box.size.x/2+w/2+dist, 5),
                module, Pleiades::LIGHT_WRITE_ENABLE
            );
            ((ModuleLightWidget*)light)->baseColors[0] = (nvgRGBAf(
                    1,.5,0,
                    1)); 
            addChild(light);        

             light = createLightCentered<SmallLight<RedLight>>(
                Vec(box.size.x/2-w/2-dist, 5),
                module, Pleiades::LIGHT_WRITE_ENABLE+1
            );
            ((ModuleLightWidget*)light)->baseColors[0] = (nvgRGBAf(
                    1,.5,0,
                    1)); 
            addChild(light);        
            

            param = createParam<LEDButton>(
                Vec(box.size.x/2-w/2-dist, 5),
                module,
                Pleiades::PARAM_LOAD, 0,1,0
                );

            center(param,1,0);
            addParam(param);
 

            module->seq_name = seq_name;

            module->ready = true;
        }
    }


};

#include "Pleiades_instance.hpp"
