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
#include <any>

#define WS2812FX_EFFECT_NAME_LEN 11
#define WS2812FX_EFFECT_PARAM_LEN 11
#define WS2812FX_EFFECT_NUM_PARAMS 5
#define WS2812FX_PARAMETER_VALUE_STRING_LEN 3

namespace WS2812FX
{
template <typename T> class EffectParameter
{
	public:
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

		template <typename U> char *getValueString();

	private:
		T minValue;
		T maxValue;
		T tickAmount;
};

template <> template <> char *NumericEffectParameter<uint8_t>::getValueString<uint8_t>()
{
	sprintf(this->valueString, "%3d", this->value);

	return this->valueString;
}

template <> template <> char *NumericEffectParameter<float>::getValueString<float>()
{
	sprintf(this->valueString, "%3f", this->value);

	return this->valueString;
}

class ColorHSVEffectParameter : public EffectParameter<colorHSV>
{
	public:
		ColorHSVEffectParameter(colorHSV hsv) : EffectParameter<colorHSV>(hsv) {}

		//TODO: Increment and decrement functions for hue, saturation, and value

		colorHSV getHSV()
		{
			colorHSV hsv = {.hue = this->value.hue, .saturation = this->value.saturation, .value = this->value.saturation};
			return hsv;
		}

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

		//TODO: Use polymorphism instead of std::any
		std::any params[WS2812FX_EFFECT_NUM_PARAMS];

//		std::any getParam(unsigned int index)
//		{
//			if(index < WS2812FX_EFFECT_NUM_PARAMS)
//			{
//				return this->params[index];
//			}
//			else
//			{
//				return NULL;
//			}
//		}
};
}
#endif
