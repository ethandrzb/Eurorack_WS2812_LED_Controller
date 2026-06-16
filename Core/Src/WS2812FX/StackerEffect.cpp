/*
 * StackerEffect.cpp
 *
 *  Created on: Jun 15, 2026
 *      Author: ethan
 */

#include <WS2812FX/StackerEffect.hpp>

void StackerEffect::updateEffect()
{
	uint16_t speed = *(static_cast<uint16_t *>(this->getParameter(0)->getValue()));
	float height = *(static_cast<float *>(this->getParameter(1)->getValue()));
	uint16_t heightRandomizationAmount = *(static_cast<uint16_t *>(this->getParameter(2)->getValue()));
	colorHSV hsv = *(static_cast<colorHSV *>(this->getParameter(3)->getValue()));
	uint16_t hueRandomizationAmount = *(static_cast<uint16_t *>(this->getParameter(4)->getValue()));
	bool flip = *(static_cast<bool *>(this->getParameter(5)->getValue()));
	bool manualMode = *(static_cast<bool *>(this->getParameter(6)->getValue()));

	static float currentHeight = 0.0f;

	// Apply speed
	TIM7->ARR = speed * 10;

//
//	WS2812_ClearLEDs();
//
//	// Draw blobs
//	for(uint8_t i = 0; i < NUM_BLOBS; i++)
//	{
//		if(constantLengthModes[i])
//		{
//			WS2812_DrawLine(startFractions[i] * NUM_LOGICAL_LEDS, endFractions[i] * NUM_LOGICAL_LEDS, rgbColors[i].red, rgbColors[i].green, rgbColors[i].blue, true);
//		}
//		else
//		{
//			WS2812_DrawLine(startFractions[i] * NUM_LOGICAL_LEDS, fabs(startFractions[i] - endFractions[i]) * NUM_LOGICAL_LEDS, rgbColors[i].red, rgbColors[i].green, rgbColors[i].blue, true);
//		}
//	}


	WS2812_SendAll();
}

//void StackerEffect::trig0Callback(void)
//{
//
//}
//
//void StackerEffect::trig1Callback(void)
//{
//
//}
