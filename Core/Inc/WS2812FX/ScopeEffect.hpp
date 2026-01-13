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
			for(uint8_t i = 0; i < NUM_TRACES; i++)
			{
				this->modMatrix[i].modSource = NULL;
				this->modMatrix[i].modDestination = static_cast<ColorHSVEffectParameter *>(this->getParameter(1 + (NUM_TRACE_PARAMETERS * i)))->_value.get();
				this->modMatrix[i].modAmount->setValue(100);
			}
		}

		void updateEffect() override;
};


#endif /* INC_WS2812FX_SCOPEEFFECT_HPP_ */
