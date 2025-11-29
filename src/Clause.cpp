#include "plugin.hpp"


struct Clause : Module {
	enum ParamId {
		ENUMS(CV_PARAMS, 12),
		ENUMS(GATE_PARAMS, 12),
		RESET_PARAM,
		RUN_PARAM,
		CLOCK_PARAM,
		LENGTH_PARAM,
		RANGE_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		RESET_INPUT,
		RUN_INPUT,
		CLOCK_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		ENUMS(GATE_OUTPUTS, 12),
		CLOCK_OUTPUT,
		TRIG_OUTPUT,
		CV_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		ENUMS(STEP_LIGHTS, 12),
		ENUMS(GATE_LIGHTS, 12),
		RUN_LIGHT,
		CLOCK_LIGHT,
		LIGHTS_LEN
	};

	bool running = true;
	bool clockPassthrough = false;

	dsp::BooleanTrigger clockButtonTrigger;
	dsp::BooleanTrigger runButtonTrigger;
	dsp::BooleanTrigger resetButtonTrigger;
	dsp::BooleanTrigger gateTriggers[8];

	dsp::SchmittTrigger clockTrigger;
	dsp::SchmittTrigger runTrigger;
	dsp::SchmittTrigger resetTrigger;

	dsp::PulseGenerator runPulse;
	dsp::PulseGenerator clockPulse;
	dsp::PulseGenerator resetPulse;

	Clause() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		for (int i = 0; i < 12; i++) {
			configParam(CV_PARAMS + i, -10.f, 10.f, 0.f, string::f("CV %d step %d", i, i + 1), " V");
		}

		for (int i = 0; i < 12; i++) {
			configButton(GATE_PARAMS + i, string::f("Step %d Trigger", i + 1));
		}

		configButton(RESET_PARAM, "Reset");
		configButton(RUN_PARAM, "Run");
		configParam(CLOCK_PARAM, -2.f, 4.f, 1.f, "Clock Tempo", " bpm", 2.f, 60.f);
		getParamQuantity(CLOCK_PARAM)->randomizeEnabled = false;
		
		configParam(LENGTH_PARAM, 1.f, 12.f, 12.f, "Sequence Length");
		getParamQuantity(LENGTH_PARAM)->randomizeEnabled = false;
		paramQuantities[LENGTH_PARAM]->snapEnabled = true;
		configParam(RANGE_PARAM, 0.f, 1.f, 1.f, "Output Range Scaling", "%", 0, 100);
		
		configInput(RESET_INPUT, "Reset");
		configInput(RUN_INPUT, "Run");
		configInput(CLOCK_INPUT, "External Clock");

		for (int i = 0; i < 12; i++) {
			configOutput(GATE_OUTPUTS + i, string::f("Step %d", i + 1));
		}
		configOutput(CLOCK_OUTPUT, "Clock");
		configOutput(TRIG_OUTPUT, "Trigger");
		configOutput(CV_OUTPUT, "Sequence");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct ClauseWidget : ModuleWidget {
	ClauseWidget(Clause* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Clause.svg")));

		addParam(createParamCentered<BefacoTinyKnob>(Vec(89.051, 47.150), module, Clause::CV_PARAMS + 0));
		addParam(createParamCentered<BefacoTinyKnob>(Vec(135.718, 47.150), module, Clause::CV_PARAMS + 1));
		addParam(createParamCentered<BefacoTinyKnob>(Vec(182.384, 47.150), module, Clause::CV_PARAMS + 2));
		addParam(createParamCentered<BefacoTinyKnob>(Vec(229.051, 47.150), module, Clause::CV_PARAMS + 3));
		addParam(createParamCentered<BefacoTinyKnob>(Vec(89.051, 144.150), module, Clause::CV_PARAMS + 4));
		addParam(createParamCentered<BefacoTinyKnob>(Vec(135.718, 144.150), module, Clause::CV_PARAMS + 5));
		addParam(createParamCentered<BefacoTinyKnob>(Vec(182.384, 144.150), module, Clause::CV_PARAMS + 6));
		addParam(createParamCentered<BefacoTinyKnob>(Vec(229.051, 144.150), module, Clause::CV_PARAMS + 7));
		addParam(createParamCentered<BefacoTinyKnob>(Vec(89.051, 241.150), module, Clause::CV_PARAMS + 8));
		addParam(createParamCentered<BefacoTinyKnob>(Vec(135.718, 241.150), module, Clause::CV_PARAMS + 9));
		addParam(createParamCentered<BefacoTinyKnob>(Vec(182.384, 241.150), module, Clause::CV_PARAMS + 10));
		addParam(createParamCentered<BefacoTinyKnob>(Vec(229.051, 241.150), module, Clause::CV_PARAMS + 11));

		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<WhiteLight>>>(Vec(89.412, 74.768), module, Clause::GATE_PARAMS + 0, Clause::GATE_LIGHTS + 0));
		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<WhiteLight>>>(Vec(136.079, 74.768), module, Clause::GATE_PARAMS + 1, Clause::GATE_LIGHTS + 1));
		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<WhiteLight>>>(Vec(182.746, 74.768), module, Clause::GATE_PARAMS + 2, Clause::GATE_LIGHTS + 2));
		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<WhiteLight>>>(Vec(229.412, 74.768), module, Clause::GATE_PARAMS + 3, Clause::GATE_LIGHTS + 3));
		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<WhiteLight>>>(Vec(89.412, 171.768), module, Clause::GATE_PARAMS + 4, Clause::GATE_LIGHTS + 4));
		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<WhiteLight>>>(Vec(136.079, 171.768), module, Clause::GATE_PARAMS + 5, Clause::GATE_LIGHTS + 5));
		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<WhiteLight>>>(Vec(182.746, 171.768), module, Clause::GATE_PARAMS + 6, Clause::GATE_LIGHTS + 6));
		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<WhiteLight>>>(Vec(229.412, 171.768), module, Clause::GATE_PARAMS + 7, Clause::GATE_LIGHTS + 7));
		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<WhiteLight>>>(Vec(89.412, 268.768), module, Clause::GATE_PARAMS + 8, Clause::GATE_LIGHTS + 8));
		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<WhiteLight>>>(Vec(136.079, 268.768), module, Clause::GATE_PARAMS + 9, Clause::GATE_LIGHTS + 9));
		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<WhiteLight>>>(Vec(182.746, 268.768), module, Clause::GATE_PARAMS + 10, Clause::GATE_LIGHTS + 10));
		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<WhiteLight>>>(Vec(229.412, 268.768), module, Clause::GATE_PARAMS + 11, Clause::GATE_LIGHTS + 11));

		addParam(createParamCentered<VCVButton>(Vec(22.278, 70.009), module, Clause::RESET_PARAM));
		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<WhiteLight>>>(Vec(22.278, 128.009), module, Clause::RUN_PARAM, Clause::RUN_LIGHT));
		addParam(createParamCentered<BefacoTinyKnob>(Vec(34.0345, 173.680), module, Clause::CLOCK_PARAM));
		
		addParam(createParamCentered<Trimpot>(Vec(34.272, 281.175), module, Clause::LENGTH_PARAM));
		addParam(createParamCentered<Trimpot>(Vec(34.272, 338.177), module, Clause::RANGE_PARAM));
		
		addInput(createInputCentered<PJ301MPort>(Vec(43.754, 53.889), module, Clause::RESET_INPUT));
		addInput(createInputCentered<PJ301MPort>(Vec(43.754, 111.889), module, Clause::RUN_INPUT));
		addInput(createInputCentered<PJ301MPort>(Vec(34.575, 212.734), module, Clause::CLOCK_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(Vec(34.0345, 245.802), module, Clause::CLOCK_OUTPUT));

		addOutput(createOutputCentered<PJ301MPort>(Vec(89.412, 101.406), module, Clause::GATE_OUTPUTS + 0));
		addOutput(createOutputCentered<PJ301MPort>(Vec(136.079, 101.406), module, Clause::GATE_OUTPUTS + 1));
		addOutput(createOutputCentered<PJ301MPort>(Vec(182.746, 101.406), module, Clause::GATE_OUTPUTS + 2));
		addOutput(createOutputCentered<PJ301MPort>(Vec(229.412, 101.406), module, Clause::GATE_OUTPUTS + 3));
		addOutput(createOutputCentered<PJ301MPort>(Vec(89.412, 198.406), module, Clause::GATE_OUTPUTS + 4));
		addOutput(createOutputCentered<PJ301MPort>(Vec(136.079, 198.406), module, Clause::GATE_OUTPUTS + 5));
		addOutput(createOutputCentered<PJ301MPort>(Vec(182.746, 198.406), module, Clause::GATE_OUTPUTS + 6));
		addOutput(createOutputCentered<PJ301MPort>(Vec(229.412, 198.406), module, Clause::GATE_OUTPUTS + 7));
		addOutput(createOutputCentered<PJ301MPort>(Vec(89.412, 295.406), module, Clause::GATE_OUTPUTS + 8));
		addOutput(createOutputCentered<PJ301MPort>(Vec(136.079, 295.406), module, Clause::GATE_OUTPUTS + 9));
		addOutput(createOutputCentered<PJ301MPort>(Vec(182.746, 295.406), module, Clause::GATE_OUTPUTS + 10));
		addOutput(createOutputCentered<PJ301MPort>(Vec(229.412, 295.406), module, Clause::GATE_OUTPUTS + 11));

		addOutput(createOutputCentered<PJ301MPort>(Vec(177.878, 336.579), module, Clause::TRIG_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(Vec(244.434, 336.579), module, Clause::CV_OUTPUT));

		addChild(createLightCentered<TinyLight<WhiteLight>>(Vec(52.930, 173.680), module, Clause::CLOCK_LIGHT));

		addChild(createLightCentered<TinyLight<WhiteLight>>(Vec(108.228, 48.717), module, Clause::STEP_LIGHTS + 0));
		addChild(createLightCentered<TinyLight<WhiteLight>>(Vec(154.895, 48.717), module, Clause::STEP_LIGHTS + 1));
		addChild(createLightCentered<TinyLight<WhiteLight>>(Vec(201.562, 48.717), module, Clause::STEP_LIGHTS + 2));
		addChild(createLightCentered<TinyLight<WhiteLight>>(Vec(248.228, 48.717), module, Clause::STEP_LIGHTS + 3));
		addChild(createLightCentered<TinyLight<WhiteLight>>(Vec(108.228, 145.717), module, Clause::STEP_LIGHTS + 4));
		addChild(createLightCentered<TinyLight<WhiteLight>>(Vec(154.895, 145.717), module, Clause::STEP_LIGHTS + 5));
		addChild(createLightCentered<TinyLight<WhiteLight>>(Vec(201.562, 145.717), module, Clause::STEP_LIGHTS + 6));
		addChild(createLightCentered<TinyLight<WhiteLight>>(Vec(248.228, 145.717), module, Clause::STEP_LIGHTS + 7));
		addChild(createLightCentered<TinyLight<WhiteLight>>(Vec(108.228, 242.717), module, Clause::STEP_LIGHTS + 8));
		addChild(createLightCentered<TinyLight<WhiteLight>>(Vec(154.895, 242.717), module, Clause::STEP_LIGHTS + 9));
		addChild(createLightCentered<TinyLight<WhiteLight>>(Vec(201.562, 242.717), module, Clause::STEP_LIGHTS + 10));
		addChild(createLightCentered<TinyLight<WhiteLight>>(Vec(248.228, 242.717), module, Clause::STEP_LIGHTS + 11));
	}
};


Model* modelClause = createModel<Clause, ClauseWidget>("Clause");