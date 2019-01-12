#
# Control instantiation strings
#

class ControlConfig():
    create_declarations ="""
    ParamWidget* param;
    InputWidget* input;
    OutputWidget* output;
    LightWidget* light;
    """

    create_strings= {
        'param':"""
    param = ParamWidget.create<{widget}>(
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
    addInput(output);    
        """,
       'light':"""
    light = ModuleLightWidget::create<{widget}>(
        Vec({xpos},{ypos}), module, {name}::{id}       
    );
    center(light,1,1):
    addChild(light);
        """,
    }
