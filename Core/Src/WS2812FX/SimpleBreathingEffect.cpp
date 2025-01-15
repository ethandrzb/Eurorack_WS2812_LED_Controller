/*
 * SimpleBreathingEffect.cpp
 *
 *  Created on: Jan 13, 2025
 *      Author: ethan
 */

#include <WS2812FX.hpp>
#include <main.h>

using namespace WS2812FX;

class SimpleBreathingEffect : public WS2812Effect
{
	public:
		SimpleBreathingEffect(uint8_t stepDelay, float stepSize, colorHSV hsv, float maxValue)
		{
			// Parameter 0: Step delay
			this->setParameter(NumericEffectParameter<uint8_t>(stepDelay, 0, 100, 1), 0);

			// Parameter 1: Step size
			this->setParameter(NumericEffectParameter<float>(stepSize, 0, 0.1, 0.005), 1);

			// Parameter 2: HSV color
			this->setParameter(ColorHSVEffectParameter(hsv), 2);

			// Parameter 3: Max color value
			this->setParameter(NumericEffectParameter<float>(maxValue, 0.05, 1, 0.05), 3);
		}

		// TODO: Change this function to be called repeatedly instead of blocking until a complete breathing cycle has completed
		// The current behavior causes the next effect to be queued until this function finishes instead of immediately going to the new effect
		void updateEffect() override
		{
//			ColorHSVEffectParameter *tmp = dynamic_cast<ColorHSVEffectParameter *>(this->getParameter(2));
			colorHSV hsv = dynamic_cast<ColorHSVEffectParameter *>(this->getParameter(2))->getValue();
			colorRGB rgb = WS2812_HSVToRGB(hsv.hue, hsv.saturation, hsv.value);

			for(float i = 0.0; i < dynamic_cast<NumericEffectParameter<float> *>(this->getParameter(3))->getValue(); i += dynamic_cast<NumericEffectParameter<float> *>(this->getParameter(1))->getValue())
			{
				rgb = WS2812_HSVToRGB(hsv.hue, hsv.saturation, i);
				WS2812_SetAllLEDs(rgb.red, rgb.green, rgb.blue);
				WS2812_SendAll();
				HAL_Delay(dynamic_cast<NumericEffectParameter<float> *>(this->getParameter(0))->getValue());
			}
			for(float i = dynamic_cast<NumericEffectParameter<float> *>(this->getParameter(3))->getValue(); i >= 0; i -= dynamic_cast<NumericEffectParameter<float> *>(this->getParameter(1))->getValue())
			{
				rgb = WS2812_HSVToRGB(hsv.hue, hsv.saturation, i);
				WS2812_SetAllLEDs(rgb.red, rgb.green, rgb.blue);
				WS2812_SendAll();
				HAL_Delay(dynamic_cast<NumericEffectParameter<float> *>(this->getParameter(0))->getValue());
			}
		}
};
