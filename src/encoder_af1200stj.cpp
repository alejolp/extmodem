/*
 * encoder_af1200stj.cpp
 *
 *  Created on: 23/08/2013
 *      Author: alejo
 *
 *  Based on the AFSK modem implementation of Sivan Toledo.
 *
 *  Code: https://github.com/sivantoledo/javAX25
 *  QEX paper: http://www.tau.ac.il/~stoledo/Bib/Pubs/QEX-JulAug-2012.pdf
 *
 */
/*
 * Audio FSK modem for AX25 (1200 Baud, 1200/2200Hz).
 *
 * Copyright (C) Sivan Toledo, 2012
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

#ifdef _MSC_VER
#define _USE_MATH_DEFINES
#endif

#include <cmath>
#include <iostream>
#include <cstdio>

#include "hdlc.h"
#include "audiosource.h"
#include "extconfig.h"

#include "encoder_af1200stj.h"

namespace extmodem {

encoder_af1200stj::encoder_af1200stj() {
	out_queue_ptr_ = 0;
}

encoder_af1200stj::~encoder_af1200stj() {

}

void encoder_af1200stj::output_callback(audiosource* a, float* buffer, unsigned long length) {
	(void)a;
	int num_channels = a->get_out_channel_count();
	int ch_idx;
	std::size_t i = 0;
	std::size_t buffer_size = length * num_channels;

	/* BEWARE: The channels samples are INTERLEAVED. There are (length * num_channels) elements in "buffer" array. */

	// std::cout << " out callback " << buffer_size << std::endl;

	{
		boost::lock_guard<boost::mutex> guard_(out_queue_mutex_);
		if (!out_queue_.empty() && !ptt_->get_tx())
			ptt_->set_tx(1);
	}

	if (num_channels >= 1) {
		i = 0;

		while (i < buffer_size) {
			buffer_ptr p;
			{
				boost::lock_guard<boost::mutex> guard_(out_queue_mutex_);
				if (out_queue_.empty())
					break;
				p = out_queue_.front();
			}

			// std::cout << " out quedan " << out_queue_.size() << " tope de " << p->size() << std::endl;

			while ((i < buffer_size) && (out_queue_ptr_ < p->size())) {
				for (ch_idx = 0; ch_idx < num_channels; ++ch_idx) {
					buffer[i] = p->at(out_queue_ptr_);
					i++;
				}
				out_queue_ptr_++;
			}

			if (out_queue_ptr_ == p->size()) {
				boost::lock_guard<boost::mutex> guard_(out_queue_mutex_);
				out_queue_.pop_front();
				out_queue_ptr_ = 0;
			}
		}
	}

	while (i < buffer_size)
		buffer[i++] = 0;

	{
		boost::lock_guard<boost::mutex> guard_(out_queue_mutex_);
		if (out_queue_.empty() && ptt_->get_tx())
			ptt_->set_tx(0);
	}
}

void encoder_af1200stj::init(audiosource* a) {
	sample_rate_ = a->get_sample_rate();

	phase_inc_f0 = (float) (2.0*M_PI*1200.0/sample_rate_);
	phase_inc_f1 = (float) (2.0*M_PI*2200.0/sample_rate_);
	phase_inc_symbol = (float) (2.0*M_PI*1200.0/sample_rate_);

	ptt_ = ptt::factory(config::Instance()->ptt_mode());
	ptt_->init(config::Instance()->ptt_port().c_str());
}

void encoder_af1200stj::generateSymbolSamples(int symbol) {
	while (tx_symbol_phase < (float) (2.0*M_PI)) {
		float sample = (float) std::sin(tx_dds_phase);
		out->push_back(sample);

		if (symbol==0) tx_dds_phase += phase_inc_f0;
		else tx_dds_phase += phase_inc_f1;

		tx_symbol_phase += phase_inc_symbol;

		//if (tx_symbol_phase > (float) (2.0*Math.PI)) tx_symbol_phase -= (float) (2.0*Math.PI);
		if (tx_dds_phase > (float) (2.0*M_PI)) tx_dds_phase -= (float) (2.0*M_PI);
	}

	tx_symbol_phase -= (float) (2.0*M_PI);
}

int encoder_af1200stj::byteToSymbols(int bits, bool stuff) {
	int symbol;
	int position = 0;

	//System.out.printf("byte=%02x stuff=%b\n",bits,stuff);
	for (int i = 0; i < 8; i++) {
		int bit = bits & 1;
		//System.out.println("i="+i+" bit="+bit);
		bits = bits >> 1;
		if (bit == 0) { // we switch sybols (frequencies)
			symbol = (tx_last_symbol == 0) ? 1 : 0;
			generateSymbolSamples(symbol);

			if (stuff)
				tx_stuff_count = 0;
			tx_last_symbol = symbol;
		} else {
			symbol = (tx_last_symbol == 0) ? 0 : 1;
			generateSymbolSamples(symbol);

			if (stuff)
				tx_stuff_count++;
			tx_last_symbol = symbol;

			if (stuff && tx_stuff_count == 5) {
				// send a zero
				//System.out.println("stuffing a zero bit!");
				symbol = (tx_last_symbol == 0) ? 1 : 0;
				generateSymbolSamples(symbol);

				tx_stuff_count = 0;
				tx_last_symbol = symbol;
			}
		}
	}
	//System.out.println("generated "+position+" samples");
	return position;
}

void encoder_af1200stj::send(frame_ptr fp) {
	std::size_t k;
	std::vector<unsigned char> &buffer = fp->get_data();

	{
		boost::lock_guard<boost::mutex> guard_(out_queue_mutex_);
		// FIXME: Do something more interesting than silently dropping the frame.
		if (out_queue_.size() > 5)
			return;
	}

	tx_symbol_phase = tx_dds_phase = 0.0f;
	tx_last_symbol = 0;
	tx_stuff_count = 0;

	std::size_t tx_delay = config::Instance()->tx_delay(), tx_tail = config::Instance()->tx_tail();

	out.reset(new std::vector<float>());

	// Preamble
	for (k = out->size(); ((out->size() - k) * 1000) / sample_rate_ < tx_delay; )
		byteToSymbols(0x7E, false);

	// Data
	for (k = 0; k < buffer.size(); ++k)
		byteToSymbols(buffer[k], true);

	// CRC
	int crc = calc_crc_ccitt(buffer.data(), buffer.size());

	byteToSymbols((crc & 0xFF) ^ 0xFF, true);
	byteToSymbols(((crc >> 8) & 0xFF) ^ 0xFF, true);

	if (config::Instance()->debug()) {
		std::vector<unsigned char> qqq(buffer);
		qqq.push_back((crc & 0xFF)^0xff);
		qqq.push_back(((crc >> 8) & 0xFF)^0xff);
		std::cout << "QQQ bufsize " << buffer.size() << " - crcqqq " << calc_crc_ccitt(qqq.data(), qqq.size()) << " - crc " << crc << " - crcok " << 0xF0B8 << std::endl;

		for (k = 0; k < qqq.size(); ++k) {
			std::printf("%02x ", qqq[k]);
		}
		std::printf("\n");
	}

	// Tail
	for (k = out->size(); ((out->size() - k) * 1000) / sample_rate_ < tx_tail; )
		byteToSymbols(0x7E, false);

	boost::lock_guard<boost::mutex> guard_(out_queue_mutex_);
	out_queue_.push_back(out);
	out.reset();
}

} /* namespace extmodem */
