/*
 * RainbowEffect.cpp
 *
 *  Created on: Feb 10, 2025
 *      Author: ethan
 */

#include "WS2812FX/RainbowEffect.hpp"

void RainbowEffect::updateEffect()
{
	int16_t density = *(static_cast<int16_t *>(this->getParameter(0)->getValue()));
	uint8_t scrollHue = *(static_cast<uint8_t *>(this->getParameter(1)->getValue()));
	uint8_t stepDelay = *(static_cast<uint8_t *>(this->getParameter(2)->getValue()));
	colorHSV hsv = *(static_cast<colorHSV *>(this->getParameter(3)->getValue()));
	bool manualMode = *(static_cast<bool *>(this->getParameter(4)->getValue()));

	static uint16_t currentHue = 0;

	currentHue = (currentHue < 360) ? currentHue + scrollHue : 0;

	if(!manualMode)
	{
		hsv.hue += currentHue;
		hsv.hue %= 360;
	}

	WS2812_FillRainbow(hsv, density);
	WS2812_SendAll();
//	HAL_Delay(stepDelay);
}
