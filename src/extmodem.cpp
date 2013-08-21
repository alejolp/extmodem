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


#include "audiosource.h"
#include "decoder.h"
#include "extmodem.h"

namespace extmodem {

modem::modem() {
}

modem::~modem() {
}

void modem::set_audiosource(audiosource_ptr p) {
	audio_source_ = p;
	decoders_.resize(p->get_channel_count());
}

void modem::add_decoder(decoder_ptr p, int ch_num) {
	decoders_[ch_num].push_back(p);
	p->init(audio_source_.get());
}

void modem::input_callback(audiosource* a, const float* buffer, unsigned long length) {
	int channel_count = a->get_channel_count();
	int ch_idx, k, deco_idx;

	if (channel_count == 1) {
		for (deco_idx = 0; deco_idx < (int)decoders_[0].size(); ++deco_idx) {
			decoders_[0][deco_idx]->input_callback(a, buffer, length);
		}
	} else {
		/* NOTE: If the audio source is stereo then the two channels are interleaved, we create a temp array
		 * to store the data contiguously. */

		tmpdata.resize(length);

		for (ch_idx = 0; ch_idx < channel_count; ++ch_idx) {
			/* Copy each channel data contiguously */
			for (k = 0; k < (int)length; ++k) {
				tmpdata[k] = buffer[channel_count * k + ch_idx];
			}

			for (deco_idx = 0; deco_idx < (int)decoders_[ch_idx].size(); ++deco_idx) {
				decoders_[ch_idx][deco_idx]->input_callback(a, tmpdata.data(), length);
			}
		}
	}
}

void modem::start_and_run() {
	audio_source_->set_listener(this);

	tcpserver_.run();
}

void modem::dispatch_packet(unsigned char *bp, unsigned int len) {

}




} /* namespace extmodem */
