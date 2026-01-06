// Take the chorus part of Water, but change the default rate multiple to log10(300)
// and drop the lfo
// the result seems to flip between a slowed down and a sped up copy
// just mix control


#include "plugin.hpp"
#include <samplerate.h>


struct Company : Module {
	enum ParamId {
		MIX_PARAM,
        MIXCV_PARAM,
		PARAMS_LEN
	};
	enum InputId {
        IN_INPUT,
		MIXCV_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};


	Company() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(MIX_PARAM, 0.f, 1.f, 0.5f, "Company Mix");
        configParam(MIXCV_PARAM, 0.f, 1.f, 0.f, "Mix CV Attenuation");
        configInput(IN_INPUT, "Signal");
		configInput(MIXCV_INPUT, "Mix CV");
		configOutput(OUT_OUTPUT, "Signal");

		src1 = src_new(SRC_SINC_FASTEST, 1, NULL);
		src2 = src_new(SRC_SINC_FASTEST, 1, NULL);
		src3 = src_new(SRC_SINC_FASTEST, 1, NULL);
	}

	float delay_out1 = 0.f;
    float delay_out2 = 0.f;
    float delay_out3 = 0.f;
    float delay_out = 0.f;
    constexpr static size_t HISTORY_SIZE = 1 << 21;
	dsp::DoubleRingBuffer<float, HISTORY_SIZE> historyBuffer[3];
	dsp::DoubleRingBuffer<float, 16> outBuffer[3];

	float lfo1 = 0.f;
	float lfo2 = 0.f;
	float lfo3 = 0.f;

	
	SRC_STATE* src1;
	SRC_STATE* src2;
	SRC_STATE* src3;
	float lastWet1 = 0.f;
	float lastWet2 = 0.f;
	float lastWet3 = 0.f;
	float clockFreq = 2.f;
	float chorus_out = 0.f;

	~Company() {
		if (src1)
			src_delete(src1);
		if (src2)
			src_delete(src2);
		if (src3)
			src_delete(src3);
	}

	void delay(float lfo_input, int mixer_input, float lastWet, SRC_STATE* src, const ProcessArgs& args) {

		float in = inputs[IN_INPUT].getVoltage();
		float feedback = 0.15f;
		float dry = in + lastWet * feedback;
		int del_buf = mixer_input - 1;

		// Compute freq
		float pitch = std::log2(1000.f) - std::log2(10000.f) * log10(300.f);
		pitch += lfo_input * 0.08f;
		float freq = clockFreq / 2.f * dsp::exp2_taylor5(pitch);
		// Number of desired delay samples
		float index = args.sampleRate / freq;
		index -= 16 + 4.f;
		index = clamp(index, 2.f, float(HISTORY_SIZE - 1));

		// Push dry sample into history buffer
		if (!historyBuffer[del_buf].full()) {
			historyBuffer[del_buf].push(dry);
		}

		if (outBuffer[del_buf].empty()) {
			// How many samples do we need consume to catch up?
			float consume = index - (historyBuffer[del_buf].size() * 2);
			double ratio = std::pow(4.f, clamp(consume / 10000.f, -1.f, 1.f));
			// DEBUG("index %f historyBuffer %lu consume %f ratio %lf", index, historyBuffer.size(), consume, ratio);

			// Convert samples from the historyBuffer to catch up or slow down so `index` and `historyBuffer.size()` eventually match approximately
			SRC_DATA srcData = {};
			srcData.data_in = (const float*) historyBuffer[del_buf].startData();
			srcData.data_out = (float*) outBuffer[del_buf].endData();
			srcData.input_frames = std::min((int) historyBuffer[del_buf].size(), 16);
			srcData.output_frames = outBuffer[del_buf].capacity();
			srcData.end_of_input = false;
			srcData.src_ratio = ratio;
			if (src)
				src_process(src, &srcData);
			historyBuffer[del_buf].startIncr(srcData.input_frames_used);
			outBuffer[del_buf].endIncr(srcData.output_frames_gen);
			// DEBUG("used %ld gen %ld", srcData.input_frames_used, srcData.output_frames_gen);
		}

		float wet = 0.f;
		if (!outBuffer[del_buf].empty()) {
			wet = outBuffer[del_buf].shift();
		}
		wet = clamp(wet, -100.f, 100.f);
		lastWet = wet;

		// Set mix output
		float mix = params[MIX_PARAM].getValue() + (inputs[MIXCV_INPUT].getVoltage() * params[MIXCV_PARAM].getValue());
		mix = clamp(mix, 0.f, 1.f);
		float out = crossfade(in, wet, mix);

		//audio_output.setVoltage(out);
		switch(mixer_input) {
			case 1:
				delay_out1 = out;
				break;
			case 2:
				delay_out2 = out;
				break;
			case 3:
				delay_out3 = out;
				break;
		}
		mixer_input = out;
		
	}

    void delay_mixer(float delay1, float delay2, float delay3) {
        delay_out += 0.3f * (delay_out1 + delay_out2 + delay_out3);
    }

	float sine_phases;
	float chorus_lfo;
	// update value of chorus sine waves
	void sine_lfo(const ProcessArgs& args) {
		// pitch and frequency
		float rate = 3.86f * log2(2.5f) + 0.183f - 4.f;
		float freq = clockFreq / 2.f * dsp::exp2_taylor5(rate);

		// Advance phase
		float deltaPhase = std::min(freq * args.sampleTime, 0.5f);
		sine_phases += deltaPhase;
		sine_phases -= std::trunc(sine_phases);

		// make sine wave
		float p = sine_phases;

		float v1 = std::sin(2 * M_PI * p);
		lfo1 = 5.f * v1;

		float v2 = std::sin(2 * M_PI * (p - 120));
		lfo2 = 5.f * v2;

		float v3 = std::sin(2 * M_PI * (p - 240));
		lfo3 = 5.f * v3;
	}


	void process(const ProcessArgs& args) override {
		// Chorus
		sine_lfo(args);
		delay(lfo1, 1, lastWet1, src1, args);
		delay(lfo2, 2, lastWet2, src2, args);
		delay(lfo3, 3, lastWet3, src3, args);
		delay_mixer(delay_out1, delay_out2, delay_out3);
		chorus_out = delay_out;
		delay_out = 0.f;

		outputs[OUT_OUTPUT].setVoltage(chorus_out);
	}
};


struct CompanyWidget : ModuleWidget {
	CompanyWidget(Company* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Company.svg")));

		addParam(createParamCentered<Davies1900hWhiteKnob>(Vec(30, 120.154), module, Company::MIX_PARAM));
		addParam(createParamCentered<BefacoTinyKnob>(Vec(30, 163.525), module, Company::MIXCV_PARAM));

		addInput(createInputCentered<PJ301MPort>(Vec(30, 197.706), module, Company::MIXCV_INPUT));
        addInput(createInputCentered<PJ301MPort>(Vec(30, 45.305), module, Company::IN_INPUT));


		addOutput(createOutputCentered<PJ301MPort>(Vec(30, 251.305), module, Company::OUT_OUTPUT));
	}
};


Model* modelCompany = createModel<Company, CompanyWidget>("Company");
