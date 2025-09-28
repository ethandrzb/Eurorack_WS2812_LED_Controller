/*
 * WS2182.c
 *
 *  Created on: Mar 17, 2024
 *      Author: ethan
 *      Adapted from ControllersTech's WS2812 video: https://youtu.be/71SRVEcbEwc
 */


#include "main.h"
#include <limits.h>
#include <math.h>
#include "WS2812.h"

uint8_t LEDData[MAX_NUM_PHYSICAL_LEDS][NUM_LED_PARAMS];

colorRGB background = {.red = 0, .blue = 0, .green = 0};

comet comets[NUM_MAX_COMETS];

uint16_t NUM_PHYSICAL_LEDS = 97;
uint16_t DOWNSAMPLING_FACTOR = 1;

// Change to SPI handle connected to LEDs
extern SPI_HandleTypeDef hspi3;
#define LED_SPI hspi3

#ifdef ENABLE_FPS_COUNTER
extern volatile uint16_t WS2812FramesSent;
#endif

// Sets the color of the LED at index to the specified RGB values in the LEDData buffer
void WS2812_SetLED(uint16_t index, uint8_t red, uint8_t green, uint8_t blue, bool additive)
{
	if(index >= NUM_LOGICAL_LEDS)
	{
		return;
	}

	// These LEDs are supposed to use GRB color ordering, but RGB seems to be the real answer
	if(additive)
	{
		LEDData[index][0] = (LEDData[index][0] + red) > UINT8_MAX ? 255 : (LEDData[index][0] + red);
		LEDData[index][1] = (LEDData[index][1] + green) > UINT8_MAX ? 255 : (LEDData[index][1] + green);
		LEDData[index][2] = (LEDData[index][2] + blue) > UINT8_MAX ? 255 : (LEDData[index][2] + blue);
	}
	else
	{
		LEDData[index][0] = red;
		LEDData[index][1] = green;
		LEDData[index][2] = blue;
	}
}

// Adds the RGB color values to the LED at index in the LEDData buffer
void WS2812_SetLEDAdditive(uint16_t index, uint8_t red, uint8_t green, uint8_t blue)
{
	WS2812_SetLED(index, red, green, blue, true);
}

// Sets the color of all LEDs to the specified RGB color
void WS2812_SetAllLEDs(uint32_t red, uint32_t green, uint32_t blue)
{
	for(int i = 0; i < NUM_LOGICAL_LEDS; i++)
	{
		WS2812_SetLED(i, red, green, blue, false);
	}
}

// Sets the color of all LEDs to black
void WS2812_ClearLEDs(void)
{
	for(int i = 0; i < NUM_LOGICAL_LEDS; i++)
	{
		for(int j = 0; j <= 2; j++)
		{
			LEDData[i][j] = 0;
		}
	}
}

// Fades all LEDs in LEDData by the specified denominator
void WS2812_FadeAll(uint8_t denominator)
{
	for(int i = 0; i < NUM_LOGICAL_LEDS; i++)
	{
		for(int j = 0; j <= 2; j++)
		{
			LEDData[i][j] /= denominator;
		}
	}
}

// Moves the position the values of the LEDs by shiftAmount in LEDData
void WS2812_ShiftLEDs(int16_t shiftAmount)
{
	uint8_t tmp[NUM_LOGICAL_LEDS][NUM_LED_PARAMS];
	for(int i = 0; i < NUM_LOGICAL_LEDS; i++)
	{
		tmp[i][0] = LEDData[(i - shiftAmount) % NUM_LOGICAL_LEDS][0];
		tmp[i][1] = LEDData[(i - shiftAmount) % NUM_LOGICAL_LEDS][1];
		tmp[i][2] = LEDData[(i - shiftAmount) % NUM_LOGICAL_LEDS][2];
	}

	for(int i = 0; i < NUM_LOGICAL_LEDS; i++)
	{
		LEDData[i][0] = tmp[i][0];
		LEDData[i][1] = tmp[i][1];
		LEDData[i][2] = tmp[i][2];
	}
}
#ifndef USE_NEW_SEND_FUNCTIONS
// Sends the RGB color value for a single LED to the LED strip
void WS2812_SendSingleLED(uint32_t red, uint32_t green, uint32_t blue)
{
	uint32_t color = (green << 16) | (red << 8) | (blue);
	uint8_t data[24];
	int index = 0;

	// Convert color data to format expected by WS2182
	// The SPI baud rate was set to 3x that of the LEDs so we can generate the bit timings expected by the LEDs
	// using simple bit sequences (0b100 for 0 and 0b110 for 1)
	for(int i = 23; i >= 0; i--)
	{

		if((color >> i) & 0x01)
		{
			// 2 high bits and 1 low bit
			data[index] = 0b110;
		}
		else
		{
			// 1 high bit and 2 low bits
			data[index] = 0b100;
		}
		index++;
	}

	HAL_SPI_Transmit(&LED_SPI, data, 24, 1000);
}

// Sends all color values in LEDData to the LED strip
void WS2812_SendAll(void)
{
	for(int i = 0; i < NUM_LOGICAL_LEDS; i++)
	{
		// Apply background color
		WS2812_SetLEDAdditive(i, background.red, background.green, background.blue);

		WS2812_SendSingleLED(LEDData[i][0], LEDData[i][1], LEDData[i][2]);
	}
}

#else

#ifndef USE_LUT_OPTIMIZATION
// Same as WS2812_SendSingleLED, but returns the data that would be sent instead of sending it
uint8_t *WS2812_GetSingleLEDData(uint32_t red, uint32_t green, uint32_t blue)
{
	uint32_t color = (green << 16) | (red << 8) | (blue);
	static uint8_t data[24];
	int index = 0;

	// Convert color data to format expected by WS2182
	// The SPI baud rate was set to 3x that of the LEDs so we can generate the bit timings expected by the LEDs
	// using simple bit sequences (0b100 for 0 and 0b110 for 1)
	for(int i = 23; i >= 0; i--)
	{

		if((color >> i) & 0x01)
		{

#ifdef WS2811_MODE
			// 3 high bits and 3 low bits
			data[index] = 0b111000;
#else
			// 2 high bits and 1 low bit
			data[index] = 0b110;
#endif
		}
		else
		{
#ifdef WS2811_MODE
			// 1 high bit and 5 low bits
			data[index] = 0b100000;
#else
			// 1 high bit and 2 low bits
			data[index] = 0b100;
#endif
		}
		index++;
	}

	return data;
}
#else
// Same as fast version of WS2812_GetSingleLEDData, but uses lookup table to process entire bytes of the color at a time instead of going bit by bit
uint8_t *WS2812_GetSingleLEDData(uint32_t red, uint32_t green, uint32_t blue)
{
	uint32_t color = (green << 16) | (red << 8) | (blue);
	static uint8_t data[24];

	// For each byte in color
	for(int i = 16; i >= 0; i -= 8)
	{
		// Copy LUT entry to data
		memcpy(data + (16 - i), byteToWS2812DataLUT[(color >> i) & 0xFF], 8 * sizeof(uint8_t));
	}

	return data;
}
#endif

void WS2812_SendAll(void)
{
	// Sample NUM_LOGICAL_LEDS and NUM_PHYSICAL_LEDS in case they change while this function is running
	const uint16_t _DOWNSAMPLING_FACTOR = DOWNSAMPLING_FACTOR;
	const uint16_t _NUM_PHYSICAL_LEDS = NUM_PHYSICAL_LEDS;

	// Pad _NUM_PHYSICAL_LEDS to be divisible by downsampling factor
	const uint16_t _NUM_PHYSICAL_LEDS_PADDED = _NUM_PHYSICAL_LEDS + (_DOWNSAMPLING_FACTOR - (_NUM_PHYSICAL_LEDS % _DOWNSAMPLING_FACTOR));
	const uint16_t _NUM_LOGICAL_LEDS_PADDED = _NUM_PHYSICAL_LEDS_PADDED / _DOWNSAMPLING_FACTOR;

	uint8_t *data[_NUM_LOGICAL_LEDS_PADDED];
	uint8_t sendData[24 * _NUM_PHYSICAL_LEDS_PADDED];

	// Convert to 1D array
	for(int i = 0; i < _NUM_LOGICAL_LEDS_PADDED; i++)
	{
		// Apply background color
		WS2812_SetLEDAdditive(i, background.red, background.green, background.blue);

		// Get data for current LED
		data[i] = WS2812_GetSingleLEDData(LEDData[i][0], LEDData[i][1], LEDData[i][2]);

		for(int groupIndex = 0; groupIndex < _DOWNSAMPLING_FACTOR; groupIndex++)
		{
			// Append to data to be sent
			for(int j = 0; j < 24; j++)
			{
				// Data and instruction synchronization barriers
				// Ensures correct memory access when code is optimized
				__DSB();
				__ISB();

				sendData[(i * 24 * _DOWNSAMPLING_FACTOR) + (groupIndex * 24) + j] = data[i][j];
			}
		}
	}

#ifdef ENABLE_FPS_COUNTER
	WS2812FramesSent++;
#endif

	// Send data to strip
	HAL_SPI_Transmit_DMA(&hspi3, sendData, 24 * _NUM_PHYSICAL_LEDS_PADDED);
}

#endif

// Initializes and deactivates all comets in buffer
void WS2812_InitMultiCometEffect(void)
{
	for(int i = 0; i < NUM_MAX_COMETS; i++)
	{
		comets[i].position = 0;
		comets[i].color.red = 0;
		comets[i].color.green = 0;
		comets[i].color.blue = 0;
		comets[i].size = 0;
		comets[i].active = false;
	}
}

// Adds a comet to be processed by WS2812_MultiCometEffect
// color: Color of comet
// size: number of pixels used for body of comet
void WS2812_AddComet(colorRGB color, uint8_t size)
{
	uint16_t index = 0;

	while(comets[index].active)
	{
		index++;
	}

	// All comets active
	if(index >= NUM_MAX_COMETS)
	{
		return;
	}

	comets[index].position = 0;
	comets[index].color = color;
	comets[index].size = size;
	comets[index].active = true;
}

// Draws all comets to LEDData and updates their position
void WS2812_MultiCometEffect(void)
{
	// Fade LEDs one step
	WS2812_FadeAll(2);

	// Increment position of active comets
	for(int i = 0; i < NUM_MAX_COMETS; i++)
	{
		if(comets[i].active)
		{
			// Deactivate comets when head reaches the end of the LED strip
			// -1 accounts for the fact that comets are drawn from [position, position + size], NOT [position + 1, position + 1 + size]
//			if(comets[i].position >= (NUM_LEDS - (comets[i].size - 1)))
			// Deactivate comet when it reaches the end of the strip
			// This is safe because WS2812_SetLED only sets the LED if it is in bounds
			if(comets[i].position >= NUM_LOGICAL_LEDS)
			{
				comets[i].active = false;
			}
			else
			{
				// Draw comet
				for(int j = 0; j < comets[i].size; j++)
				{
					WS2812_SetLED(j + comets[i].position, comets[i].color.red, comets[i].color.green, comets[i].color.blue, false);
				}

				comets[i].position++;
			}
		}
	}
}

// Handles and draws a single comet to LEDData
void WS2812_CometEffect(void)
{
	const uint8_t fadeAmount = 2;
	const uint32_t cometSize = 5;

	static uint8_t direction = 1;
	static uint8_t position = 0;

	position += direction;

	// Flip direction when we hit the ends of the strip
	if(position == (NUM_LOGICAL_LEDS - cometSize) || (position == 0))
	{
		direction *= -1;
	}

	// Draw comet
	for(int i = 0; i < cometSize; i++)
	{
		WS2812_SetLED(i + position, 64, 64, 64, false);
	}

	// Fade LEDs one step
	WS2812_FadeAll(fadeAmount);
}

// Fills LEDs from one end to display the value level on the LED strip
// color: Color of filled LEDs
// level: Number of LEDs to fill
// flip: Changes fill direction
void WS2812_SimpleMeterEffect(colorRGB color, uint16_t level, bool flip)
{
	// Clip level
	level = (level <= NUM_LOGICAL_LEDS) ? level : NUM_LOGICAL_LEDS;

	// Fill low index to high index
	if(flip)
	{
		// Filled portion
		for(int i = 0; i < level; i++)
		{
			WS2812_SetLEDAdditive(i, color.red, color.green, color.blue);
		}

		// Unfilled portion
		for(int i = level; i < NUM_LOGICAL_LEDS; i++)
		{
			WS2812_SetLEDAdditive(i, 0, 0, 0);
		}
	}
	// Fill high index to low index
	else
	{
		// Reverse fill amount to preserve higher level ==> more LEDs filled
		level = NUM_LOGICAL_LEDS - level;

		// Unfilled portion
		for(int i = 0; i < level; i++)
		{
			WS2812_SetLEDAdditive(i, 0, 0, 0);
		}

		// Filled portion
		for(int i = level; i < NUM_LOGICAL_LEDS; i++)
		{
			WS2812_SetLEDAdditive(i, color.red, color.green, color.blue);
		}
	}
}

// Fills LEDs from both ends or from the center to display the value level on the LED strip
// color: Color of filled LEDs
// level: Number of LEDs to fill
// centered: If true, the meters are drawn from the middle LED in the strip towards the ends.
	// Otherwise, the meters are drawn from each end towards the center of the strip
void WS2812_MirroredMeterEffect(colorRGB color, uint16_t level, bool centered)
{
	// Half input level to account for the fact that two LEDs are filled for every increase in level
	level >>= 1;

	// Clip level
	level = (level <= NUM_LOGICAL_LEDS >> 1) ? level : NUM_LOGICAL_LEDS >> 1;

	if(centered)
	{
		for(int i = 0; i < NUM_LOGICAL_LEDS; i++)
		{
			// Split strip into 3 zones and fill accordingly
			// Zone 1: Unfilled [0, (NUM_LEDS >> 1) - level)
			if(i < (NUM_LOGICAL_LEDS >> 1) - level)
			{
				WS2812_SetLEDAdditive(i, 0, 0, 0);
			}
			// Zone 2: Filled [(NUM_LEDS >> 1) - level, (NUM_LEDS >> 1) + level]
			else if(i < (NUM_LOGICAL_LEDS >> 1) + level)
			{
				WS2812_SetLEDAdditive(i, color.red, color.green, color.blue);
			}
			// Zone 3: Unfilled ((NUM_LEDS >> 1) + level, NUM_LEDS)
			else
			{
				WS2812_SetLEDAdditive(i, 0, 0, 0);
			}
		}
	}
	else
	{
		// Reverse fill amount to preserve higher level ==> more LEDs filled
		level = (NUM_LOGICAL_LEDS >> 1) - level;

		for(int i = 0; i < NUM_LOGICAL_LEDS; i++)
		{
			// Split strip into 3 zones and fill accordingly
			// Zone 1: Filled [0, (NUM_LEDS >> 1) - level)
			if(i < (NUM_LOGICAL_LEDS >> 1) - level)
			{
				WS2812_SetLEDAdditive(i, color.red, color.green, color.blue);
			}
			// Zone 2: Unfilled [(NUM_LEDS >> 1) - level, (NUM_LEDS >> 1) + level]
			else if(i < (NUM_LOGICAL_LEDS >> 1) + level)
			{
				WS2812_SetLEDAdditive(i, 0, 0, 0);
			}
			// Zone 3: Filled ((NUM_LEDS >> 1) + level, NUM_LEDS)
			else
			{
				WS2812_SetLEDAdditive(i, color.red, color.green, color.blue);
			}
		}
	}
}

void WS2812_FillRainbow(colorHSV startingColor, int16_t deltaHue)
{
	bool fillForward = true;

	if(deltaHue < 0)
	{
		deltaHue *= -1;
		fillForward = false;
	}

	if(fillForward)
	{
		for(int i = 0; i < NUM_LOGICAL_LEDS; i++)
		{
			colorRGB rgb = WS2812_HSVToRGB(startingColor.hue, startingColor.saturation, startingColor.value);

			WS2812_SetLED(i, rgb.red, rgb.green, rgb.blue, false);

			startingColor.hue += deltaHue;

			if(startingColor.hue >= 360)
			{
				startingColor.hue -= 360;
			}
		}
	}
	else
	{
		for(int i = NUM_LOGICAL_LEDS - 1; i >= 0; i--)
		{
			colorRGB rgb = WS2812_HSVToRGB(startingColor.hue, startingColor.saturation, startingColor.value);

			WS2812_SetLED(i, rgb.red, rgb.green, rgb.blue, false);

			startingColor.hue += deltaHue;

			if(startingColor.hue >= 360)
			{
				startingColor.hue -= 360;
			}
		}
	}

}

// Sets the color added to all LED colors prior to being sent to the strip
void WS2812_SetBackgroundColor(uint8_t red, uint8_t green, uint8_t blue)
{
	background.red = red;
	background.green = green;
	background.blue = blue;
}

void WS2812_SetBackgroundColorHSV(colorHSV *hsv)
{
	colorRGB tmp = WS2812_HSVToRGB(hsv->hue, hsv->saturation, hsv->value);

	background.red = tmp.red;
	background.green = tmp.green;
	background.blue = tmp.blue;
}


// Converts a HSV color to an RGB color struct
// hue: [0,360)
// saturation: [0,1]
// value: [0,1]
colorRGB WS2812_HSVToRGB(uint16_t hue, float saturation, float value)
{
	colorRGB retVal = {.red = 0, .green = 0, .blue = 0};

	float chroma = value * saturation;
	float x = chroma * (1.0f - fabs(fmod(((double)hue / 60.0f), 2.0) - 1.0f));
	float m = value - chroma;

	float red_prime = 0.0f;
	float green_prime = 0.0f;
	float blue_prime = 0.0f;

	if(hue >= 0 && hue < 60)
	{
		red_prime = chroma;
		green_prime = x;
		blue_prime = 0.0f;
	}
	else if(hue >= 60 && hue < 120)
	{
		red_prime = x;
		green_prime = chroma;
		blue_prime = 0.0f;
	}
	else if(hue >= 120 && hue < 180)
	{
		red_prime = 0.0f;
		green_prime = chroma;
		blue_prime = x;
	}
	else if(hue >= 180 && hue < 240)
	{
		red_prime = 0.0f;
		green_prime = x;
		blue_prime = chroma;
	}
	else if(hue >= 240 && hue < 300)
	{
		red_prime = x;
		green_prime = 0.0f;
		blue_prime = chroma;
	}
	else if(hue >= 300 && hue < 360)
	{
		red_prime = chroma;
		green_prime = 0.0f;
		blue_prime = x;
	}

	retVal.red = (red_prime + m) * 255.0;
	retVal.green = (green_prime + m) * 255.0;
	retVal.blue = (blue_prime + m) * 255.0;

	return retVal;
}
