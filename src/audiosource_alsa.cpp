/*
 * audiosource_alsa.cpp
 *
 *  Created on: 16/11/2013
 *      Author: alejo
 */

#include <cmath>
#include <cstdio>
#include <iostream>
#include <exception>
#include <vector>

#include <boost/bind.hpp>

#include "audiosource_alsa.h"
#include "extconfig.h"

namespace extmodem {

audiosource_alsa::audiosource_alsa(int sample_rate) : audiosource(sample_rate), p_handle_(0), c_handle_(0) {
	init();
}

audiosource_alsa::~audiosource_alsa() {
	close();
}

void audiosource_alsa::init() {
    int err;
    std::string device = config::Instance()->alsa_device();

	if ((err = snd_pcm_open(&p_handle_, device.c_str(), SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
		std::cerr << "snd_pcm_open playback error: " << snd_strerror(err) << std::endl;
		close();
		throw audiosourceexception("snd_pcm_open");
	}

	if ((err = snd_pcm_set_params(p_handle_,
			SND_PCM_FORMAT_S16,
			SND_PCM_ACCESS_RW_INTERLEAVED,
			get_out_channel_count(),
			get_sample_rate(),
			1, /* FIXME: resample? */
			500000)) < 0) { /* 0.5sec */
		std::cerr << "snd_pcm_set_params playback error: " << snd_strerror(err) << " ch:" << get_out_channel_count() << " rate: " << get_sample_rate() << std::endl;
		close();
		throw audiosourceexception("snd_pcm_set_params");
	}

	if ((err = snd_pcm_open(&c_handle_, device.c_str(), SND_PCM_STREAM_CAPTURE, 0)) < 0) {
		std::cerr << "snd_pcm_open capture error: " << snd_strerror(err) << std::endl;
		close();
		throw audiosourceexception("snd_pcm_open");
	}

	if ((err = snd_pcm_set_params(c_handle_,
			SND_PCM_FORMAT_S16,
			SND_PCM_ACCESS_RW_INTERLEAVED,
			get_in_channel_count(),
			get_sample_rate(),
			1, /* FIXME: resample? */
			500000)) < 0) { /* 0.5sec */
		std::cerr << "snd_pcm_set_params capture error: " << snd_strerror(err) << " ch:" << get_in_channel_count() << " rate: " << get_sample_rate() << std::endl;
		close();
		throw audiosourceexception("snd_pcm_set_params");
	}

}

void audiosource_alsa::close() {
	if (p_handle_) {
		snd_pcm_close(p_handle_);
		p_handle_ = 0;
	}
	if (c_handle_) {
		snd_pcm_close(c_handle_);
		c_handle_ = 0;
	}
}

void audiosource_alsa::loop_async() {
	thread_ = boost::thread(boost::bind(&audiosource_alsa::loop_async_thread_proc, this));
}

void audiosource_alsa::loop_async_thread_proc() {
	std::vector<short> buffer;
	std::vector<float> bufferf;
	snd_pcm_sframes_t frames;
	int ret;

	buffer.resize(config::Instance()->frames_per_buff());
	bufferf.resize(config::Instance()->frames_per_buff());

	if ((ret = snd_pcm_prepare(c_handle_)) < 0) {
		std::cerr << "snd_pcm_prepare capture error: " << snd_strerror(ret) << std::endl;
		close();
		return;
	}

	if ((ret = snd_pcm_prepare(p_handle_)) < 0) {
		std::cerr << "snd_pcm_prepare playback error: " << snd_strerror(ret) << std::endl;
		close();
		return;
	}

	for (;;) {
		/* Read data from the soundcard */
		frames = snd_pcm_readi(c_handle_, buffer.data(), buffer.size());

		if (frames == -EPIPE) {
			std::cerr << "snd_pcm_readi EPIPE OVERRUN error: " << snd_strerror(frames) << std::endl;
			snd_pcm_prepare(c_handle_);
			frames = 0;
		} else if (frames < 0) {
			frames = snd_pcm_recover(c_handle_, frames, 0);
		}

		if (frames < 0) {
			std::cerr << "snd_pcm_readi error: " << snd_strerror(frames) << std::endl;
		}

		if (frames > 0 && get_listener()) {
			for (int i = 0; i < frames; ++i) {
				bufferf[i] = buffer[i] * 1.0 / 32768.0f;
			}
			get_listener()->input_callback(this, bufferf.data(), frames);
		}

		/* Write data to the soundcard */
		get_listener()->output_callback(this, bufferf.data(), bufferf.size());

		for (size_t i = 0 ; i < bufferf.size(); ++i) {
			buffer[i] = static_cast<short>( bufferf[i] * 32767 );
		}

        frames = snd_pcm_writei(p_handle_, buffer.data(), buffer.size());
        if (frames < 0) {
                frames = snd_pcm_recover(p_handle_, frames, 0);
        }
        if (frames < 0) {
    			std::cerr << "snd_pcm_writei error: " << snd_strerror(frames) << std::endl;
        }
        if (frames > 0 && frames < (snd_pcm_sframes_t)buffer.size()) {
			std::cerr << "ALSA short write, expected " << buffer.size() << " wrote " << frames << std::endl;
        }
	}

	close();
}

} /* namespace extmodem */
