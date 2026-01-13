/*
 * ScopeEffect.cpp
 *
 *  Created on: Jan 13, 2026
 *      Author: ethan
 */

#include "WS2812FX/ScopeEffect.hpp"

void ScopeEffect::updateEffect()
{
	colorHSV hsvColors[NUM_TRACES];
	colorRGB rgbColors[NUM_TRACES];
	uint8_t stepDelay = *(static_cast<uint8_t *>(this->getParameter(0)->getValue()));;

	for(uint8_t i = 0; i < NUM_TRACES; i++)
	{
		hsvColors[i] = *(static_cast<colorHSV *>(this->getParameter(1 + (NUM_TRACE_PARAMETERS * i))->getValue()));
		rgbColors[i] = WS2812_HSVToRGB(hsvColors[i].hue, hsvColors[i].saturation, hsvColors[i].value);
	}

	// Apply value to frame timer
	TIM7->ARR = stepDelay * 10;

	// Shift all LEDS 1 index down
	WS2812_ShiftLEDs(1);

	// Draw current value of each trace to first LED in strip
	for(uint8_t i = 0; i < NUM_TRACES; i++)
	{
		WS2812_SetLEDAdditive(0, rgbColors[i].red, rgbColors[i].green, rgbColors[i].blue);
	}

	WS2812_SendAll();
}



