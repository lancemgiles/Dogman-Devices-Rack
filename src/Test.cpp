#include "plugin.hpp"


struct Test : Module {
	enum ParamId {
		PATH13372_PARAM,
		ELLIPSE13480_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		PATH13426_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		PATH13482_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	Test() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(PATH13372_PARAM, 0.f, 1.f, 0.f, "");
		configParam(ELLIPSE13480_PARAM, 0.f, 1.f, 0.f, "");
		configInput(PATH13426_INPUT, "");
		configOutput(PATH13482_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct TestWidget : ModuleWidget {
	TestWidget(Test* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Test.svg")));


		addParam(createParamCentered<Davies1900hWhiteKnob>(mm2px(Vec(16.256, 30.966)), module, Test::PATH13372_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(16.256, 46.823)), module, Test::ELLIPSE13480_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(16.256, 60.991)), module, Test::PATH13426_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(16.256, 82.972)), module, Test::PATH13482_OUTPUT));
	}
};


Model* modelTest = createModel<Test, TestWidget>("Test");