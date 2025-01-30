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
#define WS2812FX_PARAMETER_VALUE_STRING_LEN 5

namespace WS2812FX
{

class EffectParameterBase
{
	public:
		std::string name;
		char valueString[WS2812FX_PARAMETER_VALUE_STRING_LEN];

		EffectParameterBase(std::string name) : name(name) {}
		virtual ~EffectParameterBase() {}
		virtual void *getValue() = 0;
		virtual char *getValueString() = 0;
		virtual void incrementValue() = 0;
		virtual void decrementValue() = 0;
};

template <typename T> class EffectParameter : public EffectParameterBase
{
	public:
		T value;

		EffectParameter(T value, std::string name) : EffectParameterBase(name), value(value) {}
		virtual ~EffectParameter() {}

		void setValue(T newValue)
		{
			this->value = newValue;
		}

		void *getValue() override
		{
			return static_cast<void *>(&(this->value));
		}

		char *getValueString() override
		{
			snprintf(this->valueString, WS2812FX_PARAMETER_VALUE_STRING_LEN, "XXX");

			return this->valueString;
		}
};

template <typename T> class NumericEffectParameter : public EffectParameter<T>
{
	public:
		NumericEffectParameter(T value, std::string name, T minValue, T maxValue, T tickAmount) : EffectParameter<T>(value, name), minValue(minValue), maxValue(maxValue), tickAmount(tickAmount) {}

		void incrementValue() override
		{
			this->value = (this->value < this->maxValue) ? this->value + this->tickAmount : this->maxValue;
		}

		void decrementValue() override
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

		char *getValueString() override
		{
			if constexpr(std::is_same_v<T, uint8_t>)
			{
				snprintf(this->valueString, WS2812FX_PARAMETER_VALUE_STRING_LEN, "%3d", *(static_cast<uint8_t *>(this->getValue())));
			}
			else if constexpr(std::is_same_v<T, float>)
			{
				snprintf(this->valueString, WS2812FX_PARAMETER_VALUE_STRING_LEN, "%1.2f", *(static_cast<float *>(this->getValue())));
			}
			else
			{
				snprintf(this->valueString, WS2812FX_PARAMETER_VALUE_STRING_LEN, "   ");
			}

			return this->valueString;
		}

	private:
		T minValue;
		T maxValue;
		T tickAmount;
};

class ColorHSVEffectParameter : public EffectParameter<colorHSV>
{
	public:
		ColorHSVEffectParameter(colorHSV hsv, std::string name) : EffectParameter<colorHSV>(hsv, name) {}

		//TODO: Encapsulate hue, saturation, and value in discrete NumericEffectParameters
		//TODO: Increment and decrement functions for hue, saturation, and value
		void incrementValue() override
		{
			return;
		}

		void decrementValue() override
		{
			return;
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
		std::unique_ptr<EffectParameterBase> params[WS2812FX_EFFECT_NUM_PARAMS];

		EffectParameterBase *getParameter(uint16_t index)
		{
			if(index < WS2812FX_EFFECT_NUM_PARAMS)
			{
				return this->params[index].get();
			}
			return nullptr;
		}

		template <typename T> void setParameter(const T &newParam, uint16_t index)
		{
			if(index < WS2812FX_EFFECT_NUM_PARAMS)
			{
				this->params[index] = std::make_unique<T>(newParam);
			}
		}
};
}
#endif
