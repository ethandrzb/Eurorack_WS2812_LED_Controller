/*
 * MeterEffect.cpp
 *
 *  Created on: Feb 8, 2025
 *      Author: ethan
 */

#include <WS2812FX/MeterEffect.hpp>

void MeterEffect::updateEffect()
{
	float meterFillFractions[NUM_METERS];
	colorHSV hsvColors[NUM_METERS];
	bool flips[NUM_METERS];
	bool mirrors[NUM_METERS];

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
