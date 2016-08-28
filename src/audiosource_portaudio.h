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

#ifndef AUDIOSOURCE_PORTAUDIO_H_
#define AUDIOSOURCE_PORTAUDIO_H_

#ifdef PORTAUDIO_FOUND

#include <portaudio.h>
#include "audiosource.h"


namespace extmodem {

class audiosource_portaudio : public audiosource {
public:
	explicit audiosource_portaudio(int sample_rate);
	virtual ~audiosource_portaudio();

	virtual void loop_async();

	virtual void list_devices();
	
private:
	void init();
	void close();

private:
	PaStream *stream_in;
	PaStream *stream_out;
	PaError err;
};

} /* namespace extmodem */

#endif

#endif /* AUDIOSOURCE_PORTAUDIO_H_ */
