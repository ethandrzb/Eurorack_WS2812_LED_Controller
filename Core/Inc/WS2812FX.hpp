/*
 * WS2812FX.cpp
 *
 *  Created on: Jan 13, 2025
 *      Author: ethan
 */

#ifndef INC_WS2812FX_HPP_
#define INC_WS2812FX_HPP_

#include <main.h>
#include <WS2812.h>
#include <stdio.h>

#define WS2812FX_EFFECT_NAME_LEN 11
#define WS2812FX_EFFECT_PARAM_LEN 11
#define WS2812FX_EFFECT_NUM_PARAMS 5
// 3 chars + null char
#define WS2812FX_PARAMETER_VALUE_STRING_LEN 4

namespace WS2812FX
{

class EffectParameterBase
{
	public:
		virtual ~EffectParameterBase() {}
};

template <typename T> class EffectParameter : public EffectParameterBase
{
	public:
		virtual ~EffectParameter() {}
		char name[WS2812FX_EFFECT_PARAM_LEN];
		char valueString[WS2812FX_PARAMETER_VALUE_STRING_LEN];
		T value;
		virtual char *getValueString();

		EffectParameter(T value) : value(value) {}

		void setValue(T newValue)
		{
			this->value = newValue;
		}

		T getValue()
		{
			return this->value;
		}
};

template <typename T> class NumericEffectParameter : public EffectParameter<T>
{
	public:
		NumericEffectParameter(T value, T minValue, T maxValue, T tickAmount) : EffectParameter<T>(value), minValue(minValue), maxValue(maxValue), tickAmount(tickAmount) {}

		void incrementValue()
		{
			this->value = (this->value < this->maxValue) ? this->value + this->tickAmount : this->maxValue;
		}

		void decrementValue()
		{
			this->value = (this->value > this->minValue) ? this->value - this->tickAmount : this->minValue;
		}

		T getMinValue()
		{
			return this->minValue;
		}

		T getMaxValue()
		{
			return this->maxValue;
		}

		template <typename U> char *getValueString();

	private:
		T minValue;
		T maxValue;
		T tickAmount;
};

template <> template <> char *NumericEffectParameter<uint8_t>::getValueString<uint8_t>()
{
	snprintf(this->valueString, WS2812FX_PARAMETER_VALUE_STRING_LEN, "%3d", this->getValue());

	return this->valueString;
}

template <> template <> char *NumericEffectParameter<float>::getValueString<float>()
{
	snprintf(this->valueString, WS2812FX_PARAMETER_VALUE_STRING_LEN, "%3f", this->getValue());

	return this->valueString;
}

class ColorHSVEffectParameter : public EffectParameter<colorHSV>
{
	public:
		ColorHSVEffectParameter(colorHSV hsv) : EffectParameter<colorHSV>(hsv) {}

		//TODO: Increment and decrement functions for hue, saturation, and value

		// Shouldn't I need an override here?
		char *getValueString()
		{
			sprintf(this->valueString, "   ");

			return this->valueString;
		}
};

class WS2812Effect
{
	public:
		char name[WS2812FX_EFFECT_NAME_LEN];
		virtual void updateEffect() = 0;
		EffectParameterBase *params[WS2812FX_EFFECT_NUM_PARAMS];

		EffectParameterBase *getParameter(uint16_t index)
		{
			return (index < WS2812FX_EFFECT_NUM_PARAMS) ? params[index]: NULL;
		}

		void setParameter(EffectParameterBase newParam, uint16_t index)
		{
			if(index < WS2812FX_EFFECT_NUM_PARAMS)
			{
				*(this->params[index]) = newParam;
			}
		}
};
}
#endif
