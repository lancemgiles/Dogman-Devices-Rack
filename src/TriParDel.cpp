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