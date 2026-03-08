/*
 * MeterEffect.cpp
 *
 *  Created on: Feb 8, 2025
 *      Author: ethan
 */

#include <WS2812FX/MeterEffect.hpp>

void MeterEffect::updateEffect()
{
	// FOR EACH METER
	//////////////////////////////////
	// Fraction of strip to fill
	float meterFillFractions[NUM_METERS];
	// Color
	colorHSV hsvColors[NUM_METERS];
	// If meter IS NOT mirrored, determines whether meter fills from LED index 0 towards the end (false) or from the end towards LED index 0 (true)
	// If meter IS mirroed, determines whether the meter fills from the extreme ends of the strip towards the middle (false) or from the middle of the strip towards the extreme ends (true)
	bool flips[NUM_METERS];
	// Determines whether the meter should be mirrored across the middle index of the strip
	bool mirrors[NUM_METERS];
	//////////////////////////////////

	colorRGB rgbColors[NUM_METERS];

	// Retrieve parameters for each meter
	for(int i = 0; i < NUM_METERS; i++)
	{
		meterFillFractions[i] = *(static_cast<float *>(this->getParameter(NUM_METER_PARAMETERS * i)->getValue()));
		hsvColors[i] = *(static_cast<colorHSV *>(this->getParameter((NUM_METER_PARAMETERS * i) + 1)->getValue()));
		flips[i] = *(static_cast<bool *>(this->getParameter((NUM_METER_PARAMETERS * i) + 2)->getValue()));
		mirrors[i] = *(static_cast<bool *>(this->getParameter((NUM_METER_PARAMETERS * i) + 3)->getValue()));

		rgbColors[i] = WS2812_HSVToRGB(hsvColors[i].hue, hsvColors[i].saturation, hsvColors[i].value);
	}

	// Use static frame period
	TIM7->ARR = 50;

	WS2812_ClearLEDs();

	// Draw meters
	for(uint8_t i = 0; i < NUM_METERS; i++)
	{
		if(mirrors[i])
		{
			WS2812_MirroredMeterEffect(rgbColors[i], meterFillFractions[i], flips[i], true, false);
		}
		else
		{
			WS2812_SimpleMeterEffect(rgbColors[i], meterFillFractions[i], flips[i], true, false);
		}
	}


	WS2812_SendAll();
}
