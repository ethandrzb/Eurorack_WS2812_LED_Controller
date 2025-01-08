#include "WS2812FX.h"
#include "main.h"

// TODO: Change this function to be called repeatedly instead of blocking until a complete breathing cycle has completed
// The current behavior causes the next effect to be queued until this function finishes instead of immediately going to the new effect
void WS2812FX_SimpleBreathingEffect(uint8_t stepDelay, float stepSize, colorHSV color, float maxValue)
{
	colorRGB rgb = WS2812_HSVToRGB(color.hue, color.saturation, color.value);

	for(float i = 0.0; i < maxValue; i += stepSize)
	{
		rgb = WS2812_HSVToRGB(color.hue, color.saturation, i);
		WS2812_SetAllLEDs(rgb.red, rgb.green, rgb.blue);
		WS2812_SendAll();
		HAL_Delay(stepDelay);
	}
	for(float i = maxValue; i >= 0; i -= stepSize)
	{
		rgb = WS2812_HSVToRGB(color.hue, color.saturation, i);
		WS2812_SetAllLEDs(rgb.red, rgb.green, rgb.blue);
		WS2812_SendAll();
		HAL_Delay(stepDelay);
	}
}
