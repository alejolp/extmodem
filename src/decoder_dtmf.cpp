/*
 * decoder_dtmf.cpp
 *
 *  Created on: 19/08/2013
 *      Author: alejo
 *
 *  Borrowed code from "multimon", by Thomas Sailer (sailer@ife.ee.ethz.ch, hb9jnx@hb9w.che.eu).
 */
/*
 *      demod_dtmf.c -- DTMF signalling demodulator/decoder
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
#include <cstdlib>
#include <iostream>

#include "audiosource.h"
#include "decoder_dtmf.h"
#include "multimon_utils.h"
#include "extmodem.h"


namespace extmodem {

#define BLOCKNUM 4    /* must match numbers in multimon.h */

#if 0
#define SAMPLE_RATE 22050
#define BLOCKLEN (SAMPLE_RATE/100)  /* 10ms blocks */
#define PHINC(x) ((x)*0x10000/SAMPLE_RATE)
#endif

namespace {

	static const char *dtmf_transl = "123A456B789C*0#D";

	static int find_max_idx(const float *f)
	{
		float en = 0;
		int idx = -1, i;

		for (i = 0; i < 4; i++)
			if (f[i] > en) {
				en = f[i];
				idx = i;
			}
		if (idx < 0)
			return -1;
		en *= 0.1f;
		for (i = 0; i < 4; i++)
			if (idx != i && f[i] > en)
				return -1;
		return idx;
	}

}


decoder_dtmf::decoder_dtmf(modem* em) : em_(em) {
	std::memset(&dtmf, 0, sizeof(dtmf));

	sample_rate_ = em->get_audiosource()->get_sample_rate();
	blocklen_ = (sample_rate_/100);

	/*
	unsigned int dtmf_phinc[8] = {
			PHINC(1209), PHINC(1336), PHINC(1477), PHINC(1633),
			PHINC(697), PHINC(770), PHINC(852), PHINC(941)
		};
		*/

	dtmf_phinc[0] = PHINC(1209);
	dtmf_phinc[1] = PHINC(1336);
	dtmf_phinc[2] = PHINC(1477);
	dtmf_phinc[3] = PHINC(1633);
	dtmf_phinc[4] = PHINC(697);
	dtmf_phinc[5] = PHINC(770);
	dtmf_phinc[6] = PHINC(852);
	dtmf_phinc[7] = PHINC(941);
}

decoder_dtmf::~decoder_dtmf() {}

int decoder_dtmf::process_block()
{
	float tote;
	float totte[16];
	int i, j;

	tote = 0;
	for (i = 0; i < BLOCKNUM; i++)
		tote += dtmf.energy[i];
	for (i = 0; i < 16; i++) {
		totte[i] = 0;
		for (j = 0; j < BLOCKNUM; j++)
			totte[i] += dtmf.tenergy[j][i];
	}
	for (i = 0; i < 8; i++)
		totte[i] = fsqr(totte[i]) + fsqr(totte[i+8]);
	memmove(dtmf.energy+1, dtmf.energy,
		sizeof(dtmf.energy) - sizeof(dtmf.energy[0]));
	dtmf.energy[0] = 0;
#if 0
	memmove(dtmf.tenergy+1, dtmf.tenergy,
		sizeof(dtmf.tenergy) - sizeof(dtmf.tenergy[0]));
#else
	for (i = 1; i < 4; ++i)
		memmove(dtmf.tenergy+4-i, dtmf.tenergy+4-i-1, sizeof(dtmf.tenergy[0]));
#endif
	memset(dtmf.tenergy, 0, sizeof(dtmf.tenergy[0]));
	tote *= (BLOCKNUM*blocklen_*0.5);  /* adjust for block lengths */
	/*
	verbprintf(10, "DTMF: Energies: %8.5f  %8.5f %8.5f %8.5f %8.5f  %8.5f %8.5f %8.5f %8.5f\n",
		   tote, totte[0], totte[1], totte[2], totte[3], totte[4], totte[5], totte[6], totte[7]);
		   */
	if ((i = find_max_idx(totte)) < 0)
		return -1;
	if ((j = find_max_idx(totte+4)) < 0)
		return -1;
	if ((tote * 0.4) > (totte[i] + totte[j+4]))
		return -1;
	return (i & 3) | ((j << 2) & 0xc);
}

void decoder_dtmf::input_callback(audiosource* a, const float* buffer, unsigned long length) {
	(void)a;
	float s_in;
	int i;

	for (; length > 0; length--, buffer++) {
		s_in = *buffer;
		dtmf.energy[0] += fsqr(s_in);
		for (i = 0; i < 8; i++) {
			dtmf.tenergy[0][i] += COS(dtmf.ph[i]) * s_in;
			dtmf.tenergy[0][i + 8] += SIN(dtmf.ph[i]) * s_in;
			dtmf.ph[i] += dtmf_phinc[i];
		}
		if ((dtmf.blkcount--) <= 0) {
			dtmf.blkcount = blocklen_;
			i = process_block();
			if (i != dtmf.lastch && i >= 0) {
				std::cerr << "DTMF: " << dtmf_transl[i] << std::endl;
			}
			dtmf.lastch = i;
		}
	}
}

} /* namespace extmodem */
