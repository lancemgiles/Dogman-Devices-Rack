#pragma once

#include <algorithm>
#include <functional>

struct Grammar : Module {
	enum ParamId {
		VOLTAGEA1_PARAM,
		VOLTAGEA2_PARAM,
		VOLTAGEA3_PARAM,
		VOLTAGEA4_PARAM,
		VOLTAGEA5_PARAM,
		VOLTAGEA6_PARAM,
		VOLTAGEA7_PARAM,
		VOLTAGEA8_PARAM,
		VOLTAGEA9_PARAM,
		VOLTAGEA10_PARAM,
		VOLTAGEA11_PARAM,
		VOLTAGEA12_PARAM,
		VOLTAGEASCALE_PARAM,
		SELECTA1_PARAM,
		SELECTA2_PARAM,
		SELECTA3_PARAM,
		SELECTA4_PARAM,
		SELECTA5_PARAM,
		SELECTA6_PARAM,
		SELECTA7_PARAM,
		SELECTA8_PARAM,
		SELECTA9_PARAM,
		SELECTA10_PARAM,
		SELECTA11_PARAM,
		SELECTA12_PARAM,
		VOLTAGEB1_PARAM,
		VOLTAGEB2_PARAM,
		VOLTAGEB3_PARAM,
		VOLTAGEB4_PARAM,
		VOLTAGEB5_PARAM,
		VOLTAGEB6_PARAM,
		VOLTAGEB7_PARAM,
		VOLTAGEB8_PARAM,
		VOLTAGEB9_PARAM,
		VOLTAGEB10_PARAM,
		VOLTAGEB11_PARAM,
		VOLTAGEB12_PARAM,
		VOLTAGEBSCALE_PARAM,
		VOLTAGEABCSCALE_PARAM,
		SELECTB1_PARAM,
		SELECTB2_PARAM,
		SELECTB3_PARAM,
		SELECTB4_PARAM,
		SELECTB5_PARAM,
		SELECTB6_PARAM,
		SELECTB7_PARAM,
		SELECTB8_PARAM,
		SELECTB9_PARAM,
		SELECTB10_PARAM,
		SELECTB11_PARAM,
		SELECTB12_PARAM,
		VOLTAGEC1_PARAM,
		VOLTAGEC2_PARAM,
		VOLTAGEC3_PARAM,
		VOLTAGEC4_PARAM,
		VOLTAGEC5_PARAM,
		VOLTAGEC6_PARAM,
		VOLTAGEC7_PARAM,
		VOLTAGEC8_PARAM,
		VOLTAGEC9_PARAM,
		VOLTAGEC10_PARAM,
		VOLTAGEC11_PARAM,
		VOLTAGEC12_PARAM,
		VOLTAGECSCALE_PARAM,
		SELECTC1_PARAM,
		SELECTC2_PARAM,
		SELECTC3_PARAM,
		SELECTC4_PARAM,
		SELECTC5_PARAM,
		SELECTC6_PARAM,
		SELECTC7_PARAM,
		SELECTC8_PARAM,
		SELECTC9_PARAM,
		SELECTC10_PARAM,
		SELECTC11_PARAM,
		SELECTC12_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		CLOCKA_INPUT,
		CLOCKABC_INPUT,
		SELECTA1_INPUT,
		SELECTA2_INPUT,
		SELECTA3_INPUT,
		SELECTA4_INPUT,
		SELECTA5_INPUT,
		SELECTA6_INPUT,
		SELECTA7_INPUT,
		SELECTA8_INPUT,
		SELECTA9_INPUT,
		SELECTA10_INPUT,
		SELECTA11_INPUT,
		SELECTA12_INPUT,
		CLOCKB_INPUT,
		SELECTB1_INPUT,
		SELECTB2_INPUT,
		SELECTB3_INPUT,
		SELECTB4_INPUT,
		SELECTB5_INPUT,
		SELECTB6_INPUT,
		SELECTB7_INPUT,
		SELECTB8_INPUT,
		SELECTB9_INPUT,
		SELECTB10_INPUT,
		SELECTB11_INPUT,
		SELECTB12_INPUT,
		CLOCKC_INPUT,
		SELECTC1_INPUT,
		SELECTC2_INPUT,
		SELECTC3_INPUT,
		SELECTC4_INPUT,
		SELECTC5_INPUT,
		SELECTC6_INPUT,
		SELECTC7_INPUT,
		SELECTC8_INPUT,
		SELECTC9_INPUT,
		SELECTC10_INPUT,
		SELECTC11_INPUT,
		SELECTC12_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		TRIGGERA1_OUTPUT,
		TRIGGERA2_OUTPUT,
		TRIGGERA3_OUTPUT,
		TRIGGERA4_OUTPUT,
		TRIGGERA5_OUTPUT,
		TRIGGERA6_OUTPUT,
		TRIGGERA7_OUTPUT,
		TRIGGERA8_OUTPUT,
		TRIGGERA9_OUTPUT,
		TRIGGERA10_OUTPUT,
		TRIGGERA11_OUTPUT,
		TRIGGERA12_OUTPUT,
		CVA_OUTPUT,
		GATEA_OUTPUT,
		GATEABC_OUTPUT,
		CVABC_OUTPUT,
		TRIGGERB1_OUTPUT,
		TRIGGERB2_OUTPUT,
		TRIGGERB3_OUTPUT,
		TRIGGERB4_OUTPUT,
		TRIGGERB5_OUTPUT,
		TRIGGERB6_OUTPUT,
		TRIGGERB7_OUTPUT,
		TRIGGERB8_OUTPUT,
		TRIGGERB9_OUTPUT,
		TRIGGERB10_OUTPUT,
		TRIGGERB11_OUTPUT,
		TRIGGERB12_OUTPUT,
		CVB_OUTPUT,
		GATEB_OUTPUT,
		TRIGGERC1_OUTPUT,
		TRIGGERC2_OUTPUT,
		TRIGGERC3_OUTPUT,
		TRIGGERC4_OUTPUT,
		TRIGGERC5_OUTPUT,
		TRIGGERC6_OUTPUT,
		TRIGGERC7_OUTPUT,
		TRIGGERC8_OUTPUT,
		TRIGGERC9_OUTPUT,
		TRIGGERC10_OUTPUT,
		TRIGGERC11_OUTPUT,
		TRIGGERC12_OUTPUT,
		CVC_OUTPUT,
		GATEC_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		TRIGGERA1_LIGHT,
		TRIGGERA2_LIGHT,
		TRIGGERA3_LIGHT,
		TRIGGERA4_LIGHT,
		TRIGGERA5_LIGHT,
		TRIGGERA6_LIGHT,
		TRIGGERA7_LIGHT,
		TRIGGERA8_LIGHT,
		TRIGGERA9_LIGHT,
		TRIGGERA10_LIGHT,
		TRIGGERA11_LIGHT,
		TRIGGERA12_LIGHT,
		TRIGGERB1_LIGHT,
		TRIGGERB2_LIGHT,
		TRIGGERB3_LIGHT,
		TRIGGERB4_LIGHT,
		TRIGGERB5_LIGHT,
		TRIGGERB6_LIGHT,
		TRIGGERB7_LIGHT,
		TRIGGERB8_LIGHT,
		TRIGGERB9_LIGHT,
		TRIGGERB10_LIGHT,
		TRIGGERB11_LIGHT,
		TRIGGERB12_LIGHT,
		TRIGGERC1_LIGHT,
		TRIGGERC2_LIGHT,
		TRIGGERC3_LIGHT,
		TRIGGERC4_LIGHT,
		TRIGGERC5_LIGHT,
		TRIGGERC6_LIGHT,
		TRIGGERC7_LIGHT,
		TRIGGERC8_LIGHT,
		TRIGGERC9_LIGHT,
		TRIGGERC10_LIGHT,
		TRIGGERC11_LIGHT,
		TRIGGERC12_LIGHT,
		LIGHTS_LEN
	};

	Grammar() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(VOLTAGEA1_PARAM, -10.f, 10.f, 0.f, "Voltage A1", " V");
		configParam(VOLTAGEA2_PARAM, -10.f, 10.f, 0.f, "Voltage A1", " V");
		configParam(VOLTAGEA3_PARAM, -10.f, 10.f, 0.f, "Voltage A1", " V");
		configParam(VOLTAGEA4_PARAM, -10.f, 10.f, 0.f, "Voltage A1", " V");
		configParam(VOLTAGEA5_PARAM, -10.f, 10.f, 0.f, "Voltage A1", " V");
		configParam(VOLTAGEA6_PARAM, -10.f, 10.f, 0.f, "Voltage A1", " V");
		configParam(VOLTAGEA7_PARAM, -10.f, 10.f, 0.f, "Voltage A1", " V");
		configParam(VOLTAGEA8_PARAM, -10.f, 10.f, 0.f, "Voltage A1", " V");
		configParam(VOLTAGEA9_PARAM, -10.f, 10.f, 0.f, "Voltage A1", " V");
		configParam(VOLTAGEA10_PARAM, -10.f, 10.f, 0.f, "Voltage A1", " V");
		configParam(VOLTAGEA11_PARAM, -10.f, 10.f, 0.f, "Voltage A1", " V");
		configParam(VOLTAGEA12_PARAM, -10.f, 10.f, 0.f, "Voltage A1", " V");
		configParam(VOLTAGEASCALE_PARAM, 0.f, 1.f, 1.f, "Voltage A Scale", "%", 0, 100);
		configButton(SELECTA1_PARAM, "Select Step A1");
		configButton(SELECTA2_PARAM, "Select Step A2");
		configButton(SELECTA3_PARAM, "Select Step A3");
		configButton(SELECTA4_PARAM, "Select Step A4");
		configButton(SELECTA5_PARAM, "Select Step A5");
		configButton(SELECTA6_PARAM, "Select Step A6");
		configButton(SELECTA7_PARAM, "Select Step A7");
		configButton(SELECTA8_PARAM, "Select Step A8");
		configButton(SELECTA9_PARAM, "Select Step A9");
		configButton(SELECTA10_PARAM, "Select Step A10");
		configButton(SELECTA11_PARAM, "Select Step A11");
		configButton(SELECTA12_PARAM, "Select Step A12");
		configParam(VOLTAGEB1_PARAM, -10.f, 10.f, 0.f, "Voltage B1", " V");
		configParam(VOLTAGEB2_PARAM, -10.f, 10.f, 0.f, "Voltage B2", " V");
		configParam(VOLTAGEB3_PARAM, -10.f, 10.f, 0.f, "Voltage B3", " V");
		configParam(VOLTAGEB4_PARAM, -10.f, 10.f, 0.f, "Voltage B4", " V");
		configParam(VOLTAGEB5_PARAM, -10.f, 10.f, 0.f, "Voltage B5", " V");
		configParam(VOLTAGEB6_PARAM, -10.f, 10.f, 0.f, "Voltage B6", " V");
		configParam(VOLTAGEB7_PARAM, -10.f, 10.f, 0.f, "Voltage B7", " V");
		configParam(VOLTAGEB8_PARAM, -10.f, 10.f, 0.f, "Voltage B8", " V");
		configParam(VOLTAGEB9_PARAM, -10.f, 10.f, 0.f, "Voltage B9", " V");
		configParam(VOLTAGEB10_PARAM, -10.f, 10.f, 0.f, "Voltage B10", " V");
		configParam(VOLTAGEB11_PARAM, -10.f, 10.f, 0.f, "Voltage B11", " V");
		configParam(VOLTAGEB12_PARAM, -10.f, 10.f, 0.f, "Voltage B12", " V");
		configParam(VOLTAGEBSCALE_PARAM, 0.f, 1.f, 1.f, "Voltage B Scale", "%", 0, 100);
		configParam(VOLTAGEABCSCALE_PARAM, 0.f, 1.f, 1.f, "Polyphonic Voltage Scale", "%", 0, 100);
		configButton(SELECTB1_PARAM, "Select Step B1");
		configButton(SELECTB2_PARAM, "Select Step B2");
		configButton(SELECTB3_PARAM, "Select Step B3");
		configButton(SELECTB4_PARAM, "Select Step B4");
		configButton(SELECTB5_PARAM, "Select Step B5");
		configButton(SELECTB6_PARAM, "Select Step B6");
		configButton(SELECTB7_PARAM, "Select Step B7");
		configButton(SELECTB8_PARAM, "Select Step B8");
		configButton(SELECTB9_PARAM, "Select Step B9");
		configButton(SELECTB10_PARAM, "Select Step B10");
		configButton(SELECTB11_PARAM, "Select Step B11");
		configButton(SELECTB12_PARAM, "Select Step B12");
		configParam(VOLTAGEC1_PARAM, -10.f, 10.f, 0.f, "Voltage C1", " V");
		configParam(VOLTAGEC2_PARAM, -10.f, 10.f, 0.f, "Voltage C2", " V");
		configParam(VOLTAGEC3_PARAM, -10.f, 10.f, 0.f, "Voltage C3", " V");
		configParam(VOLTAGEC4_PARAM, -10.f, 10.f, 0.f, "Voltage C4", " V");
		configParam(VOLTAGEC5_PARAM, -10.f, 10.f, 0.f, "Voltage C5", " V");
		configParam(VOLTAGEC6_PARAM, -10.f, 10.f, 0.f, "Voltage C6", " V");
		configParam(VOLTAGEC7_PARAM, -10.f, 10.f, 0.f, "Voltage C7", " V");
		configParam(VOLTAGEC8_PARAM, -10.f, 10.f, 0.f, "Voltage C8", " V");
		configParam(VOLTAGEC9_PARAM, -10.f, 10.f, 0.f, "Voltage C9", " V");
		configParam(VOLTAGEC10_PARAM, -10.f, 10.f, 0.f, "Voltage C10", " V");
		configParam(VOLTAGEC11_PARAM, -10.f, 10.f, 0.f, "Voltage C11", " V");
		configParam(VOLTAGEC12_PARAM, -10.f, 10.f, 0.f, "Voltage C12", " V");
		configParam(VOLTAGECSCALE_PARAM, 0.f, 1.f, 1.f, "Voltage C Scale", "%", 0, 100);
		configButton(SELECTC1_PARAM, "Select Step C1");
		configButton(SELECTC2_PARAM, "Select Step C2");
		configButton(SELECTC3_PARAM, "Select Step C3");
		configButton(SELECTC4_PARAM, "Select Step C4");
		configButton(SELECTC5_PARAM, "Select Step C5");
		configButton(SELECTC6_PARAM, "Select Step C6");
		configButton(SELECTC7_PARAM, "Select Step C7");
		configButton(SELECTC8_PARAM, "Select Step C8");
		configButton(SELECTC9_PARAM, "Select Step C9");
		configButton(SELECTC10_PARAM, "Select Step C10");
		configButton(SELECTC11_PARAM, "Select Step C11");
		configButton(SELECTC12_PARAM, "Select Step C12");
		configInput(CLOCKA_INPUT, "Clock A");
		configInput(CLOCKABC_INPUT, "Polyphonic Clock");
		configInput(SELECTA1_INPUT, "Select A1");
		configInput(SELECTA2_INPUT, "Select A2");
		configInput(SELECTA3_INPUT, "Select A3");
		configInput(SELECTA4_INPUT, "Select A4");
		configInput(SELECTA5_INPUT, "Select A5");
		configInput(SELECTA6_INPUT, "Select A6");
		configInput(SELECTA7_INPUT, "Select A7");
		configInput(SELECTA8_INPUT, "Select A8");
		configInput(SELECTA9_INPUT, "Select A9");
		configInput(SELECTA10_INPUT, "Select A10");
		configInput(SELECTA11_INPUT, "Select A11");
		configInput(SELECTA12_INPUT, "Select A12");
		configInput(CLOCKB_INPUT, "Clock B");
		configInput(SELECTB1_INPUT, "Select B1");
		configInput(SELECTB2_INPUT, "Select B2");;
		configInput(SELECTB3_INPUT, "Select B3");
		configInput(SELECTB4_INPUT, "Select B4");
		configInput(SELECTB5_INPUT, "Select B5");
		configInput(SELECTB6_INPUT, "Select B6");
		configInput(SELECTB7_INPUT, "Select B7");
		configInput(SELECTB8_INPUT, "Select B8");
		configInput(SELECTB9_INPUT, "Select B9");
		configInput(SELECTB10_INPUT, "Select B10");
		configInput(SELECTB11_INPUT, "Select B11");
		configInput(SELECTB12_INPUT, "Select B12");
		configInput(CLOCKC_INPUT, "Clock C");
		configInput(SELECTC1_INPUT, "Select C1");
		configInput(SELECTC2_INPUT, "Select C2");
		configInput(SELECTC3_INPUT, "Select C3");
		configInput(SELECTC4_INPUT, "Select C4");
		configInput(SELECTC5_INPUT, "Select C5");
		configInput(SELECTC6_INPUT, "Select C6");
		configInput(SELECTC7_INPUT, "Select C7");
		configInput(SELECTC8_INPUT, "Select C8");
		configInput(SELECTC9_INPUT, "Select C9");
		configInput(SELECTC10_INPUT, "Select C10");
		configInput(SELECTC11_INPUT, "Select C11");
		configInput(SELECTC12_INPUT, "Select C12");
		configOutput(TRIGGERA1_OUTPUT, "Step A1 Trigger");
		configOutput(TRIGGERA2_OUTPUT, "Step A2 Trigger");
		configOutput(TRIGGERA3_OUTPUT, "Step A3 Trigger");
		configOutput(TRIGGERA4_OUTPUT, "Step A4 Trigger");
		configOutput(TRIGGERA5_OUTPUT, "Step A5 Trigger");
		configOutput(TRIGGERA6_OUTPUT, "Step A6 Trigger");
		configOutput(TRIGGERA7_OUTPUT, "Step A7 Trigger");
		configOutput(TRIGGERA8_OUTPUT, "Step A8 Trigger");
		configOutput(TRIGGERA9_OUTPUT, "Step A9 Trigger");
		configOutput(TRIGGERA10_OUTPUT, "Step A10 Trigger");
		configOutput(TRIGGERA11_OUTPUT, "Step A11 Trigger");
		configOutput(TRIGGERA12_OUTPUT, "Step A12 Trigger");
		configOutput(CVA_OUTPUT, "CV A");
		configOutput(GATEA_OUTPUT, "Any A Step");
		configOutput(GATEABC_OUTPUT, "Any Polyphonic Step");
		configOutput(CVABC_OUTPUT, "Polyphonic CV");
		configOutput(TRIGGERB1_OUTPUT, "Step B1 Trigger");
		configOutput(TRIGGERB2_OUTPUT, "Step B2 Trigger");
		configOutput(TRIGGERB3_OUTPUT, "Step B3 Trigger");
		configOutput(TRIGGERB4_OUTPUT, "Step B4 Trigger");
		configOutput(TRIGGERB5_OUTPUT, "Step B5 Trigger");
		configOutput(TRIGGERB6_OUTPUT, "Step B6 Trigger");
		configOutput(TRIGGERB7_OUTPUT, "Step B7 Trigger");
		configOutput(TRIGGERB8_OUTPUT, "Step B8 Trigger");
		configOutput(TRIGGERB9_OUTPUT, "Step B9 Trigger");
		configOutput(TRIGGERB10_OUTPUT, "Step B10 Trigger");
		configOutput(TRIGGERB11_OUTPUT, "Step B11 Trigger");
		configOutput(TRIGGERB12_OUTPUT, "Step B12 Trigger");
		configOutput(CVB_OUTPUT, "CV B");
		configOutput(GATEB_OUTPUT, "Any B Step");
		configOutput(TRIGGERC1_OUTPUT, "Step C1 Trigger");
		configOutput(TRIGGERC2_OUTPUT, "Step C2 Trigger");
		configOutput(TRIGGERC3_OUTPUT, "Step C3 Trigger");
		configOutput(TRIGGERC4_OUTPUT, "Step C4 Trigger");
		configOutput(TRIGGERC5_OUTPUT, "Step C5 Trigger");
		configOutput(TRIGGERC6_OUTPUT, "Step C6 Trigger");
		configOutput(TRIGGERC7_OUTPUT, "Step C7 Trigger");
		configOutput(TRIGGERC8_OUTPUT, "Step C8 Trigger");
		configOutput(TRIGGERC9_OUTPUT, "Step C9 Trigger");
		configOutput(TRIGGERC10_OUTPUT, "Step C10 Trigger");
		configOutput(TRIGGERC11_OUTPUT, "Step C11 Trigger");
		configOutput(TRIGGERC12_OUTPUT, "Step C12 Trigger");
		configOutput(CVC_OUTPUT, "CV C");
		configOutput(GATEC_OUTPUT, "Any C Step");
	}

	void process(const ProcessArgs& args) override {
	}
};


struct GrammarWidget : ModuleWidget {
	GrammarWidget(Grammar* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Grammar.svg")));

		addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<Trimpot>(mm2px(Vec(12.524, 6.216)), module, Grammar::VOLTAGEA1_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(21.224, 6.216)), module, Grammar::VOLTAGEA2_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(30.172, 6.216)), module, Grammar::VOLTAGEA3_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(39.12, 6.216)), module, Grammar::VOLTAGEA4_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(48.068, 6.216)), module, Grammar::VOLTAGEA5_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(57.015, 6.216)), module, Grammar::VOLTAGEA6_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(65.963, 6.216)), module, Grammar::VOLTAGEA7_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(74.911, 6.216)), module, Grammar::VOLTAGEA8_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(83.858, 6.216)), module, Grammar::VOLTAGEA9_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(92.806, 6.216)), module, Grammar::VOLTAGEA10_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(101.754, 6.216)), module, Grammar::VOLTAGEA11_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(110.702, 6.216)), module, Grammar::VOLTAGEA12_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(121.247, 11.125)), module, Grammar::VOLTAGEASCALE_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(12.524, 34.24)), module, Grammar::SELECTA1_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(21.214, 34.47)), module, Grammar::SELECTA2_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(30.172, 34.543)), module, Grammar::SELECTA3_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(39.12, 34.543)), module, Grammar::SELECTA4_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(48.068, 34.543)), module, Grammar::SELECTA5_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(57.015, 34.543)), module, Grammar::SELECTA6_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(65.963, 34.543)), module, Grammar::SELECTA7_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(74.911, 34.543)), module, Grammar::SELECTA8_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(83.858, 34.543)), module, Grammar::SELECTA9_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(92.806, 34.543)), module, Grammar::SELECTA10_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(101.754, 34.543)), module, Grammar::SELECTA11_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(110.702, 34.543)), module, Grammar::SELECTA12_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(12.524, 48.549)), module, Grammar::VOLTAGEB1_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(21.224, 48.549)), module, Grammar::VOLTAGEB2_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(30.172, 48.549)), module, Grammar::VOLTAGEB3_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(39.12, 48.549)), module, Grammar::VOLTAGEB4_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(48.068, 48.549)), module, Grammar::VOLTAGEB5_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(57.015, 48.549)), module, Grammar::VOLTAGEB6_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(65.963, 48.549)), module, Grammar::VOLTAGEB7_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(74.911, 48.549)), module, Grammar::VOLTAGEB8_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(83.858, 48.549)), module, Grammar::VOLTAGEB9_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(92.806, 48.549)), module, Grammar::VOLTAGEB10_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(101.754, 48.549)), module, Grammar::VOLTAGEB11_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(110.702, 48.549)), module, Grammar::VOLTAGEB12_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(121.247, 53.458)), module, Grammar::VOLTAGEBSCALE_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(144.044, 66.125)), module, Grammar::VOLTAGEABCSCALE_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(12.524, 76.574)), module, Grammar::SELECTB1_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(21.214, 76.803)), module, Grammar::SELECTB2_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(30.172, 76.877)), module, Grammar::SELECTB3_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(39.12, 76.877)), module, Grammar::SELECTB4_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(48.068, 76.877)), module, Grammar::SELECTB5_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(57.015, 76.877)), module, Grammar::SELECTB6_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(65.963, 76.877)), module, Grammar::SELECTB7_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(74.911, 76.877)), module, Grammar::SELECTB8_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(83.858, 76.877)), module, Grammar::SELECTB9_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(92.806, 76.877)), module, Grammar::SELECTB10_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(101.754, 76.877)), module, Grammar::SELECTB11_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(110.702, 76.877)), module, Grammar::SELECTB12_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(12.524, 90.882)), module, Grammar::VOLTAGEC1_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(21.224, 90.882)), module, Grammar::VOLTAGEC2_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(30.172, 90.882)), module, Grammar::VOLTAGEC3_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(39.12, 90.882)), module, Grammar::VOLTAGEC4_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(48.068, 90.882)), module, Grammar::VOLTAGEC5_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(57.015, 90.882)), module, Grammar::VOLTAGEC6_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(65.963, 90.882)), module, Grammar::VOLTAGEC7_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(74.911, 90.882)), module, Grammar::VOLTAGEC8_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(83.858, 90.882)), module, Grammar::VOLTAGEC9_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(92.806, 90.882)), module, Grammar::VOLTAGEC10_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(101.754, 90.882)), module, Grammar::VOLTAGEC11_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(110.702, 90.882)), module, Grammar::VOLTAGEC12_PARAM));
		addParam(createParamCentered<Trimpot>(mm2px(Vec(121.247, 95.791)), module, Grammar::VOLTAGECSCALE_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(12.524, 118.907)), module, Grammar::SELECTC1_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(21.214, 119.136)), module, Grammar::SELECTC2_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(30.172, 119.21)), module, Grammar::SELECTC3_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(39.12, 119.21)), module, Grammar::SELECTC4_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(48.068, 119.21)), module, Grammar::SELECTC5_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(57.015, 119.21)), module, Grammar::SELECTC6_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(65.963, 119.21)), module, Grammar::SELECTC7_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(74.911, 119.21)), module, Grammar::SELECTC8_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(83.858, 119.21)), module, Grammar::SELECTC9_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(92.806, 119.21)), module, Grammar::SELECTC10_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(101.754, 119.21)), module, Grammar::SELECTC11_PARAM));
		addParam(createParamCentered<VCVButton>(mm2px(Vec(110.702, 119.21)), module, Grammar::SELECTC12_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(130.857, 11.125)), module, Grammar::CLOCKA_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(144.044, 17.057)), module, Grammar::CLOCKABC_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.524, 26.497)), module, Grammar::SELECTA1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(21.224, 26.497)), module, Grammar::SELECTA2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30.172, 26.497)), module, Grammar::SELECTA3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.12, 26.497)), module, Grammar::SELECTA4_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(48.068, 26.497)), module, Grammar::SELECTA5_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(57.015, 26.497)), module, Grammar::SELECTA6_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(65.963, 26.497)), module, Grammar::SELECTA7_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(74.911, 26.497)), module, Grammar::SELECTA8_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(83.858, 26.497)), module, Grammar::SELECTA9_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(92.806, 26.497)), module, Grammar::SELECTA10_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(101.754, 26.497)), module, Grammar::SELECTA11_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(110.702, 26.497)), module, Grammar::SELECTA12_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(130.857, 53.458)), module, Grammar::CLOCKB_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.524, 68.83)), module, Grammar::SELECTB1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(21.224, 68.83)), module, Grammar::SELECTB2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30.172, 68.83)), module, Grammar::SELECTB3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.12, 68.83)), module, Grammar::SELECTB4_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(48.068, 68.83)), module, Grammar::SELECTB5_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(57.015, 68.83)), module, Grammar::SELECTB6_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(65.963, 68.83)), module, Grammar::SELECTB7_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(74.911, 68.83)), module, Grammar::SELECTB8_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(83.858, 68.83)), module, Grammar::SELECTB9_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(92.806, 68.83)), module, Grammar::SELECTB10_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(101.754, 68.83)), module, Grammar::SELECTB11_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(110.702, 68.83)), module, Grammar::SELECTB12_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(130.857, 95.791)), module, Grammar::CLOCKC_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(12.524, 111.164)), module, Grammar::SELECTC1_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(21.224, 111.164)), module, Grammar::SELECTC2_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(30.172, 111.164)), module, Grammar::SELECTC3_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(39.12, 111.164)), module, Grammar::SELECTC4_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(48.068, 111.164)), module, Grammar::SELECTC5_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(57.015, 111.164)), module, Grammar::SELECTC6_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(65.963, 111.164)), module, Grammar::SELECTC7_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(74.911, 111.164)), module, Grammar::SELECTC8_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(83.858, 111.164)), module, Grammar::SELECTC9_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(92.806, 111.164)), module, Grammar::SELECTC10_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(101.754, 111.164)), module, Grammar::SELECTC11_INPUT));
		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(110.702, 111.164)), module, Grammar::SELECTC12_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.524, 16.358)), module, Grammar::TRIGGERA1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.224, 16.358)), module, Grammar::TRIGGERA2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(30.172, 16.358)), module, Grammar::TRIGGERA3_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(39.12, 16.358)), module, Grammar::TRIGGERA4_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(48.068, 16.358)), module, Grammar::TRIGGERA5_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(57.015, 16.358)), module, Grammar::TRIGGERA6_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(65.963, 16.358)), module, Grammar::TRIGGERA7_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(74.911, 16.358)), module, Grammar::TRIGGERA8_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(83.858, 16.358)), module, Grammar::TRIGGERA9_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(92.806, 16.358)), module, Grammar::TRIGGERA10_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(101.754, 16.358)), module, Grammar::TRIGGERA11_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(110.702, 16.358)), module, Grammar::TRIGGERA12_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(121.247, 26.039)), module, Grammar::CVA_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(130.867, 26.039)), module, Grammar::GATEA_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(144.044, 33.089)), module, Grammar::GATEABC_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(144.044, 50.772)), module, Grammar::CVABC_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.524, 58.692)), module, Grammar::TRIGGERB1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.224, 58.692)), module, Grammar::TRIGGERB2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(30.172, 58.692)), module, Grammar::TRIGGERB3_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(39.12, 58.692)), module, Grammar::TRIGGERB4_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(48.068, 58.692)), module, Grammar::TRIGGERB5_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(57.015, 58.692)), module, Grammar::TRIGGERB6_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(65.963, 58.692)), module, Grammar::TRIGGERB7_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(74.911, 58.692)), module, Grammar::TRIGGERB8_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(83.858, 58.692)), module, Grammar::TRIGGERB9_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(92.806, 58.692)), module, Grammar::TRIGGERB10_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(101.754, 58.692)), module, Grammar::TRIGGERB11_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(110.702, 58.692)), module, Grammar::TRIGGERB12_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(121.247, 68.372)), module, Grammar::CVB_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(130.867, 68.372)), module, Grammar::GATEB_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(12.524, 101.025)), module, Grammar::TRIGGERC1_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(21.224, 101.025)), module, Grammar::TRIGGERC2_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(30.172, 101.025)), module, Grammar::TRIGGERC3_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(39.12, 101.025)), module, Grammar::TRIGGERC4_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(48.068, 101.025)), module, Grammar::TRIGGERC5_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(57.015, 101.025)), module, Grammar::TRIGGERC6_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(65.963, 101.025)), module, Grammar::TRIGGERC7_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(74.911, 101.025)), module, Grammar::TRIGGERC8_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(83.858, 101.025)), module, Grammar::TRIGGERC9_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(92.806, 101.025)), module, Grammar::TRIGGERC10_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(101.754, 101.025)), module, Grammar::TRIGGERC11_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(110.702, 101.025)), module, Grammar::TRIGGERC12_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(121.247, 110.705)), module, Grammar::CVC_OUTPUT));
		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(130.867, 110.705)), module, Grammar::GATEC_OUTPUT));

		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(16.9, 14.358)), module, Grammar::TRIGGERA1_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(25.917, 14.358)), module, Grammar::TRIGGERA2_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(34.844, 14.358)), module, Grammar::TRIGGERA3_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(43.771, 14.358)), module, Grammar::TRIGGERA4_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(52.698, 14.358)), module, Grammar::TRIGGERA5_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(61.625, 14.358)), module, Grammar::TRIGGERA6_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(70.552, 14.358)), module, Grammar::TRIGGERA7_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(79.479, 14.358)), module, Grammar::TRIGGERA8_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(88.406, 14.358)), module, Grammar::TRIGGERA9_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(97.333, 14.358)), module, Grammar::TRIGGERA10_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(106.26, 14.358)), module, Grammar::TRIGGERA11_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(115.187, 14.358)), module, Grammar::TRIGGERA12_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(16.99, 56.692)), module, Grammar::TRIGGERB1_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(25.917, 56.692)), module, Grammar::TRIGGERB2_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(34.844, 56.692)), module, Grammar::TRIGGERB3_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(43.771, 56.692)), module, Grammar::TRIGGERB4_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(52.698, 56.692)), module, Grammar::TRIGGERB5_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(61.625, 56.692)), module, Grammar::TRIGGERB6_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(70.552, 56.692)), module, Grammar::TRIGGERB7_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(79.479, 56.692)), module, Grammar::TRIGGERB8_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(88.406, 56.692)), module, Grammar::TRIGGERB9_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(97.333, 56.692)), module, Grammar::TRIGGERB10_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(106.26, 56.692)), module, Grammar::TRIGGERB11_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(115.187, 56.692)), module, Grammar::TRIGGERB12_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(16.99, 99.025)), module, Grammar::TRIGGERC1_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(25.917, 99.025)), module, Grammar::TRIGGERC2_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(34.844, 99.025)), module, Grammar::TRIGGERC3_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(43.771, 99.025)), module, Grammar::TRIGGERC4_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(52.698, 99.025)), module, Grammar::TRIGGERC5_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(61.625, 99.025)), module, Grammar::TRIGGERC6_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(70.552, 99.025)), module, Grammar::TRIGGERC7_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(79.479, 99.025)), module, Grammar::TRIGGERC8_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(88.406, 99.025)), module, Grammar::TRIGGERC9_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(97.333, 99.025)), module, Grammar::TRIGGERC10_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(106.26, 99.025)), module, Grammar::TRIGGERC11_LIGHT));
		addChild(createLightCentered<TinyLight<WhiteLight>>(mm2px(Vec(115.187, 99.025)), module, Grammar::TRIGGERC12_LIGHT));
	}
};


Model* modelGrammar = createModel<Grammar, GrammarWidget>("Grammar");