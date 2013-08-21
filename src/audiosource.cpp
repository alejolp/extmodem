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
#include <exception>
#include "audiosource.h"

/* FIXME */

namespace extmodem {

audiosource::audiosource(int sample_rate) : sample_rate_(sample_rate), listener_(0) {}

audiosource::~audiosource() {}

audiosource_portaudio::audiosource_portaudio(int sample_rate) : audiosource(sample_rate) {
	init();
}

audiosource_portaudio::~audiosource_portaudio() {}

namespace {

	static int portaudio_callback(const void *input, void *output,
			unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags, void *userData)
	{
		audiosource_portaudio* pa = static_cast<audiosource_portaudio*>(userData);
		if (pa->get_listener()) {
			const float* finput = static_cast<const float*>(input);
			pa->get_listener()->input_callback(pa, finput, frameCount);
		}
		return 0;
	}

}

void audiosource_portaudio::init() {
	err = Pa_Initialize();
	if( err != paNoError ) {
		std::cerr << "PortAudio error: " << Pa_GetErrorText( err ) << std::endl;
		throw audiosourceexception("Pa_Initialize");
	}

	PaHostApiIndex api_idx;

	const PaHostApiInfo* info = Pa_GetHostApiInfo(Pa_GetDefaultHostApi());

	std::cerr << "Default device: " << info->name << std::endl;

	for (api_idx = 0; api_idx < Pa_GetHostApiCount(); ++api_idx) {
		info = Pa_GetHostApiInfo(api_idx);
		std::cerr << "device " << api_idx << ": " << info->name << std::endl;
	}

	// For the "mm" decoder to work, the frames_per_buffer value should be
	// relatively high. "SAMPLE_RATE" seems OK. The downside is that having a
	// large buffer introduces a delay proportional to the sample rate.
	//

	int frames_per_buffer = get_sample_rate(); /* paFramesPerBufferUnspecified */

    /* Open an audio I/O stream. */
    err = Pa_OpenDefaultStream( &stream,
    							get_channel_count(),          /* input channels */
                                0,          /* output */
                                paFloat32,  /* 32 bit floating point output */
                                get_sample_rate(),
                                frames_per_buffer, /* frames per buffer, i.e. the number
                                                   of sample frames that PortAudio will
                                                   request from the callback. Many apps
                                                   may want to use
                                                   paFramesPerBufferUnspecified, which
                                                   tells PortAudio to pick the best,
                                                   possibly changing, buffer size.*/
                                portaudio_callback, /* this is your callback function */
                                static_cast<void*>(this) ); /*This is a pointer that will be passed to
                                                   your callback*/
	if( err != paNoError ) {
		std::cerr << "PortAudio error: " << Pa_GetErrorText( err ) << std::endl;
		throw audiosourceexception("Pa_OpenDefaultStream");
	}

	err = Pa_StartStream( stream );
	if( err != paNoError ) {
		std::cerr << "PortAudio error: " << Pa_GetErrorText( err ) << std::endl;
		throw audiosourceexception("Pa_StartStream");
	}
}

void audiosource_portaudio::close() {
	err = Pa_StopStream(stream);
	if (err != paNoError) {
		std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
		throw audiosourceexception("Pa_StopStream");
	}

	err = Pa_CloseStream(stream);
	if (err != paNoError) {
		std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
		throw audiosourceexception("Pa_CloseStream");
	}

	err = Pa_Terminate();
	if (err != paNoError) {
		std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
		throw audiosourceexception("Pa_Terminate");
	}
}


} /* namespace extmodem */
