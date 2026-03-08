/*
 * CometEffect.cpp
 *
 *  Created on: Sep 28, 2025
 *      Author: ethan
 */

#include "WS2812FX/CometEffect.hpp"

void CometEffect::updateEffect()
{
	// FOR AUTOMATICALLY SPAWNED COMETS
	///////////////////////////////////
	// Number of pixels occupied by comet body
	uint8_t cometSize = *(static_cast<uint8_t *>(this->getParameter(0)->getValue()));
	// Comet color
	colorHSV cometColor = *(static_cast<colorHSV *>(this->getParameter(1)->getValue()));
	// Period between movement increments (I know the name is counter-intuitive)
	uint8_t speed = *(static_cast<uint8_t *>(this->getParameter(2)->getValue()));
	// Interval between automatically spawned comets. Setting this to zero disables automatic spawning
	uint8_t interval = *(static_cast<uint8_t *>(this->getParameter(3)->getValue()));
	// Determines whether comets travel towards the end of the strip (true) or towards LED index 0 (false)
	bool direction = *(static_cast<bool *>(this->getParameter(4)->getValue()));
	///////////////////////////////////

	// Number of iterations since last spotlight was spawned in automatic mode
	static uint8_t spawnIterationCounter = 0;

	// Use static frame period
	TIM7->ARR = 50;

	if((interval > 0) && (spawnIterationCounter > interval))
	{
		spawnIterationCounter = 0;

		colorRGB color = WS2812_HSVToRGB(cometColor.hue, cometColor.saturation, cometColor.value);

		WS2812_AddComet(color, cometSize, speed, direction);
	}

	WS2812_MultiCometEffect();
	WS2812_SendAll();
	spawnIterationCounter++;
}

void CometEffect::trig0Callback(void)
{
	colorRGB color = {.red = 128, .green = 128, .blue = 128};

	WS2812_AddComet(color, 5, 4, true);
}

void CometEffect::trig1Callback(void)
{
	colorRGB color = {.red = 168, .green = 0, .blue = 0};

	WS2812_AddComet(color, 1, 1, false);
}

void CometEffect::trig2Callback(void)
{
	colorRGB color = {.red = 0, .green = 128, .blue = 0};

	WS2812_AddComet(color, 3, 3, true);
}

void CometEffect::trig3Callback(void)
{
	colorRGB color = {.red = 0, .green = 0, .blue = 128};

	WS2812_AddComet(color, 2, 2, false);
}
