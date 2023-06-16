/**
 * @file Maths.c
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

#include <math.h>

#include "Gpu_Hal.h" /// Use PROGMEM
#include "Maths.h"

#define doubleswap(x, y) {double z = x;x=y;y=z;}
#define xyzswap(x, y) {Math_3d_Xyz_t z = x;x=y;y=z;}

 /* Optimized implementation of sin and cos table - precision is 16 bit */
PROGMEM prog_uint16_t sintab[] = {
	0, 402, 804, 1206, 1607, 2009, 2410, 2811, 3211, 3611, 4011, 4409, 4807, 5205, 5601, 5997, 6392,
	6786, 7179, 7571, 7961, 8351, 8739, 9126, 9511, 9895, 10278, 10659, 11038, 11416, 11792, 12166, 12539,
	12909, 13278, 13645, 14009, 14372, 14732, 15090, 15446, 15799, 16150, 16499, 16845, 17189, 17530, 17868,
	18204, 18537, 18867, 19194, 19519, 19840, 20159, 20474, 20787, 21096, 21402, 21705, 22004, 22301, 22594,
	22883, 23169, 23452, 23731, 24006, 24278, 24546, 24811, 25072, 25329, 25582, 25831, 26077, 26318, 26556, 26789,
	27019, 27244, 27466, 27683, 27896, 28105, 28309, 28510, 28706, 28897, 29085, 29268, 29446, 29621, 29790, 29955,
	30116, 30272, 30424, 30571, 30713, 30851, 30984, 31113, 31236, 31356, 31470, 31580, 31684, 31785, 31880, 31970,
	32056, 32137, 32213, 32284, 32350, 32412, 32468, 32520, 32567, 32609, 32646, 32678, 32705, 32727, 32744, 32757,
	32764, 32767, 32764 };

/**
 * @brief Sin function
 *
 * @param a Angle
 * @return int16_t
 */
int16_t Math_Qsin(uint16_t a)
{
	uint8_t f;
	int16_t s0;
	int16_t s1;

	if (a & 32768) {
		return -Math_Qsin(a & 32767);
	}
	if (a & 16384) {
		a = 32768 - a;
	}
	f = a & 127;
	s0 = pgm_read_word(sintab + (a >> 7));
	s1 = pgm_read_word(sintab + (a >> 7) + 1);
	return (int16_t)(s0 + (int16_t)((int32_t)f * (s1 - s0) >> 7));
}

/**
 * @brief Cos function
 *
 * @param a Angle
 * @return int16_t
 */
int16_t Math_Qcos(uint16_t a)
{
	return (Math_Qsin(a + 16384));
}

/**
 * @brief Polar function
 *
 * @param r
 * @param th
 * @param x
 * @param y
 * @param ox
 * @param oy
 */
void Math_Polarxy(int32_t r, float th, int32_t* x, int32_t* y, int32_t ox, int32_t oy) {
	*x = (16 * ox) + (((long)r * Math_Qsin((uint16_t)th)) >> 11) + 16;
	*y = (16 * oy) - (((long)r * Math_Qcos((uint16_t)th)) >> 11);
}

/**
  * @brief Da function
  * 1 uint = 0.5 degree.
  * 1 circle = 720 degree
  *
  * @param i
  * @param degree
  * @return float
  */
float Math_Da(float i, int16_t degree)
{
	return (i - degree) * 32768 / 360;
}

float Math_Power(float x, unsigned int y)
{
	if (y == 0)
		return 1;
	else if (y % 2 == 0)
		return Math_Power(x, y / 2) * Math_Power(x, y / 2);
	else
		return x * Math_Power(x, y / 2) * Math_Power(x, y / 2);
}

uint32_t Math_Points_Distance(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2) {
	return (uint32_t)sqrt((double)(Math_Power((float)(x2 - x1), 2) + Math_Power((float)(y2 - y1), 2)));
}

uint32_t Math_Points_Nearby_NextX(uint32_t x1, uint32_t y1, uint32_t y2, uint32_t Distance) {
	return (uint32_t)sqrt((double)Math_Power((float)Distance, 2) - Math_Power((float)abs(y2 - y1), 2)) + x1;
}

/*
   Normalise a vector
*/
void Normalise(Math_3d_Xyz_t* p)
{
	double length;

	length = sqrt(p->x * p->x + p->y * p->y + p->z * p->z);
	if (length != 0) {
		p->x /= length;
		p->y /= length;
		p->z /= length;
	}
	else {
		p->x = 0;
		p->y = 0;
		p->z = 0;
	}
}

/*
   Rotate a point p by angle theta around an arbitrary axis r
   Return the rotated point.
   Positive angles are anticlockwise looking down the axis
   towards the origin.
   Assume right hand coordinate system.
*/
Math_3d_Xyz_t Math_3D_ArbitraryRotate(Math_3d_Xyz_t p, double theta, Math_3d_Xyz_t r) {
	Math_3d_Xyz_t q = { 0.0,0.0,0.0 };
	double costheta;
	double sintheta;

	Normalise(&r);
	costheta = cos(theta);
	sintheta = sin(theta);

	q.x += (costheta + (1 - costheta) * r.x * r.x) * p.x;
	q.x += ((1 - costheta) * r.x * r.y - r.z * sintheta) * p.y;
	q.x += ((1 - costheta) * r.x * r.z + r.y * sintheta) * p.z;

	q.y += ((1 - costheta) * r.x * r.y + r.z * sintheta) * p.x;
	q.y += (costheta + (1 - costheta) * r.y * r.y) * p.y;
	q.y += ((1 - costheta) * r.y * r.z - r.x * sintheta) * p.z;

	q.z += ((1 - costheta) * r.x * r.z - r.y * sintheta) * p.x;
	q.z += ((1 - costheta) * r.y * r.z + r.x * sintheta) * p.y;
	q.z += (costheta + (1 - costheta) * r.z * r.z) * p.z;

	return(q);
}

/*
   Rotate a point p by angle theta around an arbitrary line segment p1-p2
   Return the rotated point.
   Positive angles are anticlockwise looking down the axis
   towards the origin.
   Assume right hand coordinate system.
*/
Math_3d_Xyz_t Math_3D_ArbitraryRotate2(Math_3d_Xyz_t p, double theta, Math_3d_Xyz_t p1, Math_3d_Xyz_t p2)
{
	Math_3d_Xyz_t q = { 0.0,0.0,0.0 };
	double costheta;
	double sintheta;
	Math_3d_Xyz_t r;

	r.x = p2.x - p1.x;
	r.y = p2.y - p1.y;
	r.z = p2.z - p1.z;
	p.x -= p1.x;
	p.y -= p1.y;
	p.z -= p1.z;
	Normalise(&r);

	sintheta = sin(theta);
	costheta = cos(theta);

	q.y += ((1 - costheta) * r.x * r.y + r.z * sintheta) * p.x;
	q.y += (costheta + (1 - costheta) * r.y * r.y) * p.y;
	q.y += ((1 - costheta) * r.y * r.z - r.x * sintheta) * p.z;

	q.x += (costheta + (1 - costheta) * r.x * r.x) * p.x;
	q.x += ((1 - costheta) * r.x * r.y - r.z * sintheta) * p.y;
	q.x += ((1 - costheta) * r.x * r.z + r.y * sintheta) * p.z;

	q.z += ((1 - costheta) * r.x * r.z - r.y * sintheta) * p.x;
	q.z += ((1 - costheta) * r.y * r.z + r.x * sintheta) * p.y;
	q.z += (costheta + (1 - costheta) * r.z * r.z) * p.z;

	q.x += p1.x;
	q.y += p1.y;
	q.z += p1.z;
	return(q);
}

static Math_3d_Xyz_t subVector(Math_3d_Xyz_t p1, Math_3d_Xyz_t p2) {
	Math_3d_Xyz_t ret;
	ret.x = p1.x - p2.x;
	ret.y = p1.y - p2.y;
	ret.z = p1.z - p2.z;

	return ret;
}

static Math_3d_Xyz_t normalVector(Math_3d_Xyz_t p1, Math_3d_Xyz_t p2) {
	Math_3d_Xyz_t n;

	n.x = p1.y * p2.z - p1.z * p2.y;
	n.y = p1.z * p2.x - p1.x * p2.z;
	n.z = p1.x * p2.y - p1.y * p2.x;

	return n;
}

static double dotProduct(Math_3d_Xyz_t a, Math_3d_Xyz_t b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

int Math_3D_Backface_Find_Visible(Math_3d_Face_t face, Math_3d_Xyz_t view) {
	Math_3d_Xyz_t v12 = subVector(face.p2, face.p1);
	Math_3d_Xyz_t v13 = subVector(face.p3, face.p1);
	Math_3d_Xyz_t N = normalVector(v12, v13);
	Math_3d_Xyz_t S = subVector(view, face.p1);

	double Dot = dotProduct(N, S);

	return Dot >= 0;
}

