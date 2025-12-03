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
	dsp::DoubleRingBuffer<float, HISTORY_SIZE> historyBuffer;
	dsp::DoubleRingBuffer<float, 16> outBuffer;
	SRC_STATE* src;
	float lastWet = 0.f;
	dsp::RCFilter lowpassFilter;
	dsp::RCFilter highpassFilter;
	float clockFreq = 1.f;
	dsp::Timer clockTimer;
	dsp::SchmittTrigger clockTrigger;
	float clockPhase = 0.f;

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

        src = src_new(SRC_SINC_FASTEST, 1, NULL);
	}

    ~TriParDel() {
		if (src)
			src_delete(src);
	}

	void process(const ProcessArgs& args) override {
        clockFreq = 2.f;
        // Get input to delay block
		float in = inputs[ONE_INPUT].getVoltageSum();
		float feedback = params[FEEDBACK1_PARAM].getValue(); // + inputs[FEEDBACK1_INPUT].getVoltage() / 10.f;
		feedback = clamp(feedback, 0.f, 1.f);
		float dry = in + lastWet * feedback;

		// Compute freq
		// Scale time knob to 1V/oct pitch based on formula explained in constructor, for backwards compatibility
		float pitch = std::log2(1000.f) - std::log2(10000.f) * params[TIME1_PARAM].getValue();
		pitch += inputs[TIMECV1_INPUT].getVoltage() * params[TIMECV1_PARAM].getValue();
		float freq = clockFreq / 2.f * dsp::exp2_taylor5(pitch);
		// Number of desired delay samples
		float index = args.sampleRate / freq;
		// In order to delay accurate samples, subtract by the historyBuffer size, and an experimentally tweaked amount.
		index -= 16 + 4.f;
		index = clamp(index, 2.f, float(HISTORY_SIZE - 1));
		// DEBUG("freq %f index %f", freq, index);


		// Push dry sample into history buffer
		if (!historyBuffer.full()) {
			historyBuffer.push(dry);
		}

		if (outBuffer.empty()) {
			// How many samples do we need consume to catch up?
			float consume = index - historyBuffer.size();
			double ratio = std::pow(4.f, clamp(consume / 10000.f, -1.f, 1.f));
			// DEBUG("index %f historyBuffer %lu consume %f ratio %lf", index, historyBuffer.size(), consume, ratio);

			// Convert samples from the historyBuffer to catch up or slow down so `index` and `historyBuffer.size()` eventually match approximately
			SRC_DATA srcData = {};
			srcData.data_in = (const float*) historyBuffer.startData();
			srcData.data_out = (float*) outBuffer.endData();
			srcData.input_frames = std::min((int) historyBuffer.size(), 16);
			srcData.output_frames = outBuffer.capacity();
			srcData.end_of_input = false;
			srcData.src_ratio = ratio;
			if (src)
				src_process(src, &srcData);
			historyBuffer.startIncr(srcData.input_frames_used);
			outBuffer.endIncr(srcData.output_frames_gen);
			// DEBUG("used %ld gen %ld", srcData.input_frames_used, srcData.output_frames_gen);
		}

		float wet = 0.f;
		if (!outBuffer.empty()) {
			wet = outBuffer.shift();
		}
		wet = clamp(wet, -100.f, 100.f);

		// Apply color to delay wet output
		// float color = params[TONE_PARAM].getValue() + inputs[TONE_INPUT].getVoltage() / 10.f * params[TONE_CV_PARAM].getValue();
		// color = clamp(color, 0.f, 1.f);
		// float colorFreq = std::pow(100.f, 2.f * color - 1.f);

		// float lowpassFreq = clamp(20000.f * colorFreq, 20.f, 20000.f);
		// lowpassFilter.setCutoffFreq(lowpassFreq / args.sampleRate);
		// lowpassFilter.process(wet);
		// wet = lowpassFilter.lowpass();

		// float highpassFreq = clamp(20.f * colorFreq, 20.f, 20000.f);
		// highpassFilter.setCutoff(highpassFreq / args.sampleRate);
		// highpassFilter.process(wet);
		// wet = highpassFilter.highpass();

		// Set wet output
		//outputs[WET_OUTPUT].setVoltage(wet);
		lastWet = wet;

		// Set mix output
		float mix = params[MIX1_PARAM].getValue(); // + inputs[MIX_INPUT].getVoltage() / 10.f * params[MIX_CV_PARAM].getValue();
		//mix = clamp(mix, 0.f, 1.f);
		float out = crossfade(in, wet, mix);
		outputs[ONE_OUTPUT].setVoltage(out);

		// Clock light
		// clockPhase += freq * args.sampleTime;
		// if (clockPhase >= 1.f) {
		// 	clockPhase -= 1.f;
		// 	lights[CLOCK_LIGHT].setBrightness(1.f);
		// }
		// else {
		// 	lights[CLOCK_LIGHT].setBrightnessSmooth(0.f, args.sampleTime);
		// }
	}
};


struct TriParDelWidget : ModuleWidget {
	TriParDelWidget(TriParDel* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/TriParDel.svg")));

		addParam(createParamCentered<Davies1900hWhiteKnob>(Vec(96.086, 91.992), module, TriParDel::TIME1_PARAM));

        addParam(createParamCentered<Trimpot>(Vec(114.915, 138.942), module, TriParDel::TIMECV1_PARAM));
        
        addParam(createParamCentered<Davies1900hWhiteKnob>(Vec(96.086, 197.992), module, TriParDel::FEEDBACK1_PARAM));

        addParam(createParamCentered<Davies1900hWhiteKnob>(Vec(96.086, 265.992), module, TriParDel::MIX1_PARAM));


        addInput(createInputCentered<PJ301MPort>(Vec(96.082, 45.726), module, TriParDel::ONE_INPUT));
        addInput(createInputCentered<PJ301MPort>(Vec(77.913, 138.432), module, TriParDel::TIMECV1_INPUT));

        addOutput(createOutputCentered<PJ301MPort>(Vec(96.086, 317.714), module, TriParDel::ONE_OUTPUT));

        

	}
};


Model* modelTriParDel = createModel<TriParDel, TriParDelWidget>("TriParDel");