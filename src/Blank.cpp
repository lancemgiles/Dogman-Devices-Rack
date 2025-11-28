#include "plugin.hpp"
#include "SVGQuery.hpp"


struct Blank : Module {
	enum ParamId {
		KNOB_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		INPUTS_LEN
	};
	enum OutputId {
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	Blank() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(KNOB_PARAM, 0.f, 1.f, 0.5f, "knob");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct BlankWidget : ModuleWidget {
	BlankWidget(Blank* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Blank.svg")));

		addParam(createParamCentered<Davies1900hWhiteKnob>(Vec(234, 65), module, Blank::KNOB_PARAM));


	}
};


Model* modelBlank = createModel<Blank, BlankWidget>("Blank");