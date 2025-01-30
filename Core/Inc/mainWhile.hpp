/*
 * mainWhile.hpp
 *
 *  Created on: Jan 13, 2025
 *      Author: ethan
 */

#ifndef INC_MAINWHILE_HPP_
#define INC_MAINWHILE_HPP_

#include "WS2812.h"

void mainWhileCpp(void);
void updateMenuCpp(void);
void incrementValueCpp(uint8_t effectIndex, uint8_t parameterIndex);
void decrementValueCpp(uint8_t effectIndex, uint8_t parameterIndex);

#ifdef __cplusplus
extern "C"
{
#endif
	void mainWhileC(void);
	void updateMenuC(void);
	void incrementValueC(uint8_t effectIndex, uint8_t parameterIndex);
	void decrementValueC(uint8_t effectIndex, uint8_t parameterIndex);
#ifdef __cplusplus
}
#endif

#endif /* INC_MAINWHILE_HPP_ */
