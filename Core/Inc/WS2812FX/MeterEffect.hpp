/*
 * SimpleMeterEffect.hpp
 *
 *  Created on: Feb 8, 2025
 *      Author: ethan
 */

#ifndef INC_WS2812FX_METEREFFECT_HPP_
#define INC_WS2812FX_METEREFFECT_HPP_

#include <WS2812FX/WS2812FX.hpp>
#include <main.h>

using namespace WS2812FX;

class MeterEffect : public WS2812Effect
{
public:
	//TODO: Remove second meter from effect when effects can be designed/stacked instead of hardcoded
	MeterEffect(uint8_t fill, colorHSV hsv, uint8_t flip, uint8_t mirrored)
	{
		snprintf(this->name, WS2812FX_EFFECT_NAME_LEN, "Meter");

		// Parameters 0 and 3: Meter fill amount
		this->setParameter(NumericEffectParameter<uint8_t>(fill, "Fill 0", 0, NUM_PHYSICAL_LEDS, 1), 0);
		this->setParameter(NumericEffectParameter<uint8_t>(fill, "Fill 1", 0, NUM_PHYSICAL_LEDS, 1), 3);

		// Parameters 1 and 4: Meter color
		this->setParameter(ColorHSVEffectParameter(hsv, "Color 0"), 1);
		hsv.hue = (hsv.hue + 180) % 360;
		this->setParameter(ColorHSVEffectParameter(hsv, "Color 1"), 4);

		// Parameters 2 and 5: Flip meter
		//TODO: Add support for boolean NumericEffectParameters
		this->setParameter(NumericEffectParameter<uint8_t>(flip, "Flip 0", 0, 1, 1), 2);
		this->setParameter(NumericEffectParameter<uint8_t>(!flip, "Flip 1", 0, 1, 1), 5);

		// Parameters 6 and 7: Mirror meter
		this->setParameter(NumericEffectParameter<uint8_t>(mirrored, "Mirror 0", 0, 1, 1), 6);
		this->setParameter(NumericEffectParameter<uint8_t>(mirrored, "Mirror 1", 0, 1, 1), 7);
	}

	void updateEffect() override;
};

#endif /* INC_WS2812FX_METEREFFECT_HPP_ */
