/*
 * CometEffect.cpp
 *
 *  Created on: Sep 28, 2025
 *      Author: ethan
 */

#include "WS2812FX/CometEffect.hpp"

void CometEffect::updateEffect()
{
	uint8_t stepDelay = *(static_cast<uint8_t *>(this->getParameter(0)->getValue()));
	uint8_t cometSize = *(static_cast<uint8_t *>(this->getParameter(1)->getValue()));
	colorHSV cometColor = *(static_cast<colorHSV *>(this->getParameter(2)->getValue()));
	uint8_t interval = *(static_cast<uint8_t *>(this->getParameter(3)->getValue()));
	bool triggerComet = *(static_cast<bool *>(this->getParameter(4)->getValue()));

	static uint8_t iterations = 0;

	// Apply value to frame timer
	TIM7->ARR = stepDelay * 10;

	if(iterations > interval)
	{
		iterations = 0;

		colorRGB color = WS2812_HSVToRGB(cometColor.hue, cometColor.saturation, cometColor.value);

		WS2812_AddComet(color, cometSize);
	}

	if(triggerComet)
	{
		colorRGB color = {.red = 128, .green = 128, .blue = 128};

		WS2812_AddComet(color, cometSize);

		(static_cast<BooleanEffectParameter *>(this->getParameter(4)))->setValue(false);
	}

	WS2812_MultiCometEffect();
	WS2812_SendAll();
	iterations++;
}
