/*
 * SimpleBreathingEffect.cpp
 *
 *  Created on: Jan 13, 2025
 *      Author: ethan
 */

#include <WS2812FX.hpp>
#include <main.h>

class SimpleBreathingEffect : public WS2812FX::WS2812Effect
{
	public:
		SimpleBreathingEffect(uint8_t stepDelay, float stepSize, colorHSV hsv, float maxValue)
		{
			// Parameter 0: Step delay
			this->params[0] = WS2812FX::NumericEffectParameter<uint8_t>(stepDelay, 0, 100, 1);

			// Parameter 1: Step size
			this->params[1] = WS2812FX::NumericEffectParameter<float>(stepSize, 0, 0.1, 0.005);

			// Parameter 2: HSV color
			this->params[2] = WS2812FX::EffectParameter<colorHSV>(hsv);

			// Parameter 3: Max color value
			this->params[3] = WS2812FX::NumericEffectParameter<float>(maxValue, 0.05, 1, 0.05);
		}

		// TODO: Change this function to be called repeatedly instead of blocking until a complete breathing cycle has completed
		// The current behavior causes the next effect to be queued until this function finishes instead of immediately going to the new effect
		void updateEffect()
		{
			colorHSV hsv = std::static_cast<colorHSV>(this->params[2]);
			colorRGB rgb = WS2812_HSVToRGB(this->params[2].hue, this->params[2].saturation, this->params[2].value);

			for(float i = 0.0; i < maxValue; i += this->params[1]->value)
			{
				rgb = WS2812_HSVToRGB(this->params[2].value.hue, this->params[2].value.saturation, i);
				WS2812_SetAllLEDs(rgb.red, rgb.green, rgb.blue);
				WS2812_SendAll();
				HAL_Delay(this->params[2].value);
			}
			for(float i = maxValue; i >= 0; i -= this->params[1]->value)
			{
				rgb = WS2812_HSVToRGB(this->params[2].value.hue, this->params[2].value.saturation, i);
				WS2812_SetAllLEDs(rgb.red, rgb.green, rgb.blue);
				WS2812_SendAll();
				HAL_Delay(this->params[2].value);
			}
		}
};
