/*
 * SimpleMeterEffect.cpp
 *
 *  Created on: Feb 8, 2025
 *      Author: ethan
 */

#include <WS2812FX/MeterEffect.hpp>

void MeterEffect::updateEffect()
{
	uint16_t meterFill0 = *(static_cast<uint16_t *>(this->getParameter(0)->getValue()));
	colorHSV hsv0 = *(static_cast<colorHSV *>(this->getParameter(1)->getValue()));
	uint8_t flip0 = *(static_cast<bool *>(this->getParameter(2)->getValue()));
	uint16_t meterFill1 = *(static_cast<uint16_t *>(this->getParameter(3)->getValue()));
	colorHSV hsv1 = *(static_cast<colorHSV *>(this->getParameter(4)->getValue()));
	uint8_t flip1 = *(static_cast<bool *>(this->getParameter(5)->getValue()));
	uint8_t mirror0 = *(static_cast<bool *>(this->getParameter(6)->getValue()));
	uint8_t mirror1 = *(static_cast<bool *>(this->getParameter(7)->getValue()));

	colorRGB rgb0 = WS2812_HSVToRGB(hsv0.hue, hsv0.saturation, hsv0.value);
	colorRGB rgb1 = WS2812_HSVToRGB(hsv1.hue, hsv1.saturation, hsv1.value);

	WS2812_ClearLEDs();

	if(mirror0)
	{
		WS2812_MirroredMeterEffect(rgb0, meterFill0, flip0);
	}
	else
	{
		WS2812_SimpleMeterEffect(rgb0, meterFill0, flip0);
	}

	if(mirror1)
	{
		WS2812_MirroredMeterEffect(rgb1, meterFill1, flip1);
	}
	else
	{
		WS2812_SimpleMeterEffect(rgb1, meterFill1, flip1);
	}

	WS2812_SendAll();
}
