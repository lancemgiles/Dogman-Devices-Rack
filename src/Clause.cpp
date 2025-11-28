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

		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(23.88, 14.366)), module, Clause::CV_PARAMS + 0));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(38.326, 14.366)), module, Clause::CV_PARAMS + 1));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(52.383, 14.366)), module, Clause::CV_PARAMS + 2));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(23.88, 38.579)), module, Clause::CV_PARAMS + 3));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(38.326, 38.579)), module, Clause::CV_PARAMS + 4));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(52.383, 38.579)), module, Clause::CV_PARAMS + 5));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(23.88, 63.286)), module, Clause::CV_PARAMS + 6));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(38.326, 63.286)), module, Clause::CV_PARAMS + 7));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(52.383, 63.286)), module, Clause::CV_PARAMS + 8));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(23.88, 88.359)), module, Clause::CV_PARAMS + 9));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(38.326, 88.359)), module, Clause::CV_PARAMS + 10));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(52.383, 88.359)), module, Clause::CV_PARAMS + 11));

		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(23.88, 22.125)), module, Clause::GATE_PARAMS + 0, Clause::GATE_LIGHTS + 0));
		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(38.326, 22.125)), module, Clause::GATE_PARAMS + 1, Clause::GATE_LIGHTS + 1));
		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(52.383, 22.125)), module, Clause::GATE_PARAMS + 2, Clause::GATE_LIGHTS + 2));
		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(23.88, 46.996)), module, Clause::GATE_PARAMS + 3, Clause::GATE_LIGHTS + 3));
		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(38.326, 46.996)), module, Clause::GATE_PARAMS + 4, Clause::GATE_LIGHTS + 4));
		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(52.383, 46.996)), module, Clause::GATE_PARAMS + 5, Clause::GATE_LIGHTS + 5));
		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(23.88, 70.937)), module, Clause::GATE_PARAMS + 6, Clause::GATE_LIGHTS + 6));
		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(38.326, 70.937)), module, Clause::GATE_PARAMS + 7, Clause::GATE_LIGHTS + 7));
		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(52.383, 70.937)), module, Clause::GATE_PARAMS + 8, Clause::GATE_LIGHTS + 8));
		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(23.88, 96.563)), module, Clause::GATE_PARAMS + 9, Clause::GATE_LIGHTS + 9));
		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(38.326, 96.563)), module, Clause::GATE_PARAMS + 10, Clause::GATE_LIGHTS + 10));
		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(52.383, 96.563)), module, Clause::GATE_PARAMS + 11, Clause::GATE_LIGHTS + 11));

		addParam(createParamCentered<VCVButton>(mm2px(Vec(6.14, 23.806)), module, Clause::RESET_PARAM));
		addParam(createLightParamCentered<VCVLightButton<MediumSimpleLight<WhiteLight>>>(mm2px(Vec(6.14, 42.856)), module, Clause::RUN_PARAM, Clause::RUN_LIGHT));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(9.069, 58.731)), module, Clause::CLOCK_PARAM));
		
		addParam(createParamCentered<Trimpot>(mm2px(Vec(9.107, 93.611)), module, Clause::LENGTH_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(8.862, 112.833)), module, Clause::RANGE_PARAM));
		
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(11.432, 17.538)), module, Clause::RESET_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(11.432, 37.117)), module, Clause::RUN_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.069, 72.035)), module, Clause::CLOCK_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(8.917, 80.845)), module, Clause::CLOCK_OUTPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(23.88, 29.729)), module, Clause::GATE_OUTPUTS + 0));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(38.326, 29.729)), module, Clause::GATE_OUTPUTS + 1));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(52.383, 29.729)), module, Clause::GATE_OUTPUTS + 2));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(23.88, 54.6)), module, Clause::GATE_OUTPUTS + 3));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(38.326, 54.6)), module, Clause::GATE_OUTPUTS + 4));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(52.383, 54.6)), module, Clause::GATE_OUTPUTS + 5));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(23.88, 78.54)), module, Clause::GATE_OUTPUTS + 6));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(38.326, 78.54)), module, Clause::GATE_OUTPUTS + 7));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(52.383, 78.54)), module, Clause::GATE_OUTPUTS + 8));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(23.88, 104.166)), module, Clause::GATE_OUTPUTS + 9));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(38.326, 104.166)), module, Clause::GATE_OUTPUTS + 10));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(52.383, 104.166)), module, Clause::GATE_OUTPUTS + 11));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(35.495, 113.774)), module, Clause::TRIG_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(54.156, 113.774)), module, Clause::CV_OUTPUT));

		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(13.828, 62.405)), module, Clause::CLOCK_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(43.418, 12.053)), module, Clause::STEP_LIGHTS + 0));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(28.57, 12.234)), module, Clause::STEP_LIGHTS + 1));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(56.616, 12.053)), module, Clause::STEP_LIGHTS + 2));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(28.57, 35.559)), module, Clause::STEP_LIGHTS + 3));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(43.418, 35.559)), module, Clause::STEP_LIGHTS + 4));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(56.616, 35.559)), module, Clause::STEP_LIGHTS + 5));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(28.57, 60.972)), module, Clause::STEP_LIGHTS + 6));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(43.418, 60.972)), module, Clause::STEP_LIGHTS + 7));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(56.616, 60.972)), module, Clause::STEP_LIGHTS + 8));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(28.57, 86.091)), module, Clause::STEP_LIGHTS + 9));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(43.418, 86.091)), module, Clause::STEP_LIGHTS + 10));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(56.616, 86.091)), module, Clause::STEP_LIGHTS + 11));
	}
};


Model* modelClause = createModel<Clause, ClauseWidget>("Clause");