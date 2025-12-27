#include "plugin.hpp"
#include <samplerate.h>

using simd::float_4;


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
		configParam(RATE_PARAM, 0.001f, 5.f, 2.5f, "Chorus and Tremolo Rate", "%", 0, 100);
		configParam(TREMOLODEPTH_PARAM, 0.f, 1.f, 0.5f, "Tremolo Depth", "%", 0, 100);
		configParam(CHORUSCV_PARAM, 0.f, 1.f, 0.f, "Chorus Depth CV");
		configParam(RATECV_PARAM, 0.f, 1.f, 0.f, "Rate CV");
		configParam(TREMOLOCV_PARAM, 0.f, 1.f, 0.f, "Tremolo Depth CV", "%", 0, 100);
		configInput(CHORUSCV_INPUT, "Chorus Depth CV");
		configInput(RATECV_INPUT, "Chorus and Tremolo Rate CV");
		configInput(TREMOLOCV_INPUT, "Tremolo Depth CV");
		configInput(AUDIO_INPUT, "Audio");
		configOutput(AUDIO_OUTPUT, "Audio");

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

	float trem_out = 0.f;

	~Water() {
		if (src1)
			src_delete(src1);
		if (src2)
			src_delete(src2);
		if (src3)
			src_delete(src3);
	}

	void delay(float lfo_input, int mixer_input, float lastWet, SRC_STATE* src, const ProcessArgs& args) {

        // Get input to delay block
		float in = inputs[AUDIO_INPUT].getVoltage();
		float feedback = 0.15f;
		// feedback = clamp(feedback, 0.f, 1.f);
		float dry = in + lastWet * feedback;
		int del_buf = mixer_input - 1;

		// Compute freq
		float pitch = std::log2(1000.f) - std::log2(10000.f) * (log10(0.003f * 1000) / 4);
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
		float mix = params[CHORUSDEPTH_PARAM].getValue() + inputs[CHORUSCV_INPUT].getVoltage() * params[CHORUSDEPTH_PARAM].getValue();
		//mix = clamp(mix, 0.f, 1.f);
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
        delay_out += (delay_out1 * 0.333f) + (delay_out2 * 0.333f) + (delay_out3 * 0.333f);
    }

	float sine_phases;
	float chorus_lfo;
	// update value of chorus sine waves
	void sine_lfo(const ProcessArgs& args) {
		// pitch and frequency
		float rateCV = clamp(inputs[RATECV_INPUT].getVoltage() * params[RATECV_PARAM].getValue() + 0.183f,-5.f, 5.f);
		float combinedRate = 2.256f * params[RATE_PARAM].getValue() + rateCV - 4.f;
		float freq = clockFreq / 2.f * dsp::exp2_taylor5(combinedRate);

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

	float triangle_phase = 0.f;


	void tremolo(const ProcessArgs& args) {
		//float p = triangle_phase;
		INFO("triangle_phase: %f", triangle_phase);
		float rate = params[RATE_PARAM].getValue();
		rate += inputs[RATECV_INPUT].getVoltage() * params[RATECV_PARAM].getValue();
		// float freq = dsp::exp2_taylor5(rate);
		//rate *= 0.1f;

		float deltaPhase = std::min(rate * args.sampleTime, 0.5f);
		triangle_phase += deltaPhase;
		triangle_phase -= std::trunc(triangle_phase);

		// Triangle wave
		
		triangle_phase += 0.25f;
		//float wave = 4.f * std::fabsf(triangle_phase - round(triangle_phase)) - 1.f;
		float lfo = std::sin(2 * M_PI * triangle_phase);
		INFO("wave: %f", lfo);

		float dry = chorus_out;

		// Tremolo Depth
		float depth = params[TREMOLODEPTH_PARAM].getValue();
		float tDepthCV = clamp(inputs[TREMOLOCV_INPUT].getVoltage() / 10.f, 0.f, 1.f);
		tDepthCV *= params[TREMOLOCV_PARAM].getValue();
		depth += tDepthCV;

		// Apply tremolo
		float wet = clamp(dry * lfo, -10.f, 10.f);
		INFO("trem_lfo: %f", lfo);

		trem_out = crossfade(dry, wet, depth);


	}


	void process(const ProcessArgs& args) override {
		// Chorus
		sine_lfo(args);
		delay(lfo1, 1, lastWet1, src1, args);
		delay(lfo2, 2, lastWet2, src2, args);
		delay(lfo3, 3, lastWet3, src3, args);
		delay_mixer(delay_out1, delay_out2, delay_out3);
		chorus_out = clamp(delay_out, -10.f, 10.f);
		delay_out = 0.f;

		//outputs[AUDIO_OUTPUT].setVoltage(chorus_out);

		// Tremolo
		tremolo(args);
		// float blinkPhaseT = 0.f;
		// float dry = inputs[AUDIO_INPUT].getVoltage();
		// float tPhase = 0.75f;
		// // Tremolo Rate
		// float tRate = params[RATE_PARAM].getValue() * 5.89f;
		// float tFreq = std::pow(2.f, tRate);
		// tPhase += tFreq * args.sampleTime;
		// if (tPhase > 10.f)
		// 	tPhase -= 10.f;
		// // this converts the phase to radians, sine of which is -1...1 -> make -5...5 and add 5 to make unipolar
		// //float tLFO = (std::sin(2.f * M_PI * tPhase) * 5.f) + 5;
		// // Triangle Wave
		// float tLFO = 5.f * (4.f * std::fabsf(tPhase - std::roundf(tPhase)) - 1.f);

		// // Tremolo Rate Light
		// lights[TREMOLORATE_LIGHT].setBrightness(tLFO);

		
		

		
		float out = clamp(trem_out + chorus_out, -10.f, 10.f);
		outputs[AUDIO_OUTPUT].setVoltage(out);



	}
};


struct WaterWidget : ModuleWidget {
	WaterWidget(Water* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Water.svg")));

		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		// addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		// addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

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
