/*
 * WS2812FX.cpp
 *
 *  Created on: Jan 13, 2025
 *      Author: ethan
 */

#ifndef INC_WS2812FX_HPP_
#define INC_WS2812FX_HPP_

#include <functional>
#include <limits.h>
#include <main.h>
#include <memory>
#include <stdio.h>
#include <string>
#include <WS2812.h>

#define WS2812FX_EFFECT_NAME_LEN 11
#define WS2812FX_EFFECT_PARAM_LEN 11
#define WS2812FX_EFFECT_MAX_PARAMS 8
#define WS2812FX_EFFECT_MAX_MOD_SLOTS 3
// 4 chars + null char
#define WS2812FX_PARAMETER_VALUE_STRING_LEN 5

// MUST be consistent with ADC data type used for CV inputs
#define RAW_MODULATION_MIN 0
#define RAW_MODULATION_MAX UINT8_MAX

namespace WS2812FX
{

class EffectParameterBase
{
	public:
		std::string name;
		uint16_t *modulationSource;
		float modulationScale;
		char valueString[WS2812FX_PARAMETER_VALUE_STRING_LEN];

		EffectParameterBase(std::string name) : name(name), modulationSource(NULL), modulationScale(0.0f) {}
		virtual ~EffectParameterBase() {}
		virtual void *getValue() = 0;
		virtual void *getValueRaw() = 0;
		virtual char *getValueString() = 0;
		virtual void incrementValue() = 0;
		virtual void decrementValue() = 0;
};

template <typename T> class EffectParameter : public EffectParameterBase
{
	public:
		T value;
		std::function<T(T)> modulationMapper;

		EffectParameter(T value, std::string name) : EffectParameterBase(name), value(value) {}
		virtual ~EffectParameter() {}

		void setValue(T newValue)
		{
			this->value = newValue;
		}

		void setModulationSource(uint16_t *newModulationSource)
		{
			this->modulationSource = newModulationSource;
		}

		void *getValue() override
		{
			return static_cast<void *>(&(this->value));
		}

		// Not adding modulation to value here because not every type supports the '+'
		void *getValueRaw() override
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
		//TODO: Add support for custom mapping functions
		NumericEffectParameter(T value, std::string name, T minValue, T maxValue, T tickAmount) : EffectParameter<T>(value, name), minValue(minValue), maxValue(maxValue), tickAmount(tickAmount)
		{
			this->modulatedValue = this->value;

			// Create linear mapping function from raw ADC values to min and max values for this parameter
			this->modulationMapper = [=](T raw) {

				//TODO: Convert raw ADC data to signed when the appropriate circuitry has been designed and connected
				// Trying to implement the offsets and other logic necessary to handle bipolar CV signals without actually being able to process them has been rather difficult
				if constexpr(std::is_same_v<T, int16_t>)
				{
					// Move zero position of ADC data for signed parameters
					raw += 128;
				}

				// Positive minimum values will cause the mapped modulation value to be shifted up if a traditional linear mapping is used
				// The unmodulated value of this parameter is also bound by this Positive minimum, meaning that the minimum modulated value is 2x the minimum value
				// In other words, if the range of values for a parameter is [a,b], the modulation applied to the value must be bound to the range [0, b-a].
				return ((raw - RAW_MODULATION_MIN) * (maxValue - minValue) / (RAW_MODULATION_MAX - RAW_MODULATION_MIN)) + ((minValue < 0) ? minValue : 0);
			};
		}

		void incrementValue() override
		{
			this->value = (this->value < this->maxValue) ? this->value + this->tickAmount : this->maxValue;
		}

		void decrementValue() override
		{
			this->value = (this->value > this->minValue) ? this->value - this->tickAmount : this->minValue;
		}

		void *getValue() override
		{
			// I feel like I should need this null check, but it causes problems when add it
//			if(this->modulationSource == NULL)
//			{
////				return this->getValueRaw();
//				return static_cast<void *>(&this->modulatedValue);
//			}

			// This might cause an overflow!
			// Make sure to use a sufficiently large data type
//			this->modulatedValue = this->value + this->modulationMapper((T)((float)*(this->modulationSource)) * (this->modulationScale));
			this->modulatedValue = this->value + this->modulationMapper(*(this->modulationSource));

			// Clip range of modulated value
			if(this->modulatedValue > this->maxValue)
			{
				this->modulatedValue = this->maxValue;
			}
			else if(this->modulatedValue < this->minValue)
			{
				this->modulatedValue = this->minValue;
			}

			return static_cast<void *>(&this->modulatedValue);
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
				snprintf(this->valueString, WS2812FX_PARAMETER_VALUE_STRING_LEN, "%3d", *(static_cast<uint8_t *>(this->getValueRaw())));
			}
			else if constexpr(std::is_same_v<T, uint16_t>)
			{
				snprintf(this->valueString, WS2812FX_PARAMETER_VALUE_STRING_LEN, "%3d", *(static_cast<uint16_t *>(this->getValueRaw())));
			}
			else if constexpr(std::is_same_v<T, int16_t>)
			{
				snprintf(this->valueString, WS2812FX_PARAMETER_VALUE_STRING_LEN, "%3d", *(static_cast<int16_t *>(this->getValueRaw())));
			}
			else if constexpr(std::is_same_v<T, float>)
			{
				snprintf(this->valueString, WS2812FX_PARAMETER_VALUE_STRING_LEN, "%1.2f", *(static_cast<float *>(this->getValueRaw())));
			}

			return this->valueString;
		}

	private:
		T modulatedValue;
		T minValue;
		T maxValue;
		T tickAmount;
};

class BooleanEffectParameter : public EffectParameter<bool>
{
	public:
		BooleanEffectParameter(bool value, std::string name) : EffectParameter<bool>(value, name)
		{
			this->modulatedValue = false;
		}

		void incrementValue() override
		{
			this->value = true;
		}

		void decrementValue() override
		{
			this->value = false;
		}

		void *getValue() override
		{
			//See note in NumericEffectParameter::getValue about null check
//			if(this->modulationSource == NULL)
//			{
//				return static_cast<void *>(&this->value);
//			}

			this->modulatedValue = this->value || (*(this->modulationSource) > 127);

			return static_cast<void *>(&this->modulatedValue);
		}

		char *getValueString() override
		{
			snprintf(this->valueString, WS2812FX_PARAMETER_VALUE_STRING_LEN, "[%s]", (value) ? "x" : " ");

			return this->valueString;
		}

	private:
		bool modulatedValue;
};

//TODO: Add custom mapping function for hue modulation to allow hue to wrap around
class ColorHSVEffectParameter : public EffectParameter<colorHSV>
{
	public:
		std::shared_ptr<NumericEffectParameter<uint16_t>> _hue = std::make_shared<NumericEffectParameter<uint16_t>>(180, "Hue", 0, 360, 5);
		std::shared_ptr<NumericEffectParameter<float>> _saturation = std::make_shared<NumericEffectParameter<float>>(1.0, "Saturation", 0, 1, 0.05);
		std::shared_ptr<NumericEffectParameter<float>> _value = std::make_shared<NumericEffectParameter<float>>(0.2, "Value", 0, 1, 0.05);

		ColorHSVEffectParameter(colorHSV hsv, std::string name) : EffectParameter<colorHSV>(hsv, name)
		{
			_hue->setValue(hsv.hue);
			_saturation->setValue(hsv.saturation);
			_value->setValue(hsv.value);
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

		// Members of the colorHSV struct are broken into their own parameters, so we need to update those instead of the struct itself
		// Changing the wrapped value directly (e.g., this->value = newValue) DOES NOT WORK
		// This due to the fact getValue reconstructs the colorHSV struct using the values contained in the subparameters instead of returning the raw value
		void setValue(colorHSV hsv)
		{
			_hue->setValue(hsv.hue);
			_saturation->setValue(hsv.saturation);
			_value->setValue(hsv.value);
		}

		void incrementValueByIndex(uint8_t index)
		{
			switch(index)
			{
				case 0:
					_hue->incrementValue();
					break;
				case 1:
					_saturation->incrementValue();
					break;
				case 2:
					_value->incrementValue();
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
					_hue->decrementValue();
					break;
				case 1:
					_saturation->decrementValue();
					break;
				case 2:
					_value->decrementValue();
					break;
				default:
					return;
			}
		}

		// Reconstruct colorHSV struct from values in NumericEffectParameters
		void *getValue() override
		{
			this->value.hue = *(static_cast<uint16_t *>(_hue->getValue()));
			this->value.saturation = *(static_cast<float *>(_saturation->getValue()));
			this->value.value = *(static_cast<float *>(_value->getValue()));

			return static_cast<void *>(&(value));
		}

		// Reconstruct colorHSV struct from values in NumericEffectParameters
		void *getValueRaw() override
		{
			this->value.hue = *(static_cast<uint16_t *>(_hue->getValueRaw()));
			this->value.saturation = *(static_cast<float *>(_saturation->getValueRaw()));
			this->value.value = *(static_cast<float *>(_value->getValueRaw()));

			return static_cast<void *>(&(value));
		}

		// Shouldn't I need an override here?
		char *getValueString()
		{
			sprintf(this->valueString, "~");

			return this->valueString;
		}
};

class ModMatrixEntry
{
	public:
		uint16_t *modSource;
		EffectParameterBase* modDestination;
		// Using an effect parameter to store the amount means we should be able to modulate modulation amounts with minimal effort
		std::unique_ptr<NumericEffectParameter<int16_t>> modAmount;

		ModMatrixEntry()
		{
			//TODO: Prefix names of amount parameter with identifier to differentiate them from other mod amounts
			this->modAmount = std::make_unique<NumericEffectParameter<int16_t>>(5, "Mod Amount", -100, 100, 1);
		}

		//TODO: Write a function to convert modAmount to a percentage of the range of modDestination and apply this value to modDestination
};

class WS2812Effect
{
	public:
		char name[WS2812FX_EFFECT_NAME_LEN];
		std::shared_ptr<EffectParameterBase> params[WS2812FX_EFFECT_MAX_PARAMS];
		std::vector<std::shared_ptr<EffectParameterBase>> params_expanded;
		ModMatrixEntry modMatrix[WS2812FX_EFFECT_MAX_MOD_SLOTS];

		EffectParameterBase *getParameter(uint16_t index)
		{
			if(index < WS2812FX_EFFECT_MAX_PARAMS)
			{
				return this->params[index].get();
			}
			return nullptr;
		}

		//TODO: Dedicated function to get the value from an effect parameter?
		// This should cut down on the code needed to fetch the value stored in a parameter.

		template <typename T> void setParameter(const T &newParam, uint16_t index)
		{
			if(index < WS2812FX_EFFECT_MAX_PARAMS)
			{
				this->params[index] = std::make_shared<T>(newParam);
			}
		}

		std::vector<std::shared_ptr<EffectParameterBase>> getExpandedParameters()
		{
			// Only do this once per effect
			if(params_expanded.size() > 0)
			{
				return params_expanded;
			}

			// Add unused placeholder parameter as valid modulation destination to deactivate a mod slot
			params_expanded.push_back(noneEffectParameter);

			for(int i = 0; i < WS2812FX_EFFECT_MAX_PARAMS; i++)
			{
				// Ensure parameters are valid objects before adding them to either vector
				if(dynamic_cast<ColorHSVEffectParameter *>(this->getParameter(i)))
				{
					ColorHSVEffectParameter *tmp = dynamic_cast<ColorHSVEffectParameter *>(this->getParameter(i));

					// Prepend color name to sub parameters to differentiate them in the combined list
					tmp->_hue->name = tmp->name.substr(0, 8) + " Hue";
					tmp->_saturation->name = tmp->name.substr(0, 8) + " Sat";
					tmp->_value->name = tmp->name.substr(0, 8) + " Val";

					// Break current color into sub parameters before adding to list
					params_expanded.push_back(tmp->_hue);
					params_expanded.push_back(tmp->_saturation);
					params_expanded.push_back(tmp->_value);
				}
				else if(dynamic_cast<EffectParameterBase *>(this->getParameter(i)))
				{
					params_expanded.push_back(params[i]);
				}
			}

			return params_expanded;
		}

		virtual void updateEffect() = 0;
		virtual void initModMatrixDefaults() = 0;

	private:
		//TODO: Make one empty parameter and use for all effects instead of creating a separate empty parameter for each effect
		// Should only be necessary if RAM becomes an issue
		std::shared_ptr<NumericEffectParameter<uint8_t>> noneEffectParameter = std::make_unique<NumericEffectParameter<uint8_t>>(0, "None", 0, 1, 1);
};
}
#endif
