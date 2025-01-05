#include "WS2812FX.h"
#include "main.h"

void WS2812FX_SimpleBreathingEffect(uint8_t speed, colorRGB color)
{
	for(int i = 0; i < 128; i++)
	{
		for(int led = 0; led < NUM_LOGICAL_LEDS; led++)
		{
			WS2812_SetLED(led, i, i, i);
		}
		WS2812_SendAll();
		HAL_Delay(speed);
	}
	for(int i = 128; i >= 0; i--)
	{
		for(int led = 0; led < NUM_LOGICAL_LEDS; led++)
		{
			WS2812_SetLED(led, i, i, i);
		}
		WS2812_SendAll();
		HAL_Delay(speed);
	}
}
