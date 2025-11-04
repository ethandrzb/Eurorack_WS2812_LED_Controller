/*
 * SimpleBreathingEffect.cpp
 *
 *  Created on: Jan 13, 2025
 *      Author: ethan
 */

#include "WS2812FX/SimpleBreathingEffect.hpp"

void SimpleBreathingEffect::updateEffect()
{
	static bool direction = true;
	static float currentValue = 0.0;

	uint8_t stepDelay = *(static_cast<uint8_t *>(this->getParameter(0)->getValue()));
	float stepSize = *(static_cast<float *>(this->getParameter(1)->getValue()));
	colorHSV hsv = *(static_cast<colorHSV *>(this->getParameter(2)->getValue()));
	float maxValue = *(static_cast<float *>(this->getParameter(3)->getValue()));

	colorRGB rgb = WS2812_HSVToRGB(hsv.hue, hsv.saturation, hsv.value);

	// Apply value to frame timer
	TIM7->ARR = stepDelay * 10;

	if(direction)
	{
		// Increase to max value
		if(currentValue < maxValue)
		{
			rgb = WS2812_HSVToRGB(hsv.hue, hsv.saturation, currentValue);
			WS2812_SetAllLEDs(rgb.red, rgb.green, rgb.blue);
			WS2812_SendAll();

			currentValue += stepSize;
		}
		// Reverse direction when max value reached
		else
		{
			direction = !direction;
		}
	}
	else
	{
		// Decrease to 0 value
		if(currentValue > 0)
		{
			rgb = WS2812_HSVToRGB(hsv.hue, hsv.saturation, currentValue);
			WS2812_SetAllLEDs(rgb.red, rgb.green, rgb.blue);
			WS2812_SendAll();

			currentValue -= stepSize;
		}
		// Reverse direction when 0 value reached
		else
		{
			direction = !direction;
		}
	}
}
