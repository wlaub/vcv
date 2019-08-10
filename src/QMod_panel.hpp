
    ParamWidget* param;
    PortWidget* input;
    PortWidget* output;
    LightWidget* light;
    

    input = createInputCentered<PJ301MPort>(
        Vec(69.000005,68.999995), module, QMod::INPUT_Q_IN
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(25.000003000000007,69.0), module, QMod::INPUT_I_IN
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(113.0,69.0), module, QMod::INPUT_FREQ
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(69.0,113.0), module, QMod::INPUT_MOD_IN
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(113.0,245.0), module, QMod::INPUT_FREQ+1
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(69.000001,289.0), module, QMod::INPUT_MOD_IN+1
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(245.00000000000006,68.99999950000162), module, QMod::INPUT_Q_IN+2
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(201.0,69.0), module, QMod::INPUT_I_IN+2
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(157.0,69.0), module, QMod::INPUT_FREQ+2
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(201.0,113.0), module, QMod::INPUT_MOD_IN+2
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(157.0,245.0), module, QMod::INPUT_FREQ+3
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(201.0,289.0), module, QMod::INPUT_MOD_IN+3
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(113.0,201.0), module, QMod::INPUT_AUX_I
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(157.0,201.0), module, QMod::INPUT_AUX_Q
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(245.00000000000003,332.9999950000016), module, QMod::INPUT_Q_IN+3
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(201.0,333.0), module, QMod::INPUT_I_IN+3
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(68.999999,332.99999499999933), module, QMod::INPUT_Q_IN+1
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(25.0,333.0), module, QMod::INPUT_I_IN+1
        );
    addInput(input);    
        

    output = createOutputCentered<PJ301MPort>(
        Vec(25.000001267401725,113.00000493338682), module, QMod::OUTPUT_MOD_OUT
        );
    addOutput(output);    
        

    output = createOutputCentered<PJ301MPort>(
        Vec(25.000005267401725,289.0000049333919), module, QMod::OUTPUT_MOD_OUT+1
        );
    addOutput(output);    
        

    output = createOutputCentered<PJ301MPort>(
        Vec(245.00000326740172,113.00000493339186), module, QMod::OUTPUT_MOD_OUT+2
        );
    addOutput(output);    
        

    output = createOutputCentered<PJ301MPort>(
        Vec(245.00000326740172,289.0000049333919), module, QMod::OUTPUT_MOD_OUT+3
        );
    addOutput(output);    
        

    output = createOutputCentered<PJ301MPort>(
        Vec(69.00000000000001,157.0), module, QMod::OUTPUT_Q_OUT
        );
    addOutput(output);    
        

    output = createOutputCentered<PJ301MPort>(
        Vec(24.999997999999998,157.0), module, QMod::OUTPUT_I_OUT
        );
    addOutput(output);    
        

    output = createOutputCentered<PJ301MPort>(
        Vec(245.0,157.0), module, QMod::OUTPUT_Q_OUT+2
        );
    addOutput(output);    
        

    output = createOutputCentered<PJ301MPort>(
        Vec(201.0,157.0), module, QMod::OUTPUT_I_OUT+2
        );
    addOutput(output);    
        

    output = createOutputCentered<PJ301MPort>(
        Vec(245.0,245.0), module, QMod::OUTPUT_Q_OUT+3
        );
    addOutput(output);    
        

    output = createOutputCentered<PJ301MPort>(
        Vec(201.0,245.0), module, QMod::OUTPUT_I_OUT+3
        );
    addOutput(output);    
        

    output = createOutputCentered<PJ301MPort>(
        Vec(68.999999,245.0), module, QMod::OUTPUT_Q_OUT+1
        );
    addOutput(output);    
        

    output = createOutputCentered<PJ301MPort>(
        Vec(25.0,245.0), module, QMod::OUTPUT_I_OUT+1
        );
    addOutput(output);    
        

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(25.0, 201.0), 
        module, QMod::PARAM_IN_GAIN
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(113.0, 157.0), 
        module, QMod::PARAM_FREQ_OFFSET
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(113.0, 113.0), 
        module, QMod::PARAM_FREQ_ATV
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(113.0, 333.0), 
        module, QMod::PARAM_FREQ_OFFSET+1
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(113.0, 289.0), 
        module, QMod::PARAM_FREQ_ATV+1
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(157.0, 157.0), 
        module, QMod::PARAM_FREQ_OFFSET+2
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(157.0, 113.0), 
        module, QMod::PARAM_FREQ_ATV+2
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(157.0, 333.0), 
        module, QMod::PARAM_FREQ_OFFSET+3
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(157.0, 289.0), 
        module, QMod::PARAM_FREQ_ATV+3
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(69.0, 201.0), 
        module, QMod::PARAM_IN_GAIN+1
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(245.0, 201.0), 
        module, QMod::PARAM_IN_GAIN+2
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(201.0, 201.0), 
        module, QMod::PARAM_IN_GAIN+3
    ));
    