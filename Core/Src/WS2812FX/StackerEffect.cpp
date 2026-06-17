/*
 * StackerEffect.cpp
 *
 *  Created on: Jun 15, 2026
 *      Author: ethan
 */

#include <WS2812FX/StackerEffect.hpp>

StackerEffect::stackerState state = StackerEffect::CLEAR_TOWER;

void StackerEffect::updateEffect()
{
	uint16_t speed = *(static_cast<uint16_t *>(this->getParameter(0)->getValue()));
	uint16_t height = *(static_cast<uint16_t *>(this->getParameter(1)->getValue()));
	uint16_t heightRandomizationAmount = *(static_cast<uint16_t *>(this->getParameter(2)->getValue()));
	colorHSV hsv = *(static_cast<colorHSV *>(this->getParameter(3)->getValue()));
	uint16_t hueRandomizationAmount = *(static_cast<uint16_t *>(this->getParameter(4)->getValue()));
	bool manualMode = *(static_cast<bool *>(this->getParameter(5)->getValue()));

	// State variables
	static uint16_t towerHeight = 0;
	static uint16_t pieceHeight = 0;
	static uint16_t pieceHeightFaded = 0;
	static uint16_t piecePosition = 1;
	static colorHSV pieceColor = hsv;

	// Apply speed
	TIM7->ARR = speed * 5;

	// If there's room on the strip for a piece of non-zero length
	if(towerHeight >= NUM_LOGICAL_LEDS)
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
			piecePosition = 1;

			// Apply randomization to height and clip to max value
			pieceHeight = height;
			if(heightRandomizationAmount > 0)
			{
				pieceHeight += (rand() % heightRandomizationAmount);
				pieceHeight = MIN(pieceHeight, 50);
			}

			// Apply randomization to color and wrap around color wheel
			pieceColor = hsv;
			if(hueRandomizationAmount > 0)
			{
				//TODO: Change this to make it properly cover the entire color wheel?
				// 3 is close enough to the 3.6 scale factor needed without added complexity
				pieceColor.hue += (rand() % (hueRandomizationAmount * 3));
				pieceColor.hue %= 360;
			}

			pieceHeightFaded = 0;

			if(!manualMode)
			{
				state = FADING_NEW_PIECE;
			}
			break;
		case FADING_NEW_PIECE:
			// Fade piece in until faded height matches real height
			pieceHeightFaded++;
			if(pieceHeightFaded < pieceHeight)
			{
				if(pieceHeightFaded < NUM_LOGICAL_LEDS - towerHeight)
				{
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
				WS2812_DrawLineHSV(0.0f, (float)pieceHeight, pieceColor, false);
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
			if(piecePosition + pieceHeight > NUM_LOGICAL_LEDS - towerHeight)
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
	}

	WS2812_SendAll();
}

void StackerEffect::trig0Callback(void)
{
	//TODO: Update this callback to immediately drop the currently falling piece to the top of the tower and start dropping another
	if(state != DROPPING_CURRENT_PIECE)
	{
		state = FADING_NEW_PIECE;
	}
}

void StackerEffect::trig1Callback(void)
{
	state = CLEAR_TOWER;
}
