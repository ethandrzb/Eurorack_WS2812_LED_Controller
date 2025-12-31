/*
 * WS2182.h
 *
 *  Created on: Mar 17, 2024
 *      Author: ethan
 */

#ifndef INC_WS2812_H_
#define INC_WS2812_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


// NUM_PHYSICAL_LEDS should be evenly divisible by DOWNSAMPLING_FACTOR
// Round NUM_PHYSICAL_LEDS up to the nearest multiple, even if your strip has fewer than that amount
#define MAX_NUM_PHYSICAL_LEDS 1024
extern uint16_t NUM_PHYSICAL_LEDS;

//TODO: Change to power of 2 and bit shift in sendAll?
extern uint16_t DOWNSAMPLING_FACTOR;

extern uint16_t FRACTAL_FACTOR;

//#ifdef DOWNSAMPLING_FACTOR
#define NUM_LOGICAL_LEDS (NUM_PHYSICAL_LEDS / DOWNSAMPLING_FACTOR)
//#else
//#define NUM_LOGICAL_LEDS NUM_PHYSICAL_LEDS
//#endif

#define NUM_LED_PARAMS 3
#define NUM_MAX_COMETS 30

#define USE_NEW_SEND_FUNCTIONS

#define USE_LUT_OPTIMIZATION

#ifdef USE_LUT_OPTIMIZATION
#include "ByteToWS2812DataLookupTable.h"
#endif

// NOTE: WS2811 have a lower maximum frame rate than WS2812.
// Effects that work on the WS2812 might have to be rate limited or slowed down to work on WS2811.
//#define WS2811_MODE

// Calculate FPS and show to user (display on OLED or send over USART2 depending on callback body)
// WARNING: Enabling this feature is known to cause flickering each time the value is calculated
//#define ENABLE_PERFORMANCE_MONITOR

#ifndef MAX
#define MAX(x,y) ((x) > (y)) ? (x) : (y)
#endif

#ifndef MIN
#define MIN(x,y) ((x) < (y)) ? (x) : (y)
#endif

typedef struct colorRGB
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} colorRGB;

typedef struct colorHSV
{
	uint16_t hue;
	float saturation;
	float value;
} colorHSV;

typedef struct comet
{
	uint16_t position;
	colorRGB color;
	uint8_t size;
	uint8_t speed;
	uint8_t ticksElapsed;
	bool active;
	bool forward;
} comet;

// Base LED functions
void WS2812_SetLED(uint16_t index, uint8_t red, uint8_t green, uint8_t blue, bool additive);
void WS2812_SetLEDAdditive(uint16_t index, uint8_t red, uint8_t green, uint8_t blue);
void WS2812_DrawLine(float position, float length, uint8_t red, uint8_t green, uint8_t blue, bool additive);
void WS2812_SetAllLEDs(uint32_t red, uint32_t green, uint32_t blue);
void WS2812_ClearLEDs(void);
void WS2812_FadeAll(uint8_t denominator);
void WS2812_ShiftLEDs(int16_t shiftAmount);

// Communication with LED strip
#ifndef USE_NEW_SEND_FUNCTIONS
void WS2812_SendSingleLED(uint32_t red, uint32_t green, uint32_t blue);
#else
uint8_t *WS2812_GetSingleLEDData(uint32_t red, uint32_t green, uint32_t blue);
#endif
void WS2812_SendAll(void);

// Effect functions
void WS2812_InitMultiCometEffect();
void WS2812_AddComet(colorRGB color, uint8_t size, uint8_t speed, bool forward);
void WS2812_MultiCometEffect(void);
void WS2812_CometEffect(void);
void WS2812_SimpleMeterEffect(colorRGB color, float level, bool flip, bool percentageMode, bool discrete);
void WS2812_MirroredMeterEffect(colorRGB color, float level, bool centered, bool percentageMode, bool discrete);
void WS2812_FillRainbow(colorHSV startingColor, int16_t deltaHue);

// Utility functions
void WS2812_SetBackgroundColor(uint8_t red, uint8_t green, uint8_t blue);
void WS2812_SetBackgroundColorHSV(colorHSV *hsv);
colorRGB WS2812_HSVToRGB(uint16_t hue, float saturation, float value);

#ifdef __cplusplus
}
#endif

#endif /* INC_WS2812_H_ */
