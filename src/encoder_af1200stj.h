/*
 * encoder_af1200stj.h
 *
 *  Created on: 23/08/2013
 *      Author: alejo
 */

#ifndef ENCODER_AF1200STJ_H_
#define ENCODER_AF1200STJ_H_

#include <vector>
#include <deque>
#include <cstdlib>

#include <boost/smart_ptr.hpp>

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
	std::size_t out_queue_ptr_;
	ptt_ptr ptt_;
};

} /* namespace extmodem */
#endif /* ENCODER_AF1200STJ_H_ */
