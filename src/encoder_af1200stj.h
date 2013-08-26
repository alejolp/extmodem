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

#ifndef ENCODER_AF1200STJ_H_
#define ENCODER_AF1200STJ_H_

#include <vector>
#include <deque>
#include <cstdlib>

#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>

#include "encoder.h"
#include "ptt.h"


namespace extmodem {

class encoder_af1200stj : public encoder {
public:
	encoder_af1200stj();
	virtual ~encoder_af1200stj();

	virtual void output_callback(audiosource* a, float* buffer, unsigned long length);
	virtual void init(audiosource* a);
	virtual void send(frame_ptr fp);

private:
	typedef boost::shared_ptr< std::vector<float> > buffer_ptr;

	void generateSymbolSamples(int symbol);
	int byteToSymbols(int bits, bool stuff);

	float tx_symbol_phase;
	float tx_dds_phase;
	int sample_rate_;
	float phase_inc_f0;
	float phase_inc_f1;
	float phase_inc_symbol;
	int tx_last_symbol;
	int tx_stuff_count;
	buffer_ptr out;

	std::deque<buffer_ptr> out_queue_;
	boost::mutex out_queue_mutex_;
	std::size_t out_queue_ptr_;
	ptt_ptr ptt_;
};

} /* namespace extmodem */
#endif /* ENCODER_AF1200STJ_H_ */
