/*
 * SimpleMeterEffect.hpp
 *
 *  Created on: Feb 8, 2025
 *      Author: ethan
 */

#ifndef INC_WS2812FX_SIMPLEMETEREFFECT_HPP_
#define INC_WS2812FX_SIMPLEMETEREFFECT_HPP_

#include <WS2812FX/WS2812FX.hpp>
#include <main.h>

using namespace WS2812FX;

class SimpleMeterEffect : public WS2812Effect
{
public:
	SimpleMeterEffect(uint8_t fill, colorHSV hsv, uint8_t flip)
	{
		snprintf(this->name, WS2812FX_EFFECT_NAME_LEN, "S. Meter");

		// Parameter 0: Meter fill amount
		this->setParameter(NumericEffectParameter<uint8_t>(fill, "Fill 0", 0, NUM_PHYSICAL_LEDS, 1), 0);

		// Parameter 1: Meter color
		this->setParameter(ColorHSVEffectParameter(hsv, "Color 0"), 1);

		// Parameter 2: Flip meter
		//TODO: Add support for boolean NumericEffectParameters
		this->setParameter(NumericEffectParameter<uint8_t>(flip, "Flip 0", 0, 1, 1), 2);

		// Parameter 0: Meter fill amount
		this->setParameter(NumericEffectParameter<uint8_t>(fill, "Fill 1", 0, NUM_PHYSICAL_LEDS, 1), 3);

		// Parameter 1: Meter color
		hsv.hue = (hsv.hue + 180) % 360;
		this->setParameter(ColorHSVEffectParameter(hsv, "Color 1"), 4);

		// Parameter 2: Flip meter
		//TODO: Add support for boolean NumericEffectParameters
		this->setParameter(NumericEffectParameter<uint8_t>(!flip, "Flip 1", 0, 1, 1), 5);
	}

	void updateEffect() override;
};

#endif /* INC_WS2812FX_SIMPLEMETEREFFECT_HPP_ */
