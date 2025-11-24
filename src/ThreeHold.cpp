#include "plugin.hpp"


struct ThreeHold : Module {
	enum ParamId {
		SCALE1_PARAM,
		SCALE2_PARAM,
		SCALE3_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		SIG1_INPUT,
		TRIG1_INPUT,
		SIG2_INPUT,
		TRIG2_INPUT,
		SIG3_INPUT,
		TRIG3_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT1_OUTPUT,
		OUT2_OUTPUT,
		OUT3_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	static const int maxPolyphony = engine::PORT_MAX_CHANNELS;
	int currentPolyphony1 = 1;
	int currentPolyphony2 = 1;
	int currentPolyphony3 = 1;

	dsp::SchmittTrigger trigger1[maxPolyphony];
	dsp::SchmittTrigger trigger2[maxPolyphony];
	dsp::SchmittTrigger trigger3[maxPolyphony];

	float sig1[maxPolyphony] = {0.0};
	float sig2[maxPolyphony] = {0.0};
	float sig3[maxPolyphony] = {0.0};

	ThreeHold() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(SCALE1_PARAM, 0.f, 1.f, 1.f, "Scale Input 1", "%", 0, 100);
		configParam(SCALE2_PARAM, 0.f, 1.f, 1.f, "Scale Input 2", "%", 0, 100);
		configParam(SCALE3_PARAM, 0.f, 1.f, 1.f, "Scale Input 3", "%", 0, 100);
		configInput(SIG1_INPUT, "Signal 1");
		configInput(TRIG1_INPUT, "Trigger 1");
		configInput(SIG2_INPUT, "Signal 2");
		configInput(TRIG2_INPUT, "Trigger 2");
		configInput(SIG3_INPUT, "Signal 3");
		configInput(TRIG3_INPUT, "Trigger 3");
		configOutput(OUT1_OUTPUT, "Output 1");
		configOutput(OUT2_OUTPUT, "Output 2");
		configOutput(OUT3_OUTPUT, "Output 3");
	}

	void process(const ProcessArgs& args) override {
		currentPolyphony1 = std::max(1, inputs[SIG1_INPUT].getChannels());
		currentPolyphony2 = std::max(1, inputs[SIG2_INPUT].getChannels());
		currentPolyphony3 = std::max(1, inputs[SIG3_INPUT].getChannels());

		outputs[OUT1_OUTPUT].setChannels(currentPolyphony1);
		outputs[OUT2_OUTPUT].setChannels(currentPolyphony2);
		outputs[OUT3_OUTPUT].setChannels(currentPolyphony3);

		for (int c = 0; c < currentPolyphony1; c ++) {
			if (trigger1[c].process(inputs[TRIG1_INPUT].getPolyVoltage(c) / 0.7)) {
				sig1[c] = inputs[SIG1_INPUT].getVoltage(c) * params[SCALE1_PARAM].getValue();
				outputs[OUT1_OUTPUT].setVoltage(sig1[c], c);
			}
		}

		for (int c = 0; c < currentPolyphony2; c ++) {
			if (trigger2[c].process(inputs[TRIG2_INPUT].getPolyVoltage(c) / 0.7)) {
				sig2[c] = inputs[SIG2_INPUT].getVoltage(c) * params[SCALE2_PARAM].getValue();
				outputs[OUT2_OUTPUT].setVoltage(sig2[c], c);
			}
		}
		for (int c = 0; c < currentPolyphony3; c ++) {
			if (trigger3[c].process(inputs[TRIG3_INPUT].getPolyVoltage(c) / 0.7)) {
				sig3[c] = inputs[SIG3_INPUT].getVoltage(c) * params[SCALE3_PARAM].getValue();
				outputs[OUT3_OUTPUT].setVoltage(sig3[c], c);
			}
		}

	}
};


struct ThreeHoldWidget : ModuleWidget {
	ThreeHoldWidget(ThreeHold* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/ThreeHold.svg")));

		// addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<Trimpot>(mm2px(Vec(22.023, 20.079)), module, ThreeHold::SCALE1_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(22.023, 51.3)), module, ThreeHold::SCALE2_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(22.023, 83.05)), module, ThreeHold::SCALE3_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.241, 20.079)), module, ThreeHold::SIG1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.241, 32.974)), module, ThreeHold::TRIG1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.241, 51.3)), module, ThreeHold::SIG2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.241, 64.195)), module, ThreeHold::TRIG2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.241, 83.05)), module, ThreeHold::SIG3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(9.241, 95.945)), module, ThreeHold::TRIG3_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.023, 32.974)), module, ThreeHold::OUT1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.023, 64.195)), module, ThreeHold::OUT2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(22.023, 95.945)), module, ThreeHold::OUT3_OUTPUT));
	}
};


Model* modelThreeHold = createModel<ThreeHold, ThreeHoldWidget>("ThreeHold");
