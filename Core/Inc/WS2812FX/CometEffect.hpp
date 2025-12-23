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
	CometEffect(uint8_t cometSize, colorHSV hsv, uint8_t interval, uint8_t speed)
		{
			snprintf(this->name, WS2812FX_EFFECT_NAME_LEN, "Comet");

			// Parameter 0: Comet size
			this->setParameter(NumericEffectParameter<uint8_t>(cometSize, "Comet Size", 1, 20, 1), 0);

			// Parameter 1: Comet color
			this->setParameter(ColorHSVEffectParameter(hsv, "Comet Color"), 1);

			// Parameter 2: Comet speed
			this->setParameter(NumericEffectParameter<uint8_t>(speed, "Comet Speed", 1, 100, 1), 2);

			// Parameter 3: Interval between comet triggers
			this->setParameter(NumericEffectParameter<uint8_t>(interval, "Comet Interval", 1, 100, 5), 3);

			// Parameter 4: Comet trigger
			this->setParameter(BooleanEffectParameter(false, "Trigger comet"), 4);

			// Parameter 5: Comet direction
			this->setParameter(BooleanEffectParameter(true, "Comet direction"), 5);

			this->initModMatrixDefaults();

			WS2812_InitMultiCometEffect();
		}

		// Init default mod matrix for this effect
		void initModMatrixDefaults() override
		{
			this->modMatrix[0].modSource = NULL;
			this->modMatrix[0].modDestination = this->getParameter(0);
			this->modMatrix[0].modAmount->setValue(25);

			this->modMatrix[1].modSource = NULL;
			this->modMatrix[1].modDestination = this->getParameter(2);
			this->modMatrix[1].modAmount->setValue(25);

			this->modMatrix[2].modSource = NULL;
			this->modMatrix[2].modDestination = static_cast<ColorHSVEffectParameter *>(this->getParameter(1))->_hue.get();
			this->modMatrix[2].modAmount->setValue(100);

			this->modMatrix[3].modSource = NULL;
			this->modMatrix[3].modDestination = this->getParameter(4);
			this->modMatrix[3].modAmount->setValue(100);

			this->modMatrix[4].modSource = NULL;
			this->modMatrix[4].modDestination = this->getParameter(5);
			this->modMatrix[4].modAmount->setValue(100);
		}

		void updateEffect() override;
};

#endif /* INC_WS2812FX_COMETEFFECT_HPP_ */
