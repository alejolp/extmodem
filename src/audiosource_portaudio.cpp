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

#ifdef PORTAUDIO_FOUND

#include <cmath>
#include <iostream>
#include <exception>

#include "audiosource_portaudio.h"
#include "extconfig.h"

namespace extmodem {


audiosource_portaudio::audiosource_portaudio(int sample_rate) : audiosource(sample_rate) {
	init();
}

audiosource_portaudio::~audiosource_portaudio() {
	close();
}

namespace {

	static int portaudio_in_callback(const void *input, void *output,
			unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags, void *userData)
	{
		(void)output;
		(void)timeInfo;
		(void)statusFlags;

		audiosource_portaudio* pa = static_cast<audiosource_portaudio*>(userData);

		if (pa->get_listener()) {
			const float* finput = static_cast<const float*>(input);
			pa->get_listener()->input_callback(pa, finput, frameCount);
		}
		return paContinue;
	}

	static int portaudio_out_callback(const void *input, void *output,
			unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo,
			PaStreamCallbackFlags statusFlags, void *userData)
	{
		(void)input;
		(void)timeInfo;
		(void)statusFlags;

		audiosource_portaudio* pa = static_cast<audiosource_portaudio*>(userData);

		// std::cerr << " out buffer size " << frameCount << " flags " << statusFlags << std::endl;

		if (pa->get_listener()) {
			float* foutput = static_cast<float*>(output);

			pa->get_listener()->output_callback(pa, foutput, frameCount * pa->get_out_channel_count());

#if 0
			int num_channels = pa->get_out_channel_count();
			int buffer_size = frameCount * num_channels;
			int ch_idx;
			static int dds_idx = 0;

			for (int i=0; i<buffer_size; ) {
				for (ch_idx=0; ch_idx<num_channels; ++ch_idx) {
					foutput[i] = std::sin((2.0*M_PI*440.0*(dds_idx%pa->get_sample_rate()))/pa->get_sample_rate());
					i++;
				}
				dds_idx++;
			}
#endif
		}

		return paContinue;
	}

}

void audiosource_portaudio::init() {
	err = Pa_Initialize();
	if( err != paNoError ) {
		std::cerr << "PortAudio error: " << Pa_GetErrorText( err ) << std::endl;
		throw audiosourceexception("Pa_Initialize");
	}

	//int frames_per_buffer = get_sample_rate();
	int frames_per_buffer = config::Instance()->frames_per_buff();

	/*
	 * We have two separate streams for input and output to work-around a Debian specific
	 * bug on PortAudio.
	 */

	PaStreamParameters input_params;
	PaStreamParameters output_params;

	input_params.device = (config::Instance()->portaudio_input_device() < 0) ? Pa_GetDefaultInputDevice() : config::Instance()->portaudio_input_device();
	input_params.channelCount = get_in_channel_count();
	input_params.sampleFormat = paFloat32;
	input_params.suggestedLatency = 1;
	input_params.hostApiSpecificStreamInfo = 0;

	output_params.device = (config::Instance()->portaudio_output_device() < 0) ? Pa_GetDefaultOutputDevice() : config::Instance()->portaudio_output_device();
	output_params.channelCount = get_out_channel_count();
	output_params.sampleFormat = paFloat32;
	output_params.suggestedLatency = 1;
	output_params.hostApiSpecificStreamInfo = 0;

	/* frames per buffer, i.e. the number
	   of sample frames that PortAudio will
	   request from the callback. Many apps
	   may want to use
	   paFramesPerBufferUnspecified, which
	   tells PortAudio to pick the best,
	   possibly changing, buffer size.*/

    /* Open an audio I/O stream. */
    err = Pa_OpenStream( &stream_in,
    							&input_params,
    							0,
                                get_sample_rate(),
                                frames_per_buffer, 
                                paNoFlag,
                                portaudio_in_callback,
                                static_cast<void*>(this) );

	if( err != paNoError ) {
		std::cerr << "PortAudio in error: " << Pa_GetErrorText( err ) << std::endl;
		throw audiosourceexception("Pa_OpenStream");
	}

	err = Pa_StartStream( stream_in );
	if( err != paNoError ) {
		std::cerr << "PortAudio in error: " << Pa_GetErrorText( err ) << std::endl;
		throw audiosourceexception("Pa_StartStream");
	}

    /* Open an audio I/O stream. */
    err = Pa_OpenStream( &stream_out,
    							0,
    							&output_params,
                                get_sample_rate(),
                                frames_per_buffer, 
                                paNoFlag,
                                portaudio_out_callback,
                                static_cast<void*>(this) );

	if( err != paNoError ) {
		std::cerr << "PortAudio out error: " << Pa_GetErrorText( err ) << std::endl;
		throw audiosourceexception("Pa_OpenStream");
	}

	err = Pa_StartStream( stream_out );
	if( err != paNoError ) {
		std::cerr << "PortAudio out error: " << Pa_GetErrorText( err ) << std::endl;
		throw audiosourceexception("Pa_StartStream");
	}
}

void audiosource_portaudio::close() {
	err = Pa_StopStream(stream_in);
	if (err != paNoError) {
		std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
		throw audiosourceexception("Pa_StopStream");
	}

	err = Pa_CloseStream(stream_in);
	if (err != paNoError) {
		std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
		throw audiosourceexception("Pa_CloseStream");
	}

	err = Pa_StopStream(stream_out);
	if (err != paNoError) {
		std::cerr << "PortAudio error: " << Pa_GetErrorText(err) << std::endl;
		throw audiosourceexception("Pa_StopStream");
	}

	err = Pa_CloseStream(stream_out);
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

void audiosource_portaudio::loop_async() {

}

void audiosource_portaudio::list_devices() {
	PaHostApiIndex api_idx;

	const PaHostApiInfo* info = Pa_GetHostApiInfo(Pa_GetDefaultHostApi());

	std::cerr << "Default Host API: " << Pa_GetDefaultHostApi() << "; " << info->name << std::endl;

	for (api_idx = 0; api_idx < Pa_GetHostApiCount(); ++api_idx) {
		info = Pa_GetHostApiInfo(api_idx);
		std::cerr << "   Host API " << api_idx << ": " << info->name << std::endl;
	}

	std::cerr << "PortAudio device_count=" << Pa_GetDeviceCount() << std::endl;
	std::cerr << "PortAudio DefaultInputDevice=" << Pa_GetDefaultInputDevice() << std::endl;
	std::cerr << "PortAudio DefaultOutputDevice=" << Pa_GetDefaultOutputDevice() << std::endl;

	for (PaDeviceIndex i = 0; i < Pa_GetDeviceCount(); ++i) {
		const PaDeviceInfo *devinfo = Pa_GetDeviceInfo(i);
		std::cerr << "Device " << i << ": " << devinfo->name << std::endl;
		std::cerr << "   MaxInCH=" << devinfo->maxInputChannels 
			<< " MaxOutCH=" << devinfo ->maxOutputChannels 
			<< " defaultSampleRate=" << devinfo ->defaultSampleRate << std::endl;
	}
}

} /* namespace extmodem */

#endif
