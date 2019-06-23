
    ParamWidget* param;
    Port* input;
    Port* output;
    LightWidget* light;
    

    input = createPort<PJ301MPort>(
        Vec(69.000005,68.999995), PortWidget::INPUT, module, QMod::INPUT_Q_IN
        );
    center(input,1,1);
    addInput(input);    
        

    input = createPort<PJ301MPort>(
        Vec(25.000003000000007,69.0), PortWidget::INPUT, module, QMod::INPUT_I_IN
        );
    center(input,1,1);
    addInput(input);    
        

    input = createPort<PJ301MPort>(
        Vec(113.0,69.0), PortWidget::INPUT, module, QMod::INPUT_FREQ
        );
    center(input,1,1);
    addInput(input);    
        

    input = createPort<PJ301MPort>(
        Vec(69.0,113.0), PortWidget::INPUT, module, QMod::INPUT_MOD_IN
        );
    center(input,1,1);
    addInput(input);    
        

    input = createPort<PJ301MPort>(
        Vec(113.0,245.0), PortWidget::INPUT, module, QMod::INPUT_FREQ+1
        );
    center(input,1,1);
    addInput(input);    
        

    input = createPort<PJ301MPort>(
        Vec(69.000001,289.0), PortWidget::INPUT, module, QMod::INPUT_MOD_IN+1
        );
    center(input,1,1);
    addInput(input);    
        

    input = createPort<PJ301MPort>(
        Vec(245.00000000000006,68.99999950000162), PortWidget::INPUT, module, QMod::INPUT_Q_IN+2
        );
    center(input,1,1);
    addInput(input);    
        

    input = createPort<PJ301MPort>(
        Vec(201.0,69.0), PortWidget::INPUT, module, QMod::INPUT_I_IN+2
        );
    center(input,1,1);
    addInput(input);    
        

    input = createPort<PJ301MPort>(
        Vec(157.0,69.0), PortWidget::INPUT, module, QMod::INPUT_FREQ+2
        );
    center(input,1,1);
    addInput(input);    
        

    input = createPort<PJ301MPort>(
        Vec(201.0,113.0), PortWidget::INPUT, module, QMod::INPUT_MOD_IN+2
        );
    center(input,1,1);
    addInput(input);    
        

    input = createPort<PJ301MPort>(
        Vec(157.0,245.0), PortWidget::INPUT, module, QMod::INPUT_FREQ+3
        );
    center(input,1,1);
    addInput(input);    
        

    input = createPort<PJ301MPort>(
        Vec(201.0,289.0), PortWidget::INPUT, module, QMod::INPUT_MOD_IN+3
        );
    center(input,1,1);
    addInput(input);    
        

    input = createPort<PJ301MPort>(
        Vec(113.0,201.0), PortWidget::INPUT, module, QMod::INPUT_AUX_I
        );
    center(input,1,1);
    addInput(input);    
        

    input = createPort<PJ301MPort>(
        Vec(157.0,201.0), PortWidget::INPUT, module, QMod::INPUT_AUX_Q
        );
    center(input,1,1);
    addInput(input);    
        

    input = createPort<PJ301MPort>(
        Vec(245.00000000000003,332.9999950000016), PortWidget::INPUT, module, QMod::INPUT_Q_IN+3
        );
    center(input,1,1);
    addInput(input);    
        

    input = createPort<PJ301MPort>(
        Vec(201.0,333.0), PortWidget::INPUT, module, QMod::INPUT_I_IN+3
        );
    center(input,1,1);
    addInput(input);    
        

    input = createPort<PJ301MPort>(
        Vec(68.999999,332.99999499999933), PortWidget::INPUT, module, QMod::INPUT_Q_IN+1
        );
    center(input,1,1);
    addInput(input);    
        

    input = createPort<PJ301MPort>(
        Vec(25.0,333.0), PortWidget::INPUT, module, QMod::INPUT_I_IN+1
        );
    center(input,1,1);
    addInput(input);    
        

    output = createPort<PJ301MPort>(
        Vec(25.000001267401725,113.00000493338682), PortWidget::OUTPUT, module, QMod::OUTPUT_MOD_OUT
        );
    center(output,1,1);
    addOutput(output);    
        

    output = createPort<PJ301MPort>(
        Vec(25.000005267401725,289.0000049333919), PortWidget::OUTPUT, module, QMod::OUTPUT_MOD_OUT+1
        );
    center(output,1,1);
    addOutput(output);    
        

    output = createPort<PJ301MPort>(
        Vec(245.00000326740172,113.00000493339186), PortWidget::OUTPUT, module, QMod::OUTPUT_MOD_OUT+2
        );
    center(output,1,1);
    addOutput(output);    
        

    output = createPort<PJ301MPort>(
        Vec(245.00000326740172,289.0000049333919), PortWidget::OUTPUT, module, QMod::OUTPUT_MOD_OUT+3
        );
    center(output,1,1);
    addOutput(output);    
        

    output = createPort<PJ301MPort>(
        Vec(69.00000000000001,157.0), PortWidget::OUTPUT, module, QMod::OUTPUT_Q_OUT
        );
    center(output,1,1);
    addOutput(output);    
        

    output = createPort<PJ301MPort>(
        Vec(24.999997999999998,157.0), PortWidget::OUTPUT, module, QMod::OUTPUT_I_OUT
        );
    center(output,1,1);
    addOutput(output);    
        

    output = createPort<PJ301MPort>(
        Vec(245.0,157.0), PortWidget::OUTPUT, module, QMod::OUTPUT_Q_OUT+2
        );
    center(output,1,1);
    addOutput(output);    
        

    output = createPort<PJ301MPort>(
        Vec(201.0,157.0), PortWidget::OUTPUT, module, QMod::OUTPUT_I_OUT+2
        );
    center(output,1,1);
    addOutput(output);    
        

    output = createPort<PJ301MPort>(
        Vec(245.0,245.0), PortWidget::OUTPUT, module, QMod::OUTPUT_Q_OUT+3
        );
    center(output,1,1);
    addOutput(output);    
        

    output = createPort<PJ301MPort>(
        Vec(201.0,245.0), PortWidget::OUTPUT, module, QMod::OUTPUT_I_OUT+3
        );
    center(output,1,1);
    addOutput(output);    
        

    output = createPort<PJ301MPort>(
        Vec(68.999999,245.0), PortWidget::OUTPUT, module, QMod::OUTPUT_Q_OUT+1
        );
    center(output,1,1);
    addOutput(output);    
        

    output = createPort<PJ301MPort>(
        Vec(25.0,245.0), PortWidget::OUTPUT, module, QMod::OUTPUT_I_OUT+1
        );
    center(output,1,1);
    addOutput(output);    
        

    param = createParam<RoundTinyBlackKnob>(
        Vec(25.0, 201.0), 
        module, QMod::PARAM_IN_GAIN,
        -1, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<RoundTinyBlackKnob>(
        Vec(113.0, 157.0), 
        module, QMod::PARAM_FREQ_OFFSET,
        -3, 3, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<RoundTinyBlackKnob>(
        Vec(113.0, 113.0), 
        module, QMod::PARAM_FREQ_ATV,
        -1, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<RoundTinyBlackKnob>(
        Vec(113.0, 333.0), 
        module, QMod::PARAM_FREQ_OFFSET+1,
        -3, 3, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<RoundTinyBlackKnob>(
        Vec(113.0, 289.0), 
        module, QMod::PARAM_FREQ_ATV+1,
        -1, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<RoundTinyBlackKnob>(
        Vec(157.0, 157.0), 
        module, QMod::PARAM_FREQ_OFFSET+2,
        -3, 3, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<RoundTinyBlackKnob>(
        Vec(157.0, 113.0), 
        module, QMod::PARAM_FREQ_ATV+2,
        -1, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<RoundTinyBlackKnob>(
        Vec(157.0, 333.0), 
        module, QMod::PARAM_FREQ_OFFSET+3,
        -3, 3, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<RoundTinyBlackKnob>(
        Vec(157.0, 289.0), 
        module, QMod::PARAM_FREQ_ATV+3,
        -1, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<RoundTinyBlackKnob>(
        Vec(69.0, 201.0), 
        module, QMod::PARAM_IN_GAIN+1,
        -1, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<RoundTinyBlackKnob>(
        Vec(245.0, 201.0), 
        module, QMod::PARAM_IN_GAIN+2,
        -1, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<RoundTinyBlackKnob>(
        Vec(201.0, 201.0), 
        module, QMod::PARAM_IN_GAIN+3,
        -1, 1, 0
    );
    center(param,1,1);
    addParam(param);
        