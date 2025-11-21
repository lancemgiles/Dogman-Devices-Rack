#include "plugin.hpp"


struct Clause : Module {
	enum ParamId {
		VOLTAGE1_PARAM,
		VOLTAGE7_PARAM,
		VOLTAGE2_PARAM,
		VOLTAGE8_PARAM,
		RESET_PARAM,
		VOLTAGE3_PARAM,
		VOLTAGE9_PARAM,
		VOLTAGE4_PARAM,
		VOLTAGE10_PARAM,
		LENGTH_PARAM,
		VOLTAGE5_PARAM,
		VOLTAGE11_PARAM,
		VOLTAGE6_PARAM,
		VOLTAGE12_PARAM,
		DIRECTION_PARAM,
		RANGE_PARAM,
		SELECT_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		CLOCK_INPUT,
		RESET_INPUT,
		DIRECTION_INPUT,
		SELECTCV_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		SEQUENCE_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		STEP1_LIGHT,
		STEP7_LIGHT,
		STEP2_LIGHT,
		STEP8_LIGHT,
		STEP3_LIGHT,
		STEP9_LIGHT,
		STEP4_LIGHT,
		STEP10_LIGHT,
		STEP5_LIGHT,
		STEP11_LIGHT,
		STEP6_LIGHT,
		STEP12_LIGHT,
		LIGHTS_LEN
	};

	bool running = true;
	bool clockPassthrough = false;

	dsp::BooleanTrigger clockButtonTrigger;
	dsp::BooleanTrigger runButtonTrigger;
	dsp::BooleanTrigger resetButtonTrigger;
	dsp::BooleanTrigger gateTriggers[12];

	dsp::SchmittTrigger clockTrigger;
	dsp::SchmittTrigger runTrigger;
	dsp::SchmittTrigger resetTrigger;

	dsp::PulseGenerator runPulse;
	dsp::PulseGenerator clockPulse;
	dsp::PulseGenerator resetPulse;

	/** Phase of internal LFO */
	float phase = 0.f;
	int index = 0;
	bool gates[12] = {};

	Clause() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(VOLTAGE1_PARAM, -10.f, 10.f, 0.f, "Voltage", " V");
		configParam(VOLTAGE2_PARAM, -10.f, 10.f, 0.f, "Voltage", " V");
		configParam(VOLTAGE3_PARAM, -10.f, 10.f, 0.f, "Voltage", " V");
		configParam(VOLTAGE4_PARAM, -10.f, 10.f, 0.f, "Voltage", " V");
		configParam(VOLTAGE5_PARAM, -10.f, 10.f, 0.f, "Voltage", " V");
		configParam(VOLTAGE6_PARAM, -10.f, 10.f, 0.f, "Voltage", " V");
		configParam(VOLTAGE7_PARAM, -10.f, 10.f, 0.f, "Voltage", " V");
		configParam(VOLTAGE8_PARAM, -10.f, 10.f, 0.f, "Voltage", " V");
		configParam(VOLTAGE9_PARAM, -10.f, 10.f, 0.f, "Voltage", " V");
		configParam(VOLTAGE10_PARAM, -10.f, 10.f, 0.f, "Voltage", " V");
		configParam(VOLTAGE11_PARAM, -10.f, 10.f, 0.f, "Voltage", " V");
		configParam(VOLTAGE12_PARAM, -10.f, 10.f, 0.f, "Voltage", " V");

		configButton(RESET_PARAM, "Reset");
		configSwitch(DIRECTION_PARAM, 0.f, 1.f, 0.f, "Direction", {"Forward", "Reverse"});

		configParam(LENGTH_PARAM, 1.f, 12.f, 12.f, "Sequence Length");
		configParam(RANGE_PARAM, 0.f, 1.f, 1.f, "Output Range Scaling", "%", 0, 100);
		configParam(SELECT_PARAM, 1.f, 12.f, 1.f, "Select Sequence Step");

		configInput(CLOCK_INPUT, "Clock");
		configInput(RESET_INPUT, "Reset");
		configInput(DIRECTION_INPUT, "Direction");
		configInput(SELECTCV_INPUT, "Select CV");
		configOutput(SEQUENCE_OUTPUT, "Sequence");
	}

	void onReset() override {
		clockPassthrough = false;
		for (int i = 0; i < 12; i++) {
			gates[i] = true;
		}
		index = 0;
	}

	void onRandomize() override {
		for (int i = 0; i < 12; i++) {
			gates[i] = random::get<bool>();
		}
	}

	// void rotateStates(int delta) {
	// 	// Rotate CV params
	// 	for (int j = 0; j < 3; j++) {
	// 		float cvs[12];
	// 		for (int i = 0; i < 12; i++) {
	// 			cvs[i] = params[CV_PARAMS + 12 * j + i].getValue();
	// 		}
	// 		for (int i = 0; i < 12; i++) {
	// 			int index = eucMod(i + delta, 12);
	// 			params[CV_PARAMS + 12 * j + index].setValue(cvs[i]);
	// 		}
	// 	}
	// 	// Rotate gates
	// 	bool gates[12];
	// 	for (int i = 0; i < 12; i++) {
	// 		gates[i] = this->gates[i];
	// 	}
	// 	for (int i = 0; i < 12; i++) {
	// 		int index = eucMod(i + delta, 12);
	// 		this->gates[index] = gates[i];
	// 	}
	// }

	void process(const ProcessArgs& args) override {
		
	}
};


struct ClauseWidget : ModuleWidget {
	ClauseWidget(Clause* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Clause.svg")));

		addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(23.88, 13.837)), module, Clause::VOLTAGE1_PARAM));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(38.035, 13.837)), module, Clause::VOLTAGE7_PARAM));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(23.88, 27.123)), module, Clause::VOLTAGE2_PARAM));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(38.035, 27.123)), module, Clause::VOLTAGE8_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(8.289, 40.37)), module, Clause::RESET_PARAM));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(23.88, 40.409)), module, Clause::VOLTAGE3_PARAM));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(38.035, 40.409)), module, Clause::VOLTAGE9_PARAM));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(23.88, 53.695)), module, Clause::VOLTAGE4_PARAM));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(38.035, 53.695)), module, Clause::VOLTAGE10_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(8.289, 55.331)), module, Clause::LENGTH_PARAM));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(23.88, 66.981)), module, Clause::VOLTAGE5_PARAM));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(38.035, 66.981)), module, Clause::VOLTAGE11_PARAM));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(23.88, 80.267)), module, Clause::VOLTAGE6_PARAM));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(38.035, 80.267)), module, Clause::VOLTAGE12_PARAM));
		addParam(createParamCentered<CKSS>(mm2px(Vec(8.289, 82.313)), module, Clause::DIRECTION_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(38.508, 94.939)), module, Clause::RANGE_PARAM));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(8.289, 99.18)), module, Clause::SELECT_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.289, 13.837)), module, Clause::CLOCK_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.289, 29.339)), module, Clause::RESET_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.289, 71.892)), module, Clause::DIRECTION_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.289, 115.461)), module, Clause::SELECTCV_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(38.035, 110.911)), module, Clause::SEQUENCE_OUTPUT));

		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(23.88, 20.887)), module, Clause::STEP1_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(38.035, 20.887)), module, Clause::STEP7_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(23.88, 33.404)), module, Clause::STEP2_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(38.035, 33.404)), module, Clause::STEP8_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(23.88, 46.634)), module, Clause::STEP3_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(38.035, 46.634)), module, Clause::STEP9_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(23.88, 60.392)), module, Clause::STEP4_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(38.035, 60.392)), module, Clause::STEP10_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(23.88, 73.621)), module, Clause::STEP5_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(38.035, 73.621)), module, Clause::STEP11_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(23.88, 87.379)), module, Clause::STEP6_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(38.035, 87.379)), module, Clause::STEP12_LIGHT));
	}
};


Model* modelClause = createModel<Clause, ClauseWidget>("Clause");