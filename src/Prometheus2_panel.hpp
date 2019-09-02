
    ParamWidget* param;
    PortWidget* input;
    PortWidget* output;
    LightWidget* light;
    

    input = createInputCentered<PJ301MPort>(
        Vec(59.99999999999999,234.44), module, Prometheus2::INPUT_LENGTH_CV
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(100.0,234.44), module, Prometheus2::INPUT_PARAM_1_CV
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(20.0,234.44), module, Prometheus2::INPUT_PARAM_0_CV
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(20.000000250000003,278.8799950000016), module, Prometheus2::INPUT_VOCT
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(20.0,323.32000000000005), module, Prometheus2::INPUT_EXT_CLK
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(100.0,101.12), module, Prometheus2::INPUT_GLITCH_GATE
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(20.0,101.12), module, Prometheus2::INPUT_FREQ_LOCK_GATE
        );
    addInput(input);    
        

    output = createOutputCentered<PJ301MPort>(
        Vec(100.00000176740171,323.3200049333881), module, Prometheus2::OUTPUT_OUT
        );
    addOutput(output);    
        

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(100.0, 190.0), 
        module, Prometheus2::PARAM_PARAM_1_CV_ATV
    ));
    

    addParam(createParamCentered<CKSS>(
        Vec(100.0, 56.68), 
        module, Prometheus2::PARAM_GLITCH_BUTTON
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(60.00000000000001, 190.0), 
        module, Prometheus2::PARAM_LENGTH_CV_ATV
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(100.0, 145.56), 
        module, Prometheus2::PARAM_PARAM_1_COARSE
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(60.00000000000001, 101.12), 
        module, Prometheus2::PARAM_LENGTH_FINE
    ));
    

    addParam(createParamCentered<CKSS>(
        Vec(20.0, 56.68), 
        module, Prometheus2::PARAM_PARAM_ORDER
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(20.0, 145.56), 
        module, Prometheus2::PARAM_PARAM_0_COARSE
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(60.0, 145.56), 
        module, Prometheus2::PARAM_LENGTH_OFFSET
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(20.0, 190.0), 
        module, Prometheus2::PARAM_PARAM_0_CV_ATV
    ));
    

    addParam(createParamCentered<CKSS>(
        Vec(60.0, 56.68000000000001), 
        module, Prometheus2::PARAM_FREQ_LOCK
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(60.0, 278.88), 
        module, Prometheus2::PARAM_PITCH_COARSE
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(100.0, 278.88), 
        module, Prometheus2::PARAM_PITCH_FINE
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(60.0, 323.32000000000005), 
        module, Prometheus2::PARAM_BIAS_CONTROL
    ));
    