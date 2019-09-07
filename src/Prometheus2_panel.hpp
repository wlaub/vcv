
    ParamWidget* param;
    PortWidget* input;
    PortWidget* output;
    LightWidget* light;
    

    input = createInputCentered<PJ301MPort>(
        Vec(59.99999999999999,278.88), module, Prometheus2::INPUT_LENGTH_CV
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(100.0,278.88), module, Prometheus2::INPUT_PARAM_0_CV
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
        Vec(60.000000000000014,323.32), module, Prometheus2::INPUT_GLITCH_GATE
        );
    addInput(input);    
        

    output = createOutputCentered<PJ301MPort>(
        Vec(100.00000076740172,323.3200049333856), module, Prometheus2::OUTPUT_OUT
        );
    addOutput(output);    
        

    addParam(createParamCentered<CKSS>(
        Vec(80.0, 56.68000000000001), 
        module, Prometheus2::PARAM_GLITCH_BUTTON
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(60.00000000000001, 234.44), 
        module, Prometheus2::PARAM_LENGTH_CV_ATV
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(60.00000000000001, 145.56), 
        module, Prometheus2::PARAM_LENGTH_FINE
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(100.0, 190.0), 
        module, Prometheus2::PARAM_PARAM_0_COARSE
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(60.0, 190.0), 
        module, Prometheus2::PARAM_LENGTH_OFFSET
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(100.0, 234.44), 
        module, Prometheus2::PARAM_PARAM_0_CV_ATV
    ));
    

    addParam(createParamCentered<CKSS>(
        Vec(40.0, 57.790000000000006), 
        module, Prometheus2::PARAM_FREQ_LOCK
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(20.0, 190.0), 
        module, Prometheus2::PARAM_PITCH_COARSE
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(20.0, 145.56), 
        module, Prometheus2::PARAM_PITCH_FINE
    ));
    

    addParam(createParamCentered<CKSS>(
        Vec(40.0, 101.12), 
        module, Prometheus2::PARAM_BIAS_CONTROL
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(100.0, 145.56), 
        module, Prometheus2::PARAM_PARAM_0_FINE
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(80.0, 101.12), 
        module, Prometheus2::PARAM_GLITCH_RATE
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(20.0, 234.44), 
        module, Prometheus2::PARAM_VOCT_ATV
    ));
    