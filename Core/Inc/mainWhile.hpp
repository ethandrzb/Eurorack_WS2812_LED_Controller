/*
 * mainWhile.hpp
 *
 *  Created on: Jan 13, 2025
 *      Author: ethan
 */

#ifndef INC_MAINWHILE_HPP_
#define INC_MAINWHILE_HPP_

#include "WS2812.h"

#define WS2812FX_NUM_EFFECTS 5
#define HSV_PICKER_NUM_MENU_ITEMS 3

void mainWhileCpp(void);
void updateMenuCpp(void);
void drawMenuSimpleParameter(void);
void drawMenuColorPalette(void);
void drawHSVPicker(void);
void drawMenuModMatrix(void);
void drawMenuSettings(void);
void incrementValueCpp(uint8_t effectIndex, uint8_t parameterIndex, uint8_t parameterSubIndex);
void decrementValueCpp(uint8_t effectIndex, uint8_t parameterIndex, uint8_t parameterSubIndex);
void incrementMenuItemIndexCpp(void);
void decrementMenuItemIndexCpp(void);
void updateSelectedModDestinationCpp(void);
void updateModulationScale(void);
void refreshModMatrix(void);

#ifdef __cplusplus
extern "C"
{
#endif
	void mainWhileC(void);
	void gateInput0HighCallbackC(void);
	void gateInput1HighCallbackC(void);
	void gateInput2HighCallbackC(void);
	void gateInput3HighCallbackC(void);
	void updateMenuC(void);
	void incrementValueC(uint8_t effectIndex, uint8_t parameterIndex, uint8_t parameterSubIndex);
	void decrementValueC(uint8_t effectIndex, uint8_t parameterIndex, uint8_t parameterSubIndex);
	void incrementMenuItemIndexC(void);
	void decrementMenuItemIndexC(void);
	void updateSelectedModDestinationC(void);
	void updateEffectC(void);
#ifdef __cplusplus
}
#endif

#endif /* INC_MAINWHILE_HPP_ */
