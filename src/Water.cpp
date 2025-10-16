#include "plugin.hpp"

float blinkPhaseT = 0.f;

struct Water : Module {
	enum ParamId {
		CHORUSDEPTH_PARAM,
		RATE_PARAM,
		TREMOLODEPTH_PARAM,
		CHORUSCV_PARAM,
		RATECV_PARAM,
		TREMOLOCV_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		CHORUSCV_INPUT,
		RATECV_INPUT,
		TREMOLOCV_INPUT,
		AUDIO_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		AUDIO_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		CHORUSRATE_LIGHT,
		TREMOLORATE_LIGHT,
		LIGHTS_LEN
	};

	float tPhase = 0.5f;

	Water() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(CHORUSDEPTH_PARAM, 0.f, 1.f, 0.f, "Chorus Depth");
		configParam(RATE_PARAM, -4.f, 5.f, 1.f, "Chorus and Tremolo Rate", " Hz", 2, 1);
		configParam(TREMOLODEPTH_PARAM, 0.f, 1.f, 0.5f, "Tremolo Depth", "%", 0, 100);
		configParam(CHORUSCV_PARAM, 0.f, 1.f, 0.f, "Chorus Depth CV");
		configParam(RATECV_PARAM, 0.f, 1.f, 0.f, "Rate CV");
		configParam(TREMOLOCV_PARAM, 0.f, 1.f, 0.f, "Tremolo Depth CV", "%", 0, 100);
		configInput(CHORUSCV_INPUT, "Chorus Depth CV");
		configInput(RATECV_INPUT, "Chorus and Tremolo Rate CV");
		configInput(TREMOLOCV_INPUT, "Tremolo Depth CV");
		configInput(AUDIO_INPUT, "Audio");
		configOutput(AUDIO_OUTPUT, "Audio");
	}

	void process(const ProcessArgs& args) override {
		float dry = inputs[AUDIO_INPUT].getVoltage();
		// Tremolo Rate
		float tRate = params[RATE_PARAM].getValue();
		float tFreq = std::pow(2.f, tRate);
		tPhase += tFreq * args.sampleTime;
		if (tPhase > 1.f)
			tPhase -= 1.f;
		float tLFO = std::sin(2.f * M_PI * tPhase);

		// Tremolo Rate Light
		lights[TREMOLORATE_LIGHT].setBrightness(tLFO);

		// Tremolo Depth
		float tDepth = params[TREMOLODEPTH_PARAM].getValue();
		float tDepthCV = 1.f;

		if (inputs[TREMOLOCV_INPUT].isConnected()) {
			float tDepthCVGain = params[TREMOLOCV_PARAM].getValue();
			tDepthCV = clamp(inputs[TREMOLOCV_INPUT].getVoltage() / 10.f, 0.f, 1.f);
			tDepthCVGain *= tDepthCV;
			tDepth *= tDepthCVGain;
		}

		// Apply tremolo
		float tGain = 1.f;
		float tGainCV = clamp(tLFO / 5.f, 0.f, 1.f);
		tGain *= tGainCV;
		float tWet = dry * tGain;

		float tOut = crossfade(dry, tWet, tDepth);

		outputs[AUDIO_OUTPUT].setVoltage(tOut);



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

		addParam(createParamCentered<Davies1900hWhiteKnob>(mm2px(Vec(16.484, 30.652)), module, Water::CHORUSDEPTH_PARAM));
		addParam(createParamCentered<Davies1900hWhiteKnob>(mm2px(Vec(45.704, 31.037)), module, Water::RATE_PARAM));
		addParam(createParamCentered<Davies1900hWhiteKnob>(mm2px(Vec(75.967, 30.875)), module, Water::TREMOLODEPTH_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(10.019, 45.679)), module, Water::CHORUSCV_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(38.536, 45.719)), module, Water::RATECV_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(69.133, 45.865)), module, Water::TREMOLOCV_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(24.071, 45.395)), module, Water::CHORUSCV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(52.314, 45.561)), module, Water::RATECV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(82.549, 45.827)), module, Water::TREMOLOCV_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(66.378, 116.484)), module, Water::AUDIO_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(79.314, 116.564)), module, Water::AUDIO_OUTPUT));

		addChild(createLightCentered<MediumLight<BlueLight>>(mm2px(Vec(32.26, 21.675)), module, Water::CHORUSRATE_LIGHT));
		addChild(createLightCentered<MediumLight<BlueLight>>(mm2px(Vec(60.143, 21.997)), module, Water::TREMOLORATE_LIGHT));
	}
};


Model* modelWater = createModel<Water, WaterWidget>("Water");
