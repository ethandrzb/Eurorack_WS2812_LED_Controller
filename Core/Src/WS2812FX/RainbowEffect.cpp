/*
 * RainbowEffect.cpp
 *
 *  Created on: Feb 10, 2025
 *      Author: ethan
 */

#include "WS2812FX/RainbowEffect.hpp"

void RainbowEffect::updateEffect()
{
	int8_t density = *(static_cast<int8_t *>(this->getParameter(0)->getValue()));
	uint8_t scrollHue = *(static_cast<uint8_t *>(this->getParameter(1)->getValue()));
	uint8_t stepDelay = *(static_cast<uint8_t *>(this->getParameter(2)->getValue()));
	colorHSV hsv = *(static_cast<colorHSV *>(this->getParameter(3)->getValue()));

	hsv.hue = (hsv.hue < 360) ? hsv.hue + scrollHue : 0;

	static_cast<ColorHSVEffectParameter *>(this->getParameter(3))->setValue(hsv);

	WS2812_FillRainbow(hsv, density);
	WS2812_SendAll();
	HAL_Delay(stepDelay);
}


