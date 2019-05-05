
#include "TechTechTechnologies.hpp"

#define DEPTH 2

#define DPRINT printf

struct Step
{
    // 0 : slew
    // 1 : primary microtone
    // 2 : primary tone
    // 3 : primary octave
    // 4 : trigger
    // 5 : secondary tone 
    // 6 : secondary octave
    unsigned char values[7]={0,0,0,3,0,0,3};

    float getValue(Step triggerStep, int index, int subindex, float* tones, float prevTone)
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
        tone_values[1] = values[6]+tones[values[5]] -3;
        unsigned char trigger = triggerStep.values[4];
    trigger = values[4];

        unsigned char tlast = trigger+1;
        unsigned char tfirst = 8-index;

        unsigned char triggerIndex = tfirst%tlast;

        DPRINT("    %i%%%i = %i\n", tfirst, tlast, triggerIndex);

        triggerIndex= triggerIndex ==0 ? 0: 1;

        DPRINT("    ");
        for(int i = 0; i < 7; ++i)
        {
            DPRINT("%i ", values[i]);
        }
        DPRINT("\n");
        DPRINT("    Param: %i, %i, %f\n", index, subindex, prevTone);
        DPRINT("    Tones: %f, %f, %i\n", tone_values[0], tone_values[1], triggerIndex);


        if(!isfinite(prevTone)) return tone_values[triggerIndex];

        float alpha = min(1.0,float(subindex)/(values[0]+1));

        DPRINT("    %i/%i -> %f\n", subindex-1, values[0]+1, alpha);

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
//
    unsigned char digits[DEPTH] = {1};

    Address()
    {
        for (int i = 0; i < DEPTH; ++i)
            digits[i] = 1;
    }

    int step()
    {//Increments the step and returns flags showing which indices rolled over
        int result = 0;

        for (int i = 0; i < DEPTH; ++i)
        {
            int revidx = DEPTH-1-i;
            digits[revidx] += 1;
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
        DPRINT("0o%07o\n",get_address(DEPTH+1));
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

};

struct Sequence
{
    struct Address address;
    struct Step steps[(1<<3*DEPTH)];
    
    float prevTone[DEPTH] = {0};
    float prevValue[DEPTH] = {0};

    Sequence()
    {
        for (int i = 0; i < DEPTH; ++i)
        {
            prevTone[i] = 0;
            prevValue[i] = 0;
        }
    }

    int step()
    {
        int rolls = address.step();
        for (int i = 0; i < DEPTH; ++i)
        {
            if (rolls & (1<<(i+1)))
            {
                prevTone[i] = prevValue[i];
                DPRINT("Rolled %i : %f -> %f\n", i, prevValue[i], prevTone[i]);
            }
        } 

        return 0;
    }

    float get_value(Address fromAdd, Sequence triggerSeq)
    {
        float result = 0;

        float tones[7] = {0, 1.0/7, 2.0/7, 3.0/7, 4.0/7, 5.0/7, 6.0/7}; //TODO: Temporary

        fromAdd.print();

        for (int i = -1; i < DEPTH; ++i)
        {
            int index = 1;
            int subindex = 7;
            if(i < DEPTH-1)
                index = address.digits[i+1];
            if(i < DEPTH-2)
                subindex = address.digits[i+2];
            int addIdx = fromAdd.get_address(i+1);
            Step mainStep = steps[addIdx];
            Step triggerStep = triggerSeq.steps[addIdx];

            float prevToneLocal = NAN;
            if (i < DEPTH-1)
                prevToneLocal = prevTone[i+1];

            DPRINT("0o%07o:\n", addIdx);
            float stepVal = mainStep.getValue(
                triggerStep, index, subindex, 
                tones, prevToneLocal
                );
            prevValue[i+1] = stepVal;
            result += stepVal;
            DPRINT("  %i: %f / %f\n", i, stepVal, prevToneLocal);
//    float getValue(Step triggerStep, int index, int subindex, float* tones, float prevTone)
 
        }
        DPRINT("\n");
        return result;
    }

};

struct Pleiades : Module {
    /* +ENUMS */
    #include "Pleiades_enums.hpp"
    /* -ENUMS */
    
    /* +TRIGGER_VARS */
    #include "Pleiades_vars.hpp"
    /* -TRIGGER_VARS */

    struct Sequence sequences[7];
    int seq_idx = 0; //currently selected sequence

    struct Address address;

    int counter = 0;


    Pleiades() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {}
    void step() override;

    // For more advanced Module features, read Rack's engine.hpp header file
    // - toJson, fromJson: serialization of internal data
    // - onSampleRateChange: event triggered by a change of sample rate
    // - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
};


void Pleiades::step() {
    float deltaTime = engineGetSampleTime();


    /*  +INPUT_PROCESSING */
    #include "Pleiades_inputs.hpp"
    /*  -INPUT_PROCESSING */

    ++counter;
    if (counter < 100) return;
    counter = 0;

    int N = 1;

    
    for(int i = 0; i < N; ++i)
    {
        Step* tstep = &(sequences[i].steps[0]);

//        tstep->values[3] = 1;        
        for (int i = 0; i < 7; ++i)
        {
            tstep->values[i] = char(param_mode[i]);
        }

    }
    


    for(int i = 0; i < N; ++ i)
    {
        sequences[i].step();
    }

    for(int i = 0; i < N; ++ i)
    {
        float val =sequences[i].get_value(sequences[i].address, sequences[(i+1)%7]);
        output_out[i] = val;
        DPRINT("=%f\n", val);
    }
    DPRINT("\n");

    /*
    if(counter < 49*7+2)
    {
        ++counter;
        int res= address.step();
        for (int i = 0 ; i < DEPTH; ++i)
        {
            if ((res&(1<<(DEPTH-i-1))) == 0)
            {
                printf(" ");
            }
            else
            {
                printf("V");
            }
        }
        printf("\n", res);
        address.print();
        for (int i = 0; i < DEPTH+1; ++i)
        {
            printf("%08x\n", address.get_address(i));
        }
    }   
    */
    /*  +OUTPUT_PROCESSING */
    #include "Pleiades_outputs.hpp"
    /*  -OUTPUT_PROCESSING */



}


struct PleiadesWidget : ModuleWidget {
    PleiadesWidget(Pleiades *module) : ModuleWidget(module) {
        box.size = Vec(22.0 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

        {
            SVGPanel *panel = new SVGPanel();
            panel->box.size = box.size;
            panel->setBackground(SVG::load(assetPlugin(plugin, "res/Pleiades.svg")));
            addChild(panel);
        }



        addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        /* +CONTROL INSTANTIATION */
        #include "Pleiades_panel.hpp"
        /* -CONTROL INSTANTIATION */
    }
};

#include "Pleiades_instance.hpp"

