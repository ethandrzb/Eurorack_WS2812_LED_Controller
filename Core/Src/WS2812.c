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
uint16_t FRACTAL_FACTOR = 1;

// Change to SPI handle connected to LEDs
extern SPI_HandleTypeDef hspi3;
#define LED_SPI hspi3

#ifdef ENABLE_PERFORMANCE_MONITOR
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

// Alias for SetLED with additive set to true
void WS2812_SetLEDAdditive(uint16_t index, uint8_t red, uint8_t green, uint8_t blue)
{
	WS2812_SetLED(index, red, green, blue, true);
}

// Sets one or more LEDs at the specified to the specified color to create the illusion of non-discrete position
void WS2812_DrawLine(float position, float length, uint8_t red, uint8_t green, uint8_t blue, bool additive)
{
	uint16_t index;
	float remaining;

	// Determine fraction of first LED to draw
	float firstLEDFraction = 1.0f - (position - ((uint16_t) position));

	// Use length as fraction if length is very small (less than one LED)
	firstLEDFraction = MIN(firstLEDFraction, length);

	// Clip length to prevent out-of-bounds indexing
	remaining = MIN(length, NUM_LOGICAL_LEDS - position);

	// Cast position to integer index
	index = position;

	// Draw first LED
	if(remaining > 0.0f)
	{
		WS2812_SetLEDAdditive(index, red * firstLEDFraction, green * firstLEDFraction, blue * firstLEDFraction);
		index++;
		remaining -= firstLEDFraction;
	}

	// Draw middle LEDs
	while(remaining > 1.0f)
	{
		WS2812_SetLEDAdditive(index, red, green, blue);
		index++;
		remaining--;
	}

	// Draw last LED
	if(remaining > 0.0f)
	{
		WS2812_SetLEDAdditive(index, red * remaining, green * remaining, blue * remaining);
	}
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
	// Sample DOWNSAMPLING_FACTOR and NUM_PHYSICAL_LEDS in case they change while this function is running
	const uint16_t _NUM_PHYSICAL_LEDS = NUM_PHYSICAL_LEDS;
	const uint16_t _DOWNSAMPLING_FACTOR = DOWNSAMPLING_FACTOR;
	const uint16_t _FRACTAL_FACTOR = FRACTAL_FACTOR;

	// Pad _NUM_PHYSICAL_LEDS to be divisible by downsampling factor
	const uint16_t _NUM_PHYSICAL_LEDS_PADDED = _NUM_PHYSICAL_LEDS + (_DOWNSAMPLING_FACTOR - (_NUM_PHYSICAL_LEDS % _DOWNSAMPLING_FACTOR));
	const uint16_t _NUM_LOGICAL_LEDS_PADDED = _NUM_PHYSICAL_LEDS_PADDED / _DOWNSAMPLING_FACTOR;

	const uint16_t _FRACTAL_GROUP_SIZE = _NUM_LOGICAL_LEDS_PADDED / _FRACTAL_FACTOR;

	uint8_t *data[_NUM_LOGICAL_LEDS_PADDED];
	uint8_t sendData[24 * _NUM_PHYSICAL_LEDS_PADDED];

	// Convert to 1D array
	for(int i = 0; i < _NUM_LOGICAL_LEDS_PADDED; i++)
	{
		// Apply background color
		WS2812_SetLEDAdditive(i, background.red, background.green, background.blue);

		// Get data for current LED
		if(_FRACTAL_GROUP_SIZE <= 1)
		{
			// Fractal effect disabled
			data[i] = WS2812_GetSingleLEDData(LEDData[i][0], LEDData[i][1], LEDData[i][2]);
		}
		else
		{
			// Fractal effect enabled
			//TODO: Figure out why fractal doesn't line up with the end of the strip correctly
			uint16_t LEDIndex = (i % _FRACTAL_GROUP_SIZE) * _FRACTAL_FACTOR;
			data[i] = WS2812_GetSingleLEDData(LEDData[LEDIndex][0], LEDData[LEDIndex][1], LEDData[LEDIndex][2]);
		}

		for(int groupIndex = 0; groupIndex < _DOWNSAMPLING_FACTOR; groupIndex++)
		{
			// Append to data to be sent
			for(int j = 0; j < 24; j++)
			{
				sendData[(i * 24 * _DOWNSAMPLING_FACTOR) + (groupIndex * 24) + j] = data[i][j];
			}
		}
	}

#ifdef ENABLE_PERFORMANCE_MONITOR
	WS2812FramesSent++;
#endif

	// Send data to strip
	HAL_SPI_Transmit_DMA(&LED_SPI, sendData, 24 * _NUM_PHYSICAL_LEDS_PADDED);
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
		comets[i].speed = 1;
		comets[i].forward = true;
		comets[i].active = false;
	}
}

// Adds a comet to be processed by WS2812_MultiCometEffect
// color: Color of comet
// size: number of pixels used for body of comet
void WS2812_AddComet(colorRGB color, uint8_t size, uint8_t speed, bool forward)
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

	comets[index].position = forward ? 0 : NUM_LOGICAL_LEDS;
	comets[index].color = color;
	comets[index].size = size;
	comets[index].speed = speed;
	comets[index].ticksElapsed = 0;
	comets[index].forward = forward;
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
			// Deactivate comets when head reaches one end of the LED strip
			// -1 accounts for the fact that comets are drawn from [position, position + size], NOT [position + 1, position + 1 + size]
//			if(comets[i].position >= (NUM_LEDS - (comets[i].size - 1)))
			// Deactivate comet when it reaches the end of the strip
			// This is safe because WS2812_SetLED only sets the LED if it is in bounds
			if(((comets[i].forward) && (comets[i].position >= NUM_LOGICAL_LEDS)) || ((!comets[i].forward) && (comets[i].position <= 0)))
			{
				comets[i].active = false;
			}
			else
			{
				// Draw comet
				for(int j = 0; j < comets[i].size; j++)
				{
					WS2812_SetLED(j + comets[i].position, comets[i].color.red, comets[i].color.green, comets[i].color.blue, true);
				}

				// Advanced comet position after specified number of frames have elapsed
				if(comets[i].ticksElapsed % comets[i].speed == 0)
				{
					comets[i].position += (comets[i].forward) ? 1 : -1;
				}

				// Increment number of frames this comet has lived and wrap around by speed
				comets[i].ticksElapsed = (comets[i].ticksElapsed >= comets[i].speed) ? 0 : comets[i].ticksElapsed + 1;
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
// percentageMode: If true, interpret level argument as fraction of strip to fill. Else, interpret as fractional number of LEDs to fill.
// discrete: If true, meter length is truncated to nearest integer index
void WS2812_SimpleMeterEffect(colorRGB color, float level, bool flip, bool percentageMode, bool discrete)
{
	if(percentageMode)
	{
		// Clip to 100%
		level = (level <= 1.0f) ? level : 1.0f;

		// Interpret level as percentage of full strip
		level *= NUM_LOGICAL_LEDS;
	}
	else
	{
		// Clip level
		level = (level <= NUM_LOGICAL_LEDS) ? level : NUM_LOGICAL_LEDS;
	}

	// Only render discrete LEDS
	if(discrete)
	{
		// Fill low index to high index
		if(!flip)
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
	// Render fractional LEDs
	else
	{
		if(!flip)
		{
			WS2812_DrawLine(0.0f, level, color.red, color.green, color.blue, true);
		}
		else
		{
			WS2812_DrawLine(NUM_LOGICAL_LEDS - level, level, color.red, color.green, color.blue, true);
		}
	}
}

// Fills LEDs from both ends or from the center to display the value level on the LED strip
// color: Color of filled LEDs
// level: Number of LEDs to fill
// centered: If true, the meters are drawn from the middle LED in the strip towards the ends.
	// Otherwise, the meters are drawn from each end towards the center of the strip
// percentageMode: If true, interpret level argument as fraction of strip to fill. Else, interpret as fractional number of LEDs to fill.
// discrete: If true, meter length is truncated to nearest integer index
void WS2812_MirroredMeterEffect(colorRGB color, float level, bool centered, bool percentageMode, bool discrete)
{
	if(percentageMode)
	{
		// Clip to 100%
		level = (level <= 1.0f) ? level : 1.0f;

		// Interpret level as percentage of half the strip
		level *= (NUM_LOGICAL_LEDS >> 1);
	}
	else
	{
		// Half input level to account for the fact that two LEDs are filled for every increase in level
		level /= 2.0f;

		// Clip level
		level = (level <= NUM_LOGICAL_LEDS >> 1) ? level : NUM_LOGICAL_LEDS >> 1;
	}

	if(discrete)
	{
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
	else
	{
		if(centered)
		{
			WS2812_DrawLine((NUM_LOGICAL_LEDS >> 1) - level, level * 2.0f, color.red, color.green, color.blue, true);
		}
		else
		{
			// The meters advanced towards the center from the far ends of the strip
			// This can be implemented as two SimpleMeterEffects with opposing flip arguments to make them start from opposite sides of the strip
			// The "level" argument was already processed at the start of this function, so we pass it as-is to SimpleMeterEffect
			// and set the arguments to always interpret the level as a value, regardless of percentageMode in this function
			WS2812_SimpleMeterEffect(color, level, false, false, false);
			WS2812_SimpleMeterEffect(color, level, true, false, false);
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

	// Wrap extreme hue values around
	// Change to modulus if argument is not clipped (e.g., by NumericEffectParameter's ModulationMapper)
	if(hue >= 360)
	{
		hue -= 360;
	}

	// Clip max saturation and value
	saturation = MIN(MAX(saturation, 0.0f), 1.0f);
	value = MIN(MAX(value, 0.0f), 1.0f);

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
