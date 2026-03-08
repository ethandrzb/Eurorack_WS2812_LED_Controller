/*
 * BlobEffect.cpp
 *
 *  Created on: Feb 8, 2025
 *      Author: ethan
 */

#include <WS2812FX/BlobEffect.hpp>

void BlobEffect::updateEffect()
{
	// Start position of each blob
	float startFractions[NUM_BLOBS];
	// End positions or total length of each blob
	float endFractions[NUM_BLOBS];
	// Blob colors
	colorHSV hsvColors[NUM_BLOBS];
	// Determines whether a blob will treat endFractions as the end location of the blob (false) or total length of the blob away from the starting point (true)
	bool constantLengthModes[NUM_BLOBS];
	// Blob colors
	colorRGB rgbColors[NUM_BLOBS];

	// Retrieve parameters for each meter
	for(int i = 0; i < NUM_BLOBS; i++)
	{
		startFractions[i] = *(static_cast<float *>(this->getParameter(NUM_BLOB_PARAMETERS * i)->getValue()));
		endFractions[i] = *(static_cast<float *>(this->getParameter((NUM_BLOB_PARAMETERS * i) + 1)->getValue()));
		hsvColors[i] = *(static_cast<colorHSV *>(this->getParameter((NUM_BLOB_PARAMETERS * i) + 2)->getValue()));
		constantLengthModes[i] = *(static_cast<bool *>(this->getParameter((NUM_BLOB_PARAMETERS * i) + 3)->getValue()));

		rgbColors[i] = WS2812_HSVToRGB(hsvColors[i].hue, hsvColors[i].saturation, hsvColors[i].value);
	}

	// Use static frame period
	TIM7->ARR = 50;

	WS2812_ClearLEDs();

	// Draw blobs
	for(uint8_t i = 0; i < NUM_BLOBS; i++)
	{
		if(constantLengthModes[i])
		{
			WS2812_DrawLine(startFractions[i] * NUM_LOGICAL_LEDS, endFractions[i] * NUM_LOGICAL_LEDS, rgbColors[i].red, rgbColors[i].green, rgbColors[i].blue, true);
		}
		else
		{
			WS2812_DrawLine(startFractions[i] * NUM_LOGICAL_LEDS, fabs(startFractions[i] - endFractions[i]) * NUM_LOGICAL_LEDS, rgbColors[i].red, rgbColors[i].green, rgbColors[i].blue, true);
		}
	}


	WS2812_SendAll();
}
