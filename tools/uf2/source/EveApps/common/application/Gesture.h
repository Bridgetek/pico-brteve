/**
 * @file Gesture.h
 * @brief Gesture for touch screen
 *
 * @author Tuan Nguyen <tuan.nguyen@brtchip.com>
 *
 * @date 2019
 * 
 * MIT License
 *
 * Copyright (c) [2019] [Bridgetek Pte Ltd (BRTChip)]
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef GESTURE_H_
#define GESTURE_H_

#include "EVE_CoCmd.h"

typedef struct Gesture_Touch {
	uint32_t tagTrackTouched;
	uint32_t tagPressed;
	uint32_t tagReleased;
	uint32_t trackValLine;
	uint32_t trackValCircle;
	uint8_t isTouch;
	uint8_t isSwipe;
	uint16_t touchX;
	uint16_t touchY;
	int velocityX;
}Gesture_Touch_t;

uint8_t Gesture_GetTag(EVE_HalContext *phost);
Gesture_Touch_t* Gesture_Renew(EVE_HalContext *phost);
Gesture_Touch_t* Gesture_Get();

#endif /* GESTURE_H_ */
