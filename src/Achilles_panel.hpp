
    ParamWidget* param;
    PortWidget* input;
    PortWidget* output;
    LightWidget* light;
    

    input = createInputCentered<PJ301MPort>(
        Vec(22.522725,100.000006), module, Achilles::INPUT_ENV
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(22.522725,220.000005), module, Achilles::INPUT_ENV+3
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(22.522725,140.000005), module, Achilles::INPUT_ENV+1
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(22.5000005,180.000005), module, Achilles::INPUT_ENV+2
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(22.499999999999996,340.000005), module, Achilles::INPUT_MATCH
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(102.50568000000001,340.000005), module, Achilles::INPUT_VOCT
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(142.5,260.000005), module, Achilles::INPUT_EXT_ENV
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(182.49999499999998,259.999995), module, Achilles::INPUT_NOISE_INPUT
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(222.499995,260.000005), module, Achilles::INPUT_NOISE_LEVEL
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(142.50568,340.000005), module, Achilles::INPUT_FM
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(182.5,339.999995), module, Achilles::INPUT_FEEDBACK
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(62.50000000000001,340.00001499999996), module, Achilles::INPUT_GATE
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(302.500005,340.000005), module, Achilles::INPUT_LP_LEVEL
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(342.500005,340.000005), module, Achilles::INPUT_BP_LEVEL
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(382.500005,340.000005), module, Achilles::INPUT_HP_LEVEL
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(382.499995,260.000005), module, Achilles::INPUT_LP_SHAPE
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(382.500005,219.999995), module, Achilles::INPUT_LP_BW
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(382.499985,180.00001500000002), module, Achilles::INPUT_BP_SHAPE
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(382.500005,140.00000500000002), module, Achilles::INPUT_BP_BW
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(382.500005,99.999998), module, Achilles::INPUT_HP_SHAPE
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(382.500005,60.000007), module, Achilles::INPUT_HP_BW
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(102.5,220.0), module, Achilles::INPUT_AUX_NOISE
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(262.499995,220.0), module, Achilles::INPUT_AUX_NOISE+1
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(182.49999499999998,179.999995), module, Achilles::INPUT_NOISE_MIX
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(102.5,59.999995), module, Achilles::INPUT_NOISE_SHAPE
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(102.5,99.99999499999998), module, Achilles::INPUT_NOISE_BW
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(262.50000500000004,59.99999699999999), module, Achilles::INPUT_NOISE_SHAPE+1
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(262.499995,99.99999600000001), module, Achilles::INPUT_NOISE_BW+1
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(262.499995,139.999995), module, Achilles::INPUT_NOISE_SLOPE+1
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(102.5,139.999995), module, Achilles::INPUT_NOISE_SLOPE
        );
    addInput(input);    
        

    input = createInputCentered<PJ301MPort>(
        Vec(222.5,299.999995), module, Achilles::INPUT_DELAY_CLOCK
        );
    addInput(input);    
        

    output = createOutputCentered<PJ301MPort>(
        Vec(262.499995,300.0), module, Achilles::OUTPUT_DELAY_OUT
        );
    addOutput(output);    
        

    output = createOutputCentered<PJ301MPort>(
        Vec(62.499995000000006,300.0), module, Achilles::OUTPUT_ENV_GATE
        );
    addOutput(output);    
        

    output = createOutputCentered<PJ301MPort>(
        Vec(102.49999500000001,260.0), module, Achilles::OUTPUT_ENV
        );
    addOutput(output);    
        

    output = createOutputCentered<PJ301MPort>(
        Vec(142.499995,220.0), module, Achilles::OUTPUT_NOISE
        );
    addOutput(output);    
        

    output = createOutputCentered<PJ301MPort>(
        Vec(222.499995,220.0), module, Achilles::OUTPUT_NOISE+1
        );
    addOutput(output);    
        

    output = createOutputCentered<PJ301MPort>(
        Vec(182.50000500000002,220.0), module, Achilles::OUTPUT_NOISE_OUT
        );
    addOutput(output);    
        

    output = createOutputCentered<PJ301MPort>(
        Vec(182.5,300.0), module, Achilles::OUTPUT_DELAY_IN
        );
    addOutput(output);    
        

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(22.522723, 60.000001999999995), 
        module, Achilles::PARAM_ENV_ATV
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(62.49999749999999, 60.000001999999995), 
        module, Achilles::PARAM_ENV_OFFSET
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(62.50000000000001, 100.000001), 
        module, Achilles::PARAM_ENV_OFFSET+1
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(62.5, 140.0), 
        module, Achilles::PARAM_ENV_ATV+1
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(62.5, 180.0), 
        module, Achilles::PARAM_ENV_ATV+2
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(62.5, 220.0), 
        module, Achilles::PARAM_ENV_OFFSET+2
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(62.5, 260.00001), 
        module, Achilles::PARAM_ENV_OFFSET+3
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(22.522724999999998, 260.00001), 
        module, Achilles::PARAM_ENV_ATV+3
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(222.5, 340.0), 
        module, Achilles::PARAM_FEEDBACK_ATV
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(262.5, 340.0), 
        module, Achilles::PARAM_OUTPUT_LEVEL
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(102.50568, 300.0), 
        module, Achilles::PARAM_VOCT_OFFSET
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(142.5, 300.0), 
        module, Achilles::PARAM_FM_ATV
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(262.5, 260.0), 
        module, Achilles::PARAM_NOISE_LEVEL_ATV
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(22.5, 300.0), 
        module, Achilles::PARAM_MATCH_ATV
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(182.499995, 140.0), 
        module, Achilles::PARAM_NOISE_MIX_OFFSET
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(182.505675, 59.99999), 
        module, Achilles::PARAM_NOISE_MIX_WIDTH
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(182.499995, 100.0), 
        module, Achilles::PARAM_NOISE_CURVE_SLOPE
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(302.500005, 300.0), 
        module, Achilles::PARAM_LP_LEVEL_ATV
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(342.500005, 300.0), 
        module, Achilles::PARAM_BP_LEVEL_ATV
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(382.500005, 300.0), 
        module, Achilles::PARAM_HP_LEVEL_ATV
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(342.499995, 260.0), 
        module, Achilles::PARAM_LP_SHAPE_ATV
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(342.500005, 219.99999), 
        module, Achilles::PARAM_LP_BW_ATV
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(302.499995, 260.00001), 
        module, Achilles::PARAM_LP_SHAPE_OFFSET
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(302.499995, 220.0), 
        module, Achilles::PARAM_LP_BW_OFFSET
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(342.499995, 180.00001), 
        module, Achilles::PARAM_BP_SHAPE_ATV
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(342.499995, 140.0), 
        module, Achilles::PARAM_BP_BW_ATV
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(302.499995, 180.00002), 
        module, Achilles::PARAM_BP_SHAPE_OFFSET
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(302.499995, 140.00001), 
        module, Achilles::PARAM_BP_BW_OFFSET
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(342.499995, 100.000011), 
        module, Achilles::PARAM_HP_SHAPE_ATV
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(342.500005, 60.000001999999995), 
        module, Achilles::PARAM_HP_BW_ATV
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(302.499995, 100.000021), 
        module, Achilles::PARAM_HP_SHAPE_OFFSET
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(302.499995, 60.000012), 
        module, Achilles::PARAM_HP_BW_OFFSET
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(142.47727500000002, 59.999992000000006), 
        module, Achilles::PARAM_NOISE_SHAPE_ATV
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(222.499995, 180.0), 
        module, Achilles::PARAM_NOISE_ATV+1
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(142.499995, 180.0), 
        module, Achilles::PARAM_NOISE_ATV
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(222.499995, 59.999992000000006), 
        module, Achilles::PARAM_NOISE_SHAPE_ATV+1
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(222.499995, 99.999991), 
        module, Achilles::PARAM_NOISE_BW_ATV+1
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(142.477275, 99.99999), 
        module, Achilles::PARAM_NOISE_BW_ATV
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(142.477275, 139.99999000000003), 
        module, Achilles::PARAM_NOISE_SLOPE_ATV
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(262.499995, 180.0), 
        module, Achilles::PARAM_AUX_NOISE_MIX+1
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(102.499995, 180.0), 
        module, Achilles::PARAM_AUX_NOISE_MIX
    ));
    

    addParam(createParamCentered<RoundTinyBlackKnob>(
        Vec(222.499995, 139.99999), 
        module, Achilles::PARAM_NOISE_SLOPE_ATV+1
    ));
    