#include "plugin.hpp"


struct Water : Module {
	enum ParamId {
		CHORUSDEPTH_PARAM,
		RATE_PARAM,
		TREMOLODEPTH_PARAM,
		CHORUSCV_PARAM,
		RATECV_PARAM,
		TREMOLOCV_PARAM,
		BLEND_PARAM,
		BLENDCV_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		CHORUSCV_INPUT,
		RATECV_INPUT,
		TREMOLOCV_INPUT,
		BLENDCV_INPUT,
		AUDIO_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		AUDIO_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		RATE_LIGHT,
		LIGHTS_LEN
	};

	Water() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(CHORUSDEPTH_PARAM, 0.f, 1.f, 0.f, "Chorus Depth");
		configParam(RATE_PARAM, 0.f, 1.f, 0.f, "Chorus and Tremolo Rate");
		configParam(TREMOLODEPTH_PARAM, 0.f, 1.f, 0.f, "Tremolo Depth");
		configParam(CHORUSCV_PARAM, 0.f, 1.f, 0.f, "Chorus Depth CV");
		configParam(RATECV_PARAM, 0.f, 1.f, 0.f, "Rate CV");
		configParam(TREMOLOCV_PARAM, 0.f, 1.f, 0.f, "Tremolo Depth CV");
		configParam(BLEND_PARAM, 0.f, 1.f, 0.f, "Blend");
		configParam(BLENDCV_PARAM, 0.f, 1.f, 0.f, "Blend CV");
		configInput(CHORUSCV_INPUT, "Chorus Depth CV");
		configInput(RATECV_INPUT, "Chorus and Tremolo Rate CV");
		configInput(TREMOLOCV_INPUT, "Tremolo Depth CV");
		configInput(BLENDCV_INPUT, "Blend CV");
		configInput(AUDIO_INPUT, "Audio");
		configOutput(AUDIO_OUTPUT, "Audio");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct WaterWidget : ModuleWidget {
	WaterWidget(Water* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/WaterPanel.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(16.484, 30.652)), module, Water::CHORUSDEPTH_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(45.704, 31.037)), module, Water::RATE_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(75.967, 30.875)), module, Water::TREMOLODEPTH_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(10.019, 45.679)), module, Water::CHORUSCV_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(38.536, 45.719)), module, Water::RATECV_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(69.133, 45.865)), module, Water::TREMOLOCV_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(74.485, 88.419)), module, Water::BLEND_PARAM));
		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(80.668, 102.912)), module, Water::BLENDCV_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(24.071, 45.395)), module, Water::CHORUSCV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(52.314, 45.561)), module, Water::RATECV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(82.549, 45.827)), module, Water::TREMOLOCV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(66.214, 102.55)), module, Water::BLENDCV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(66.378, 116.484)), module, Water::AUDIO_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(79.314, 116.564)), module, Water::AUDIO_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(12.264, 106.205)), module, Water::RATE_LIGHT));
	}
};


Model* modelWater = createModel<Water, WaterWidget>("Water");
