#include "plugin.hpp"

static const int maxPolyphony = engine::PORT_MAX_CHANNELS;


struct ThreeFO : Module {
	enum ParamId {
		RATE_PARAM,
		RATEATTEN_PARAM,
		SCALE_PARAM,
		OFFSET_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		RATECV_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		ONE_OUTPUT,
		TWO_OUTPUT,
		THREE_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		TWO_LIGHT,
		ONE_LIGHT,
		THREE_LIGHT,
		LIGHTS_LEN
	};

	float phaseAccumulators[maxPolyphony] = {};
    float phaseAdvance[maxPolyphony] = {};
    int currentPolyphony = 1;
    int loopCounter = 0;
	bool outputOne = false;
	bool outputTwo = false;
	bool outputThree = false;


	ThreeFO() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(RATE_PARAM, -8.f, 10.f, 1.f, "Rate", " Hz", 2, 0.125f);
		configParam(RATEATTEN_PARAM, -1.f, 1.f, 0.f, "Rate CV Attenuverter", "%", 0, 100);
		configParam(SCALE_PARAM, 0.f, 1.f, 1.f, "Scale", "%", 0, 100);
		configParam(OFFSET_PARAM, -5.f, 5.f, 0.f, "Offset", " V");
		configInput(RATECV_INPUT, "Rate CV");
		configOutput(ONE_OUTPUT, "Output 1");
		configOutput(TWO_OUTPUT, "Output 2");
		configOutput(THREE_OUTPUT, "Output 3");
	}

	void process(const ProcessArgs& args) override {
		if (loopCounter-- == 0) {
			loopCounter = 3;
			processEvery4Samples(args);
		}

		generateOutput();
	}

	void processEvery4Samples(const ProcessArgs& args) {
		currentPolyphony = std::max(1, inputs[RATECV_INPUT].getChannels());

		outputs[ONE_OUTPUT].setChannels(currentPolyphony);
		outputs[TWO_OUTPUT].setChannels(currentPolyphony);
		outputs[THREE_OUTPUT].setChannels(currentPolyphony);

		outputOne = outputs[ONE_OUTPUT].isConnected();
		outputTwo = outputs[TWO_OUTPUT].isConnected();
		outputThree = outputs[THREE_OUTPUT].isConnected();

		float rateParam = params[RATE_PARAM].value;
		for (int i = 0; i < currentPolyphony; i++) {
			float rateCV = inputs[RATECV_INPUT].getVoltage(i) * params[RATEATTEN_PARAM].getValue();
			float combinedRate = rateParam + rateCV - 4.f;

			const float q = float(std::log2(2)); // 2Hz?
			combinedRate += q;

			const float rate = std::pow(2.f, combinedRate);

			const float normalizedRate = args.sampleTime * rate;
			phaseAdvance[i] = normalizedRate;
		}
	}

	void generateOutput() {
		float offset = params[OFFSET_PARAM].getValue();
		float scale = params[SCALE_PARAM].getValue();
		for (int i = 0; i < currentPolyphony; i++) {
			phaseAccumulators[i] += phaseAdvance[i];
			if (phaseAccumulators[i] > 1.f) {
				phaseAccumulators[i] -= 1.f;
			}
			if (outputOne) {
				float radianPhase = phaseAccumulators[i] * 2 * float(M_PI);
				float sinWaveOne = std::sin(radianPhase) * 5 * scale + offset;
				outputs[ONE_OUTPUT].setVoltage(sinWaveOne, i);
			}
			if (outputTwo) {
				float radianPhase = phaseAccumulators[i] * 2 * float(M_PI);
				float sinWaveTwo = std::sin(radianPhase - 120) * 5 * scale + offset;
				outputs[TWO_OUTPUT].setVoltage(sinWaveTwo, i);
			}
			if (outputThree) {
				float radianPhase = phaseAccumulators[i] * 2 * float(M_PI);
				float sinWaveThree = std::sin(radianPhase - 240) * 5 * scale + offset;
				outputs[THREE_OUTPUT].setVoltage(sinWaveThree, i);
			}
		}
	}
};


struct ThreeFOWidget : ModuleWidget {
	ThreeFOWidget(ThreeFO* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/ThreeFO.svg")));

		addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<Davies1900hWhiteKnob>(mm2px(Vec(15.24, 25.89)), module, ThreeFO::RATE_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(15.24, 40.917)), module, ThreeFO::RATEATTEN_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(6.996, 66.144)), module, ThreeFO::SCALE_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(23.523, 66.016)), module, ThreeFO::OFFSET_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(15.24, 52.274)), module, ThreeFO::RATECV_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(23.222, 83.755)), module, ThreeFO::ONE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(23.222, 96.72)), module, ThreeFO::TWO_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(23.222, 109.685)), module, ThreeFO::THREE_OUTPUT));
	}
};


Model* modelThreeFO = createModel<ThreeFO, ThreeFOWidget>("ThreeFO");
