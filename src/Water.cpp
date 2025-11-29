#include "plugin.hpp"
#include <samplerate.h>


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
		float blinkPhaseT = 0.f;
		float dry = inputs[AUDIO_INPUT].getVoltage();
		float tPhase = 0.5f;
		// Tremolo Rate
		float tRate = params[RATE_PARAM].getValue();
		float tFreq = std::pow(2.f, tRate);
		tPhase += tFreq * args.sampleTime;
		if (tPhase > 1.f)
			tPhase -= 1.f;
		// this converts the phase to radians, sine of which is -1...1 -> make -5...5 and add 5 to make unipolar
		float tLFO = (std::sin(2.f * M_PI * tPhase) * 5.f) + 5;

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
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Water.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<Davies1900hWhiteKnob>(Vec(41.713, 68.293), module, Water::CHORUSDEPTH_PARAM));
		addParam(createParamCentered<Davies1900hWhiteKnob>(Vec(142.5, 68.293), module, Water::RATE_PARAM));
		addParam(createParamCentered<Davies1900hWhiteKnob>(Vec(243.286, 68.293), module, Water::TREMOLODEPTH_PARAM));
		addParam(createParamCentered<BefacoTinyKnob>(Vec(24.384, 125.995), module, Water::CHORUSCV_PARAM));
		addParam(createParamCentered<BefacoTinyKnob>(Vec(123.722, 125.995), module, Water::RATECV_PARAM));
		addParam(createParamCentered<BefacoTinyKnob>(Vec(223.742, 127.359), module, Water::TREMOLOCV_PARAM));

		addInput(createInputCentered<PJ301MPort>(Vec(61.066, 125.995), module, Water::CHORUSCV_INPUT));
		addInput(createInputCentered<PJ301MPort>(Vec(160.405, 125.995), module, Water::RATECV_INPUT));
		addInput(createInputCentered<PJ301MPort>(Vec(260.424, 127.359), module, Water::TREMOLOCV_INPUT));
		addInput(createInputCentered<PJ301MPort>(Vec(41.713, 345.619), module, Water::AUDIO_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(Vec(243.286, 345.619), module, Water::AUDIO_OUTPUT));

		addChild(createLightCentered<MediumLight<BlueLight>>(Vec(92.106, 45.0163), module, Water::CHORUSRATE_LIGHT));
		addChild(createLightCentered<MediumLight<BlueLight>>(Vec(192.893, 45.016), module, Water::TREMOLORATE_LIGHT));
	}
};


Model* modelWater = createModel<Water, WaterWidget>("Water");
