/*
 * Sound card modem for Amateur Radio AX25.
 *
 * Copyright (C) Alejandro Santos, 2013, alejolp@gmail.com.
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

#ifndef DECODER_DTMF_H_
#define DECODER_DTMF_H_

#include "decoder.h"


namespace extmodem {

class decoder_dtmf : public decoder {
public:
	explicit decoder_dtmf();
	virtual ~decoder_dtmf();

	virtual void input_callback(audiosource* a, const float* buffer, unsigned long length);

private:
	struct l1_state_dtmf {
		unsigned int ph[8];
		float energy[4];
		float tenergy[4][16];
		int blkcount;
		int lastch;
	} dtmf;

	int process_block();
};

} /* namespace extmodem */
#endif /* DECODER_DTMF_H_ */
