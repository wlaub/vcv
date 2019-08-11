
    ParamWidget* param;
    PortWidget* input;
    PortWidget* output;
    LightWidget* light;
    

    input = createInputCentered<PJ301MPort>(
        Vec(240.362745,45.429099), module, Pleiades::INPUT_CLOCK
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(87.6372525,45.429097), module, Pleiades::INPUT_ANALOG
        );
    addInput(input);    
        

    output = createOutputCentered<PJ301MPort>(
        Vec(163.999995,72.0), module, Pleiades::OUTPUT_OUT+3
        );
    addOutput(output);    
        

    output = createOutputCentered<PJ301MPort>(
        Vec(60.36812268943471,122.24161106769014), module, Pleiades::OUTPUT_OUT+2
        );
    addOutput(output);    
        

    output = createOutputCentered<PJ301MPort>(
        Vec(35.30944304481176,233.3724479082614), module, Pleiades::OUTPUT_OUT+1
        );
    addOutput(output);    
        

    output = createOutputCentered<PJ301MPort>(
        Vec(106.72794023571953,322.92817473240893), module, Pleiades::OUTPUT_OUT
        );
    addOutput(output);    
        

    output = createOutputCentered<PJ301MPort>(
        Vec(267.6318836444233,122.2416232683049), module, Pleiades::OUTPUT_OUT+4
        );
    addOutput(output);    
        

    output = createOutputCentered<PJ301MPort>(
        Vec(292.69055682760586,233.37245373895388), module, Pleiades::OUTPUT_OUT+5
        );
    addOutput(output);    
        

    output = createOutputCentered<PJ301MPort>(
        Vec(221.27206326740173,322.9281749333919), module, Pleiades::OUTPUT_OUT+6
        );
    addOutput(output);    
        

    output = createOutputCentered<PJ301MPort>(
        Vec(302.1758432674017,313.0111349333919), module, Pleiades::OUTPUT_OUT+7
        );
    addOutput(output);    
        

    addParam(createParamCentered<TTTEncoder>(
        Vec(163.999995, 204.0), 
        module, Pleiades::PARAM_CENTER
    ));
    

    addParam(createParamCentered<TTTEncoder>(
        Vec(163.999995, 116.0), 
        module, Pleiades::PARAM_STEP+3
    ));
    

    addParam(createParamCentered<TTTEncoder>(
        Vec(233.08792499999998, 149.49441000000002), 
        module, Pleiades::PARAM_STEP+4
    ));
    

    addParam(createParamCentered<TTTEncoder>(
        Vec(249.793705, 223.58164), 
        module, Pleiades::PARAM_STEP+5
    ));
    

    addParam(createParamCentered<TTTEncoder>(
        Vec(202.181375, 283.28545), 
        module, Pleiades::PARAM_STEP+6
    ));
    

    addParam(createParamCentered<TTTEncoder>(
        Vec(125.818625, 283.28545), 
        module, Pleiades::PARAM_STEP
    ));
    

    addParam(createParamCentered<TTTEncoder>(
        Vec(78.20629600000001, 223.58163000000002), 
        module, Pleiades::PARAM_STEP+1
    ));
    

    addParam(createParamCentered<TTTEncoder>(
        Vec(94.912082, 149.49440000000004), 
        module, Pleiades::PARAM_STEP+2
    ));
    

    addParam(createParamCentered<ITTTEncoder>(
        Vec(60.369806, 285.75405), 
        module, Pleiades::PARAM_MODE+2
    ));
    

    addParam(createParamCentered<TTTEncoder>(
        Vec(25.824171000000003, 313.01119), 
        module, Pleiades::PARAM_CONFIG
    ));
    

    addParam(createParamCentered<ITTTEncoder>(
        Vec(267.63358500000004, 285.75403), 
        module, Pleiades::PARAM_MODE+4
    ));
    

    addParam(createParamCentered<ITTTEncoder>(
        Vec(164.00169499999998, 335.99563), 
        module, Pleiades::PARAM_MODE+3
    ));
    

    addParam(createParamCentered<ITTTEncoder>(
        Vec(292.692285, 174.62319000000002), 
        module, Pleiades::PARAM_MODE+5
    ));
    

    addParam(createParamCentered<ITTTEncoder>(
        Vec(35.31114600000001, 174.62318), 
        module, Pleiades::PARAM_MODE+1
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(283.261275, 55.215543), 
        module, Pleiades::PARAM_CONFIG+2
    ));
    

    addParam(createParamCentered<ITTTEncoder>(
        Vec(221.273775, 85.06745699999999), 
        module, Pleiades::PARAM_MODE+6
    ));
    

    addParam(createParamCentered<ITTTEncoder>(
        Vec(106.729635, 85.0674405), 
        module, Pleiades::PARAM_MODE
    ));
    

    addParam(createParamCentered<ITTTEncoder>(
        Vec(44.7421, 55.21555600000001), 
        module, Pleiades::PARAM_CONFIG+1
    ));
    