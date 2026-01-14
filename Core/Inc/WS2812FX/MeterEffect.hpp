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
	const uint8_t NUM_METERS = 2;
	const uint8_t NUM_METER_PARAMETERS = 4;
	//TODO: Increase size of fill parameters to avoid overflow with larger LED strips
	MeterEffect(float fillFraction, colorHSV hsv, uint8_t flip, uint8_t mirror)
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
			this->setParameter(NumericEffectParameter<float>(fillFraction, "Fill % " + std::to_string(i), 0.0f, 1.0f, 0.01f), (NUM_METER_PARAMETERS * i));
			this->setParameter(ColorHSVEffectParameter(hsv, "Color " + std::to_string(i)), (NUM_METER_PARAMETERS * i + 1));
			this->setParameter(BooleanEffectParameter(flip, "Flip " + std::to_string(i)), (NUM_METER_PARAMETERS * i + 2));
			this->setParameter(BooleanEffectParameter(mirror, "Mirror " + std::to_string(i)), (NUM_METER_PARAMETERS * i + 3));

			// Vary initial parameters
			fillFraction += 0.05f;
			hsv.hue = (hsv.hue + 90) % 360;
			flip = !flip;
		}

		this->initModMatrixDefaults();
	}

	// Init default mod matrix for this effect
	void initModMatrixDefaults() override
	{
		// Only pre-map modulation for first two meters
		for(uint8_t i = 0; i < 2; i++)
		{
			// Map meter fill amounts to CV inputs A and B
			this->modMatrix[i].modSource = NULL;
			this->modMatrix[i].modDestination = this->getParameter(4 * i);
			this->modMatrix[i].modAmount->setValue(100);

			// Map meter hues to CV inputs C and D
			this->modMatrix[i + 2].modSource = NULL;
			this->modMatrix[i + 2].modDestination = static_cast<ColorHSVEffectParameter *>(this->getParameter((NUM_METER_PARAMETERS * i + 1)))->_hue.get();
			this->modMatrix[i + 2].modAmount->setValue(0);
		}
	}

	void updateEffect() override;
};

#endif /* INC_WS2812FX_METEREFFECT_HPP_ */
