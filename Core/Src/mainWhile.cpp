#include "mainWhile.hpp"
#include "WS2812FX/SimpleBreathingEffect.hpp"
#include "../../Drivers/ssd1306/ssd1306.h"

extern "C"
{
	void mainWhileC(void)
	{
		mainWhileCpp();
	}

	void updateMenuC(void)
	{
		updateMenuCpp();
	}
}

extern uint8_t LEDIndex;
extern char OLED_buffer[30];
extern uint8_t menu_layer;
extern GPIO_PinState LEDStates[4];

colorRGB rgb = {.red = 0, .green = 0, .blue = 0};
colorHSV hsv = {.hue = 200, .saturation = 1.0, .value = 1.0};
SimpleBreathingEffect sbe = SimpleBreathingEffect(10, 0.005, hsv, 0.25);

void mainWhileCpp(void)
{

	while(1)
	{
		switch(LEDIndex)
		{
			case 0:
				sbe.updateEffect();
//				WS2812FX_SimpleBreathingEffect(10, 0.005, hsv, 0.25);
				break;
			case 1:
				rgb.red = 32;
				rgb.green = 0;
				rgb.blue = 32;
				WS2812_ClearLEDs();
				WS2812_SimpleMeterEffect(rgb, 45, true);
				WS2812_SendAll();
				break;
			case 2:
				rgb.red = 0;
				rgb.green = 32;
				rgb.blue = 16;
				WS2812_ClearLEDs();
				WS2812_MirroredMeterEffect(rgb, 17, false);
				WS2812_SendAll();
				break;
			case 3:
				static colorHSV hsvRainbow = {.hue = 0, .saturation = 1.0f, .value = 0.2f};
				hsvRainbow.hue = (hsvRainbow.hue < 360) ? hsvRainbow.hue + 1 : 0;
				WS2812_FillRainbow(hsvRainbow, 3);
				WS2812_SendAll();
				break;
		}
	}
}

void updateMenuCpp()
{
	// Show LED states on screen
	for(uint8_t i = 0; i < 4; i++)
	{
	  // Display menu item
	  uint8_t y = i * 12 + 18;
	  ssd1306_SetCursor(1, y);
//	  sprintf(OLED_buffer, "%d: LED %d state", i, i);
	  sprintf(OLED_buffer, "%d: %s", i, sbe.getParameter<EffectParameterBase>(i)->name.c_str());

	  ssd1306_WriteString(OLED_buffer, Font_7x10, White);
	  ssd1306_DrawRectangle(0, y - 1, 99, y + 9, ((i == LEDIndex) && (menu_layer == ROOT)) ? White : Black);

	  // Display item value
	  ssd1306_SetCursor(100, y);
//	  sprintf(OLED_buffer, "%-3s", (LEDStates[i] == GPIO_PIN_SET) ? "ON" : "OFF");
	  sprintf(OLED_buffer, "%-3s", sbe.getParameter<EffectParameterBase>(i)->getValueString());
	  ssd1306_WriteString(OLED_buffer, Font_7x10, ((i == LEDIndex) && (menu_layer == LEVEL_1)) ? Black : White);
	}

	ssd1306_UpdateScreen();
}
