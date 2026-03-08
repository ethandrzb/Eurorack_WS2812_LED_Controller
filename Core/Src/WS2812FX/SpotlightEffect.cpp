/*
 * SpotlightEffect.cpp
 *
 *  Created on: Feb 8, 2026
 *      Author: ethan
 */

#include <WS2812FX/SpotlightEffect.hpp>

bool trig0Active = false;

void SpotlightEffect::updateEffect()
{
	// Change in spotlight width per iteration
	uint16_t dwRaw = *(static_cast<uint16_t *>(this->getParameter(0)->getValue()));
	// Max width
	float wMax = *(static_cast<float *>(this->getParameter(1)->getValue()));
	// Maximum number of active spotlights
	uint16_t maxActiveSpots = *(static_cast<uint16_t *>(this->getParameter(2)->getValue()));
	// Number of iterations between spotlight spawns in automatic mode
	uint16_t spawnPeriod = *(static_cast<uint16_t *>(this->getParameter(3)->getValue()));
	// Spotlight color
	colorHSV color = *(static_cast<colorHSV *>(this->getParameter(4)->getValue()));
	// Enables manual mode (disables automatic spawning)
	bool manualMode = *(static_cast<bool *>(this->getParameter(5)->getValue()));

	// Number of iterations since last spotlight was spawned in automatic mode
	static uint16_t spawnIterationCounter = 0;

	// Scale dw to size of single LED (1.0 = 1 LED completely filled)
	float dwScaled = dwRaw / 1000.0f;

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
				if(((spawnIterationCounter >= spawnPeriod) && !manualMode) || trig0Active)
				{
					spot->state = SPOTLIGHT_INCREASING;

					spot->color = WS2812_HSVToRGB(color.hue, color.saturation, color.value);
					spot->dw = dwScaled;
					spot->w = 0.0f;
					spot->x = (float)(rand() % 100) / 100.0f;
				}

				// Reset spawn triggers
				if(spawnIterationCounter >= spawnPeriod)
				{
					spawnIterationCounter = 0;
				}

				if(trig0Active)
				{
					trig0Active = false;
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
			// Shape width to make spot grow more naturally
			float cosWidth = (-1.0f * (cosf((spot->w / wMax) * 3.141f) / 2.0f) + 0.5f) * wMax;

			// Find left end of spot from current width
			float leftBound = spot->x - (cosWidth / 2.0f);

			// Clip range to end of strip
			leftBound = (leftBound >= 0) ? leftBound : 0;

			//TODO: Fix bug causing left side of spot to flicker when spot->w is close to wMax for certain numbers of LEDs
			// Bug observed when NUM_PHYSICAL_LEDS is in the interval [66, 126]
			WS2812_DrawLine(leftBound * NUM_PHYSICAL_LEDS, cosWidth * NUM_PHYSICAL_LEDS, spot->color.red, spot->color.green, spot->color.blue, true);
		}
	}

	WS2812_SendAll();
	spawnIterationCounter++;
}

void SpotlightEffect::trig0Callback()
{
	trig0Active = true;
}
