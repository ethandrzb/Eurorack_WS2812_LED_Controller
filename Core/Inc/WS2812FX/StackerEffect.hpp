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

	StackerEffect(uint16_t fallSpeed, uint16_t pieceSize, uint16_t sizeRandomizationAmount, colorHSV hsv, uint16_t hueRandomizationAmount, bool manualMode)
	{
		snprintf(this->name, WS2812FX_EFFECT_NAME_LEN, "Stacker");

		// Parameter 0: Speed of falling pieces
		this->setParameter(NumericEffectParameter<uint16_t>(fallSpeed, "Speed", 1, 100, 1), 0);

		// Parameter 1: Height of falling pieces
		this->setParameter(NumericEffectParameter<uint16_t>(pieceSize, "Height", 1, 50, 1), 1);

		// Parameter 2: Height randomization amount
		this->setParameter(NumericEffectParameter<uint16_t>(sizeRandomizationAmount, "Height Random", 0, 100, 1), 2);

		// Parameter 3: Piece color
		this->setParameter(ColorHSVEffectParameter(hsv, "Piece Color"), 3);

		// Parameter 4: Piece hue randomization amount
		this->setParameter(NumericEffectParameter<uint16_t>(hueRandomizationAmount, "Hue Random", 0, 100, 1), 4);

		// Parameter 5: Manual mode
		this->setParameter(BooleanEffectParameter(manualMode, "Manual mode"), 5);

		this->initModMatrixDefaults();
	}

	// Init default mod matrix for this effect
	void initModMatrixDefaults() override
	{
		this->modMatrix[0].modSource = NULL;
		this->modMatrix[0].modDestination = this->getParameter(0);
		this->modMatrix[0].modAmount->setValue(0);

		this->modMatrix[1].modSource = NULL;
		this->modMatrix[1].modDestination = this->getParameter(1);
		this->modMatrix[1].modAmount->setValue(0);

		this->modMatrix[2].modSource = NULL;
		this->modMatrix[2].modDestination = this->getParameter(2);
		this->modMatrix[2].modAmount->setValue(0);

		this->modMatrix[3].modSource = NULL;
		this->modMatrix[3].modDestination = this->getParameter(4);
		this->modMatrix[3].modAmount->setValue(0);
	}

	void updateEffect() override;
	void trig0Callback(void) override;
	void trig1Callback(void) override;
};

#endif /* INC_WS2812FX_STACKEREFFECT_HPP_ */
