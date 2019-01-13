"""
plugin
    the name of the plugin e.g. Fundamental[.hpp]
modname
    the module class name
modname_full
    the readable module name
tags
    a string representing the module tags

control_instantiation
    a string containing all the control widget instantiations
enums
    a string containing all of the enum definitions
trigger_variables
    a string instantiating schmitt triggers etc generated from svg
input_processing
    a string that handles input/param value acqusition and simple computations
    e.g. attunation/attenuverters/curves/clipping
    Includes the creation of variables for output_processing
output_processing
    a string containing code for automated output processing using variable
    names provided in the svg along with simple computations. Uses variables
    created in input_processing
"""

module = """
#include "{plugin}.hpp"


struct {modname} : Module {{
    /* +ENUMS */
    #include "{modname}_enums.hpp"
    /* -ENUMS */
    
    /* +TRIGGER_VARS */
    #include "{modname}_vars.hpp"
    /* -TRIGGER_VARS */


    {modname}() : Module(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS) {{}}
    void step() override;

    // For more advanced Module features, read Rack's engine.hpp header file
    // - toJson, fromJson: serialization of internal data
    // - onSampleRateChange: event triggered by a change of sample rate
    // - onReset, onRandomize, onCreate, onDelete: implements special behavior when user clicks these from the context menu
}};


void {modname}::step() {{
    float deltaTime = engineGetSampleTime();


    /*  +INPUT_PROCESSING */
    #include "{modname}_inputs.hpp"
    /*  -INPUT_PROCESSING */



    /*  +OUTPUT_PROCESSING */
    #include "{modname}_outputs.hpp"
    /*  -OUTPUT_PROCESSING */



}}


struct {modname}Widget : ModuleWidget {{
    {modname}Widget({modname} *module) : ModuleWidget(module) {{
        setPanel(SVG::load(assetPlugin(plugin, "res/{modname}.svg")));

        addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        /* +CONTROL INSTANTIATION */
        #include "{modname}_panel.hpp"
        /* -CONTROL INSTANTIATION */
    }}
}};


// Specify the Module and ModuleWidget subclass, human-readable
// author name for categorization per plugin, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.
Model *model{modname} = Model::create<{modname}, {modname}Widget>("{plugin}", "{modname}", "{modname_full}", {tags});
"""

