
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

    create_strings= {
        'param':"""
    param = ParamWidget::create<{widget}>(
        Vec({xpos}, {ypos}), 
        module, {name}::{id},
        {min}, {max}, {default}
    );
    center(param,1,1);
    addParam(param);
        """,
        'input':"""
    input = Port::create<PJ301MPort>(
        Vec({xpos},{ypos}), Port::INPUT, module, {name}::{id}
        );
    center(input,1,1);
    addInput(input);    
        """,
        'output':"""
    output = Port::create<PJ301MPort>(
        Vec({xpos},{ypos}), Port::OUTPUT, module, {name}::{id}
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

