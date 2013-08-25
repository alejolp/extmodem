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

class modem : public audiosourcelistener {
public:
	modem();
	virtual ~modem();

	void set_audiosource(audiosource_ptr p);
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
	tcpserver tcpserver_;
};

} /* namespace extmodem */
#endif /* EXTMODEM_H_ */
