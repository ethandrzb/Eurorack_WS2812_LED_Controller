/*
 * StackerEffect.hpp
 *
 *  Created on: Jun 15, 2026
 *      Author: ethan
 */

#ifndef INC_WS2812FX_STACKEREFFECT_HPP_
#define INC_WS2812FX_STACKEREFFECT_HPP_

#include <WS2812FX/WS2812FX.hpp>
#include <main.h>
#include <math.h>

using namespace WS2812FX;

class StackerEffect : public WS2812Effect
{
public:
	typedef enum stackerState{
		CLEAR_TOWER,
		SPAWN_PIECE,
		FADING_NEW_PIECE,
		DROPPING_CURRENT_PIECE
	} stackerState;

	StackerEffect(uint16_t fallSpeed, uint16_t pieceSize, uint16_t sizeRandomizationAmount, colorHSV hsv, uint16_t hueRandomizationAmount, bool flip, bool manualMode)
	{
		snprintf(this->name, WS2812FX_EFFECT_NAME_LEN, "Stacker");

		// Parameter 0: Speed of falling pieces
		this->setParameter(NumericEffectParameter<uint16_t>(fallSpeed, "Speed", 1, 100, 1), 0);

		// Parameter 1: Height of falling pieces
		this->setParameter(NumericEffectParameter<uint16_t>(pieceSize, "Height", 1, 25, 1), 1);

		// Parameter 2: Height randomization amount
		this->setParameter(NumericEffectParameter<uint16_t>(sizeRandomizationAmount, "Height Random", 0, 100, 1), 2);

		// Parameter 3: Piece color
		this->setParameter(ColorHSVEffectParameter(hsv, "Piece Color"), 3);

		// Parameter 4: Piece hue randomization amount
		this->setParameter(NumericEffectParameter<uint16_t>(hueRandomizationAmount, "Hue Random", 0, 100, 1), 4);

		// Parameter 5: Flip
		this->setParameter(BooleanEffectParameter(flip, "Flip"), 5);

		// Parameter 6: Manual mode
		this->setParameter(BooleanEffectParameter(manualMode, "Manual mode"), 6);

		this->initModMatrixDefaults();
	}

	// Init default mod matrix for this effect
	void initModMatrixDefaults() override
	{
//		// Only pre-map modulation for first two blobs
//		for(uint8_t i = 0; i < 2; i++)
//		{
//			// Map blob start positions to CV inputs A and B
//			this->modMatrix[2 * i].modSource = NULL;
//			this->modMatrix[2 * i].modDestination = this->getParameter(NUM_BLOB_PARAMETERS * i);
//			this->modMatrix[2 * i].modAmount->setValue(0);
//
//			// Map meter hues to CV inputs C and D
//			this->modMatrix[2 * i + 1].modSource = NULL;
//			this->modMatrix[2 * i + 1].modDestination = this->getParameter(1 + (NUM_BLOB_PARAMETERS * i));
//			this->modMatrix[2 * i + 1].modAmount->setValue(0);
//		}
	}

	void updateEffect() override;
//	void trig0Callback(void) override;
//	void trig1Callback(void) override;
};

#endif /* INC_WS2812FX_STACKEREFFECT_HPP_ */
