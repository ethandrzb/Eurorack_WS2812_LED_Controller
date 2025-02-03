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

	void incrementValueC(uint8_t effectIndex, uint8_t parameterIndex, uint8_t parameterSubIndex)
	{
		incrementValueCpp(effectIndex, parameterIndex, parameterSubIndex);
	}

	void decrementValueC(uint8_t effectIndex, uint8_t parameterIndex, uint8_t parameterSubIndex)
	{
		decrementValueCpp(effectIndex, parameterIndex, parameterSubIndex);
	}
}

extern uint8_t effectIndex;
extern uint8_t menuItemIndex;
extern uint8_t HSVPickerIndex;
extern char OLED_buffer[30];
extern uint8_t menu_layer;

colorRGB rgb = {.red = 0, .green = 0, .blue = 0};
colorHSV hsv = {.hue = 200, .saturation = 1.0, .value = 1.0};
SimpleBreathingEffect sbe = SimpleBreathingEffect(10, 0.005, hsv, 0.25);
WS2812Effect *fx[WS2812FX_NUM_EFFECTS];

void mainWhileCpp(void)
{
	// Collect effects
//	fx[0] = &sbe;

	// Fill all FX slots with same FX instance until more FX are implemented
	for(int i = 0; i < WS2812FX_NUM_EFFECTS; i++)
	{
		fx[i] = &sbe;
	}

	updateMenuCpp();

	while(1)
	{
		fx[effectIndex]->updateEffect();
//		switch(LEDIndex)
//		{
//			case 0:
//				sbe.updateEffect();
////				WS2812FX_SimpleBreathingEffect(10, 0.005, hsv, 0.25);
//				break;
//			case 1:
//				rgb.red = 32;
//				rgb.green = 0;
//				rgb.blue = 32;
//				WS2812_ClearLEDs();
//				WS2812_SimpleMeterEffect(rgb, 45, true);
//				WS2812_SendAll();
//				break;
//			case 2:
//				rgb.red = 0;
//				rgb.green = 32;
//				rgb.blue = 16;
//				WS2812_ClearLEDs();
//				WS2812_MirroredMeterEffect(rgb, 17, false);
//				WS2812_SendAll();
//				break;
//			case 3:
//				static colorHSV hsvRainbow = {.hue = 0, .saturation = 1.0f, .value = 0.2f};
//				hsvRainbow.hue = (hsvRainbow.hue < 360) ? hsvRainbow.hue + 1 : 0;
//				WS2812_FillRainbow(hsvRainbow, 3);
//				WS2812_SendAll();
//				break;
//		}
	}
}

void updateMenuCpp()
{
	ssd1306_Fill(Black);

	// Display title bar
	ssd1306_SetCursor(0, 0);
//	sprintf(OLED_buffer, "%d %s", effectIndex, fx[effectIndex]->name);
	sprintf(OLED_buffer, "%d", effectIndex);
	ssd1306_WriteString(OLED_buffer, Font_11x18, White);

	// Show LED states on screen
	drawMenuLevel1();

	// Draw color picker
	if(menu_layer == HSV_PICKER_ROOT || menu_layer == HSV_PICKER_VALUE_SELECTED)
	{
		drawHSVPicker();
	}

	ssd1306_UpdateScreen();
}

void drawMenuLevel1(void)
{
	for(uint8_t i = 0; i < 4; i++)
	{
	  // Display menu item
	  uint8_t y = i * 12 + 18;
	  ssd1306_SetCursor(1, y);
	  sprintf(OLED_buffer, "%s", fx[effectIndex]->getParameter(i)->name.c_str());

	  ssd1306_WriteString(OLED_buffer, Font_7x10, White);
	  ssd1306_DrawRectangle(0, y - 1, 89, y + 9, ((i == menuItemIndex) && (menu_layer == ROOT)) ? White : Black);

	  // Display item value
	  ssd1306_SetCursor(90, y);
	  sprintf(OLED_buffer, "%-3s", fx[effectIndex]->getParameter(i)->getValueString());

	  ssd1306_WriteString(OLED_buffer, Font_7x10, ((i == menuItemIndex) && (menu_layer == LEVEL_1)) ? Black : White);
	}
}

void drawHSVPicker(void)
{
	ssd1306_FillRectangle(16, 16, 112, 48, Black);
	ssd1306_DrawRectangle(16, 16, 112, 48, White);

	// Display labels
	sprintf(OLED_buffer, "H");
	ssd1306_SetCursor(17, 18);
	ssd1306_WriteString(OLED_buffer, Font_7x10, ((HSVPickerIndex == 0) && (menu_layer == HSV_PICKER_ROOT)) ? Black : White);

	sprintf(OLED_buffer, "S");
	ssd1306_SetCursor(17, 28);
	ssd1306_WriteString(OLED_buffer, Font_7x10, ((HSVPickerIndex == 1) && (menu_layer == HSV_PICKER_ROOT)) ? Black : White);

	sprintf(OLED_buffer, "V");
	ssd1306_SetCursor(17, 38);
	ssd1306_WriteString(OLED_buffer, Font_7x10, ((HSVPickerIndex == 2) && (menu_layer == HSV_PICKER_ROOT)) ? Black : White);

	// Display HSV values
	//TODO: Find better way to get position of ColorHSVEffectParameters in a given effect
	// Maybe a separate list of colors?
	colorHSV *hsv = static_cast<colorHSV *>(fx[effectIndex]->getParameter(2)->getValue());
	sprintf(OLED_buffer, "%3d", hsv->hue);
	ssd1306_SetCursor(30, 18);
	ssd1306_WriteString(OLED_buffer, Font_7x10, ((HSVPickerIndex == 0) && (menu_layer == HSV_PICKER_VALUE_SELECTED)) ? Black : White);

	sprintf(OLED_buffer, "%.2f", hsv->saturation);
	ssd1306_SetCursor(30, 28);
	ssd1306_WriteString(OLED_buffer, Font_7x10, ((HSVPickerIndex == 1) && (menu_layer == HSV_PICKER_VALUE_SELECTED)) ? Black : White);

	sprintf(OLED_buffer, "%.2f", hsv->value);
	ssd1306_SetCursor(30, 38);
	ssd1306_WriteString(OLED_buffer, Font_7x10, ((HSVPickerIndex == 2) && (menu_layer == HSV_PICKER_VALUE_SELECTED)) ? Black : White);

	// Draw color wheel
	const uint8_t HSV_PICKER_WHEEL_MAX_RADIUS = 14;
	// Radius line angle: Hue
	// Radius: Saturation
	ssd1306_DrawCircle(96, 32, 14, White);
	ssd1306_RadiusLine(96, 32, (uint8_t)((float) HSV_PICKER_WHEEL_MAX_RADIUS * hsv->saturation), hsv->hue, White);

	// Draw value meter
	const uint8_t VALUE_METER_CENTER_X = 70;
	const uint8_t VALUE_METER_CENTER_Y = 32;
	const uint8_t VALUE_METER_BORDER_HALF_WIDTH_X = 6;
	const uint8_t VALUE_METER_BORDER_HALF_WIDTH_Y = 14;
	const uint8_t VALUE_METER_FILL_HALF_WIDTH_X = 4;
	const uint8_t VALUE_METER_FILL_MAX_LENGTH = 24;
	const uint8_t VALUE_METER_FILL_TOP_Y = VALUE_METER_CENTER_Y - (VALUE_METER_FILL_MAX_LENGTH / 2);
	const uint8_t VALUE_METER_FILL_BOTTOM_Y = VALUE_METER_CENTER_Y + (VALUE_METER_FILL_MAX_LENGTH / 2);
	uint8_t fillAmount = (uint8_t)((float) VALUE_METER_FILL_MAX_LENGTH * hsv->value);
	ssd1306_DrawRectangle(VALUE_METER_CENTER_X - VALUE_METER_BORDER_HALF_WIDTH_X,
							VALUE_METER_CENTER_Y - VALUE_METER_BORDER_HALF_WIDTH_Y,
							VALUE_METER_CENTER_X + VALUE_METER_BORDER_HALF_WIDTH_X,
							VALUE_METER_CENTER_Y + VALUE_METER_BORDER_HALF_WIDTH_Y,
							White);
	ssd1306_FillRectangle(VALUE_METER_CENTER_X - VALUE_METER_FILL_HALF_WIDTH_X,
							VALUE_METER_FILL_TOP_Y + (VALUE_METER_FILL_MAX_LENGTH - fillAmount),
							VALUE_METER_CENTER_X + VALUE_METER_FILL_HALF_WIDTH_X,
							VALUE_METER_FILL_BOTTOM_Y,
							White);
}

void incrementValueCpp(uint8_t effectIndex, uint8_t parameterIndex, uint8_t parameterSubIndex)
{
	if(dynamic_cast<ColorHSVEffectParameter *>(fx[effectIndex]->getParameter(parameterIndex)))
	{
		ColorHSVEffectParameter *tmp = static_cast<ColorHSVEffectParameter *>(fx[effectIndex]->getParameter(parameterIndex));
		tmp->incrementValueByIndex(parameterSubIndex);
	}
	else
	{
		fx[effectIndex]->getParameter(parameterIndex)->incrementValue();
	}
}

void decrementValueCpp(uint8_t effectIndex, uint8_t parameterIndex, uint8_t parameterSubIndex)
{
	if(dynamic_cast<ColorHSVEffectParameter *>(fx[effectIndex]->getParameter(parameterIndex)))
	{
		ColorHSVEffectParameter *tmp = static_cast<ColorHSVEffectParameter *>(fx[effectIndex]->getParameter(parameterIndex));
		tmp->decrementValueByIndex(parameterSubIndex);
	}
	else
	{
		fx[effectIndex]->getParameter(parameterIndex)->decrementValue();
	}
}
