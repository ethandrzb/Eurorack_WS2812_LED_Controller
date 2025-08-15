/*
 * RainbowEffect.hpp
 *
 *  Created on: Feb 10, 2025
 *      Author: ethan
 */

#ifndef INC_WS2812FX_RAINBOWEFFECT_HPP_
#define INC_WS2812FX_RAINBOWEFFECT_HPP_


#include <WS2812FX/WS2812FX.hpp>
#include <main.h>

using namespace WS2812FX;

//TODO: Add parameter to manually reset rainbow to (new) starting color
class RainbowEffect : public WS2812Effect
{
	public:
		RainbowEffect(uint8_t density, int16_t scrollHue, uint8_t stepDelay, colorHSV startHSV, bool manualMode)
		{
			snprintf(this->name, WS2812FX_EFFECT_NAME_LEN, "Rainbow");

			// Parameter 0: Rainbow density (delta hue)
			this->setParameter(NumericEffectParameter<int16_t>(density, "Density", -50, 50, 1), 0);

			// Parameter 1: Scroll hue amount
			this->setParameter(NumericEffectParameter<uint8_t>(scrollHue, "Scroll hue", 0, 100, 1), 1);

			// Parameter 2: Scroll step delay
			this->setParameter(NumericEffectParameter<uint8_t>(stepDelay, "Scroll delay", 0, 100, 1), 2);

			// Parameter 3: Starting color
			this->setParameter(ColorHSVEffectParameter(startHSV, "Start color"), 3);

			// Parameter 4: Manual mode
			this->setParameter(BooleanEffectParameter(manualMode, "Manual mode"), 4);

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
			this->modMatrix[2].modDestination = this->getParameter(2);
			this->modMatrix[2].modAmount->setValue(100);
		}

		void updateEffect() override;
};


#endif /* INC_WS2812FX_RAINBOWEFFECT_HPP_ */
