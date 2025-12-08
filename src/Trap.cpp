#include "plugin.hpp"


struct Trap : Module {
	enum ParamId {
		RATE_PARAM,
		RATECV_PARAM,
		RANGE_PARAM,
		SCALE_PARAM,
		OFFSET_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		RATECV_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		AUDIO_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	Trap() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		
		
		configParam(RATE_PARAM, -8.f, 10.f, 1.f, "Rate", " Hz", 2, 0.125f);
		configParam(RATECV_PARAM, -1.f, 1.f, 0.f, "Rate CV Attenuverter", "%", 0, 100);
		configParam(SCALE_PARAM, 0.f, 1.f, 1.f, "Scale", "%", 0, 100);
		configParam(OFFSET_PARAM, -5.f, 5.f, 0.f, "Offset", " V");
		
		configSwitch(RANGE_PARAM, 0.f, 1.f, 0.f, "Rate Range", {"Slow (LFO)", "Fast (Audio Rate)"});

		configInput(RATECV_INPUT, "Rate CV");

		configOutput(AUDIO_OUTPUT, "Output");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct TrapWidget : ModuleWidget {
	TrapWidget(Trap* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Trap.svg")));

		addParam(createParamCentered<Davies1900hWhiteKnob>(Vec(30, 76.154), module, Trap::RATE_PARAM));
		addParam(createParamCentered<Trimpot>(Vec(30, 153.528), module, Trap::RATECV_PARAM));
		addParam(createParamCentered<Trimpot>(Vec(30, 231.921), module, Trap::SCALE_PARAM));
		addParam(createParamCentered<Trimpot>(Vec(30, 269.165), module, Trap::OFFSET_PARAM));

		addParam(createParam<CKSS>(Vec(9.05, 108.339), module, Trap::RANGE_PARAM));

		addInput(createInputCentered<PJ301MPort>(Vec(30, 187.707), module, Trap::RATECV_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(Vec(30, 311.305), module, Trap::AUDIO_OUTPUT));

	}
};


Model* modelTrap = createModel<Trap, TrapWidget>("Trap");