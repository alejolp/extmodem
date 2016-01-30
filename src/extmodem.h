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


#ifndef EXTMODEM_H_
#define EXTMODEM_H_

#include <vector>
#include <boost/shared_ptr.hpp>

#include "audiosource.h"
#include "tcpserver.h"
#include "frame.h"


namespace extmodem {

class decoder;
typedef boost::shared_ptr<decoder> decoder_ptr;

class encoder;
typedef boost::shared_ptr<encoder> encoder_ptr;

//class audiosource;
typedef boost::shared_ptr<audiosource> audiosource_ptr;

class agc {
public:
	agc();
	virtual ~agc();

	void init(size_t samples_history);
	void sample(float f);
	void update();

	/* average value */
	inline float avg() const { return sum_ / samples_history_; }

	/* variance */
	inline float sigma() const { return sigma1_; }

	/* standard deviation */
	inline float stddev() const { return (sigma() > 0) ? std::sqrt(sigma()) : 0; }

	float sum_;
	float min_;
	float max_;
	float sigma1_;

private:
	size_t samples_history_;
	size_t p_;
	std::vector<float> history_;
};

class modem : public audiosourcelistener {
public:
	modem();
	virtual ~modem();

	void set_audiosource(audiosource_ptr p);
	audiosource* get_audiosource() { return audio_source_.get(); }
	void add_decoder(decoder_ptr p, int ch_num);

	void set_encoder(encoder_ptr p);
	encoder_ptr get_encoder() { return encoder_; }

	virtual void input_callback(audiosource* a, const float* input, unsigned long frameCount);
	virtual void output_callback(audiosource* a, float* buffer, unsigned long length);

	void start_and_run();

	/** Handle a new packet that comes from the sound card.
	 *
	 * @param fp
	 */
	void dispatch_packet(frame_ptr fp);

	/** Send a new packet to the sound card.
	 *
	 * @param fp
	 */
	void output_packet_to_sc(frame_ptr fp);

private:
	audiosource_ptr audio_source_;

	/** decoders_ is a vector of vectors, the outer vector is indexed b channel, the inner vector is indexed by decoder. */
	std::vector<std::vector<decoder_ptr> > decoders_;

	encoder_ptr encoder_;

private:
	std::vector<float> tmpdata;
	std::vector<agc> agcs_;
	tcpserver tcpserver_;
	unsigned int last_packet_crc_{0};
};

} /* namespace extmodem */
#endif /* EXTMODEM_H_ */
