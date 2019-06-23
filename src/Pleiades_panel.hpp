
    ParamWidget* param;
    PortWidget* input;
    PortWidget* output;
    LightWidget* light;
    

    input = createPort<PJ301MPort>(
        Vec(240.362745,45.429099), PortWidget::INPUT, module, Pleiades::INPUT_CLOCK
        );
    center(input,1,1);
    addInput(input);    
        

    input = createPort<PJ301MPort>(
        Vec(87.6372525,45.429097), PortWidget::INPUT, module, Pleiades::INPUT_ANALOG
        );
    center(input,1,1);
    addInput(input);    
        

    output = createPort<PJ301MPort>(
        Vec(163.999995,72.0), PortWidget::OUTPUT, module, Pleiades::OUTPUT_OUT+3
        );
    center(output,1,1);
    addOutput(output);    
        

    output = createPort<PJ301MPort>(
        Vec(60.36812268943471,122.24161106769014), PortWidget::OUTPUT, module, Pleiades::OUTPUT_OUT+2
        );
    center(output,1,1);
    addOutput(output);    
        

    output = createPort<PJ301MPort>(
        Vec(35.30944304481176,233.3724479082614), PortWidget::OUTPUT, module, Pleiades::OUTPUT_OUT+1
        );
    center(output,1,1);
    addOutput(output);    
        

    output = createPort<PJ301MPort>(
        Vec(106.72794023571953,322.92817473240893), PortWidget::OUTPUT, module, Pleiades::OUTPUT_OUT
        );
    center(output,1,1);
    addOutput(output);    
        

    output = createPort<PJ301MPort>(
        Vec(267.6318836444233,122.2416232683049), PortWidget::OUTPUT, module, Pleiades::OUTPUT_OUT+4
        );
    center(output,1,1);
    addOutput(output);    
        

    output = createPort<PJ301MPort>(
        Vec(292.69055682760586,233.37245373895388), PortWidget::OUTPUT, module, Pleiades::OUTPUT_OUT+5
        );
    center(output,1,1);
    addOutput(output);    
        

    output = createPort<PJ301MPort>(
        Vec(221.27206326740173,322.9281749333919), PortWidget::OUTPUT, module, Pleiades::OUTPUT_OUT+6
        );
    center(output,1,1);
    addOutput(output);    
        

    output = createPort<PJ301MPort>(
        Vec(302.1758432674017,313.0111349333919), PortWidget::OUTPUT, module, Pleiades::OUTPUT_OUT+7
        );
    center(output,1,1);
    addOutput(output);    
        

    param = createParam<TTTEncoder>(
        Vec(163.999995, 204.0), 
        module, Pleiades::PARAM_CENTER,
        0, 7, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<TTTEncoder>(
        Vec(163.999995, 116.0), 
        module, Pleiades::PARAM_STEP+3,
        0, 7, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<TTTEncoder>(
        Vec(233.08792499999998, 149.49441000000002), 
        module, Pleiades::PARAM_STEP+4,
        0, 7, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<TTTEncoder>(
        Vec(249.793705, 223.58164), 
        module, Pleiades::PARAM_STEP+5,
        0, 7, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<TTTEncoder>(
        Vec(202.181375, 283.28545), 
        module, Pleiades::PARAM_STEP+6,
        0, 7, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<TTTEncoder>(
        Vec(125.818625, 283.28545), 
        module, Pleiades::PARAM_STEP,
        0, 7, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<TTTEncoder>(
        Vec(78.20629600000001, 223.58163000000002), 
        module, Pleiades::PARAM_STEP+1,
        0, 7, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<TTTEncoder>(
        Vec(94.912082, 149.49440000000004), 
        module, Pleiades::PARAM_STEP+2,
        0, 7, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<ITTTEncoder>(
        Vec(60.369806, 285.75405), 
        module, Pleiades::PARAM_MODE+2,
        0, 7, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<TTTEncoder>(
        Vec(25.824171000000003, 313.01119), 
        module, Pleiades::PARAM_CONFIG,
        0, 7, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<ITTTEncoder>(
        Vec(267.63358500000004, 285.75403), 
        module, Pleiades::PARAM_MODE+4,
        0, 7, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<ITTTEncoder>(
        Vec(164.00169499999998, 335.99563), 
        module, Pleiades::PARAM_MODE+3,
        0, 7, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<ITTTEncoder>(
        Vec(292.692285, 174.62319000000002), 
        module, Pleiades::PARAM_MODE+5,
        0, 7, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<ITTTEncoder>(
        Vec(35.31114600000001, 174.62318), 
        module, Pleiades::PARAM_MODE+1,
        0, 7, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<RoundTinyBlackKnob>(
        Vec(283.261275, 55.215543), 
        module, Pleiades::PARAM_CONFIG+2,
        -1, 1, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<ITTTEncoder>(
        Vec(221.273775, 85.06745699999999), 
        module, Pleiades::PARAM_MODE+6,
        0, 7, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<ITTTEncoder>(
        Vec(106.729635, 85.0674405), 
        module, Pleiades::PARAM_MODE,
        0, 7, 0
    );
    center(param,1,1);
    addParam(param);
        

    param = createParam<ITTTEncoder>(
        Vec(44.7421, 55.21555600000001), 
        module, Pleiades::PARAM_CONFIG+1,
        0, 7, 0
    );
    center(param,1,1);
    addParam(param);
        
