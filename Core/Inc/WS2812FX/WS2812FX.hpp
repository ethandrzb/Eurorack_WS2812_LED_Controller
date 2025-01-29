/*
 * WS2812FX.cpp
 *
 *  Created on: Jan 13, 2025
 *      Author: ethan
 */

#ifndef INC_WS2812FX_HPP_
#define INC_WS2812FX_HPP_

#include <main.h>
#include <memory>
#include <WS2812.h>
#include <stdio.h>
#include <string>

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

		T value;
		std::string name;
		char valueString[WS2812FX_PARAMETER_VALUE_STRING_LEN];

		EffectParameter(T value, std::string name) : value(value), name(name) {}

		void setValue(T newValue)
		{
			this->value = newValue;
		}

		T getValue()
		{
			return this->value;
		}

		char *getValueString()
		{
			snprintf(this->valueString, WS2812FX_PARAMETER_VALUE_STRING_LEN, "XXX");

			return this->valueString;
		}
};

template <typename T> class NumericEffectParameter : public EffectParameter<T>
{
	public:
		NumericEffectParameter(T value, std::string name, T minValue, T maxValue, T tickAmount) : EffectParameter<T>(value, name), minValue(minValue), maxValue(maxValue), tickAmount(tickAmount) {}

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

template <> template <> char *NumericEffectParameter<uint8_t>::getValueString<uint8_t>();

template <> template <> char *NumericEffectParameter<float>::getValueString<float>();

class ColorHSVEffectParameter : public EffectParameter<colorHSV>
{
	public:
		ColorHSVEffectParameter(colorHSV hsv, std::string name) : EffectParameter<colorHSV>(hsv, name) {}

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
		std::unique_ptr<EffectParameterBase> params[WS2812FX_EFFECT_NUM_PARAMS];

		template <typename T> T *getParameter(uint16_t index) const
		{
			if(index < WS2812FX_EFFECT_NUM_PARAMS)
			{
				if constexpr(std::is_same_v<T, ColorHSVEffectParameter>)
				{
					auto retVal = dynamic_cast<ColorHSVEffectParameter *>(this->params[index].get());
					return retVal;
				}
				else
				{
					auto retVal = dynamic_cast<T *>(this->params[index].get());
					return retVal;
				}
			}
			return NULL;
		}

		template <typename T> void setParameter(const T &newParam, uint16_t index)
		{
			if(index < WS2812FX_EFFECT_NUM_PARAMS)
			{
				if constexpr(std::is_same_v<T, ColorHSVEffectParameter>)
				{
					this->params[index] = std::make_unique<ColorHSVEffectParameter>(newParam);
				}
				else
				{
					this->params[index] = std::make_unique<T>(newParam);
				}
			}
		}
};
}
#endif
