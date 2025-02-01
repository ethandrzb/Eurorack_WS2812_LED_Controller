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

extern uint8_t LEDIndex;
extern uint8_t HSVPickerIndex;
extern char OLED_buffer[30];
extern uint8_t menu_layer;

colorRGB rgb = {.red = 0, .green = 0, .blue = 0};
colorHSV hsv = {.hue = 200, .saturation = 1.0, .value = 1.0};
SimpleBreathingEffect sbe = SimpleBreathingEffect(10, 0.005, hsv, 0.25);

void mainWhileCpp(void)
{

	while(1)
	{
		sbe.updateEffect();
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

	ssd1306_SetCursor(0, 0);
	ssd1306_WriteString("LED control", Font_11x18, White);

	// Show LED states on screen
	for(uint8_t i = 0; i < 4; i++)
	{
	  // Display menu item
	  uint8_t y = i * 12 + 18;
	  ssd1306_SetCursor(1, y);
//	  sprintf(OLED_buffer, "%d: LED %d state", i, i);
	  sprintf(OLED_buffer, "%s", sbe.getParameter(i)->name.c_str());

	  ssd1306_WriteString(OLED_buffer, Font_7x10, White);
	  ssd1306_DrawRectangle(0, y - 1, 89, y + 9, ((i == LEDIndex) && (menu_layer == ROOT)) ? White : Black);

	  // Display item value
//	  ssd1306_SetCursor(100, y);
	  ssd1306_SetCursor(90, y);
	  sprintf(OLED_buffer, "%-3s", sbe.getParameter(i)->getValueString());

	  ssd1306_WriteString(OLED_buffer, Font_7x10, ((i == LEDIndex) && (menu_layer == LEVEL_1)) ? Black : White);
	}

	// Draw color picker
	if(menu_layer == HSV_PICKER_ROOT || menu_layer == HSV_PICKER_VALUE_SELECTED)
	{
		drawHSVPicker();
	}

	ssd1306_UpdateScreen();
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
	sprintf(OLED_buffer, "hhhh");
	ssd1306_SetCursor(30, 18);
	ssd1306_WriteString(OLED_buffer, Font_7x10, ((HSVPickerIndex == 0) && (menu_layer == HSV_PICKER_VALUE_SELECTED)) ? Black : White);

	sprintf(OLED_buffer, "ssss");
	ssd1306_SetCursor(30, 28);
	ssd1306_WriteString(OLED_buffer, Font_7x10, ((HSVPickerIndex == 1) && (menu_layer == HSV_PICKER_VALUE_SELECTED)) ? Black : White);

	sprintf(OLED_buffer, "vvvv");
	ssd1306_SetCursor(30, 38);
	ssd1306_WriteString(OLED_buffer, Font_7x10, ((HSVPickerIndex == 2) && (menu_layer == HSV_PICKER_VALUE_SELECTED)) ? Black : White);

	// Draw color wheel
	// Radius line angle: Hue
	// Radius: Saturation
	ssd1306_DrawCircle(96, 32, 14, White);
	ssd1306_RadiusLine(96, 32, 14, 235, White);

	// Draw value meter
	const uint8_t VALUE_METER_CENTER_X = 70;
	const uint8_t VALUE_METER_CENTER_Y = 32;
	const uint8_t VALUE_METER_BORDER_HALF_WIDTH_X = 6;
	const uint8_t VALUE_METER_BORDER_HALF_WIDTH_Y = 14;
	const uint8_t VALUE_METER_FILL_HALF_WIDTH_X = 4;
	const uint8_t VALUE_METER_FILL_MAX_LENGTH = 24;
	const uint8_t VALUE_METER_FILL_TOP_Y = VALUE_METER_CENTER_Y - (VALUE_METER_FILL_MAX_LENGTH / 2);
	const uint8_t VALUE_METER_FILL_BOTTOM_Y = VALUE_METER_CENTER_Y + (VALUE_METER_FILL_MAX_LENGTH / 2);
	uint8_t fillAmount = 0;
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
	UNUSED(effectIndex);
	UNUSED(parameterSubIndex);

	//TODO: Modify this condition to compare the parameter at parameterIndex to ColorHSVEffectParameter instead of checking if the value inside the parameter is a colorHSV
//	if constexpr(std::is_same_v<typeid(*(static_cast<colorHSV *>(sbe.getParameter(parameterIndex)->getValue()))), colorHSV>)
//	{
//		ColorHSVEffectParameter *tmp = static_cast<ColorHSVEffectParameter *>(sbe.getParameter(parameterIndex));
//		tmp->incrementValueByIndex(parameterSubIndex);
//	}
//	else
//	{
//
//	}
	sbe.getParameter(parameterIndex)->incrementValue();
}

void decrementValueCpp(uint8_t effectIndex, uint8_t parameterIndex, uint8_t parameterSubIndex)
{
	UNUSED(effectIndex);
	UNUSED(parameterSubIndex);

	//TODO: Modify this condition to compare the parameter at parameterIndex to ColorHSVEffectParameter instead of checking if the value inside the parameter is a colorHSV
//	if constexpr(std::is_same_v<typeid(static_cast<colorHSV *>(sbe.getParameter(parameterIndex)->getValue())), colorHSV *>)
//	{
//		ColorHSVEffectParameter *tmp = static_cast<ColorHSVEffectParameter *>(sbe.getParameter(parameterIndex));
//		tmp->decrementValueByIndex(parameterSubIndex);
//	}
//	else
//	{
//
//	}
	sbe.getParameter(parameterIndex)->decrementValue();
}
