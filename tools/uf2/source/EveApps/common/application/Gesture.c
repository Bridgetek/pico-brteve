/**
 * @file Gesture.c
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

#include "Platform.h"
#include "EVE_CoCmd.h"

#include "Common.h"
#include "Gesture.h"

static Gesture_Touch_t sGesture;
static uint32_t sTag;
static uint16_t sIsTouch;
static uint16_t sTouchX;
static uint16_t sTouchY;
static uint8_t sIsSwipe = 0;
static const int sMinMove = 15;

/**
 * @brief Get horizontal veloxity
 * 
 * @return int horizontal veloxity value
 */
static int Getvelocity_X() {
	static uint16_t _delay = 0;
	static int lastTouchX = 0;
	static uint32_t prev_time = 0;
	uint32_t time = EVE_millis();
	static int distance = 0;
	static long dragprev = 0;
	static int velocityX = 0;
	static int count1Touch = 0;

	if (prev_time != 0) {
		_delay += (time - prev_time);
	}
	prev_time = time;

	if (_delay < 30) {
		return velocityX;
	}
	_delay = 0;

	if (sIsTouch) {
		if (sIsSwipe == 0) {
			if (lastTouchX == 0) {
				lastTouchX = sTouchX;
			}
			else {
				distance = sTouchX - lastTouchX;
			}
			if (abs(distance) > sMinMove) { // this is a swipe
				sIsSwipe = 1;
			}
		}
	}
	else {
		distance = 0;
		lastTouchX = 0;
		sIsSwipe = 0;
	}

	if (sIsSwipe == 1) {
		velocityX = (dragprev - sTouchX) << 4;
	}
	else if (!sIsTouch) { // no touch
		int change = MAX(1, abs(velocityX) >> 5);
		if (velocityX < 0)
			velocityX += change;
		if (velocityX > 0)
			velocityX -= change;
	}
	else {
		count1Touch++;
		if (count1Touch == 5) {
			count1Touch = 0;
			velocityX = 0; // reset velocity when single touch
		}
	}

	dragprev = sTouchX;
	return velocityX;
}

/**
 * @brief Get the Tag number of Released object
 * 
 * @return uint8_t Tag number
 */
static uint32_t GetTagReleased() {
	static uint32_t oldtag = 0;
	uint32_t newTag = 0;
	uint32_t tagRet = 0;

	newTag = sTag;

	if (newTag == 0 && oldtag != newTag) {
		tagRet = oldtag;
		oldtag = 0;
	}
	else if (newTag != 0) {
		oldtag = newTag;
	}
	return tagRet;
}

/**
 * @brief Get Active tag number
 * 
 * @param phost Pointer to Hal context
 * @return uint8_t Tag number
 */
uint8_t Gesture_GetTag(EVE_HalContext *phost)
{
	static uint8_t Read_tag = 0;
	static uint8_t temp_tag = 0;
	static uint8_t ret_tag = 0;
	static uint8_t sk = 0;

	Read_tag = EVE_Hal_rd8(phost, REG_TOUCH_TAG);
	ret_tag = 0;
	if (Read_tag != 0)                // Allow if the Key is released
	{
		if (temp_tag != Read_tag)
		{
			temp_tag = Read_tag;
			sk = Read_tag;         // Load the Read tag to temp variable
		}
	}
	else
	{
		if (temp_tag != 0)
		{
			ret_tag = temp_tag;
			temp_tag = 0; //The event will be processed. Clear the tag
		}
		sk = 0;
	}
	return ret_tag;
}

/**
 * @brief Refresh gesture data
 * 
 */
Gesture_Touch_t *Gesture_Renew(Gpu_Hal_Context_t *phost)
{
	sTag = Gpu_Hal_Rd32(phost, REG_TOUCH_TAG) & 0xFF;
	sIsTouch = Gpu_Hal_Rd32(phost, REG_TOUCH_RAW_XY) != 0xFFFFFFFF;
	sTouchX = Gpu_Hal_Rd16(phost, REG_TOUCH_SCREEN_XY + 2);
	sTouchY = Gpu_Hal_Rd16(phost, REG_TOUCH_SCREEN_XY + 4);
	uint32_t track = Gpu_Hal_Rd32(phost, REG_TRACKER);
	sGesture.tagTrackTouched = track & 0xFF;
	sGesture.trackValLine = track >> 16;
	sGesture.trackValCircle = (track >> 16) * 360 / 65535;
	sGesture.isTouch = (uint8_t)sIsTouch;
	sGesture.tagPressed = sTag;
	sGesture.isSwipe = sIsSwipe;
	sGesture.tagReleased = GetTagReleased();
	sGesture.velocityX = Getvelocity_X();
	sGesture.touchX = sTouchX;
	sGesture.touchY = sTouchY;
	return &sGesture;
}

/**
 * @brief Get gesture data
 *
 * @return Gesture_Touch_t*
 */
Gesture_Touch_t *Gesture_Get()
{
	return &sGesture;
}

