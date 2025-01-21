/*
 * WS2812FX.cpp
 *
 *  Created on: Jan 20, 2025
 *      Author: ethan
 */

#include <WS2812FX/WS2812FX.hpp>

using namespace WS2812FX;

template <> template <> char *NumericEffectParameter<uint8_t>::getValueString<uint8_t>()
{
	snprintf(this->valueString, WS2812FX_PARAMETER_VALUE_STRING_LEN, "%3d", this->getValue());

	return this->valueString;
}

template <> template <> char *NumericEffectParameter<float>::getValueString<float>()
{
	snprintf(this->valueString, WS2812FX_PARAMETER_VALUE_STRING_LEN, "%1.2f", this->getValue());

	return this->valueString;
}
