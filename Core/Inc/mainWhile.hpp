/*
 * mainWhile.hpp
 *
 *  Created on: Jan 13, 2025
 *      Author: ethan
 */

#ifndef INC_MAINWHILE_HPP_
#define INC_MAINWHILE_HPP_

#include "WS2812.h"

#define WS2812FX_NUM_EFFECTS 2

void mainWhileCpp(void);
void updateMenuCpp(void);
void drawMenuNumericParameter(void);
void drawMenuColorPalette(void);
void drawHSVPicker(void);
void incrementValueCpp(uint8_t effectIndex, uint8_t parameterIndex, uint8_t parameterSubIndex);
void decrementValueCpp(uint8_t effectIndex, uint8_t parameterIndex, uint8_t parameterSubIndex);
void incrementMenuItemIndexCpp(void);
void decrementMenuItemIndexCpp(void);
void populateMenuItemsCpp(void);

#ifdef __cplusplus
extern "C"
{
#endif
	void mainWhileC(void);
	void updateMenuC(void);
	void incrementValueC(uint8_t effectIndex, uint8_t parameterIndex, uint8_t parameterSubIndex);
	void decrementValueC(uint8_t effectIndex, uint8_t parameterIndex, uint8_t parameterSubIndex);
	void incrementMenuItemIndexC(void);
	void decrementMenuItemIndexC(void);
	void populateMenuItemsC(void);
#ifdef __cplusplus
}
#endif

#endif /* INC_MAINWHILE_HPP_ */
