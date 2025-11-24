// Based on a combination of Bogaudio's BOOL and Mutable Instruments' Branches
#include "plugin.hpp"

struct LogicalProbability : Module {
	enum ParamId {
		PROBA_PARAM,
		PROBB_PARAM,
		PROBATTENA_PARAM,
		PROBATTENB_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		PROBCVA_INPUT,
		PROBCVB_INPUT,
		A_INPUT,
		B_INPUT,
		NOT_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		AND_OUTPUT,
		NOT_OUTPUT,
		OR_OUTPUT,
		XOR_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	dsp::BooleanTrigger gateTriggersA[16];
	dsp::BooleanTrigger gateTriggersB[16];
	bool probA = false;
	bool probB = false;


	LogicalProbability() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(PROBA_PARAM, 0.f, 1.f, 1.f, "Probability A", "%", 0, 100);
		configParam(PROBB_PARAM, 0.f, 1.f, 1.f, "Probability B", "%", 0, 100);
		configParam(PROBATTENA_PARAM, -1.f, 1.f, 0.f, "Probability A CV", "%", 0, 100);
		configParam(PROBATTENB_PARAM, -1.f, 1.f, 0.f, "Probability B CV", "%", 0, 100);
		configInput(PROBCVA_INPUT, "Probability A CV");
		configInput(PROBCVB_INPUT, "Probability B CV");
		configInput(A_INPUT, "A");
		configInput(B_INPUT, "B");
		configInput(NOT_INPUT, "NOT");
		configOutput(AND_OUTPUT, "AND");
		configOutput(NOT_OUTPUT, "NOT");
		configOutput(OR_OUTPUT, "OR");
		configOutput(XOR_OUTPUT, "eXclusive OR");
	}

	void process(const ProcessArgs& args) override {
		int channels = std::max({1, inputs[A_INPUT].getChannels(), inputs[B_INPUT].getChannels()});

		for (int c = 0; c < channels; c++) {
			// Probability
			if (gateTriggersA[c].process(inputs[A_INPUT].getPolyVoltage(c))) {
				float rando = random::uniform();
				float probAttenA = params[PROBATTENA_PARAM].getValue();
				float probCVA = probAttenA * inputs[PROBCVA_INPUT].getPolyVoltage(c);
				float probAParam = params[PROBA_PARAM].getValue();
				float thresA = probAParam + probCVA;
				probA = rando < thresA;
			}

			if (gateTriggersB[c].process(inputs[B_INPUT].getPolyVoltage(c))) {
				float rando = random::uniform();
				float probAttenB = params[PROBATTENB_PARAM].getValue();
				float probCVB = probAttenB * inputs[PROBCVB_INPUT].getPolyVoltage(c);
				float probBParam = params[PROBB_PARAM].getValue();
				float thresB = probBParam + probCVB;
				probB = rando < thresB;
			}

			// Logic

			bool a = probA && (inputs[A_INPUT].getPolyVoltage(c) > 1.f);
			bool b = probB && (inputs[B_INPUT].getPolyVoltage(c) > 1.f);

			outputs[AND_OUTPUT].setChannels(channels);
			outputs[AND_OUTPUT].setVoltage(10.f * (a && b), c);
			outputs[OR_OUTPUT].setChannels(channels);
			outputs[OR_OUTPUT].setVoltage(10.f * (a || b), c);
			outputs[XOR_OUTPUT].setChannels(channels);
			outputs[XOR_OUTPUT].setVoltage(10.f * (a ^ b), c);
		}

		outputs[NOT_OUTPUT].setChannels(channels);
		for (int i = 0; i < channels; i++) {
			outputs[NOT_OUTPUT].setVoltage(5.f * (inputs[NOT_INPUT].isConnected() && inputs[NOT_INPUT].getPolyVoltage(i) < 1.f), i);
		}
	}
};


struct LogicalProbabilityWidget : ModuleWidget {
	LogicalProbabilityWidget(LogicalProbability* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/LogicalProbability.svg")));

		// addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(8.203, 23.817)), module, LogicalProbability::PROBA_PARAM));
		addParam(createParamCentered<BefacoTinyKnob>(mm2px(Vec(22.115, 23.817)), module, LogicalProbability::PROBB_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(8.25, 35.301)), module, LogicalProbability::PROBATTENA_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(22.414, 35.301)), module, LogicalProbability::PROBATTENB_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.25, 47.196)), module, LogicalProbability::PROBCVA_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.414, 47.196)), module, LogicalProbability::PROBCVB_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.203, 59.367)), module, LogicalProbability::A_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.465, 59.367)), module, LogicalProbability::B_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.127, 76.154)), module, LogicalProbability::NOT_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.524, 74.806)), module, LogicalProbability::AND_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(8.127, 93.2)), module, LogicalProbability::NOT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.524, 87.288)), module, LogicalProbability::OR_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.524, 99.77)), module, LogicalProbability::XOR_OUTPUT));
	}
};


Model* modelLogicalProbability = createModel<LogicalProbability, LogicalProbabilityWidget>("LogicalProbability");
