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
	const uint8_t NUM_METERS = 4;
	const uint8_t NUM_METER_PARAMETERS = 4;
	//TODO: Increase size of fill parameters to avoid overflow with larger LED strips
	//TODO: Remove second meter from effect when effects can be designed/stacked instead of hardcoded
	MeterEffect(uint16_t fill, colorHSV hsv, uint8_t flip, uint8_t mirror)
	{
		snprintf(this->name, WS2812FX_EFFECT_NAME_LEN, "Meter");

		for(uint8_t i = 0; i < NUM_METERS; i++)
		{
			// Each meter, i , of the NUM_METERS meters created has 4 parameters indexed starting at zero
			// from the meter's 0th parameter (Fill) to its 3rd (Mirror)
			// Parameter Index | Description
			//       i         | Meter i fill amount
			//     i + 1       | Meter i color
			//     i + 2       | Flip meter i
			//     i + 3       | Mirror meter i
			this->setParameter(NumericEffectParameter<uint16_t>(fill, "Fill " + std::to_string(i), 0, NUM_PHYSICAL_LEDS, 1), (NUM_METER_PARAMETERS * i));
			this->setParameter(ColorHSVEffectParameter(hsv, "Color " + std::to_string(i)), (NUM_METER_PARAMETERS * i + 1));
			this->setParameter(BooleanEffectParameter(flip, "Flip " + std::to_string(i)), (NUM_METER_PARAMETERS * i + 2));
			this->setParameter(BooleanEffectParameter(mirror, "Mirror " + std::to_string(i)), (NUM_METER_PARAMETERS * i + 3));

			// Vary initial parameters
			fill += 5;
			hsv.hue = (hsv.hue + 90) % 360;
			flip = !flip;
		}

		this->initModMatrixDefaults();
	}

	// Init default mod matrix for this effect
	void initModMatrixDefaults() override
	{
		for(uint8_t i = 0; i < NUM_METERS; i++)
		{
			// Map meter fill amounts to CV inputs A-D
			this->modMatrix[i].modSource = NULL;
			this->modMatrix[i].modDestination = this->getParameter(4 * i);
			this->modMatrix[i].modAmount->setValue(100);

			// Map meter hues to CV inputs E-H
			this->modMatrix[i + 4].modSource = NULL;
			this->modMatrix[i + 4].modDestination = static_cast<ColorHSVEffectParameter *>(this->getParameter((NUM_METER_PARAMETERS * i + 1)))->_hue.get();
			this->modMatrix[i + 4].modAmount->setValue(0);
		}
	}

	void updateEffect() override;
};

#endif /* INC_WS2812FX_METEREFFECT_HPP_ */
