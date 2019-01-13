
#include "TechTechTechnologies.hpp"


struct Achilles : Module {
    /* +ENUMS */
    #include "Achilles_enums.hpp"
    /* -ENUMS */
    
    /* +TRIGGER_VARS */
    #include "Achilles_vars.hpp"
    /* -TRIGGER_VARS */


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

