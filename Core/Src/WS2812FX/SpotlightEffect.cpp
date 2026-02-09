/*
 * SpotlightEffect.cpp
 *
 *  Created on: Feb 8, 2026
 *      Author: ethan
 */

#include <WS2812FX/SpotlightEffect.hpp>

void SpotlightEffect::updateEffect()
{
	float dw = *(static_cast<float *>(this->getParameter(0)->getValue()));
	float wMax = *(static_cast<float *>(this->getParameter(1)->getValue()));
	uint16_t maxActiveSpots = *(static_cast<uint16_t *>(this->getParameter(2)->getValue()));
	uint16_t spawnPeriod = *(static_cast<uint16_t *>(this->getParameter(3)->getValue()));
	colorHSV color = *(static_cast<colorHSV *>(this->getParameter(4)->getValue()));

	static uint16_t iterations = 0;

	// Use static frame period
	TIM7->ARR = 50;

	WS2812_ClearLEDs();

	// Process each spotlight
	for(uint16_t i = 0; i < maxActiveSpots; i++)
	{
		spotlight *spot = this->spots[i];

		switch(spot->state)
		{
			case SPOTLIGHT_IDLE:
				// This should work, but there might be a data reuse bug in here
				if(iterations >= spawnPeriod)
				{
					spot->state = SPOTLIGHT_INCREASING;

					spot->color = WS2812_HSVToRGB(color.hue, color.saturation, color.value);
					spot->dw = dw;
					spot->w = 0.0f;
					spot->x = (float)(rand() % 100) / 100.0f;

					iterations = 0;
				}
				break;
			case SPOTLIGHT_INCREASING:
				if((spot->w + spot->dw) <= wMax)
				{
					spot->w += spot->dw;
				}
				else
				{
					spot->state = SPOTLIGHT_DECREASING;
				}
				break;
			case SPOTLIGHT_DECREASING:
				if((spot->w - spot->dw) >= 0)
				{
					spot->w -= spot->dw;
				}
				else
				{
					spot->state = SPOTLIGHT_IDLE;
				}
				break;
		}

		// Draw active spots
		if(spot->state != SPOTLIGHT_IDLE)
		{
			float leftBound = spot->x - (spot->w / 2.0f);

			// Clip range to end of strip
			leftBound = (leftBound >= 0) ? leftBound : 0;

			WS2812_DrawLine(leftBound * NUM_PHYSICAL_LEDS, spot->w * NUM_PHYSICAL_LEDS, spot->color.red, spot->color.green, spot->color.blue, true);
		}
	}

	WS2812_SendAll();
	iterations++;
}
