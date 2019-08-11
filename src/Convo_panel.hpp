
    ParamWidget* param;
    PortWidget* input;
    PortWidget* output;
    LightWidget* light;
    

    input = createInputCentered<PJ301MPort>(
        Vec(31.000000000000004,68.9999935), module, Convo::INPUT_KERNEL_IN
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(119.0,68.999995), module, Convo::INPUT_CLOCK_IN
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(31.0,157.0), module, Convo::INPUT_SIGNAL_IN
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(118.99999999999999,333.0), module, Convo::INPUT_KERNEL_IN+1
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(31.0,333.0), module, Convo::INPUT_CLOCK_IN+1
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(119.0,245.0), module, Convo::INPUT_SIGNAL_IN+1
        );
    addInput(input);    
        

    output = createOutputCentered<PJ301MPort>(
        Vec(75.00000026740172,157.00000493337927), module, Convo::OUTPUT_CONV_OUT
        );
    addOutput(output);    
        

    output = createOutputCentered<PJ301MPort>(
        Vec(75.00000026740172,245.00000493337927), module, Convo::OUTPUT_CONV_OUT+1
        );
    addOutput(output);    
        

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(119.0, 113.0), 
        module, Convo::PARAM_FREQ_OFFSET
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(75.0, 68.999995), 
        module, Convo::PARAM_KERNEL_GAIN
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(75.0, 113.0), 
        module, Convo::PARAM_WINDOW_LENGTH
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(30.999999999999996, 113.0), 
        module, Convo::PARAM_WINDOW_ROLLOF
    ));
    

    addParam(createParamCentered<CKSS>(
        Vec(119.0, 157.0), 
        module, Convo::PARAM_RUNST
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(30.999999999999996, 201.0), 
        module, Convo::PARAM_SIGNAL_IN_ATV
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(30.999999999999996, 289.0), 
        module, Convo::PARAM_FREQ_OFFSET+1
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(75.0, 333.0), 
        module, Convo::PARAM_KERNEL_GAIN+1
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(75.0, 289.0), 
        module, Convo::PARAM_WINDOW_LENGTH+1
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(119.0, 289.0), 
        module, Convo::PARAM_WINDOW_ROLLOF+1
    ));
    

    addParam(createParamCentered<CKSS>(
        Vec(30.999999999999996, 245.0), 
        module, Convo::PARAM_RUNST+1
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(119.0, 201.0), 
        module, Convo::PARAM_SIGNAL_IN_ATV+1
    ));
    