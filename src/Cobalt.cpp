#include "TechTechTechnologies.hpp"


struct CobaltI : Module {
    enum ParamId {
        START_PARAM,
        PW_PARAM,
        LENGTH_PARAM,
        FREQ_PARAM,
        SCALE_PARAM,
        PHASE_PARAM,
        OFFSET_PARAM,
        PARAMS_LEN
    };
    enum InputId {
        VOCT_INPUT,
        RESET_INPUT,
        INPUTS_LEN
    };
    enum OutputId {
        RAMP_OUTPUT,
        SQUARE_OUTPUT,
        SINE_OUTPUT,
        TRIANGLE_OUTPUT,
        OUTPUTS_LEN
    };
    enum LightId {
        LIGHTS_LEN
    };

    CobaltI() {
        config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
        configParam(START_PARAM, 1.f, 7.f, 1.f, "Starting Subharmonic");
        configParam(LENGTH_PARAM, 1.f, 7.f, 1.f, "Sequence Length");
        configParam(PW_PARAM, 0.f, 1.f, 0.5f, "Pulse Width");
        configParam(PHASE_PARAM, 0.f, 1.f, 0.25f, "Starting Phase (Cycles)");

        configParam(FREQ_PARAM, 1e-3, 600.f, 10.f, "Combined Waveform Period (s)");
        configParam(SCALE_PARAM, 0.f, 10.f, 5.f, "Scale");
        configParam(OFFSET_PARAM, 0.f, 5.f, 2.5f, "Offset");

        configInput(VOCT_INPUT, "v/oct");
        configInput(RESET_INPUT, "Reset");
        configOutput(RAMP_OUTPUT, "Ramp");
        configOutput(SQUARE_OUTPUT, "Square");
        configOutput(SINE_OUTPUT, "Sine");
        configOutput(TRIANGLE_OUTPUT, "Tiangle");
    }

    void process(const ProcessArgs& args) override {
    }
};


struct CobaltIWidget : ModuleWidget {
    CobaltIWidget(CobaltI* module) {
        setModule(module);
        setPanel(createPanel(asset::plugin(pluginInstance, "res/CobaltI.svg")));

        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(createParamCentered<RoundLargeBlackKnob>(mm2px(Vec(38.1, 33.78)), module, CobaltI::FREQ_PARAM));

        addParam(createParamCentered<RoundBlackSnapKnob>(mm2px(Vec(15.24, 18.54)), module, CobaltI::START_PARAM));
        addParam(createParamCentered<RoundBlackSnapKnob>(mm2px(Vec(15.24, 33.78)), module, CobaltI::LENGTH_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(15.24, 49.02)), module, CobaltI::PHASE_PARAM));

        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(60.96, 18.54)), module, CobaltI::PW_PARAM));
        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(60.96, 33.78)), module, CobaltI::SCALE_PARAM));

        addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(60.96, 49.02)), module, CobaltI::OFFSET_PARAM));

        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24, 94.74)), module, CobaltI::VOCT_INPUT));
        addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24, 109.98)), module, CobaltI::RESET_INPUT));

        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(45.72, 94.74)), module, CobaltI::RAMP_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(60.96, 94.74)), module, CobaltI::SQUARE_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(45.72, 109.98)), module, CobaltI::SINE_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(60.96, 109.98)), module, CobaltI::TRIANGLE_OUTPUT));
    }
};


Model* modelCobaltI = createModel<CobaltI, CobaltIWidget>("CobaltI");
