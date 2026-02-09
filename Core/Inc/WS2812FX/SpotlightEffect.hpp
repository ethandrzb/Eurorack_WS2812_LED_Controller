/*
 * SpotlightEffect.hpp
 *
 *  Created on: Feb 8, 2026
 *      Author: ethan
 */

#ifndef INC_WS2812FX_SPOTLIGHT_HPP_
#define INC_WS2812FX_SPOTLIGHT_HPP_

#include <WS2812FX/WS2812FX.hpp>
#include <main.h>

extern int16_t NUM_PHYSICAL_LEDS;

using namespace WS2812FX;

typedef enum spotlightState
{
	SPOTLIGHT_IDLE,
	SPOTLIGHT_INCREASING,
	SPOTLIGHT_DECREASING
} spotlightState;

typedef struct spotlight
{
	float x;
	float w;

	colorRGB color;

	float dw;
	//TODO: float dx;

	spotlightState state;
} spotlight;

#define MAX_ACTIVE_SPOTS 15

class SpotlightEffect : public WS2812Effect
{
public:
	SpotlightEffect(float dw, float wMax, uint16_t maxActiveSpots, uint16_t spawnPeriod, colorHSV hsv)
	{
		snprintf(this->name, WS2812FX_EFFECT_NAME_LEN, "Spotlight");

		this->setParameter(NumericEffectParameter<float>(dw, "Width step size", 0.005, 0.15, 0.005), 0);
		this->setParameter(NumericEffectParameter<float>(wMax, "Max width", 0, 1, 0.05), 1);
		this->setParameter(NumericEffectParameter<uint16_t>(maxActiveSpots, "Max active spots", 1, MAX_ACTIVE_SPOTS, 1), 2);
		this->setParameter(NumericEffectParameter<uint16_t>(spawnPeriod, "Spawn period", 0, 100, 5), 3);
		this->setParameter(ColorHSVEffectParameter(hsv, "Spot color"), 4);

		// Initialize all spotlights
		for(int i = 0; i < MAX_ACTIVE_SPOTS; i++)
		{
			spots[i] = (spotlight *) calloc(1, sizeof(spotlight));

			spots[i]->state = SPOTLIGHT_IDLE;
		}

		this->initModMatrixDefaults();
	}

	// Init default mod matrix for this effect
	void initModMatrixDefaults() override
	{
		// Directly map effect parameters to mod destinations
		for(int i = 0; i < 4; i++)
		{
			this->modMatrix[i].modSource = NULL;
			this->modMatrix[i].modDestination = this->getParameter(i);
			this->modMatrix[i].modAmount->setValue(100);
		}
	}

	void updateEffect() override;
private:
	spotlight *spots[MAX_ACTIVE_SPOTS];
};

#endif /* INC_WS2812FX_SPOTLIGHT_HPP_ */
