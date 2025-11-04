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
	}
};


struct LogicalProbabilityWidget : ModuleWidget {
	LogicalProbabilityWidget(LogicalProbability* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/LogicalProbability.svg")));

		addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH / 2, 0)));
		addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH / 2, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<Davies1900hWhiteKnob>(mm2px(Vec(8.203, 23.817)), module, LogicalProbability::PROBA_PARAM));
		addParam(createParamCentered<Davies1900hWhiteKnob>(mm2px(Vec(22.115, 23.817)), module, LogicalProbability::PROBB_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(8.203, 35.301)), module, LogicalProbability::PROBATTENA_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(22.115, 35.301)), module, LogicalProbability::PROBATTENB_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.203, 47.196)), module, LogicalProbability::PROBCVA_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.115, 47.196)), module, LogicalProbability::PROBCVB_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.203, 59.367)), module, LogicalProbability::A_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(22.115, 59.367)), module, LogicalProbability::B_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(8.203, 74.806)), module, LogicalProbability::NOT_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.115, 74.806)), module, LogicalProbability::AND_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(8.203, 87.288)), module, LogicalProbability::NOT_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.115, 87.288)), module, LogicalProbability::OR_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.115, 99.77)), module, LogicalProbability::XOR_OUTPUT));
	}
};


Model* modelLogicalProbability = createModel<LogicalProbability, LogicalProbabilityWidget>("LogicalProbability");
