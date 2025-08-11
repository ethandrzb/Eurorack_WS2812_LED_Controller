/*
 * SimpleBreathingEffect.hpp
 *
 *  Created on: Jan 14, 2025
 *      Author: ethan
 */

#ifndef INC_WS2812FX_SIMPLEBREATHINGEFFECT_HPP_
#define INC_WS2812FX_SIMPLEBREATHINGEFFECT_HPP_

#include <WS2812FX/WS2812FX.hpp>
#include <main.h>

using namespace WS2812FX;

class SimpleBreathingEffect : public WS2812Effect
{
	public:
		SimpleBreathingEffect(uint8_t stepDelay, float stepSize, colorHSV hsv, float maxValue)
		{
			snprintf(this->name, WS2812FX_EFFECT_NAME_LEN, "Breathing");

			// Parameter 0: Step delay
			this->setParameter(NumericEffectParameter<uint8_t>(stepDelay, "Step Delay", 0, 100, 1), 0);

			// Parameter 1: Step size
			this->setParameter(NumericEffectParameter<float>(stepSize, "Step Size", 0.005, 0.1, 0.005), 1);

			// Parameter 2: HSV color
			this->setParameter(ColorHSVEffectParameter(hsv, "Color"), 2);

			// Parameter 3: Max color value
			this->setParameter(NumericEffectParameter<float>(maxValue, "Max Value", 0.05, 1, 0.05), 3);

			this->initModMatrixDefaults();
		}

		// Init default mod matrix for this effect
		void initModMatrixDefaults() override
		{
			this->modMatrix[0].modSource = NULL;
			this->modMatrix[0].modDestination = this->getParameter(0);
			this->modMatrix[0].modAmount->setValue(100);

			this->modMatrix[1].modSource = NULL;
			this->modMatrix[1].modDestination = this->getParameter(1);
			this->modMatrix[1].modAmount->setValue(100);

			this->modMatrix[2].modSource = NULL;
			this->modMatrix[2].modDestination = this->getParameter(3);
			this->modMatrix[2].modAmount->setValue(100);
		}

		void updateEffect() override;
};

#endif /* INC_WS2812FX_SIMPLEBREATHINGEFFECT_HPP_ */
