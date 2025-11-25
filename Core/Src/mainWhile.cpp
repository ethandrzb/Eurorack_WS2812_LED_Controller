#include "mainWhile.hpp"
#include "WS2812FX/SimpleBreathingEffect.hpp"
#include "WS2812FX/MeterEffect.hpp"
#include "WS2812FX/RainbowEffect.hpp"
#include "WS2812FX/CometEffect.hpp"
#include "../../Drivers/ssd1306/ssd1306.h"
#include "main.h"

#include <vector>

extern uint8_t effectIndex;
extern uint8_t menuItemIndex;
extern uint8_t HSVPickerIndex;
extern uint8_t selectedModSourceIndex;
extern char OLED_buffer[30];
extern uint8_t menu_layer;

extern ADC_HandleTypeDef hadc1;
extern uint16_t rawADCData[NUM_CV_INPUTS];

std::vector<std::string> modMatrixSources = {"A", "B", "C"};

extern TIM_HandleTypeDef htim7;

colorRGB rgb = {.red = 0, .green = 0, .blue = 0};
colorHSV hsv = {.hue = 200, .saturation = 1.0, .value = 0.25};
SimpleBreathingEffect simpleBreathingEffect = SimpleBreathingEffect(20, 0.005, hsv, 0.25);
MeterEffect meterEffect = MeterEffect(10, hsv, false, true);
RainbowEffect rainbowEffect = RainbowEffect(3, 1, 4, hsv, false);
CometEffect cometEffect = CometEffect(10, 2, hsv, 50);
WS2812Effect *fx[WS2812FX_NUM_EFFECTS];
std::vector<std::shared_ptr<NumericEffectParameter<uint16_t>>> WS2812SettingParameters;
std::vector<uint16_t *> WS2812SettingValues;

bool updateMenu = false;

void mainWhileCpp(void)
{
	// Collect effects
	fx[0] = &simpleBreathingEffect;
	fx[1] = &meterEffect;
	fx[2] = &rainbowEffect;
	fx[3] = &cometEffect;

	// Collect settings
	WS2812SettingParameters.push_back(std::make_shared<NumericEffectParameter<uint16_t>>(97, "Strip Length", 1, 1024, 1));
	WS2812SettingParameters.push_back(std::make_shared<NumericEffectParameter<uint16_t>>(1, "Downsampling", 1, 25, 1));
	WS2812SettingParameters.push_back(std::make_shared<NumericEffectParameter<uint16_t>>(1, "Fractal", 1, 50, 1));

	WS2812SettingValues.push_back(&NUM_PHYSICAL_LEDS);
	WS2812SettingValues.push_back(&DOWNSAMPLING_FACTOR);
	WS2812SettingValues.push_back(&FRACTAL_GROUP_SIZE);

	// Assigns modulations defined in effect defaults
	refreshModMatrix();

	updateMenuCpp();

	// Start frame timer
	HAL_TIM_Base_Start_IT(&htim7);

	while(1)
	{
		if(updateMenu)
		{
			updateMenuCpp();
			updateMenu = false;
		}
//		fx[effectIndex]->updateEffect();

//		// Update background
//		WS2812_SetBackgroundColorHSV(static_cast<colorHSV *>(fx[effectIndex]->backgroundColorParameter->getValue()));
//
//		HAL_ADC_Start_DMA(&hadc1, (uint32_t *) rawADCData, NUM_CV_INPUTS);

		//NOTE: Noise on ADC inputs can cause the effect to appear to stutter
		// Set modulation source of all parameters to null if you're not sure why an effect is stuttering
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

	// Draw appropriate menu for current state
	switch(menu_layer)
	{
		case SIMPLE_PARAMETER_ROOT:
		case SIMPLE_PARAMETER_VALUE_SELECTED:
			drawMenuSimpleParameter();
			break;
		case COLOR_PALETTE_ROOT:
			drawMenuColorPalette();
			break;
		case MOD_MATRIX_ROOT:
		case MOD_MATRIX_DESTINATION_SELECTED:
		case MOD_MATRIX_AMOUNT_SELECTED:
			drawMenuModMatrix();
			break;
		case SETTINGS_ROOT:
		case SETTINGS_VALUE_SELECTED:
			ssd1306_Fill(Black);
			// Display title bar
			ssd1306_SetCursor(0, 0);
			sprintf(OLED_buffer, "Settings");
			// Invert text color when FX_CHANGE_BTN held
			ssd1306_WriteString(OLED_buffer, Font_11x18, White);
			drawMenuSettings();
			break;
	}

	// Draw color picker pop-up
	if(menu_layer == HSV_PICKER_ROOT || menu_layer == HSV_PICKER_VALUE_SELECTED)
	{
		drawHSVPicker();
	}

	ssd1306_UpdateScreen();
}
//TODO: Merge this function with drawMenuColorPalette. A more generic function with parameters to describe the menu should work.
void drawMenuSimpleParameter(void)
{
	uint8_t start = (menuItemIndex > 3) ? menuItemIndex - 3 : 0;
	uint8_t end = ((uint8_t)(start + 4) > fx[effectIndex]->getSimpleParameters().size()) ? fx[effectIndex]->getSimpleParameters().size() : start + 4;

	for(uint8_t i = start; i < end; i++)
	{
	  // Display menu item
	  uint8_t y = (i - start) * 12 + 18;
	  ssd1306_SetCursor(1, y);
	  sprintf(OLED_buffer, "%s", fx[effectIndex]->getSimpleParameters()[i]->name.c_str());

	  ssd1306_WriteString(OLED_buffer, Font_7x10, White);
	  ssd1306_DrawRectangle(0, y - 1, 89, y + 9, ((i == menuItemIndex) && (menu_layer == SIMPLE_PARAMETER_ROOT)) ? White : Black);

	  // Display item value
	  ssd1306_SetCursor(90, y);
	  sprintf(OLED_buffer, "%-3s", fx[effectIndex]->getSimpleParameters()[i]->getValueString());

	  ssd1306_WriteString(OLED_buffer, Font_7x10, ((i == menuItemIndex) && (menu_layer == SIMPLE_PARAMETER_VALUE_SELECTED)) ? Black : White);
	}
}

void drawMenuColorPalette(void)
{
	uint8_t start = (menuItemIndex > 3) ? menuItemIndex - 3 : 0;
	uint8_t end = ((uint8_t)(start + 4) > fx[effectIndex]->getColorParameters().size()) ? fx[effectIndex]->getColorParameters().size() : start + 4;

	for(uint8_t i = start; i < end; i++)
	{
	  // Display menu item
	  uint8_t y = (i - start) * 12 + 18;
	  ssd1306_SetCursor(1, y);
	  sprintf(OLED_buffer, "%s", fx[effectIndex]->getColorParameters()[i]->name.c_str());

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
	colorHSV *hsv = static_cast<colorHSV *>(fx[effectIndex]->getColorParameters()[menuItemIndex]->getValueRaw());
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

// Display list of available destinations and highlight chosen one for selected mod source
void drawMenuModMatrix(void)
{
	uint8_t startSource = (selectedModSourceIndex > 3) ? selectedModSourceIndex - 3 : 0;
	uint8_t endSource = ((uint8_t)(startSource + 4) > modMatrixSources.size()) ? modMatrixSources.size() : startSource + 4;

	uint8_t start = (menuItemIndex > 3) ? menuItemIndex - 3 : 0;
	uint8_t end = ((uint8_t)(start + 4) > fx[effectIndex]->getExpandedParameters().size()) ? fx[effectIndex]->getExpandedParameters().size() : start + 4;

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
	  // Current destination for this source is prefixed with '*'
	  ssd1306_SetCursor(15, y);
	  sprintf(OLED_buffer, "%s%s", (fx[effectIndex]->modMatrix[selectedModSourceIndex].modDestination == fx[effectIndex]->getExpandedParameters()[i].get()) ? "*": "",
			  fx[effectIndex]->getExpandedParameters()[i]->name.c_str());
	  ssd1306_WriteString(OLED_buffer, Font_7x10, White);
	  ssd1306_DrawRectangle(14, y - 1, 98, y + 9, ((i == menuItemIndex) && (menu_layer == MOD_MATRIX_DESTINATION_SELECTED)) ? White : Black);

	  //TODO: Move mod amount back into this loop to visualize amounts for currently selected source to all possible destinations
	  //TODO: Visualize amount to each destination mentioned in last TODO using box that gradually fills up? It might be possible to reuse the code for the "value" bar from the HSV picker popup.
	}

	// Display mod amount
	ssd1306_SetCursor(99, 36);
	sprintf(OLED_buffer, "%-3s", fx[effectIndex]->modMatrix[selectedModSourceIndex].modAmount->getValueString());
	ssd1306_WriteString(OLED_buffer, Font_7x10, (menu_layer == MOD_MATRIX_AMOUNT_SELECTED) ? Black : White);
}

void drawMenuSettings(void)
{
	uint8_t start = (menuItemIndex > 3) ? menuItemIndex - 3 : 0;
	uint8_t end = ((uint8_t)(start + 4) > WS2812SettingParameters.size()) ? WS2812SettingParameters.size() : start + 4;

	for(uint8_t i = start; i < end; i++)
	{
	  // Display menu item
	  uint8_t y = (i - start) * 12 + 18;
	  ssd1306_SetCursor(1, y);
	  sprintf(OLED_buffer, "%s", WS2812SettingParameters[i]->name.c_str());

	  ssd1306_WriteString(OLED_buffer, Font_7x10, White);
	  ssd1306_DrawRectangle(0, y - 1, 89, y + 9, ((i == menuItemIndex) && (menu_layer == SETTINGS_ROOT)) ? White : Black);

	  // Display item value
	  ssd1306_SetCursor(90, y);
	  sprintf(OLED_buffer, "%-3s", WS2812SettingParameters[i]->getValueString());

	  ssd1306_WriteString(OLED_buffer, Font_7x10, ((i == menuItemIndex) && (menu_layer == SETTINGS_VALUE_SELECTED)) ? Black : White);
	}
}

void incrementValueCpp(uint8_t effectIndex, uint8_t parameterIndex, uint8_t parameterSubIndex)
{
	switch(menu_layer)
	{
		case SIMPLE_PARAMETER_VALUE_SELECTED:
			fx[effectIndex]->getSimpleParameters()[parameterIndex]->incrementValue();
			break;
		case HSV_PICKER_VALUE_SELECTED:
			fx[effectIndex]->getColorParameters()[parameterIndex]->incrementValueByIndex(parameterSubIndex);
			break;
		case MOD_MATRIX_AMOUNT_SELECTED:
			fx[effectIndex]->modMatrix[selectedModSourceIndex].modAmount->incrementValue();
			updateModulationScale();
			break;
		case SETTINGS_VALUE_SELECTED:
			WS2812SettingParameters[parameterIndex]->incrementValue();
			*(WS2812SettingValues[parameterIndex]) = *(static_cast<uint16_t *>(WS2812SettingParameters[parameterIndex]->getValue()));
			break;
	}
}

void decrementValueCpp(uint8_t effectIndex, uint8_t parameterIndex, uint8_t parameterSubIndex)
{
	switch(menu_layer)
	{
		case SIMPLE_PARAMETER_VALUE_SELECTED:
			fx[effectIndex]->getSimpleParameters()[parameterIndex]->decrementValue();
			break;
		case HSV_PICKER_VALUE_SELECTED:
			fx[effectIndex]->getColorParameters()[parameterIndex]->decrementValueByIndex(parameterSubIndex);
			break;
		case MOD_MATRIX_AMOUNT_SELECTED:
			fx[effectIndex]->modMatrix[selectedModSourceIndex].modAmount->decrementValue();
			updateModulationScale();
			break;
		case SETTINGS_VALUE_SELECTED:
			WS2812SettingParameters[parameterIndex]->decrementValue();
			*(WS2812SettingValues[parameterIndex]) = *(static_cast<uint16_t *>(WS2812SettingParameters[parameterIndex]->getValue()));
			break;
	}
}

// While menuItemIndex is the main variable used to track cursor position, some menus may use additional variables to store cursor positions for certain levels
void incrementMenuItemIndexCpp(void)
{
	switch(menu_layer)
	{
		case SIMPLE_PARAMETER_ROOT:
			if(menuItemIndex < fx[effectIndex]->getSimpleParameters().size() - 1)
			{
				menuItemIndex++;
			}
			break;
		case COLOR_PALETTE_ROOT:
			if(menuItemIndex < fx[effectIndex]->getColorParameters().size() - 1)
			{
				menuItemIndex++;
			}
			break;
		case HSV_PICKER_ROOT:
			if(HSVPickerIndex < HSV_PICKER_NUM_MENU_ITEMS - 1)
			{
				HSVPickerIndex++;
			}
			break;
		case MOD_MATRIX_ROOT:
			if(selectedModSourceIndex < modMatrixSources.size() - 1)
			{
				selectedModSourceIndex++;
			}
			break;
		case MOD_MATRIX_DESTINATION_SELECTED:
			if(menuItemIndex < fx[effectIndex]->getExpandedParameters().size() - 1)
			{
				menuItemIndex++;
			}
			break;
		case SETTINGS_ROOT:
			if(menuItemIndex < WS2812SettingParameters.size() - 1)
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
		case HSV_PICKER_ROOT:
			HSVPickerIndex = (HSVPickerIndex > 0) ? HSVPickerIndex - 1 : 0;
			break;
		case MOD_MATRIX_ROOT:
			selectedModSourceIndex = (selectedModSourceIndex > 0) ? selectedModSourceIndex - 1 : 0;
			break;
		default:
			menuItemIndex = (menuItemIndex > 0) ? menuItemIndex - 1 : 0;
			break;
	}
}

//TODO: Move the two functions below to the WS2812Effect or ModMatrixEntry class
// Assign destination selected by cursor to currently selected mod source and updates modulation scalar
void updateSelectedModDestinationCpp(void)
{
	// Remove modulation from previous destination
	//TODO: Allow multiple destinations for the same source
	fx[effectIndex]->modMatrix[selectedModSourceIndex].modDestination->modulationSource = NULL;

	// Update mod slot with new destination
	fx[effectIndex]->modMatrix[selectedModSourceIndex].modDestination = fx[effectIndex]->getExpandedParameters()[menuItemIndex].get();

	// Update ADC reference and scale factor inside destination effect parameter
	fx[effectIndex]->modMatrix[selectedModSourceIndex].modDestination->modulationSource = &(rawADCData[selectedModSourceIndex]);
	updateModulationScale();
}

// Update scalar for currently selected modulation
void updateModulationScale(void)
{
	fx[effectIndex]->modMatrix[selectedModSourceIndex].modDestination->modulationScale = *(static_cast<int16_t *>(fx[effectIndex]->modMatrix[selectedModSourceIndex].modAmount->getValue())) / 100.0f;
}

// Informs destination parameters affected by a modulation source of said new source and scalar for all effects
void refreshModMatrix(void)
{
	for(uint8_t i = 0; i < WS2812FX_NUM_EFFECTS; i++)
	{
		for(uint8_t j = 0; j < WS2812FX_EFFECT_MAX_MOD_SLOTS; j++)
		{
			fx[i]->modMatrix[j].modSource = &(rawADCData[j]);
			fx[i]->modMatrix[j].modDestination->modulationSource = fx[i]->modMatrix[j].modSource;
			fx[i]->modMatrix[j].modDestination->modulationScale = *(static_cast<int16_t *>(fx[i]->modMatrix[j].modAmount->getValue())) / 100.0f;
		}
	}
}

extern "C"
{
	void mainWhileC(void)
	{
		mainWhileCpp();
	}

	void updateMenuC(void)
	{
		updateMenu = true;
//		updateMenuCpp();
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

	void updateSelectedModDestinationC(void)
	{
		updateSelectedModDestinationCpp();
	}
	void updateEffectC(void)
	{
		fx[effectIndex]->updateEffect();

		// Update background
		WS2812_SetBackgroundColorHSV(static_cast<colorHSV *>(fx[effectIndex]->backgroundColorParameter->getValue()));

		HAL_ADC_Start_DMA(&hadc1, (uint32_t *) rawADCData, NUM_CV_INPUTS);
	}
}
