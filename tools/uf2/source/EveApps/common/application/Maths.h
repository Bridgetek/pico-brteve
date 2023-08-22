/**
 * @file Maths.h
 * @brief Mathematic utilities
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

#ifndef MATH_H_
#define MATH_H_

#include <stdio.h>
#include <stdint.h>
#include "EVE_CoCmd.h"

typedef struct Math_3d_Xyz {
	double x, y, z;
} Math_3d_Xyz_t;

typedef struct _Math_3d_Face_t {
	Math_3d_Xyz_t p1, p2, p3;
}Math_3d_Face_t;

float Math_Da(float i, int16_t degree);
void Math_Polarxy(int32_t r, float th, int32_t *x, int32_t *y, int32_t ox, int32_t oy);
int16_t Math_Qsin(uint16_t a);
int16_t Math_Qcos(uint16_t a);
float Math_Power(float x, unsigned int y);
uint32_t Math_Points_Distance(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);
uint32_t Math_Points_Nearby_NextX(uint32_t x1, uint32_t y1, uint32_t y2, uint32_t Distance);

Math_3d_Xyz_t Math_3D_ArbitraryRotate(Math_3d_Xyz_t p, double theta, Math_3d_Xyz_t r);
Math_3d_Xyz_t Math_3D_ArbitraryRotate2(Math_3d_Xyz_t p, double theta, Math_3d_Xyz_t p1, Math_3d_Xyz_t p2);
int Math_3D_Backface_Find_Visible(Math_3d_Face_t face, Math_3d_Xyz_t view);

#endif /* MATH_H_ */
