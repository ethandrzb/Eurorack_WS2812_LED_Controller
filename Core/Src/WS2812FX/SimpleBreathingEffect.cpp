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
	colorHSV hsv = std::dynamic_pointer_cast<ColorHSVEffectParameter>(this->getParameter(2))->getValue();
	colorRGB rgb = WS2812_HSVToRGB(hsv.hue, hsv.saturation, hsv.value);

	for(float i = 0.0; i < std::dynamic_pointer_cast<NumericEffectParameter<float>>(this->getParameter(3))->getValue(); i += std::dynamic_pointer_cast<NumericEffectParameter<float>>(this->getParameter(1))->getValue())
	{
		rgb = WS2812_HSVToRGB(hsv.hue, hsv.saturation, i);
		WS2812_SetAllLEDs(rgb.red, rgb.green, rgb.blue);
		WS2812_SendAll();
		HAL_Delay(std::dynamic_pointer_cast<NumericEffectParameter<float>>(this->getParameter(0))->getValue());
	}
	for(float i = std::dynamic_pointer_cast<NumericEffectParameter<float>>(this->getParameter(3))->getValue(); i >= 0; i -= std::dynamic_pointer_cast<NumericEffectParameter<float>>(this->getParameter(1))->getValue())
	{
		rgb = WS2812_HSVToRGB(hsv.hue, hsv.saturation, i);
		WS2812_SetAllLEDs(rgb.red, rgb.green, rgb.blue);
		WS2812_SendAll();
		HAL_Delay(std::dynamic_pointer_cast<NumericEffectParameter<float>>(this->getParameter(0))->getValue());
	}
}
