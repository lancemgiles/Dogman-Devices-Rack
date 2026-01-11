#include "plugin.hpp"

using simd::float_4;


struct Knife : Module {
	enum ParamId {
		RATE_PARAM,
		RATECV_PARAM,
		SCALE_PARAM,
		OFFSET_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		RATECV_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		CV_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	Knife() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		
		
		configParam(RATE_PARAM, -8.f, 10.f, 1.f, "Rate", " Hz", 2, 0.125f);
		configParam(RATECV_PARAM, -1.f, 1.f, 0.f, "Rate CV Attenuverter", "%", 0, 100);
		configParam(SCALE_PARAM, 0.f, 1.f, 1.f, "Scale", "%", 0, 100);
		configParam(OFFSET_PARAM, -1.f, 1.f, 0.f, "Offset", " V");
		
		configInput(RATECV_INPUT, "Rate CV");

		configOutput(CV_OUTPUT, "Output");
	}

	float_4 phases[4];
	dsp::TSchmittTrigger<float_4> clockTriggers[4];
	dsp::TSchmittTrigger<float_4> resetTriggers[4];
	dsp::SchmittTrigger clockTrigger;
	float clockFreq = 2.f;
	dsp::Timer clockTimer;

	void process(const ProcessArgs& args) override {
		int channels = std::max(1, inputs[RATECV_INPUT].getChannels());
		for (int c = 0; c < channels; c += 4) {
			// Frequency
			float_4 pitch = params[RATE_PARAM].getValue();
			pitch += inputs[RATECV_INPUT].getVoltageSimd<float_4>(c) * params[RATECV_PARAM].getValue();
			float_4 freq = clockFreq / 2.f * dsp::exp2_taylor5(pitch);

			// Advance phase
			float_4 deltaPhase = simd::fmin(freq * args.sampleTime, 0.5f);
			phases[c / 4] += deltaPhase;
			phases[c / 4] -= simd::trunc(phases[c / 4]);

			float_4 scale = params[SCALE_PARAM].getValue();
			float offset = params[OFFSET_PARAM].getValue();

			// Saw and triangle
			if (outputs[CV_OUTPUT].isConnected()) {
				float_4 p = phases[c / 4];
				float_4 saw = 2.f * (p - simd::round(p)) * random::uniform();
				p += 0.25;
				float_4 tri = 4.f * simd::fabs(p - simd::round(p)) - 1.f;
				float_4 mixed = crossfade(saw, tri, 0.7f);
				mixed *= scale;
				mixed += offset;
				outputs[CV_OUTPUT].setVoltageSimd(5.f * mixed, c);
			}
		}
		outputs[CV_OUTPUT].setChannels(channels);
	}
};


struct KnifeWidget : ModuleWidget {
	KnifeWidget(Knife* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Knife.svg")));

		addParam(createParamCentered<Davies1900hWhiteKnob>(Vec(30, 76.154), module, Knife::RATE_PARAM));
		addParam(createParamCentered<Trimpot>(Vec(30, 119.528), module, Knife::RATECV_PARAM));
		addParam(createParamCentered<Trimpot>(Vec(30, 219.921), module, Knife::SCALE_PARAM));
		addParam(createParamCentered<Trimpot>(Vec(30, 263.165), module, Knife::OFFSET_PARAM));

		addInput(createInputCentered<PJ301MPort>(Vec(30, 153.707), module, Knife::RATECV_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(Vec(30, 311.305), module, Knife::CV_OUTPUT));

	}
};


Model* modelKnife = createModel<Knife, KnifeWidget>("Knife");