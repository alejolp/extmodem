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

#ifndef DECODER_AF1200STJ_H_
#define DECODER_AF1200STJ_H_

#include <cstdlib>

#include "decoder.h"
#include "hdlc.h"


namespace extmodem {

/* "stj" stands for Sivan Toledo Java. */

class decoder_af1200stj : public decoder {
public:
	decoder_af1200stj(modem* em);
	virtual ~decoder_af1200stj();

	virtual void input_callback(audiosource* a, const float* input, unsigned long frameCount);
	virtual void init(audiosource* a);

private:
	void init_state(audiosource* a, int state_idx, int emphasis, int filter_index);
	void input_callback_state(audiosource* a, const float* buffer, unsigned long length, int state_idx);

	struct demod_state {
		demod_state(modem* em) : hdlc_(em) {}
		int rate_index{};
		int sample_rate{};
		int samples_per_bit{};
		int emphasis{};
		int filter_index{};
		float* td_filter{};
		std::size_t td_filter_length{};
		float* cd_filter{};
		std::size_t cd_filter_length{};
		std::vector<float> x;
		std::vector<float> u1;
		std::vector<float> c0_real;
		std::vector<float> c0_imag;
		std::vector<float> c1_real;
		std::vector<float> c1_imag;
		std::vector<float> diff;
		float phase_inc_f0{};
		float phase_inc_f1{};
		float phase_inc_symbol{};
		float phase_f0{};
		float phase_f1{};
		std::size_t j_td{}; // time domain index
		std::size_t j_cd{}; // time domain index
		std::size_t j_corr{}; // correlation index
		std::size_t t{};
		float previous_fdiff{};
		std::size_t last_transition{};
		hdlc hdlc_;
	};
	std::vector<demod_state> states_;
	modem* em_;
};

} /* namespace extmodem */
#endif /* DECODER_AF1200STJ_H_ */
