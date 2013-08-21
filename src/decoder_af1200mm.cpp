/*
 * decoder_af1200mm.cpp
 *
 *  Created on: 19/08/2013
 *      Author: alejo
 *
 *  Borrowed code from "multimon", by Thomas Sailer (sailer@ife.ee.ethz.ch, hb9jnx@hb9w.che.eu).
 *  http://www.baycom.org/~tom/ham/linux/multimon.html
 *
 */
/*
 *      demod_afsk12.c -- 1200 baud AFSK demodulator
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

#include <cstring>
#include <cmath>
#include <iostream>


#include "decoder_af1200mm.h"
#include "multimon_utils.h"


namespace extmodem {


/*
 * Standard TCM3105 clock frequency: 4.4336MHz
 * Mark frequency: 2200 Hz
 * Space frequency: 1200 Hz
 */

#define FREQ_MARK  1200
#define FREQ_SPACE 2200
#define FREQ_SAMP  22050
#define BAUD       1200
#define SUBSAMP    2

/* ---------------------------------------------------------------------- */

#define CORRLEN ((int)(FREQ_SAMP/BAUD))
#define SPHASEINC (0x10000u*BAUD*SUBSAMP/FREQ_SAMP)

namespace {

	static float corr_mark_i[CORRLEN];
	static float corr_mark_q[CORRLEN];
	static float corr_space_i[CORRLEN];
	static float corr_space_q[CORRLEN];
	static bool were_tables_init = false;

}

decoder_af1200mm::decoder_af1200mm() {
	float f;
	int i;

	std::memset(&afsk12, 0, sizeof(afsk12));
	hdlc_.set_name("af1200 mm");

	if (!were_tables_init) {
		were_tables_init = true;
		for (f = 0, i = 0; i < CORRLEN; i++) {
			corr_mark_i[i] = std::cos(f);
			corr_mark_q[i] = std::sin(f);
			f += 2.0*M_PI*FREQ_MARK/FREQ_SAMP;
		}
		for (f = 0, i = 0; i < CORRLEN; i++) {
			corr_space_i[i] = std::cos(f);
			corr_space_q[i] = std::sin(f);
			f += 2.0*M_PI*FREQ_SPACE/FREQ_SAMP;
		}
	}
}

decoder_af1200mm::~decoder_af1200mm() {

}

void decoder_af1200mm::input_callback(audiosource* a, const float* buffer, unsigned long length) {
	float f;
	unsigned char curbit;

	if (afsk12.subsamp) {
		int numfill = SUBSAMP - afsk12.subsamp;
		if ((int)length < numfill) {
			afsk12.subsamp += length;
			return;
		}
		buffer += numfill;
		length -= numfill;
		afsk12.subsamp = 0;
	}

	/* FIXME: THIS CONVOLUTION IS CPU INTENSIVE */

	for (; length >= SUBSAMP; length -= SUBSAMP, buffer += SUBSAMP) {
		f = fsqr(mac(buffer, corr_mark_i, CORRLEN)) +
			fsqr(mac(buffer, corr_mark_q, CORRLEN)) -
			fsqr(mac(buffer, corr_space_i, CORRLEN)) -
			fsqr(mac(buffer, corr_space_q, CORRLEN));
		afsk12.dcd_shreg <<= 1;
		afsk12.dcd_shreg |= (f > 0);
		// verbprintf(10, "%c", '0'+(afsk12.dcd_shreg & 1));
		/*
		 * check if transition
		 */
		if ((afsk12.dcd_shreg ^ (afsk12.dcd_shreg >> 1)) & 1) {
			if (afsk12.sphase < (0x8000u-(SPHASEINC/2)))
				afsk12.sphase += SPHASEINC/8;
			else
				afsk12.sphase -= SPHASEINC/8;
		}
		afsk12.sphase += SPHASEINC;
		if (afsk12.sphase >= 0x10000u) {
			afsk12.sphase &= 0xffffu;
			afsk12.lasts <<= 1;
			afsk12.lasts |= afsk12.dcd_shreg & 1;
			curbit = (afsk12.lasts ^
				  (afsk12.lasts >> 1) ^ 1) & 1;
			// verbprintf(9, " %c ", '0'+curbit);
			hdlc_.rxbit(curbit);
		}
	}
	afsk12.subsamp = length;
}



} /* namespace extmodem */
