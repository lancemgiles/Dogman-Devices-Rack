// Borrows heavily from VCV's SEQ3

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

		configParam(CLOCK_PARAM, -2.f, 4.f, 1.f, "Clock Tempo", " bpm", 2.f, 60.f);
		getParamQuantity(CLOCK_PARAM)->randomizeEnabled = false;

		configButton(RUN_PARAM, "Run");
		configButton(RESET_PARAM, "Reset");

		configParam(LENGTH_PARAM, 1.f, 12.f, 12.f, "Sequence Length");
		getParamQuantity(LENGTH_PARAM)->randomizeEnabled = false;
		paramQuantities[LENGTH_PARAM]->snapEnabled = true;

		configParam(RANGE_PARAM, 0.f, 1.f, 1.f, "Output Range Scaling", "%", 0, 100);

		for (int i = 0; i < 12; i++) {
			configParam(CV_PARAMS + i, -10.f, 10.f, 0.f, string::f("CV %d step %d", i, i + 1), " V");
		}

		for (int i = 0; i < 12; i++) {
			configButton(GATE_PARAMS + i, string::f("Step %d Trigger", i + 1));
		}
		
		configInput(RESET_INPUT, "Reset");
		configInput(RUN_INPUT, "Run");
		configInput(CLOCK_INPUT, "External Clock");

		for (int i = 0; i < 12; i++) {
			configOutput(GATE_OUTPUTS + i, string::f("Step %d", i + 1));
		}
		configOutput(CLOCK_OUTPUT, "Clock");
		configOutput(TRIG_OUTPUT, "Trigger");
		configOutput(CV_OUTPUT, "CV");

		onReset();
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

	void process(const ProcessArgs& args) override {
		// Toggle run
		bool runButtonTriggered = runButtonTrigger.process(params[RUN_PARAM].getValue());
		bool runTriggered = runTrigger.process(inputs[RUN_INPUT].getVoltage(), 0.1f, 2.f);
		if (runButtonTriggered || runTriggered) {
			running ^= true;
			runPulse.trigger(1e-3f);
		}
		bool runGate = runPulse.process(args.sampleTime);

		int oldIndex = index;

		// Reset to step 1
		bool resetButtonTriggered = resetButtonTrigger.process(params[RESET_PARAM].getValue());
		bool resetTriggered = resetTrigger.process(inputs[RESET_INPUT].getVoltage(), 0.1f, 2.f);
		if (resetButtonTriggered || resetTriggered) {
			resetPulse.trigger(1e-3f);
			// Reset step index
			index = 0;
			// Reset phase
			phase = 0.f;
		}
		bool resetGate = resetPulse.process(args.sampleTime);

		// Clock
		bool clock = false;
		bool clockGate = false;
		if (running) {
			//bool clockButton = clockButtonTrigger.process(params[CLOCK_PARAM].getValue());
			if (inputs[CLOCK_INPUT].isConnected()) {
				// External clock
				bool clockTriggered = clockTrigger.process(inputs[CLOCK_INPUT].getVoltage(), 0.1f, 2.f);
				// Ignore clock while reset pulse is high
				if (clockTriggered && !resetGate) {
					clock = true;
				}
				// if (clockButton) {
				// 	clock = true;
				//}
				clockGate = clockTrigger.isHigh(); // || clockButtonTrigger.isHigh();
			}
			else {
				// Internal clock
				float clockPitch = params[CLOCK_PARAM].getValue() + inputs[CLOCK_INPUT].getVoltage();
				float clockFreq = dsp::exp2_taylor5(clockPitch);
				phase += clockFreq * args.sampleTime;
				if (phase >= 1.f && !resetGate) {
					clock = true;
					phase -= std::trunc(phase);
				}
				// if (clockButton) {
				// 	clock = true;
				// 	phase = 0.f;
				// }
				clockGate = (phase < 0.5f);
			}
		}

		// Get number of steps
		float steps = params[LENGTH_PARAM].getValue();
		int numSteps = (int) clamp(std::round(steps), 1.f, 12.f);

		// Advance step when clocked
		if (clock) {
			index++;
			if (index >= numSteps)
				index = 0;
		}
		// Trigger pulse if step was changed
		if (index != oldIndex) {
			clockPulse.trigger(1e-3f);
		}

		// Unless we're passing the clock gate, generate a pulse
		if (!clockPassthrough) {
			clockGate = clockPulse.process(args.sampleTime);
		}

		// Gate buttons
		for (int i = 0; i < 12; i++) {
			if (gateTriggers[i].process(params[GATE_PARAMS + i].getValue())) {
				gates[i] ^= true;
			}
			lights[GATE_LIGHTS + i].setBrightness(gates[i]);
		}

		// Step outputs
		for (int i = 0; i < 12; i++) {
			outputs[GATE_OUTPUTS + i].setVoltage((index == i) ? 10.f : 0.f);
			lights[STEP_LIGHTS + i + 0].setSmoothBrightness(index == i, args.sampleTime);
			lights[STEP_LIGHTS + i + 1].setBrightness(i >= numSteps);
		}

		// Outputs
		outputs[CV_OUTPUT].setVoltage(params[CV_PARAMS + 12 * 0 + index].getValue() * params[RANGE_PARAM].getValue());
		outputs[TRIG_OUTPUT].setVoltage((clockGate && gates[index]) ? 10.f : 0.f);

		outputs[CLOCK_OUTPUT].setVoltage(clockGate ? 10.f : 0.f);
	

		lights[CLOCK_LIGHT].setSmoothBrightness(clockGate, args.sampleTime);
		lights[RUN_LIGHT].setBrightness(running);
	}

	json_t* dataToJson() override {
		json_t* rootJ = json_object();

		// running
		json_object_set_new(rootJ, "running", json_boolean(running));

		// gates
		json_t* gatesJ = json_array();
		for (int i = 0; i < 12; i++) {
			json_array_insert_new(gatesJ, i, json_integer((int) gates[i]));
		}
		json_object_set_new(rootJ, "gates", gatesJ);

		// clockPassthrough
		json_object_set_new(rootJ, "clockPassthrough", json_boolean(clockPassthrough));

		return rootJ;
	}

	void dataFromJson(json_t* rootJ) override {
		// running
		json_t* runningJ = json_object_get(rootJ, "running");
		if (runningJ)
			running = json_is_true(runningJ);

		// gates
		json_t* gatesJ = json_object_get(rootJ, "gates");
		if (gatesJ) {
			for (int i = 0; i < 12; i++) {
				json_t* gateJ = json_array_get(gatesJ, i);
				if (gateJ)
					gates[i] = !!json_integer_value(gateJ);
			}
		}

		// clockPassthrough
		json_t* clockPassthroughJ = json_object_get(rootJ, "clockPassthrough");
		if (clockPassthroughJ)
			clockPassthrough = json_is_true(clockPassthroughJ);
		else
			clockPassthrough = true;
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