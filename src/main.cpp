/*
 * Sound card modem for Amateur Radio AX25.
 *
 * Copyright (C) Alejandro Santos LU4EXT, 2013, alejolp@gmail.com.
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


#include "audiosource_portaudio.h"
#include "audiosource_alsa.h"
#include "extmodem.h"
#include "extconfig.h"

#include "decoder_dtmf.h"
#include "decoder_af1200mm.h"
#include "decoder_af1200stj.h"
#include "encoder_af1200stj.h"

#include "frame.h"

#include <boost/smart_ptr.hpp>


using namespace extmodem;

#define SAMPLE_RATE 22050

int main(int argc, char **argv) {
	config* cfg = config::Instance();

	cfg->init(argc, argv);

	if (cfg->is_help()) {
		return 0;
	}

	int i;
	boost::shared_ptr<modem> em(new modem());
	boost::shared_ptr<audiosource> as;

	if (1) {
		as.reset(new audiosource_portaudio(SAMPLE_RATE));
	} else {
		as.reset(new audiosource_alsa(SAMPLE_RATE));
	}

	std::cerr << "Starting extmodem ... " << std::endl;

	em->set_audiosource(as);

	for (i = 0; i < 1 /* as->get_channel_count() */; ++i) {
		em->add_decoder(decoder_ptr(new decoder_dtmf(em.get())), i);
		em->add_decoder(decoder_ptr(new decoder_af1200mm(em.get())), i);
		em->add_decoder(decoder_ptr(new decoder_af1200stj(em.get())), i);
	}

	em->set_encoder(encoder_ptr(new encoder_af1200stj()));

	std::cerr << "Started!" << std::endl;

	em->start_and_run();
	as->loop();

	return 0;
}


