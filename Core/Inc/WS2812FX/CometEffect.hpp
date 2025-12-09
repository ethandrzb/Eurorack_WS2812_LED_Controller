/*
 * CometEffect.hpp
 *
 *  Created on: Sep 28, 2025
 *      Author: ethan
 */

#ifndef INC_WS2812FX_COMETEFFECT_HPP_
#define INC_WS2812FX_COMETEFFECT_HPP_

#include <WS2812FX/WS2812FX.hpp>
#include <main.h>

using namespace WS2812FX;

class CometEffect : public WS2812Effect
{
	public:
	CometEffect(uint8_t stepDelay, uint8_t cometSize, colorHSV hsv, uint8_t interval)
		{
			snprintf(this->name, WS2812FX_EFFECT_NAME_LEN, "Comet");

			// Parameter 0: Step delay
			this->setParameter(NumericEffectParameter<uint8_t>(stepDelay, "Step Delay", 1, 100, 1), 0);

			// Parameter 1: Comet size
			this->setParameter(NumericEffectParameter<uint8_t>(cometSize, "Comet Size", 1, 20, 1), 1);

			// Parameter 2: Comet color
			this->setParameter(ColorHSVEffectParameter(hsv, "Comet Color"), 2);

			// Parameter 3: Interval between comet triggers
			this->setParameter(NumericEffectParameter<uint8_t>(interval, "Comet Interval", 1, 100, 5), 3);

			// Parameter 4: Comet trigger
			this->setParameter(BooleanEffectParameter(false, "Trigger comet"), 4);

			this->initModMatrixDefaults();

			WS2812_InitMultiCometEffect();
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
			this->modMatrix[2].modDestination = static_cast<ColorHSVEffectParameter *>(this->getParameter(2))->_hue.get();
			this->modMatrix[2].modAmount->setValue(0);

			this->modMatrix[4].modSource = NULL;
			this->modMatrix[4].modDestination = this->getParameter(4);
			this->modMatrix[4].modAmount->setValue(100);
		}

		void updateEffect() override;
};

#endif /* INC_WS2812FX_COMETEFFECT_HPP_ */
