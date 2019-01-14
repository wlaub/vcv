
    ParamWidget* param;
    Port* input;
    Port* output;
    LightWidget* light;
    

    input = Port::create<PJ301MPort>(
        Vec(22.522725,100.000006), Port::INPUT, module, Achilles::INPUT_ENV
        );
    center(input,1,1);
    addInput(input);    
        

    input = Port::create<PJ301MPort>(
        Vec(22.522725,220.000005), Port::INPUT, module, Achilles::INPUT_ENV+3
        );
    center(input,1,1);
    addInput(input);    
        

    input = Port::create<PJ301MPort>(
        Vec(22.522725,140.000005), Port::INPUT, module, Achilles::INPUT_ENV+1
        );
    center(input,1,1);
    addInput(input);    
        

    input = Port::create<PJ301MPort>(
        Vec(22.5000005,180.000005), Port::INPUT, module, Achilles::INPUT_ENV+2
        );
    center(input,1,1);
    addInput(input);    
        

    input = Port::create<PJ301MPort>(
        Vec(22.499999999999996,340.000005), Port::INPUT, module, Achilles::INPUT_MATCH
        );
    center(input,1,1);
    addInput(input);    
        

    input = Port::create<PJ301MPort>(
        Vec(102.50568000000001,340.000005), Port::INPUT, module, Achilles::INPUT_VOCT
        );
    center(input,1,1);
    addInput(input);    
        

    input = Port::create<PJ301MPort>(
        Vec(142.5,260.000005), Port::INPUT, module, Achilles::INPUT_EXT_ENV
        );
    center(input,1,1);
    addInput(input);    
        

    input = Port::create<PJ301MPort>(
        Vec(182.49999499999998,259.999995), Port::INPUT, module, Achilles::INPUT_NOISE_INPUT
        );
    center(input,1,1);
    addInput(input);    
        

    input = Port::create<PJ301MPort>(
        Vec(222.499995,260.000005), Port::INPUT, module, Achilles::INPUT_NOISE_LEVEL
        );
    center(input,1,1);
    addInput(input);    
        

    input = Port::create<PJ301MPort>(
        Vec(142.50568,340.000005), Port::INPUT, module, Achilles::INPUT_FM
        );
    center(input,1,1);
    addInput(input);    
        

    input = Port::create<PJ301MPort>(
        Vec(182.5,339.999995), Port::INPUT, module, Achilles::INPUT_FEEDBACK
        );
    center(input,1,1);
    addInput(input);    
        

    input = Port::create<PJ301MPort>(
        Vec(62.50000000000001,340.00001499999996), Port::INPUT, module, Achilles::INPUT_GATE
        );
    center(input,1,1);
    addInput(input);    
        

    input = Port::create<PJ301MPort>(
        Vec(302.500005,340.000005), Port::INPUT, module, Achilles::INPUT_LP_LEVEL
        );
    center(input,1,1);
    addInput(input);    
        

    input = Port::create<PJ301MPort>(
        Vec(342.500005,340.000005), Port::INPUT, module, Achilles::INPUT_BP_LEVEL
        );
    center(input,1,1);
    addInput(input);    
        

    input = Port::create<PJ301MPort>(
        Vec(382.500005,340.000005), Port::INPUT, module, Achilles::INPUT_HP_LEVEL
        );
    center(input,1,1);
    addInput(input);    
        

    input = Port::create<PJ301MPort>(
        Vec(382.499995,260.000005), Port::INPUT, module, Achilles::INPUT_LP_SHAPE
        );
    center(input,1,1);
    addInput(input);    
        

    input = Port::create<PJ301MPort>(
        Vec(382.500005,219.999995), Port::INPUT, module, Achilles::INPUT_LP_BW
        );
    center(input,1,1);
    addInput(input);    
        

    input = Port::create<PJ301MPort>(
        Vec(382.499985,180.00001500000002), Port::INPUT, module, Achilles::INPUT_BP_SHAPE
        );
    center(input,1,1);
    addInput(input);    
        

    input = Port::create<PJ301MPort>(
        Vec(382.500005,140.00000500000002), Port::INPUT, module, Achilles::INPUT_BP_BW
        );
    center(input,1,1);
    addInput(input);    
        

    input = Port::create<PJ301MPort>(
        Vec(382.500005,99.999998), Port::INPUT, module, Achilles::INPUT_HP_SHAPE
        );
    center(input,1,1);
    addInput(input);    
        

    input = Port::create<PJ301MPort>(
        Vec(382.500005,60.000007), Port::INPUT, module, Achilles::INPUT_HP_BW
        );
    center(input,1,1);
    addInput(input);    
        

    input = Port::create<PJ301MPort>(
        Vec(102.5,220.0), Port::INPUT, module, Achilles::INPUT_AUX_NOISE
        );
    center(input,1,1);
    addInput(input);    
        

    input = Port::create<PJ301MPort>(
        Vec(262.499995,220.0), Port::INPUT, module, Achilles::INPUT_AUX_NOISE+1
        );
    center(input,1,1);
    addInput(input);    
        

    input = Port::create<PJ301MPort>(
        Vec(182.49999499999998,179.999995), Port::INPUT, module, Achilles::INPUT_NOISE_MIX
        );
    center(input,1,1);
    addInput(input);    
        

    input = Port::create<PJ301MPort>(
        Vec(102.5,59.999995), Port::INPUT, module, Achilles::INPUT_NOISE_SHAPE
        );
    center(input,1,1);
    addInput(input);    
        

    input = Port::create<PJ301MPort>(
        Vec(102.5,99.99999499999998), Port::INPUT, module, Achilles::INPUT_NOISE_BW
        );
    center(input,1,1);
    addInput(input);    
        

    input = Port::create<PJ301MPort>(
        Vec(262.50000500000004,59.99999699999999), Port::INPUT, module, Achilles::INPUT_NOISE_SHAPE+1
        );
    center(input,1,1);
    addInput(input);    
        

    input = Port::create<PJ301MPort>(
        Vec(262.499995,99.99999600000001), Port::INPUT, module, Achilles::INPUT_NOISE_BW+1
        );
    center(input,1,1);
    addInput(input);    
        

    input = Port::create<PJ301MPort>(
        Vec(262.499995,139.999995), Port::INPUT, module, Achilles::INPUT_NOISE_SLOPE+1
        );
    center(input,1,1);
    addInput(input);    
        

    input = Port::create<PJ301MPort>(
        Vec(102.5,139.999995), Port::INPUT, module, Achilles::INPUT_NOISE_SLOPE
        );
    center(input,1,1);
    addInput(input);    
        

    input = Port::create<PJ301MPort>(
        Vec(222.5,299.999995), Port::INPUT, module, Achilles::INPUT_DELAY_CLOCK
        );
    center(input,1,1);
    addInput(input);    
        

    output = Port::create<PJ301MPort>(
        Vec(262.499995,300.0), Port::OUTPUT, module, Achilles::OUTPUT_DELAY_OUT
        );
    center(output,1,1);
    addOutput(output);    
        

    output = Port::create<PJ301MPort>(
        Vec(62.499995000000006,300.0), Port::OUTPUT, module, Achilles::OUTPUT_ENV_GATE
        );
    center(output,1,1);
    addOutput(output);    
        

    output = Port::create<PJ301MPort>(
        Vec(102.49999500000001,260.0), Port::OUTPUT, module, Achilles::OUTPUT_ENV
        );
    center(output,1,1);
    addOutput(output);    
        

    output = Port::create<PJ301MPort>(
        Vec(142.499995,220.0), Port::OUTPUT, module, Achilles::OUTPUT_NOISE
        );
    center(output,1,1);
    addOutput(output);    
        

    output = Port::create<PJ301MPort>(
        Vec(222.499995,220.0), Port::OUTPUT, module, Achilles::OUTPUT_NOISE+1
        );
    center(output,1,1);
    addOutput(output);    
        

    output = Port::create<PJ301MPort>(
        Vec(182.50000500000002,220.0), Port::OUTPUT, module, Achilles::OUTPUT_NOISE_OUT
        );
    center(output,1,1);
    addOutput(output);    
        

    output = Port::create<PJ301MPort>(
        Vec(182.5,300.0), Port::OUTPUT, module, Achilles::OUTPUT_DELAY_IN
        );
    center(output,1,1);
    addOutput(output);    
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(22.522723, 60.000001999999995), 
        module, Achilles::PARAM_ENV_ATV,
        -1, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(62.49999749999999, 60.000001999999995), 
        module, Achilles::PARAM_ENV_OFFSET,
        0, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(62.50000000000001, 100.000001), 
        module, Achilles::PARAM_ENV_OFFSET+1,
        0, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(62.5, 140.0), 
        module, Achilles::PARAM_ENV_ATV+1,
        -1, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(62.5, 180.0), 
        module, Achilles::PARAM_ENV_ATV+2,
        -1, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(62.5, 220.0), 
        module, Achilles::PARAM_ENV_OFFSET+2,
        0, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(62.5, 260.00001), 
        module, Achilles::PARAM_ENV_OFFSET+3,
        0, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(22.522724999999998, 260.00001), 
        module, Achilles::PARAM_ENV_ATV+3,
        -1, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(222.5, 340.0), 
        module, Achilles::PARAM_FEEDBACK_ATV,
        -1, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(262.5, 340.0), 
        module, Achilles::PARAM_OUTPUT_LEVEL,
        0, 1, 0.5
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(102.50568, 300.0), 
        module, Achilles::PARAM_VOCT_OFFSET,
        -1, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(142.5, 300.0), 
        module, Achilles::PARAM_FM_ATV,
        0, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(262.5, 260.0), 
        module, Achilles::PARAM_NOISE_LEVEL_ATV,
        -1, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(22.5, 300.0), 
        module, Achilles::PARAM_MATCH_ATV,
        -1, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(182.499995, 140.0), 
        module, Achilles::PARAM_NOISE_MIX_OFFSET,
        -1, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(182.505675, 59.99999), 
        module, Achilles::PARAM_NOISE_MIX_WIDTH,
        0, 1, 0.5
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(182.499995, 100.0), 
        module, Achilles::PARAM_NOISE_CURVE_SLOPE,
        0, 1, 1
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(302.500005, 300.0), 
        module, Achilles::PARAM_LP_LEVEL_ATV,
        0, 1, 1
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(342.500005, 300.0), 
        module, Achilles::PARAM_BP_LEVEL_ATV,
        0, 1, 1
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(382.500005, 300.0), 
        module, Achilles::PARAM_HP_LEVEL_ATV,
        0, 1, 1
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(342.499995, 260.0), 
        module, Achilles::PARAM_LP_SHAPE_ATV,
        -1, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(342.500005, 219.99999), 
        module, Achilles::PARAM_LP_BW_ATV,
        -1, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(302.499995, 260.00001), 
        module, Achilles::PARAM_LP_SHAPE_OFFSET,
        -1, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(302.499995, 220.0), 
        module, Achilles::PARAM_LP_BW_OFFSET,
        0, 1, 0.5
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(342.499995, 180.00001), 
        module, Achilles::PARAM_BP_SHAPE_ATV,
        -1, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(342.499995, 140.0), 
        module, Achilles::PARAM_BP_BW_ATV,
        1, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(302.499995, 180.00002), 
        module, Achilles::PARAM_BP_SHAPE_OFFSET,
        -1, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(302.499995, 140.00001), 
        module, Achilles::PARAM_BP_BW_OFFSET,
        0, 1, 0.5
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(342.499995, 100.000011), 
        module, Achilles::PARAM_HP_SHAPE_ATV,
        -1, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(342.500005, 60.000001999999995), 
        module, Achilles::PARAM_HP_BW_ATV,
        -1, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(302.499995, 100.000021), 
        module, Achilles::PARAM_HP_SHAPE_OFFSET,
        -1, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(302.499995, 60.000012), 
        module, Achilles::PARAM_HP_BW_OFFSET,
        0, 1, 0.5
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(142.47727500000002, 59.999992000000006), 
        module, Achilles::PARAM_NOISE_SHAPE_ATV,
        -1, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(222.499995, 180.0), 
        module, Achilles::PARAM_NOISE_ATV+1,
        0, 1, 0.5
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(142.499995, 180.0), 
        module, Achilles::PARAM_NOISE_ATV,
        0, 1, 0.5
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(222.499995, 59.999992000000006), 
        module, Achilles::PARAM_NOISE_SHAPE_ATV+1,
        -1, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(222.499995, 99.999991), 
        module, Achilles::PARAM_NOISE_BW_ATV+1,
        0, 1, 0.5
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(142.477275, 99.99999), 
        module, Achilles::PARAM_NOISE_BW_ATV,
        0, 1, 0.5
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(142.477275, 139.99999000000003), 
        module, Achilles::PARAM_NOISE_SLOPE_ATV,
        -1, 1, 1
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(262.499995, 180.0), 
        module, Achilles::PARAM_AUX_NOISE_MIX+1,
        -1, 1, -1
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(102.499995, 180.0), 
        module, Achilles::PARAM_AUX_NOISE_MIX,
        -1, 1, -11
    );
    center(param,1,1);
    addParam(param);
        

    param = ParamWidget::create<RoundTinyBlackKnob>(
        Vec(222.499995, 139.99999), 
        module, Achilles::PARAM_NOISE_SLOPE_ATV+1,
        -1, 1, -1
    );
    center(param,1,1);
    addParam(param);
        