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

#include <iostream>
#include <cmath>

#include "audiosource.h"
#include "decoder.h"
#include "encoder.h"
#include "extmodem.h"
#include "extconfig.h"

namespace extmodem {

modem::modem() : tcpserver_(this) {
}

modem::~modem() {
}

void modem::set_audiosource(audiosource_ptr p) {
	audio_source_ = p;
	decoders_.resize(p->get_in_channel_count());
}

void modem::add_decoder(decoder_ptr p, int ch_num) {
	decoders_[ch_num].push_back(p);
	p->init(audio_source_.get());
}

void modem::set_encoder(encoder_ptr p) {
	encoder_ = p;
	p->init(audio_source_.get());
}


void modem::input_callback(audiosource* a, const float* buffer, unsigned long length) {
	config* cfg = config::Instance();
	unsigned int channel_count = (unsigned int) a->get_in_channel_count();
	unsigned long ch_idx, k, p, deco_idx;
	float mult_factor = cfg->audio_mult_factor();
	int enabled_channels = cfg->enabled_channels();

	if (enabled_channels == -1) {
		enabled_channels = (1 << channel_count) - 1;
	}

	if (channel_count == 1 && (enabled_channels & 1) && std::abs(mult_factor - 1.0f) < 0.0001f) {
		/* fast path, do not make a copy */
		for (deco_idx = 0; deco_idx < decoders_[0].size(); ++deco_idx) {
			decoders_[0][deco_idx]->input_callback(a, buffer, length);
		}
	} else {
		/* NOTE: If the audio source is stereo then the two channels are interleaved, we create a temp array
		 * to store the data contiguously. */

		tmpdata.resize(length);

		for (ch_idx = 0; ch_idx < channel_count; ++ch_idx) {
			if (enabled_channels & (1 << ch_idx)) {
				/* Copy each channel data contiguously */
				for (k = 0, p = ch_idx; p < length; ++k, p += channel_count) {
					tmpdata[k] = buffer[p] * mult_factor;
				}

				for (deco_idx = 0; deco_idx < decoders_[ch_idx].size(); ++deco_idx) {
					decoders_[ch_idx][deco_idx]->input_callback(a, tmpdata.data(), k);
				}
			}
		}
	}
}

void modem::output_callback(audiosource* a, float* buffer, unsigned long length) {
	(void)a;
	// int channel_count = a->get_out_channel_count();

	if (encoder_.get()) {
		encoder_->output_callback(audio_source_.get(), buffer, length);
	}
}

void modem::start_and_run() {
	audio_source_->set_listener(this);

	tcpserver_.run();
}

void modem::dispatch_packet(frame_ptr fp) {
	tcpserver_.write_to_all_safe(fp);
}

void modem::output_packet_to_sc(frame_ptr fp) {
	encoder_->send(fp);
}


} /* namespace extmodem */
