// Placeholder for custom knobs

#pragma once


struct Trimpot : app::SvgKnob {
	widget::SvgWidget* bg;

	Trimpot() {
		minAngle = -0.75 * M_PI;
		maxAngle = 0.75 * M_PI;

		bg = new widget::SvgWidget;
		fb->addChildBelow(bg, tw);

		setSvg(Svg::load(asset::system("res/ComponentLibrary/Trimpot.svg")));
		bg->setSvg(Svg::load(asset::system("res/ComponentLibrary/Trimpot_bg.svg")));
	}
};