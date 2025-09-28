/*
 * CometEffect.cpp
 *
 *  Created on: Sep 28, 2025
 *      Author: ethan
 */

#include "WS2812FX/CometEffect.hpp"

// TODO: Change this function to be called repeatedly instead of blocking until a complete breathing cycle has completed
// The current behavior causes the next effect to be queued until this function finishes instead of immediately going to the new effect
void CometEffect::updateEffect()
{
	uint8_t stepDelay = *(static_cast<uint8_t *>(this->getParameter(0)->getValue()));
	uint8_t cometSize = *(static_cast<uint8_t *>(this->getParameter(1)->getValue()));
	colorHSV cometColor = *(static_cast<colorHSV *>(this->getParameter(2)->getValue()));
	uint8_t interval = *(static_cast<uint8_t *>(this->getParameter(3)->getValue()));

	static uint8_t iterations = 0;

	if(iterations > interval)
	{
		iterations = 0;

		colorRGB color = WS2812_HSVToRGB(cometColor.hue, cometColor.saturation, cometColor.value);

		WS2812_AddComet(color, cometSize);
	}

	WS2812_MultiCometEffect();
	WS2812_SendAll();
	iterations++;

	HAL_Delay(stepDelay);
}
