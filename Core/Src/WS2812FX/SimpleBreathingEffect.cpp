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
	colorHSV hsv = this->getParameter<ColorHSVEffectParameter>(2)->getValue();
	colorRGB rgb = WS2812_HSVToRGB(hsv.hue, hsv.saturation, hsv.value);

	for(float i = 0.0; i < this->getParameter<NumericEffectParameter<float>>(3)->getValue(); i += this->getParameter<NumericEffectParameter<float>>(1)->getValue())
	{
		rgb = WS2812_HSVToRGB(hsv.hue, hsv.saturation, i);
		WS2812_SetAllLEDs(rgb.red, rgb.green, rgb.blue);
		WS2812_SendAll();
		HAL_Delay(this->getParameter<NumericEffectParameter<uint8_t>>(0)->getValue());
	}
	for(float i = this->getParameter<NumericEffectParameter<float>>(3)->getValue(); i >= 0; i -= this->getParameter<NumericEffectParameter<float>>(1)->getValue())
	{
		rgb = WS2812_HSVToRGB(hsv.hue, hsv.saturation, i);
		WS2812_SetAllLEDs(rgb.red, rgb.green, rgb.blue);
		WS2812_SendAll();
		HAL_Delay(this->getParameter<NumericEffectParameter<uint8_t>>(0)->getValue());
	}
}

char *SimpleBreathingEffect::getValueStringByIndex(uint8_t index)
{
	if(index < WS2812FX_EFFECT_NUM_PARAMS)
	{
		switch(index)
		{
			case 0:
				return this->getParameter<NumericEffectParameter<uint8_t>>(index)->getValueString<uint8_t>();
				break;
			case 1:
				return this->getParameter<NumericEffectParameter<float>>(index)->getValueString<float>();
				break;
			case 2:
				return this->getParameter<ColorHSVEffectParameter>(index)->getValueString();
				break;
			case 3:
				return this->getParameter<NumericEffectParameter<float>>(index)->getValueString<float>();
				break;
			default:
				return NULL;
				break;
		};
	}
	else
	{
		return NULL;
	}
}
