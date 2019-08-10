
class MetadataConfig():
    pass

#
# Control instantiation strings
#

class ControlConfig():


    kind_map = { #relates layer names to control kinds
        'param':'p', 
        'input':'i', 
        'output':'o',
        'params':'p', 
        'inputs':'i', 
        'outputs':'o',
        'lights': 'l',
        'light':'l',
        }
    enum_kind_map = { #relates kind to enum category
        'o':'output',
        'i':'input',
        'p':'param',
        'l':'light',
        }


    enums_template = """
    enum ParamIds {{
{param}
        NUM_PARAMS
    }};
    enum InputIds {{
{input}
        NUM_INPUTS
    }};
    enum OutputIds {{
{output}
        NUM_OUTPUTS
    }};
    enum LightIds {{
{light}
        NUM_LIGHTS
    }};
    """

    create_enum = 'ENUMS({name},{count})'

    create_declarations ="""
    ParamWidget* param;
    PortWidget* input;
    PortWidget* output;
    LightWidget* light;
    """

    paramconfig = 'configParam({id}, {min}, {max}, {default}, "{id}");'

    create_strings= {
        'param':"""
    addParam(createParamCentered<{widget}>(
        Vec({xpos}, {ypos}), 
        module, {name}::{id}
    ));
    """,
        'input':"""
    input = createPort<PJ301MPort>(
        Vec({xpos},{ypos}), PortWidget::INPUT, module, {name}::{id}
        );
    center(input,1,1);
    addInput(input);    
        """,
        'output':"""
    output = createPort<PJ301MPort>(
        Vec({xpos},{ypos}), PortWidget::OUTPUT, module, {name}::{id}
        );
    center(output,1,1);
    addOutput(output);    
        """,
       'light':"""
    light = ModuleLightWidget::create<{widget}>(
        Vec({xpos},{ypos}), module, {name}::{id}       
    );
    center(light,1,1):
    addChild(light);
        """,
    }

