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
	uint16_t height = *(static_cast<uint16_t *>(this->getParameter(1)->getValue()));
//	uint16_t heightRandomizationAmount = *(static_cast<uint16_t *>(this->getParameter(2)->getValue()));
	colorHSV hsv = *(static_cast<colorHSV *>(this->getParameter(3)->getValue()));
//	uint16_t hueRandomizationAmount = *(static_cast<uint16_t *>(this->getParameter(4)->getValue()));
//	bool flip = *(static_cast<bool *>(this->getParameter(5)->getValue()));
//	bool manualMode = *(static_cast<bool *>(this->getParameter(6)->getValue()));

	// State variables
	static stackerState state = CLEAR_TOWER;
	static uint16_t towerHeight = 0;
	static uint16_t pieceHeight = 0;
	static uint16_t pieceHeightFaded = 0;
	static uint16_t piecePosition = 0;
	static colorHSV pieceColor = hsv;

	// Apply speed
	TIM7->ARR = speed * 10;

	// If there's room on the strip for a piece of non-zero length
	if(towerHeight < NUM_LOGICAL_LEDS)
	{
		state = CLEAR_TOWER;
	}

	switch(state)
	{
		// Reset everything
		case CLEAR_TOWER:
			WS2812_ClearLEDs();

			towerHeight = 0;

			state = SPAWN_PIECE;
			break;
		// Spawn new piece
		case SPAWN_PIECE:
			// Reset piece properties
			piecePosition = 0;

			// TODO: Apply randomization to height
			pieceHeight = height;

			// TODO: Apply randomization to color
			pieceColor = hsv;

			pieceHeightFaded = 0;

			state = FADING_NEW_PIECE;
			break;
		case FADING_NEW_PIECE:
			// Fade piece in until faded height matches real height
			if(pieceHeightFaded <= pieceHeight)
			{
				if(pieceHeightFaded < NUM_LOGICAL_LEDS - towerHeight)
				{
					pieceHeightFaded++;
					WS2812_DrawLineHSV(0.0f, (float)pieceHeightFaded, pieceColor, false);
				}
				// Collision occurred before new piece could be completely faded in
				else
				{
					state = CLEAR_TOWER;
				}
			}
			else
			{
				state = DROPPING_CURRENT_PIECE;
			}

			break;
		case DROPPING_CURRENT_PIECE:
			// Clear path of new piece from bottom of strip to top of tower
			uint16_t clearRange = MIN(NUM_LOGICAL_LEDS - towerHeight, NUM_LOGICAL_LEDS);
			for(uint16_t i = 0; i < clearRange; i++)
			{
				WS2812_SetLED(i, 0, 0, 0, false);
			}

			// Draw piece at current position
			WS2812_DrawLineHSV((float)piecePosition, (float)pieceHeight, pieceColor, false);

			// Increment position of current piece
			piecePosition++;

			// Detect collision with top of tower
			if(piecePosition >= towerHeight)
			{
				// Attach piece to top of tower
				towerHeight += pieceHeight;

				if(towerHeight < NUM_LOGICAL_LEDS)
				{
					state = SPAWN_PIECE;
				}
				// Not technically required since the if-statement above always checks the tower height before running the state machine
				else
				{
					state = CLEAR_TOWER;
				}
			}
			break;

		// This state shouldn't be reachable
		default:
			state = CLEAR_TOWER;
			break;
	}

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
