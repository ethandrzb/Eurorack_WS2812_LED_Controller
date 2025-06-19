#include "mainWhile.hpp"
#include "WS2812FX/SimpleBreathingEffect.hpp"
#include "WS2812FX/MeterEffect.hpp"
#include "WS2812FX/RainbowEffect.hpp"
#include "../../Drivers/ssd1306/ssd1306.h"
#include "main.h"

#include <vector>

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

	void incrementMenuItemIndexC(void)
	{
		incrementMenuItemIndexCpp();
	}

	void decrementMenuItemIndexC(void)
	{
		decrementMenuItemIndexCpp();
	}

	void populateMenuItemsC(void)
	{
		populateMenuItemsCpp();
	}
}

extern uint8_t effectIndex;
extern uint8_t menuItemIndex;
extern uint8_t HSVPickerIndex;
extern uint8_t selectedModSourceIndex;
extern char OLED_buffer[30];
extern uint8_t menu_layer;

extern ADC_HandleTypeDef hadc1;
extern uint16_t rawADCData[NUM_CV_INPUTS];

std::vector<EffectParameterBase *> numericParams;
std::vector<ColorHSVEffectParameter *> colors;
std::vector<std::string> modMatrixSources = {"A", "B", "C", "D", "E", "F", "G", "H", "I"};
std::vector<std::string> modMatrixDestinations = {"aaaaa", "bbbbb", "ccccc", "ddddd", "eeeee"};
std::vector<std::string> modMatrixAmounts = {"000", "111", "222", "333", "444"};

colorRGB rgb = {.red = 0, .green = 0, .blue = 0};
colorHSV hsv = {.hue = 200, .saturation = 1.0, .value = 0.25};
SimpleBreathingEffect simpleBreathingEffect = SimpleBreathingEffect(10, 0.005, hsv, 0.25);
MeterEffect meterEffect = MeterEffect(10, hsv, false, true);
RainbowEffect rainbowEffect = RainbowEffect(3, 1, 1, hsv, false);
WS2812Effect *fx[WS2812FX_NUM_EFFECTS];

void mainWhileCpp(void)
{
	// Collect effects
	fx[0] = &simpleBreathingEffect;
	fx[1] = &meterEffect;
	fx[2] = &rainbowEffect;

	populateMenuItemsCpp();

	updateMenuCpp();

	while(1)
	{
		fx[effectIndex]->updateEffect();
		HAL_ADC_Start_DMA(&hadc1, (uint32_t *) rawADCData, NUM_CV_INPUTS);

		switch(effectIndex)
		{
			case 0:
				(static_cast<NumericEffectParameter<uint8_t> *>(fx[effectIndex]->getParameter(0)))->setModulation(rawADCData[0] >> 2);
				(static_cast<NumericEffectParameter<float> *>(fx[effectIndex]->getParameter(1)))->setModulation(rawADCData[1] * 0.002);
				(static_cast<NumericEffectParameter<float> *>(fx[effectIndex]->getParameter(3)))->setModulation(rawADCData[2] / 256.0f);
				break;
			case 1:
				(static_cast<NumericEffectParameter<uint8_t> *>(fx[effectIndex]->getParameter(0)))->setModulation(rawADCData[0] >> 2);
				(static_cast<NumericEffectParameter<uint8_t> *>(fx[effectIndex]->getParameter(3)))->setModulation(rawADCData[1] >> 2);
				break;
			case 2:
				(static_cast<NumericEffectParameter<int8_t> *>(fx[effectIndex]->getParameter(0)))->setModulation(rawADCData[0] >> 2);
				(static_cast<NumericEffectParameter<uint8_t> *>(fx[effectIndex]->getParameter(1)))->setModulation(rawADCData[1] >> 2);
				(static_cast<NumericEffectParameter<uint8_t> *>(fx[effectIndex]->getParameter(2)))->setModulation(rawADCData[2] >> 2);
				break;
		}
	}
}

void updateMenuCpp(void)
{
	ssd1306_Fill(Black);

	// Display title bar
	ssd1306_SetCursor(0, 0);
	sprintf(OLED_buffer, "%d %s", effectIndex, fx[effectIndex]->name);
	// Invert text color when FX_CHANGE_BTN held
	ssd1306_WriteString(OLED_buffer, Font_11x18, (HAL_GPIO_ReadPin(FX_CHANGE_BTN_GPIO_Port, FX_CHANGE_BTN_Pin) != GPIO_PIN_RESET) ? White : Black);

	// Show LED states on screen
	switch(menu_layer)
	{
		case NUMERIC_PARAMETER_ROOT:
		case NUMERIC_PARAMETER_VALUE_SELECTED:
			drawMenuNumericParameter();
			break;
		case COLOR_PALETTE_ROOT:
			drawMenuColorPalette();
			break;
		case MOD_MATRIX_ROOT:
		case MOD_MATRIX_DESTINATION_SELECTED:
		case MOD_MATRIX_AMOUNT_SELECTED:
			drawMenuModMatrix();
			break;
	}

	// Draw color picker
	if(menu_layer == HSV_PICKER_ROOT || menu_layer == HSV_PICKER_VALUE_SELECTED)
	{
		drawHSVPicker();
	}

	ssd1306_UpdateScreen();
}
//TODO: Merge this function with drawMenuColorPalette
void drawMenuNumericParameter(void)
{
	uint8_t start = (menuItemIndex > 3) ? menuItemIndex - 3 : 0;
	uint8_t end = ((uint8_t)(start + 4) > numericParams.size()) ? numericParams.size() : start + 4;

	for(uint8_t i = start; i < end; i++)
	{
	  // Display menu item
	  uint8_t y = (i - start) * 12 + 18;
	  ssd1306_SetCursor(1, y);
	  sprintf(OLED_buffer, "%s", numericParams[i]->name.c_str());

	  ssd1306_WriteString(OLED_buffer, Font_7x10, White);
	  ssd1306_DrawRectangle(0, y - 1, 89, y + 9, ((i == menuItemIndex) && (menu_layer == NUMERIC_PARAMETER_ROOT)) ? White : Black);

	  // Display item value
	  ssd1306_SetCursor(90, y);
	  sprintf(OLED_buffer, "%-3s", numericParams[i]->getValueString());

	  ssd1306_WriteString(OLED_buffer, Font_7x10, ((i == menuItemIndex) && (menu_layer == NUMERIC_PARAMETER_VALUE_SELECTED)) ? Black : White);
	}
}

void drawMenuColorPalette(void)
{
	uint8_t start = (menuItemIndex > 3) ? menuItemIndex - 3 : 0;
	uint8_t end = ((uint8_t)(start + 4) > colors.size()) ? colors.size() : start + 4;

	for(uint8_t i = start; i < end; i++)
	{
	  // Display menu item
	  uint8_t y = (i - start) * 12 + 18;
	  ssd1306_SetCursor(1, y);
	  sprintf(OLED_buffer, "%s", colors[i]->name.c_str());

	  ssd1306_WriteString(OLED_buffer, Font_7x10, White);
	  ssd1306_DrawRectangle(0, y - 1, 89, y + 9, ((i == menuItemIndex)) ? White : Black);
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
	colorHSV *hsv = static_cast<colorHSV *>(colors[menuItemIndex]->getValueRaw());
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
	// Meter position specified using center point and extending up, down, left, and right from it
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

void drawMenuModMatrix()
{
	uint8_t startSource = (selectedModSourceIndex > 3) ? selectedModSourceIndex - 3 : 0;
	uint8_t endSource = ((uint8_t)(startSource + 4) > modMatrixSources.size()) ? modMatrixSources.size() : startSource + 4;

	uint8_t start = (menuItemIndex > 3) ? menuItemIndex - 3 : 0;
	uint8_t end = ((uint8_t)(start + 4) > modMatrixDestinations.size()) ? modMatrixDestinations.size() : start + 4;

	// Display separator line between mod sources and destinations
	ssd1306_Line(11, 18, 11, 63, White);

	for(uint8_t i = startSource; i < endSource; i++)
	{
	  uint8_t y = (i - startSource) * 12 + 18;
	  // Display mod source
	  ssd1306_SetCursor(1, y);
	  sprintf(OLED_buffer, "%s", modMatrixSources[i].c_str());
	  ssd1306_FillRectangle(0, y - 1, 8, y + 9, (i == selectedModSourceIndex) ? White : Black);
	  ssd1306_WriteString(OLED_buffer, Font_7x10, (i == selectedModSourceIndex) ? Black : White);
	}

	for(uint8_t i = start; i < end; i++)
	{
	  uint8_t y = (i - start) * 12 + 18;
	  // Display mod destination
	  ssd1306_SetCursor(15, y);
	  sprintf(OLED_buffer, "%s", modMatrixDestinations[i].c_str());
	  ssd1306_WriteString(OLED_buffer, Font_7x10, White);
	  ssd1306_DrawRectangle(14, y - 1, 89, y + 9, ((i == menuItemIndex) && (menu_layer == MOD_MATRIX_DESTINATION_SELECTED)) ? White : Black);

	  // Display mod amount
	  ssd1306_SetCursor(90, y);
	  sprintf(OLED_buffer, "%-3s", modMatrixAmounts[i].c_str());
	  ssd1306_WriteString(OLED_buffer, Font_7x10, ((i == menuItemIndex) && (menu_layer == MOD_MATRIX_AMOUNT_SELECTED)) ? Black : White);
	}
}

void incrementValueCpp(uint8_t effectIndex, uint8_t parameterIndex, uint8_t parameterSubIndex)
{
	if(menu_layer == NUMERIC_PARAMETER_VALUE_SELECTED)
	{
		numericParams[parameterIndex]->incrementValue();
	}
	else if(menu_layer == HSV_PICKER_VALUE_SELECTED)
	{
		colors[parameterIndex]->incrementValueByIndex(parameterSubIndex);
	}
}

void decrementValueCpp(uint8_t effectIndex, uint8_t parameterIndex, uint8_t parameterSubIndex)
{
	if(menu_layer == NUMERIC_PARAMETER_VALUE_SELECTED)
	{
		numericParams[parameterIndex]->decrementValue();
	}
	else if(menu_layer == HSV_PICKER_VALUE_SELECTED)
	{
		colors[parameterIndex]->decrementValueByIndex(parameterSubIndex);
	}
}

// While menuItemIndex is the main variable used to track cursor position, some menus may use additional variables to store cursor positions for deeper levels
void incrementMenuItemIndexCpp(void)
{
	switch(menu_layer)
	{
		case NUMERIC_PARAMETER_ROOT:
			if(menuItemIndex < numericParams.size() - 1)
			{
				menuItemIndex++;
			}
			break;
		case COLOR_PALETTE_ROOT:
			if(menuItemIndex < colors.size() - 1)
			{
				menuItemIndex++;
			}
			break;
		case MOD_MATRIX_ROOT:
			if(selectedModSourceIndex < modMatrixSources.size() - 1)
			{
				selectedModSourceIndex++;
			}
			break;
		case MOD_MATRIX_DESTINATION_SELECTED:
			if(menuItemIndex < modMatrixDestinations.size() - 1)
			{
				menuItemIndex++;
			}
			break;
		case MOD_MATRIX_AMOUNT_SELECTED:
			if(menuItemIndex < modMatrixAmounts.size() - 1)
			{
				menuItemIndex++;
			}
			break;
	}
}

void decrementMenuItemIndexCpp(void)
{
	switch(menu_layer)
	{
		case MOD_MATRIX_ROOT:
			selectedModSourceIndex = (selectedModSourceIndex > 0) ? selectedModSourceIndex - 1 : 0;
			break;
		default:
			menuItemIndex = (menuItemIndex > 0) ? menuItemIndex - 1 : 0;
			break;
	}
}

// Groups the arbitrary parameters in each effect into numeric and color parameters based on type
// These are used to populate the items for their respective menus
//TODO: Make numericParams and colors vectors of vectors so we only have to do this once
void populateMenuItemsCpp(void)
{
	numericParams.clear();
	colors.clear();

	// Sort effect parameters into separate lists by type
	for(int i = 0; i < WS2812FX_EFFECT_MAX_PARAMS; i++)
	{
		// Ensure parameters are valid objects before adding them to either vector
		if(dynamic_cast<ColorHSVEffectParameter *>(fx[effectIndex]->getParameter(i)))
		{
			colors.push_back(static_cast<ColorHSVEffectParameter *>(fx[effectIndex]->getParameter(i)));
		}
		else if(dynamic_cast<EffectParameterBase *>(fx[effectIndex]->getParameter(i)))
		{
			numericParams.push_back(fx[effectIndex]->getParameter(i));
		}

		//TODO: Insert modulation references here?
	}
}
