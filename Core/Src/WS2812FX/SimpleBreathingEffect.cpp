/*
 * SimpleBreathingEffect.cpp
 *
 *  Created on: Jan 13, 2025
 *      Author: ethan
 */

#include "WS2812FX/SimpleBreathingEffect.hpp"

// TODO: Change this function to be called repeatedly instead of blocking until a complete breathing cycle has completed
// The current behavior causes the next effect to be queued until this function finishes instead of immediately going to the new effect
void SimpleBreathingEffect::updateEffect()
{
	uint8_t stepDelay = *(static_cast<uint8_t *>(this->getParameter(0)->getValue()));
	float stepSize = *(static_cast<float *>(this->getParameter(1)->getValue()));
	colorHSV hsv = *(static_cast<colorHSV *>(this->getParameter(2)->getValue()));
	float maxValue = *(static_cast<float *>(this->getParameter(3)->getValue()));

	colorRGB rgb = WS2812_HSVToRGB(hsv.hue, hsv.saturation, hsv.value);

	for(float i = 0.0; i < maxValue; i += stepSize)
	{
		rgb = WS2812_HSVToRGB(hsv.hue, hsv.saturation, i);
		WS2812_SetAllLEDs(rgb.red, rgb.green, rgb.blue);
		WS2812_SendAll();
		HAL_Delay(stepDelay);
	}
	for(float i = maxValue; i >= 0; i -= stepSize)
	{
		rgb = WS2812_HSVToRGB(hsv.hue, hsv.saturation, i);
		WS2812_SetAllLEDs(rgb.red, rgb.green, rgb.blue);
		WS2812_SendAll();
		HAL_Delay(stepDelay);
	}
}
