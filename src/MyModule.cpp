#include "plugin.hpp"

float phase = 0.f;
float blinkPhase = 0.f;


struct MyModule : Module {
	enum ParamId {
		PITCH_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		V_OCT_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUTPUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHT_LIGHT,
		LIGHTS_LEN
	};

	MyModule() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(PITCH_PARAM, 0.f, 1.f, 0.f, "");
		configInput(V_OCT_INPUT, "");
		configOutput(OUTPUT_OUTPUT, "");
	}

	void process(const ProcessArgs& args) override {
		// Compute the frequency from the pitch parameter and Input
		float pitch = params[PITCH_PARAM].getValue();
		pitch += inputs[V_OCT_INPUT].getVoltage();
		// default frequency is C4 = 261.6256f
		float freq = dsp::FREQ_C4 * std::pow(2.f, pitch);

		// accumulate the phase
		phase += freq * args.sampleTime;
		if (phase > 1.f)
			phase -= 1.f;

		// compute the sine OutputId
		float sine = std::sin(2.f * M_PI * phase);
		outputs[OUTPUT_OUTPUT].setVoltage(5.f * sine);

		// blink the light at 1H
		blinkPhase += args.sampleTime;
		if (blinkPhase >= 1.f)
			blinkPhase -= 1.f;
		lights[LIGHT_LIGHT].setBrightness(blinkPhase < 0.5f ? 1.f : 0.f);
	}


};


struct MyModuleWidget : ModuleWidget {
	MyModuleWidget(MyModule* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/MyModulePanel.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(19.077, 32.172)), module, MyModule::PITCH_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(19.612, 44.983)), module, MyModule::V_OCT_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(39.806, 116.284)), module, MyModule::OUTPUT_OUTPUT));

		addChild(createLightCentered<MediumLight<RedLight>>(mm2px(Vec(17.833, 91.844)), module, MyModule::LIGHT_LIGHT));
	}
};


Model* modelMyModule = createModel<MyModule, MyModuleWidget>("MyModule");
