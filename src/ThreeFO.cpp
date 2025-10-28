#include "plugin.hpp"


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

	ThreeFO() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(RATE_PARAM, 0.f, 1.f, 0.f, "");
		configParam(RATEATTEN_PARAM, 0.f, 1.f, 0.f, "");
		configParam(SCALE_PARAM, 0.f, 1.f, 0.f, "");
		configParam(OFFSET_PARAM, 0.f, 1.f, 0.f, "");
		configInput(RATECV_INPUT, "");
		configOutput(ONE_OUTPUT, "");
		configOutput(TWO_OUTPUT, "");
		configOutput(THREE_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
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

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(3.156, 81.471)), module, ThreeFO::TWO_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(8.976, 81.471)), module, ThreeFO::ONE_LIGHT));
		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(9.506, 89.938)), module, ThreeFO::THREE_LIGHT));
	}
};


Model* modelThreeFO = createModel<ThreeFO, ThreeFOWidget>("ThreeFO");
