#include "plugin.hpp"
#include <samplerate.h>



struct TriParDel : Module {
	enum ParamId {
		TIME1_PARAM,
        TIME2_PARAM,
        TIME3_PARAM,
        TIMECV1_PARAM,
        TIMECV2_PARAM,
        TIMECV3_PARAM,
        FEEDBACK1_PARAM,
        FEEDBACK2_PARAM,
        FEEDBACK3_PARAM,
        MIX1_PARAM,
        MIX2_PARAM,
        MIX3_PARAM,
		PARAMS_LEN
	};
	enum InputId {
        ONE_INPUT,
        TWO_INPUT,
        THREE_INPUT,
        TIMECV1_INPUT,
        TIMECV2_INPUT,
        TIMECV3_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
        ONE_OUTPUT,
        TWO_OUTPUT,
        THREE_OUTPUT,
        MIXED_OUTPUT,
        POLY_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

    constexpr static size_t HISTORY_SIZE = 1 << 21;
	dsp::DoubleRingBuffer<float, HISTORY_SIZE> historyBuffer[3];
	dsp::DoubleRingBuffer<float, 16> outBuffer[3];
	SRC_STATE* src1;
	SRC_STATE* src2;
	SRC_STATE* src3;
	float lastWet1 = 0.f;
	float lastWet2 = 0.f;
	float lastWet3 = 0.f;
	float clockFreq = 2.f;

	float output1 = 0.f;
	float output2 = 0.f;
	float output3 = 0.f;
	

	TriParDel() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);

        // TIME = 0.001 * 1000^TIME_PARAM
        // OR: TIME_PARAM = log10(TIME * 1000) / 4
        const float timeMin = log10(0.001f * 1000) / 4;
        const float timeMax = log10(10.f * 1000) / 4;
        const float timeDefault = log10(0.5f * 1000) / 4;

		configParam(TIME1_PARAM, timeMin, timeMax, timeDefault, "Time", " s", 10.f / 1e-3, 1e-3);
        configParam(TIME2_PARAM, timeMin, timeMax, timeDefault, "Time", " s", 10.f / 1e-3, 1e-3);
        configParam(TIME3_PARAM, timeMin, timeMax, timeDefault, "Time", " s", 10.f / 1e-3, 1e-3);

        configParam(TIMECV1_PARAM, -1.f, 1.f, 0.f, "Time CV", "%", 0, 100);
        getParamQuantity(TIMECV1_PARAM)->randomizeEnabled = false;
        configParam(TIMECV2_PARAM, -1.f, 1.f, 0.f, "Time CV", "%", 0, 100);
        getParamQuantity(TIMECV2_PARAM)->randomizeEnabled = false;
        configParam(TIMECV3_PARAM, -1.f, 1.f, 0.f, "Time CV", "%", 0, 100);
        getParamQuantity(TIMECV3_PARAM)->randomizeEnabled = false;

        configParam(FEEDBACK1_PARAM, 0.f, 1.f, 0.5f, "Feedback", "%", 0, 100);
        configParam(FEEDBACK2_PARAM, 0.f, 1.f, 0.5f, "Feedback", "%", 0, 100);
        configParam(FEEDBACK3_PARAM, 0.f, 1.f, 0.5f, "Feedback", "%", 0, 100);

        configParam(MIX1_PARAM, 0.f, 1.f, 0.5f, "Mix", "%", 0, 100);
        configParam(MIX2_PARAM, 0.f, 1.f, 0.5f, "Mix", "%", 0, 100);
        configParam(MIX3_PARAM, 0.f, 1.f, 0.5f, "Mix", "%", 0, 100);

        configInput(TIMECV1_INPUT, "Time CV");
        getInputInfo(TIMECV1_INPUT)->description = "1V/octave when Time CV is 100%";
        configInput(TIMECV2_INPUT, "Time CV");
        getInputInfo(TIMECV2_INPUT)->description = "1V/octave when Time CV is 100%";
        configInput(TIMECV3_INPUT, "Time CV");
        getInputInfo(TIMECV3_INPUT)->description = "1V/octave when Time CV is 100%";

        configInput(ONE_INPUT, "Audio 1");
        configInput(TWO_INPUT, "Audio 2");
        configInput(THREE_INPUT, "Audio 3");

        configOutput(ONE_OUTPUT, "Audio 1");
        configOutput(TWO_OUTPUT, "Audio 2");
        configOutput(THREE_OUTPUT, "Audio 3");
        configOutput(MIXED_OUTPUT, "Mixed Audio");
        configOutput(POLY_OUTPUT, "Polyphonic");

        configBypass(ONE_INPUT, ONE_OUTPUT);
        configBypass(TWO_INPUT, TWO_OUTPUT);
        configBypass(THREE_INPUT, THREE_OUTPUT);

        src1 = src_new(SRC_SINC_FASTEST, 1, NULL);
		src2 = src_new(SRC_SINC_FASTEST, 1, NULL);
		src3 = src_new(SRC_SINC_FASTEST, 1, NULL);
	}

    ~TriParDel() {
		if (src1)
			src_delete(src1);
		if (src2)
			src_delete(src2);
		if (src3)
			src_delete(src3);
	}

	void delay(rack::engine::Param feedback_param, rack::engine::Param time_param, rack::engine::Param timecv_param, rack::engine::Param mix_param, rack::engine::Input audio_input, rack::engine::Input time_input, int output, float lastWet, SRC_STATE* src, const ProcessArgs& args) {
        // Get input to delay block
		float in = audio_input.getVoltageSum();
		float feedback = feedback_param.getValue(); // + inputs[FEEDBACK1_INPUT].getVoltage() / 10.f;
		// feedback = clamp(feedback, 0.f, 1.f);
		float dry = in + lastWet * feedback;
		int del_buf = output - 1;

		// Compute freq
		// Scale time knob to 1V/oct pitch based on formula explained in constructor, for backwards compatibility
		float pitch = std::log2(1000.f) - std::log2(10000.f) * time_param.getValue();
		pitch += time_input.getVoltage() * timecv_param.getValue();
		float freq = clockFreq / 2.f * dsp::exp2_taylor5(pitch);
		// Number of desired delay samples
		float index = args.sampleRate / freq;
		// In order to delay accurate samples, subtract by the historyBuffer size, and an experimentally tweaked amount.
		index -= 16 + 4.f;
		index = clamp(index, 2.f, float(HISTORY_SIZE - 1));
		// DEBUG("freq %f index %f", freq, index);


		// Push dry sample into history buffer
		if (!historyBuffer[del_buf].full()) {
			historyBuffer[del_buf].push(dry);
		}

		if (outBuffer[del_buf].empty()) {
			// How many samples do we need consume to catch up?
			float consume = index - historyBuffer[del_buf].size();
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
		float mix = mix_param.getValue(); // + inputs[MIX_INPUT].getVoltage() / 10.f * params[MIX_CV_PARAM].getValue();
		//mix = clamp(mix, 0.f, 1.f);
		float out = crossfade(in, wet, mix);
		//audio_output.setVoltage(out);
		switch(output) {
			case 1:
				output1 = out;
				break;
			case 2:
				output2 = out;
				break;
			case 3:
				output3 = out;
				break;
		}
		
	}

	void process(const ProcessArgs& args) override {
        delay(params[FEEDBACK1_PARAM], params[TIME1_PARAM], params[TIMECV1_PARAM], params[MIX1_PARAM], inputs[ONE_INPUT], inputs[TIMECV1_INPUT], 1, lastWet1, src1, args);
		outputs[ONE_OUTPUT].setVoltage(output1);

		delay(params[FEEDBACK2_PARAM], params[TIME2_PARAM], params[TIMECV2_PARAM], params[MIX2_PARAM], inputs[TWO_INPUT], inputs[TIMECV2_INPUT], 2, lastWet2, src2, args);
		outputs[TWO_OUTPUT].setVoltage(output2);

		delay(params[FEEDBACK3_PARAM], params[TIME3_PARAM], params[TIMECV3_PARAM], params[MIX3_PARAM], inputs[THREE_INPUT], inputs[TIMECV3_INPUT], 3, lastWet3, src3, args);
		outputs[THREE_OUTPUT].setVoltage(output3);

		outputs[MIXED_OUTPUT].setVoltage(0.33f * (output1 + output2 + output3));

		outputs[POLY_OUTPUT].setChannels(3);
		outputs[POLY_OUTPUT].setVoltage(output1, 0);
		outputs[POLY_OUTPUT].setVoltage(output2, 1);
		outputs[POLY_OUTPUT].setVoltage(output3, 2);
		
	}
};


struct TriParDelWidget : ModuleWidget {
	TriParDelWidget(TriParDel* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/TriParDel.svg")));

		addParam(createParamCentered<Davies1900hWhiteKnob>(Vec(96.086, 91.992), module, TriParDel::TIME1_PARAM));
		addParam(createParamCentered<Davies1900hWhiteKnob>(Vec(172.505, 91.992), module, TriParDel::TIME2_PARAM));
		addParam(createParamCentered<Davies1900hWhiteKnob>(Vec(244.950, 91.992), module, TriParDel::TIME3_PARAM));

        addParam(createParamCentered<Trimpot>(Vec(114.915, 138.942), module, TriParDel::TIMECV1_PARAM));
		addParam(createParamCentered<Trimpot>(Vec(191.315, 138.942), module, TriParDel::TIMECV2_PARAM));
		addParam(createParamCentered<Trimpot>(Vec(263.716, 138.942), module, TriParDel::TIMECV3_PARAM));
        
        addParam(createParamCentered<Davies1900hWhiteKnob>(Vec(96.086, 197.992), module, TriParDel::FEEDBACK1_PARAM));
		addParam(createParamCentered<Davies1900hWhiteKnob>(Vec(172.505, 197.992), module, TriParDel::FEEDBACK2_PARAM));
		addParam(createParamCentered<Davies1900hWhiteKnob>(Vec(244.905, 197.992), module, TriParDel::FEEDBACK3_PARAM));

        addParam(createParamCentered<Davies1900hWhiteKnob>(Vec(96.086, 265.992), module, TriParDel::MIX1_PARAM));
		addParam(createParamCentered<Davies1900hWhiteKnob>(Vec(172.505, 265.992), module, TriParDel::MIX2_PARAM));
		addParam(createParamCentered<Davies1900hWhiteKnob>(Vec(244.905, 265.992), module, TriParDel::MIX3_PARAM));


        addInput(createInputCentered<PJ301MPort>(Vec(96.082, 45.726), module, TriParDel::ONE_INPUT));
        addInput(createInputCentered<PJ301MPort>(Vec(77.913, 138.432), module, TriParDel::TIMECV1_INPUT));
		addInput(createInputCentered<PJ301MPort>(Vec(172.505, 45.726), module, TriParDel::TWO_INPUT));
        addInput(createInputCentered<PJ301MPort>(Vec(154.314, 138.432), module, TriParDel::TIMECV2_INPUT));
		addInput(createInputCentered<PJ301MPort>(Vec(244.905, 45.726), module, TriParDel::THREE_INPUT));
        addInput(createInputCentered<PJ301MPort>(Vec(226.715, 138.432), module, TriParDel::TIMECV3_INPUT));

        addOutput(createOutputCentered<PJ301MPort>(Vec(96.086, 317.714), module, TriParDel::ONE_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(Vec(172.505, 317.714), module, TriParDel::TWO_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(Vec(244.905, 317.714), module, TriParDel::THREE_OUTPUT));
        addOutput(createOutputCentered<PJ301MPort>(Vec(172.505, 352.291), module, TriParDel::MIXED_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(Vec(244.905, 352.291), module, TriParDel::POLY_OUTPUT));
	}
};


Model* modelTriParDel = createModel<TriParDel, TriParDelWidget>("TriParDel");