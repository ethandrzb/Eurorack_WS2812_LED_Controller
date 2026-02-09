/*
 * BlobEffect.hpp
 *
 *  Created on: Jan 15, 2026
 *      Author: ethan
 */

#ifndef INC_WS2812FX_BLOBEFFECT_HPP_
#define INC_WS2812FX_BLOBEFFECT_HPP_

#include <WS2812FX/WS2812FX.hpp>
#include <main.h>
#include <math.h>

using namespace WS2812FX;

class BlobEffect : public WS2812Effect
{
public:
	const uint8_t NUM_BLOBS = 1;
	const uint8_t NUM_BLOB_PARAMETERS = 4;

	BlobEffect(float startFraction, float endFraction, colorHSV hsv, uint8_t isConstantLength)
	{
		snprintf(this->name, WS2812FX_EFFECT_NAME_LEN, "Blob");

		for(uint8_t i = 0; i < NUM_BLOBS; i++)
		{
			// Each blob, i , of the NUM_BLOBS blobs created has 4 parameters indexed starting at zero
			// from the blobs's 0th parameter (startFraction) to its 3rd (isConstantLength)
			// Parameter Index | Description
			//       i         | Blob i start position represented as fraction of total length of strip
			//     i + 1       | Blob i end position represented as fraction of total length of strip
			//     i + 2       | Blob i color
			//     i + 3       | Blob i is in constant length (moving start will move end to keep the length constant); endFraction remapped to be fraction of total length of strip to occupt
			this->setParameter(NumericEffectParameter<float>(startFraction, "Start " + std::to_string(i), 0.0f, 1.0f, 0.01f), (NUM_BLOB_PARAMETERS * i));
			this->setParameter(NumericEffectParameter<float>(endFraction, "End/Len " + std::to_string(i), 0.0f, 1.0f, 0.01f), 1 + (NUM_BLOB_PARAMETERS * i));
			this->setParameter(ColorHSVEffectParameter(hsv, "Color " + std::to_string(i)), 2 + (NUM_BLOB_PARAMETERS * i));
			this->setParameter(BooleanEffectParameter(isConstantLength, "Const len " + std::to_string(i)), 3 + (NUM_BLOB_PARAMETERS * i));

			// Vary initial parameters
			startFraction += 0.10f;
			endFraction += 0.05f;
			hsv.hue = (hsv.hue + 90) % 360;
		}

		this->initModMatrixDefaults();
	}

	// Init default mod matrix for this effect
	void initModMatrixDefaults() override
	{
		// Only pre-map modulation for first two blobs
		for(uint8_t i = 0; i < 2; i++)
		{
			// Map blob start positions to CV inputs A and B
			this->modMatrix[2 * i].modSource = NULL;
			this->modMatrix[2 * i].modDestination = this->getParameter(NUM_BLOB_PARAMETERS * i);
			this->modMatrix[2 * i].modAmount->setValue(100);

			// Map meter hues to CV inputs C and D
			this->modMatrix[2 * i + 1].modSource = NULL;
			this->modMatrix[2 * i + 1].modDestination = this->getParameter(1 + (NUM_BLOB_PARAMETERS * i));
			this->modMatrix[2 * i + 1].modAmount->setValue(100);
		}
	}

	void updateEffect() override;
};

#endif /* INC_WS2812FX_BLOBEFFECT_HPP_ */
