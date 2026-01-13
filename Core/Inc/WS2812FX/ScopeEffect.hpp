/*
 * ScopeEffect.hpp
 *
 *  Created on: Jan 13, 2025
 *      Author: ethan
 */

#ifndef INC_WS2812FX_SCOPEEFFECT_HPP_
#define INC_WS2812FX_SCOPEEFFECT_HPP_


#include <WS2812FX/WS2812FX.hpp>
#include <main.h>

using namespace WS2812FX;

class ScopeEffect : public WS2812Effect
{
	const uint8_t NUM_TRACES = 4;
	const uint8_t NUM_TRACE_PARAMETERS = 1;
	public:
		ScopeEffect(uint8_t stepDelay, colorHSV hsv)
		{
			snprintf(this->name, WS2812FX_EFFECT_NAME_LEN, "Scope");

			// Parameter 0: Scroll step delay
			this->setParameter(NumericEffectParameter<uint8_t>(stepDelay, "Step delay", 1, 100, 1), 0);

			for(uint8_t i = 0; i < NUM_TRACES; i++)
			{
				// Parameter Index | Description
				//     i + 1       | Meter i color
				this->setParameter(ColorHSVEffectParameter(hsv, "Color " + std::to_string(i)), 1 + (NUM_TRACE_PARAMETERS * i));

				// Vary initial color
				hsv.hue = (hsv.hue + 90) % 360;
			}

			this->initModMatrixDefaults();
		}

		// Init default mod matrix for this effect
		void initModMatrixDefaults() override
		{
			this->modMatrix[0].modSource = NULL;
			this->modMatrix[0].modDestination = this->getParameter(3);
			this->modMatrix[0].modAmount->setValue(100);

			this->modMatrix[1].modSource = NULL;
			this->modMatrix[1].modDestination = this->getParameter(6);
			this->modMatrix[1].modAmount->setValue(100);

			this->modMatrix[2].modSource = NULL;
			this->modMatrix[2].modDestination = this->getParameter(9);
			this->modMatrix[2].modAmount->setValue(100);

			this->modMatrix[2].modSource = NULL;
			this->modMatrix[2].modDestination = this->getParameter(12);
			this->modMatrix[2].modAmount->setValue(100);
		}

		void updateEffect() override;
};


#endif /* INC_WS2812FX_SCOPEEFFECT_HPP_ */
