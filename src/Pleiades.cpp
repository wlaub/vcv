
#include "TechTechTechnologies.hpp"

#define DEPTH 6
#define DMAIN true
#define DSEQ false
#define DTEMP false

#define BULK_DISABLE 0
#define BULK_SHIFT 1
#define BULK_ROTATE 2
#define BULK_PASTE 3
#define BULK_COPY 4
#define BULK_CLEAR 5


#define CENTER_STEP_INDEX 1

#define FORMAT_VERSION 3

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

const NVGcolor GC_ORANGE = nvgRGBAf(1, .5, 0, 1);

struct Step
{
    // 0 : slew
    // 1 : primary microtone
    // 2 : primary tone
    // 3 : primary octave
    // 4 : trigger offset
    // 5 : trigger frequency 
    // 6 : trigger level
    unsigned char values[7]={0,0,0,3,0,0,3};
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
        //index is the index of the current substep and is used to provide
        //trigger timing. Triggering within a step only begins when index
        //greater than of equal to trigger_offset.
        //
        //  If trigger_freq == 0, there is no trigger
        //
        //  If trigger_freq > 0, trigger_offset is subtracted from index, and
        //  the result is used to set trigger high when
        //  (index-trigger_offset) % (8-trigger_freq) == 0
        //
        //The trigger pattern for each trigger_freq is then:
        //
        // 0 0 0 0 0 0 0 0
        // 1 1 0 0 0 0 0 0
        // 2 1 0 0 0 0 0 1
        // 3 1 0 0 0 0 1 0
        // 4 1 0 0 0 1 0 0
        // 5 1 0 0 1 0 0 1
        // 6 1 0 1 0 1 0 1
        //!7 1 1 1 1 1 1 1 //Not implemented 
        //
        //trigger_phase rotates this sequence, and if it is negative, reverses
        //the sequence, and counts offset from the end instead of the beginning
        //
        //TODO trigger lengths instead of phase?
        // 0, 1/7, 2/7, 3/7, 1, 2, 3
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

        float tone_value;
        tone_value = values[3]+tones[values[2]]+tones[values[1]]/7.0 - 3;
       
        unsigned char trigger_offset = triggerStep->values[5]+1;
        unsigned char trigger_freq = triggerStep->values[4];
        signed char trigger_phase = triggerStep->values[6] - 3; //TODO:Implement this

        unsigned char triggerIndex = 0;
        if(trigger_freq > 0)
        {
            if(index >= trigger_offset and trigger_phase >= 0)
            {
                if ((
                    (index-trigger_offset-trigger_phase) %
                    (8-trigger_freq)) == 0)
                {
                    tone_value += 5;
                }
            }
            else if(index <= 8-trigger_offset and trigger_phase < 0)
            {
                if ((
                    (
                     (8-index)
                     -(trigger_offset)
                     +(trigger_phase+1)
                     ) 
                    %
                    (8-trigger_freq)) == 0)
                {
                    tone_value += 5;
                }
            }
 
        }


        DPRINT(DSEQ, "    ");
        for(int i = 0; i < 7; ++i)
        {
            DPRINT(DSEQ, "%i ", values[i]);
        }
        DPRINT(DSEQ, "\n");
        DPRINT(DSEQ, "    Param: %i, %i, %f\n", index, subindex, prevTone);
        DPRINT(DSEQ, "    Tones: %f, %i\n", tone_value, triggerIndex);

        if(!std::isfinite(prevTone)) return tone_value;

        float alpha = min(1.0,float(subindex)/(values[0]+1));

        DPRINT(DSEQ, "    %i/%i -> %f\n", subindex-1, values[0]+1, alpha);

        return tone_value*alpha+prevTone*(1-alpha);
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

    int* coefficients[DEPTH] = {0};

    Address()
    {
        if(coefficients[0] == 0)
        {//Generate lookup table of coefficients for computing addresses
//            coefficients = new int*[DEPTH];
            for(int i = 0; i < DEPTH; ++ i)
            {
                coefficients[i] = new int[7];
                for(int j = 0; j < 7; ++ j)
                {
                    coefficients[i][j] = j*(1-pow(7,DEPTH-i))/(1-7);
//                    printf("%i ", coefficients[i][j]);
                }
//                printf("\n");
            }
        }

        for (int i = 0; i < DEPTH; ++i)
            digits[i] = 0;
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

            if(digits[revidx] >= 7) 
            {
                digits[revidx] = 0;
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
        for(int i = 0; i < DEPTH; ++i)
        {
            printf("%i",digits[i]);
        }
        printf(" -> %x\n", get_address(DEPTH));
    }

    int get_address(int depth)
    { //Returns the address of the step at the given depth
        int result = 0;
        for(int i = 0; i < depth; ++i)
        {
            result += coefficients[i][digits[i]]+1;
        }
        return result;
    }

    int get_sub_address(int depth, unsigned char index)
    { //Returns the address of the index'th child of the step at the given
      //depth
        int result = get_address(depth);
        result += coefficients[depth][index]+1;
        return result;
    }

};


struct Sequence
{
    struct Step* steps;
    //struct Step steps[(1<<3*DEPTH)];
    
    float prevTone[DEPTH+1] = {0};
    float prevValue[DEPTH+1] = {0};

//    int length = 1<<(3*DEPTH);
    int length = (1-pow(7,DEPTH+1))/(1-7);

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

/*    
    ~Sequence()
    {
        delete[] steps;
    }
*/

    Sequence* copy()
    {
        Sequence *result = new Sequence();
        for(int i = 0; i < 7; ++ i)
        {
            result->receive(this, i);
        }
        return result;
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

//        fromAdd.print();        

        for (int i = -1; i < DEPTH; ++i)
        {
            int index = 1;
            int subindex = 7;

            if(i < DEPTH-1)
                index = fromAdd.digits[i+1]+1;
            if(i < DEPTH-2)
                subindex = fromAdd.digits[i+2]+1;

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

    void bulk_shift(int edit_index, int new_index, int depth_idx)
    {
        //Swap steps at each index from given depth
    }
    void bulk_rotate(int edit_index, int new_index, int depth_idx)
    {
        //Rotate entire set of steps by difference between indices
    }
    void bulk_clear(int edit_index, int new_index, int depth_idx)
    {
        //Clear step
    }

    //TODO: Copy and paste

    void receive(Sequence *from, int index)
    {
        //Swap values with another sequence for the given index
        //Used to convert sequence formats

        for(int i = 0; i < length; ++i)
        {
            steps[i].values[index] = from->steps[i].values[index];
        }
    }

    void clear(int index)
    {
        Step ref_step;
        for(int i = 0; i < length; ++i)
        {
            steps[i].values[index] = ref_step.values[index];
        }
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

    //Order matters here for some reason
    int seq_idx = 0; //currently selected sequence   
    struct Sequence sequences[7];

    struct Address address;
    int depth_idx = DEPTH>>1;

    int clock_out_depth = 0;

    struct EncoderController* encoders[NUM_PARAMS];
    int encoder_delta[NUM_PARAMS];

    SchmittTrigger saveTrigger;
    SchmittTrigger loadTrigger;

    SchmittTrigger clockTrigger;
    int counter = 0;
    int clockCounter = 0;
    float basePeriod = 49;
    float scalePeriod = 1.0/(49);
    float clockPeriod = 49;

    int outputCounter = 0;

    bool update_steps = false;
    bool ready = false;
    bool write_enable = false;
    char write_name[256];

    float tones[7] = {0, 1.0/7, 2.0/7, 3.0/7, 4.0/7, 5.0/7, 6.0/7};

    int get_complement(int index);

    void updateStepKnobs();
    void updateCenterFromStep();

    void step() override;

    void mode0Callback(unsigned char new_value);

    TextField* seq_name;

    void getFilename(char* into, const char* key, int index);
    json_t *dataToJson() override;
    void dataFromJson(json_t *rootJ) override;
    void loadSequence(const char* filename);

    LightWidget** addressLights;

    Pleiades() 
    {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    }

   

    // For more advanced Module features, read Rack's engine.hpp header file
    // - dataToJson, dataFromJson: serialization of internal data
    // - onSampleRateChange: event triggered by a change of sample rate
    // - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};

void Pleiades::getFilename(char* into, const char* key, int index)
{
    sprintf(into, "PleiadesSeq_%s_%i.dat", key, index);
}

json_t* Pleiades::dataToJson()
{
    json_t *rootJ = json_object();

    char tstr[256];
    char filename[256];

    json_object_set_new(rootJ, "seq_name",
        json_string(write_name)
        );

    json_object_set_new(rootJ, "format_id", json_integer(FORMAT_VERSION));


    for(int i = 0; i < 7; ++i)
    {
        sprintf(tstr, "seq_%i", i);
        getFilename(filename, write_name,i);
        sequences[i].toStr(filename);

        json_object_set_new(rootJ, tstr,
            json_string(filename)
            );
    }

    return rootJ;
}

void Pleiades::loadSequence(const char* key)
{
    char filename[1024];
    for(int i = 0; i < 7; ++i)
    {
        getFilename(filename, key, i);
        sequences[i].fromStr(filename);
    }
}

void Pleiades::dataFromJson(json_t *rootJ)
{
    char tstr[256];
    Module::dataFromJson(rootJ);

    int format_id = json_integer_value(json_object_get(rootJ, "format_id"));

    seq_name->setText(json_string_value(json_object_get(rootJ, "seq_name")));
    sprintf(write_name, seq_name->text.c_str());

    if(format_id != FORMAT_VERSION)
    {
        printf("Warning: Pleiades invalid sequence version.\n");
        seq_name->setText("ERROR");
        sprintf(write_name, seq_name->text.c_str());

    }
    else
    {
        loadSequence(write_name);

        updateStepKnobs();
    }

}




void Pleiades::updateStepKnobs()
{
    for(int i = 0 ; i < 7; ++i)
    {
        int step_index = address.get_sub_address(depth_idx, i);
        printf("step index = %o\n", step_index);
        unsigned char value_index = encoders[PARAM_MODE+1]->getValue();

        unsigned char* step_values = 
            sequences[seq_idx].steps[step_index].values;

        encoders[PARAM_STEP+i]->setValues(step_values);
        DPRINT(DMAIN, "STEP UPDATED %o %i %i\n", 
                step_index, value_index,
                step_values[value_index]
                );
    }

    int step_index = address.get_address(depth_idx);
    unsigned char* step_values = 
        sequences[seq_idx].steps[step_index].values;

    encoders[PARAM_CENTER]->setValues(step_values, 1);


}

int Pleiades::get_complement(int index)
{
    //For the given port index, return the index of the controlling sequence.
    //

    return 6-index;
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
    float sampleRate = engineGetSampleRate();

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
        dataToJson();
    }
    if(saveTrigger.process(params[PARAM_LOAD].value))
    {
        loadSequence(seq_name->text.c_str());
        sprintf(write_name, seq_name->text.c_str());
    }
    //set write enable lights here
    write_enable = (strcmp(seq_name->text.c_str(), write_name)==0);
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
        int depth_delta = encoder_delta[PARAM_MODE+3];
        depth_idx += depth_delta;

        //clip depth
        if(depth_idx < 0) depth_idx = 0;
        else if(depth_idx >= DEPTH) depth_idx=DEPTH-1;

        //Assign selected index to address if not root
        if(depth_idx == 0 && encoders[PARAM_MODE+5]->getValue() == 0)
            encoders[PARAM_CENTER]->setMode(1);
        else
            encoders[PARAM_CENTER]->setMode(0);
            int center_value = encoders[PARAM_CENTER]->getValue(0);
            address.digits[depth_idx-1] = center_value;



        updateStepKnobs();

            for (int i = 0; i < DEPTH; ++i)
            {
                for (int j =0; j < 7; ++j)
                {   //i = depth,  j = idx
                    //id = depth*7 + index
                    if (i > depth_idx)
                    {
                    ((ModuleLightWidget*)addressLights[i*7+j])->baseColors[0] = GC_ORANGE; 
                    }
                    else
                    {
                    ((ModuleLightWidget*)addressLights[i*7+j])->baseColors[0] = SCHEME_GREEN; 
                    }
                }
            }



        DPRINT(DMAIN, "DEPTH INDEX CHANGED %i\n", depth_idx);
    }

    //MODE 4

    if(encoder_delta[PARAM_MODE+4] != 0)
    {
        clock_out_depth=encoders[PARAM_MODE+4]->getValue();
    }



    //MODE 5 (Center knob function)
    if(encoder_delta[PARAM_MODE+5] != 0)
    {
        if(depth_idx == 0 && encoders[PARAM_MODE+5]->getValue() == 0)
        {
            encoders[PARAM_CENTER]->setMode(1);
        }
        else
        {
            encoders[PARAM_CENTER]->setMode(0);
        }

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

    //
    //TODO Internal Clock control
    //TODO Reset input
    //

    //STEP 0-6 (Sequence step configurations)
    for(int i = 0 ; i < 7; ++i)
    {
        if(encoder_delta[PARAM_STEP+i] != 0)
        {
            int step_index = address.get_sub_address(depth_idx, i);
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
            {
                //new address
                if(depth_idx > 0)
                {
                    int edit_index = address.digits[depth_idx-1];
                    int new_index = center_value;
                    address.digits[depth_idx-1] = center_value;
                    //bulk editing effects
                    if(false) //TODO: when edit button active
                    {
                        int bulk_mode = encoders[PARAM_MODE+2]->getValue();
                        for(int i = 0; i < N; ++i)
                        {
                        if(i != seq_idx) continue; //TODO:future home of multisequence edit control
                        switch(bulk_mode)
                        {
                            case BULK_SHIFT:
                                sequences[i].bulk_shift(edit_index, new_index, depth_idx);
                            break;
                            case BULK_ROTATE:
                                sequences[i].bulk_rotate(edit_index, new_index, depth_idx);
                            break;
                            case BULK_COPY:
                            break;
                            case BULK_PASTE:
                            break;
                            case BULK_CLEAR:
                                sequences[i].bulk_clear(edit_index, new_index, depth_idx);
                            break;
                            case BULK_DISABLE:
                            break;
    //                        case BULK_DISABLE:
    //                        break;
                            default:
                                printf("BULK MODE ERROR: %i\n", bulk_mode);
                            break; 
                        }
                        }
                    }
                    //Load new steps into knobs
                    updateStepKnobs();
                }
            }
            break;
            case 1: //Root step control
            
                step_index = address.get_address(depth_idx);
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
//                output_out[7] = 10*(clockCounter-basePeriod)/basePeriod;
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
        clockPeriod = sampleRate*scalePeriod;
    }


    if (counter < clockPeriod)
    {
        ++counter;
    }
    else
    {
        counter = 0;

        //Step sequence
        int rolls = address.step(depth_idx, sync);
        for(int i = 0; i < N; ++ i)
        {
            sequences[i].step(rolls);
        }
    
        int clock_depth_idx = clock_out_depth-1; 
        if(clock_out_depth == 0)//Default to track depth_idx
        {
            clock_depth_idx=depth_idx;
        }
        
        output_out[7] = (address.digits[clock_depth_idx] == 0)?5:0;
        if(rolls & (1<<clock_depth_idx))
        {
            address.print();
        }


        //Update address lights
        int prev = 1;
        for(int i = 0; i < DEPTH; ++i)
        {
            for (int j = 0; j < 7; ++j)
            {
                lights[LIGHT_ADDRESS+i*7+j].value= 
                    ( 
                     (j <= address.digits[i] and j >= prev) or
                     (j >= address.digits[i] and j <= prev)
                      ?.1:0);
                if(j == address.digits[i])
                    lights[LIGHT_ADDRESS+i*7+j].value=1;
           }
           prev =  address.digits[i];
        }


//        printf("%i\n", (unsigned char)(param_mode[4]));
        //output_out[7] = address.digits[encoders[PARAM_MODE+4]->getValue()]-1;

        //Update port lights
        for(int i = 0; i < N; ++i)
        {
            lights[LIGHT_PORT+i*2].value = (i == seq_idx?1:0); //Value target
            lights[LIGHT_PORT+i*2+1].value = 
                (get_complement(i) == seq_idx?1:0); //Trigger target
        }

        //Generate outputs
        for(int i = 0; i < N; ++ i)
        {
            DPRINT(DTEMP, "TSEQ: %i\n", get_complement(i));
            float val = sequences[i].get_value(address, sequences[get_complement(i)], tones);
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
            module->addressLights = new LightWidget*[DEPTH*7];
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
                    if (i > module->depth_idx)
                    {
                    ((ModuleLightWidget*)light)->baseColors[0] = GC_ORANGE; 
                    }
                    module->addressLights[i*7+j] = light;
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
                Vec(box.size.x/2+w/2+dist, 15),
                module,
                Pleiades::PARAM_SAVE, 0,1,0
                );

            center(param,1,1);
            addParam(param);

            light = createLightCentered<SmallLight<RedLight>>(
                Vec(box.size.x/2+w/2+dist, 15),
                module, Pleiades::LIGHT_WRITE_ENABLE
            );
            ((ModuleLightWidget*)light)->baseColors[0] = GC_ORANGE; 
            addChild(light);        

            param = createParam<LEDButton>(
                Vec(box.size.x/2-w/2-dist, 15),
                module,
                Pleiades::PARAM_LOAD, 0,1,0
                );

            center(param,1,1);
            addParam(param);
 
             light = createLightCentered<SmallLight<RedLight>>(
                Vec(box.size.x/2-w/2-dist, 15),
                module, Pleiades::LIGHT_WRITE_ENABLE+1
            );
            ((ModuleLightWidget*)light)->baseColors[0] = GC_ORANGE;
            addChild(light);        
 
            module->seq_name = seq_name;

            module->ready = true;
        }
    }


};

#include "Pleiades_instance.hpp"
