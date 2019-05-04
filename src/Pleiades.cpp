
#include "TechTechTechnologies.hpp"


struct Pleiades : Module {
    /* +ENUMS */
    #include "Pleiades_enums.hpp"
    /* -ENUMS */
    
    /* +TRIGGER_VARS */
    #include "Pleiades_vars.hpp"
    /* -TRIGGER_VARS */


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

