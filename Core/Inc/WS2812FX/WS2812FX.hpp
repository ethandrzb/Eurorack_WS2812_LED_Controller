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
#define WS2812FX_EFFECT_MAX_PARAMS 8
// 4 chars + null char
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

//TODO: Add support for custom format strings
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
			// Add support for additional number types
			if constexpr(std::is_same_v<T, uint8_t>)
			{
				snprintf(this->valueString, WS2812FX_PARAMETER_VALUE_STRING_LEN, "%3d", *(static_cast<uint8_t *>(this->getValue())));
			}
			if constexpr(std::is_same_v<T, uint16_t>)
			{
				snprintf(this->valueString, WS2812FX_PARAMETER_VALUE_STRING_LEN, "%3d", *(static_cast<uint16_t *>(this->getValue())));
			}
			else if constexpr(std::is_same_v<T, float>)
			{
				snprintf(this->valueString, WS2812FX_PARAMETER_VALUE_STRING_LEN, "%1.2f", *(static_cast<float *>(this->getValue())));
			}

			return this->valueString;
		}

	private:
		T minValue;
		T maxValue;
		T tickAmount;
};

class BooleanEffectParameter : public EffectParameter<bool>
{
	BooleanEffectParameter(bool value, std::string name) : EffectParameter<bool>(value, name) {}

	void incrementValue() override
	{
		this->value = true;
	}

	void decrementValue() override
	{
		this->value = false;
	}

	char *getValueString() override
	{
		snprintf(this->valueString, WS2812FX_PARAMETER_VALUE_STRING_LEN, "[%s]", (value) ? "x" : " ");

		return this->valueString;
	}
};

class ColorHSVEffectParameter : public EffectParameter<colorHSV>
{
	public:
		colorHSV hsv;
		NumericEffectParameter<uint16_t> hue = NumericEffectParameter<uint16_t>(180, "Hue", 0, 360, 5);
		NumericEffectParameter<float> saturation = NumericEffectParameter<float>(1.0, "Saturation", 0, 1, 0.05);
		NumericEffectParameter<float> value = NumericEffectParameter<float>(0.2, "Value", 0, 1, 0.05);

		ColorHSVEffectParameter(colorHSV hsv, std::string name) : EffectParameter<colorHSV>(hsv, name)
		{
			this->hsv = hsv;
			hue.setValue(hsv.hue);
			saturation.setValue(hsv.saturation);
			value.setValue(hsv.value);
		}

		// This class does not support basic increment/decrement
		// The *ByIndex versions of increment and decrement must be used instead
		// Stubbing them out prevents crashes if they somehow get called at runtime
		void incrementValue() override
		{
			return;
		}

		void decrementValue() override
		{
			return;
		}

		void incrementValueByIndex(uint8_t index)
		{
			switch(index)
			{
				case 0:
					hue.incrementValue();
					break;
				case 1:
					saturation.incrementValue();
					break;
				case 2:
					value.incrementValue();
					break;
				default:
					return;
			}
		}

		void decrementValueByIndex(uint8_t index)
		{
			switch(index)
			{
				case 0:
					hue.decrementValue();
					break;
				case 1:
					saturation.decrementValue();
					break;
				case 2:
					value.decrementValue();
					break;
				default:
					return;
			}
		}

		// Reconstruct colorHSV struct from values in NumericEffectParameters
		void *getValue() override
		{
			this->hsv.hue = *(static_cast<uint16_t *>(hue.getValue()));
			this->hsv.saturation = *(static_cast<float *>(saturation.getValue()));
			this->hsv.value = *(static_cast<float *>(value.getValue()));

			return static_cast<void *>(&(hsv));
		}

		// Shouldn't I need an override here?
		char *getValueString()
		{
			sprintf(this->valueString, "~");

			return this->valueString;
		}
};

class WS2812Effect
{
	public:
		char name[WS2812FX_EFFECT_NAME_LEN];
		virtual void updateEffect() = 0;
		std::unique_ptr<EffectParameterBase> params[WS2812FX_EFFECT_MAX_PARAMS];

		EffectParameterBase *getParameter(uint16_t index)
		{
			if(index < WS2812FX_EFFECT_MAX_PARAMS)
			{
				return this->params[index].get();
			}
			return nullptr;
		}

		template <typename T> void setParameter(const T &newParam, uint16_t index)
		{
			if(index < WS2812FX_EFFECT_MAX_PARAMS)
			{
				this->params[index] = std::make_unique<T>(newParam);
			}
		}
};
}
#endif
