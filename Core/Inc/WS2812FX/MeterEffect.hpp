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
	//TODO: Increase size of fill parameters to avoid overflow with larger LED strips
	//TODO: Remove second meter from effect when effects can be designed/stacked instead of hardcoded
	MeterEffect(uint16_t fill, colorHSV hsv, uint8_t flip, uint8_t mirror)
	{
		snprintf(this->name, WS2812FX_EFFECT_NAME_LEN, "Meter");

		// Parameters 0 and 3: Meter fill amount
		this->setParameter(NumericEffectParameter<uint16_t>(fill, "Fill 0", 0, NUM_PHYSICAL_LEDS, 1), 0);
		this->setParameter(NumericEffectParameter<uint16_t>(fill, "Fill 1", 0, NUM_PHYSICAL_LEDS, 1), 3);

		// Parameters 1 and 4: Meter color
		this->setParameter(ColorHSVEffectParameter(hsv, "Color 0"), 1);
		hsv.hue = (hsv.hue + 180) % 360;
		this->setParameter(ColorHSVEffectParameter(hsv, "Color 1"), 4);

		// Parameters 2 and 5: Flip meter
		this->setParameter(BooleanEffectParameter(flip, "Flip 0"), 2);
		this->setParameter(BooleanEffectParameter(!flip, "Flip 1"), 5);

		// Parameters 6 and 7: Mirror meter
		this->setParameter(BooleanEffectParameter(mirror, "Mirror 0"), 6);
		this->setParameter(BooleanEffectParameter(mirror, "Mirror 1"), 7);

		this->initModMatrixDefaults();
	}

	// Init default mod matrix for this effect
	void initModMatrixDefaults() override
	{
		this->modMatrix[0].modSource = NULL;
		this->modMatrix[0].modDestination = this->getParameter(0);
		this->modMatrix[0].modAmount->setValue(0);

		this->modMatrix[1].modSource = NULL;
		this->modMatrix[1].modDestination = this->getParameter(3);
		this->modMatrix[1].modAmount->setValue(0);

		this->modMatrix[2].modSource = NULL;
		this->modMatrix[2].modDestination = static_cast<ColorHSVEffectParameter *>(this->getParameter(1))->_hue.get();
		this->modMatrix[2].modAmount->setValue(0);
	}

	void updateEffect() override;
};

#endif /* INC_WS2812FX_METEREFFECT_HPP_ */
