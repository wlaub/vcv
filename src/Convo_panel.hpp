
    ParamWidget* param;
    Port* input;
    Port* output;
    LightWidget* light;
    

    input = createPort<PJ301MPort>(
        Vec(31.000000000000004,68.9999935), PortWidget::INPUT, module, Convo::INPUT_KERNEL_IN
        );
    center(input,1,1);
    addInput(input);    
        

    input = createPort<PJ301MPort>(
        Vec(119.0,68.999995), PortWidget::INPUT, module, Convo::INPUT_CLOCK_IN
        );
    center(input,1,1);
    addInput(input);    
        

    input = createPort<PJ301MPort>(
        Vec(31.0,157.0), PortWidget::INPUT, module, Convo::INPUT_SIGNAL_IN
        );
    center(input,1,1);
    addInput(input);    
        

    input = createPort<PJ301MPort>(
        Vec(118.99999999999999,333.0), PortWidget::INPUT, module, Convo::INPUT_KERNEL_IN+1
        );
    center(input,1,1);
    addInput(input);    
        

    input = createPort<PJ301MPort>(
        Vec(31.0,333.0), PortWidget::INPUT, module, Convo::INPUT_CLOCK_IN+1
        );
    center(input,1,1);
    addInput(input);    
        

    input = createPort<PJ301MPort>(
        Vec(119.0,245.0), PortWidget::INPUT, module, Convo::INPUT_SIGNAL_IN+1
        );
    center(input,1,1);
    addInput(input);    
        

    output = createPort<PJ301MPort>(
        Vec(75.00000026740172,157.00000493337927), PortWidget::OUTPUT, module, Convo::OUTPUT_CONV_OUT
        );
    center(output,1,1);
    addOutput(output);    
        

    output = createPort<PJ301MPort>(
        Vec(75.00000026740172,245.00000493337927), PortWidget::OUTPUT, module, Convo::OUTPUT_CONV_OUT+1
        );
    center(output,1,1);
    addOutput(output);    
        

    param = createParam<RoundTinyBlackKnob>(
        Vec(119.0, 113.0), 
        module, Convo::PARAM_FREQ_OFFSET,
        -3, 3, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<RoundTinyBlackKnob>(
        Vec(75.0, 68.999995), 
        module, Convo::PARAM_KERNEL_GAIN,
        -1, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<RoundTinyBlackKnob>(
        Vec(75.0, 113.0), 
        module, Convo::PARAM_WINDOW_LENGTH,
        0, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<RoundTinyBlackKnob>(
        Vec(30.999999999999996, 113.0), 
        module, Convo::PARAM_WINDOW_ROLLOF,
        0, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<CKSS>(
        Vec(119.0, 157.0), 
        module, Convo::PARAM_RUNST,
        0, 1, 1
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<RoundTinyBlackKnob>(
        Vec(30.999999999999996, 201.0), 
        module, Convo::PARAM_SIGNAL_IN_ATV,
        0, 2, 1
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<RoundTinyBlackKnob>(
        Vec(30.999999999999996, 289.0), 
        module, Convo::PARAM_FREQ_OFFSET+1,
        -3, 3, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<RoundTinyBlackKnob>(
        Vec(75.0, 333.0), 
        module, Convo::PARAM_KERNEL_GAIN+1,
        -1, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<RoundTinyBlackKnob>(
        Vec(75.0, 289.0), 
        module, Convo::PARAM_WINDOW_LENGTH+1,
        0, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<RoundTinyBlackKnob>(
        Vec(119.0, 289.0), 
        module, Convo::PARAM_WINDOW_ROLLOF+1,
        0, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<CKSS>(
        Vec(30.999999999999996, 245.0), 
        module, Convo::PARAM_RUNST+1,
        0, 1, 1
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<RoundTinyBlackKnob>(
        Vec(119.0, 201.0), 
        module, Convo::PARAM_SIGNAL_IN_ATV+1,
        0, 2, 1
    );
    center(param,1,1);
    addParam(param);
        