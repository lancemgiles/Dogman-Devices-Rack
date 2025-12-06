#pragma once

struct Water : Module {
    float delay_out1 = 0.f;
    float delay_out2 = 0.f;
    float delay_out3 = 0.f;
    float delay_out = 0.f;
    constexpr static size_t HISTORY_SIZE = 1 << 21;
	dsp::DoubleRingBuffer<float, HISTORY_SIZE> historyBuffer[3];
	dsp::DoubleRingBuffer<float, 16> outBuffer[3];

    void delay(rack::engine::Param depth_param, rack::engine::Input audio_input, rack::engine::Input lfo_input, int mixer_input, float lastWet, SRC_STATE* src, const ProcessArgs& args) {
        // Get input to delay block
		float in = audio_input.getVoltageSum();
		float feedback = 0.15f;
		feedback = clamp(feedback, 0.f, 1.f);
		float dry = in + lastWet * feedback;
		int del_buf = output - 1;

		// Compute freq
		float pitch = std::log2(1000.f) - std::log2(10000.f) * log10(300);
		pitch += lfo_input.getVoltage() * 0.1f;
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
		float mix = mix_param.getValue(); // + inputs[MIX_INPUT].getVoltage() / 10.f * params[MIX_CV_PARAM].getValue();
		//mix = clamp(mix, 0.f, 1.f);
		float out = crossfade(in, wet, mix);
		//audio_output.setVoltage(out);
		switch(mixer_input) {
			case 1:
				delay_out1 = mixer_input;
				break;
			case 2:
				delay_out2 = mixer_input;
				break;
			case 3:
				delay_out3 = mixer_input;
				break;
		}
		
	}

    void delay_mixer(float delay1, float delay2, float delay3) {
        delay_out += delay_out1 + delay_out2 + delay_out3;
    }
};