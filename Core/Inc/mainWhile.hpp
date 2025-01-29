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

#ifdef __cplusplus
extern "C"
{
#endif
	void mainWhileC(void);
	void updateMenuC(void);
#ifdef __cplusplus
}
#endif

#endif /* INC_MAINWHILE_HPP_ */
