/*
 * multimon_utils.h
 *
 *  Created on: 19/08/2013
 *      Author: Alejandro Santos LU4EXT
 *
 *  Borrowed code from "multimon", by Thomas Sailer (sailer@ife.ee.ethz.ch, hb9jnx@hb9w.che.eu).
 */
/*
 *      multimon.h -- Monitor for many different modulation formats
 *
 *      Copyright (C) 1996
 *          Thomas Sailer (sailer@ife.ee.ethz.ch, hb9jnx@hb9w.che.eu)
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#ifndef MULTIMON_UTILS_H_
#define MULTIMON_UTILS_H_

extern float costabf[0x400];

#define COS(x) costabf[(((x)>>6)&0x3ffu)]
#define SIN(x) COS((x)+0xc000)

extern inline float fsqr(float f)
{
	return f*f;
}

#ifndef __HAVE_ARCH_MAC
extern inline float mac(const float *a, const float *b, unsigned int size)
{
	float sum = 0;
	unsigned int i;

	for (i = 0; i < size; i++)
		sum += (*a++) * (*b++);
	return sum;
}
#endif /* __HAVE_ARCH_MAC */

int verbprintf(int log_level, const char* fmt, ...);


#endif /* MULTIMON_UTILS_H_ */
