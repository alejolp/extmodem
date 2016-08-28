/*
 * audiosource_alsa.cpp
 *
 *  Created on: 16/11/2013
 *      Author: alejo
 */

#ifdef ALSA_FOUND

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
 		std::string in_device = config::Instance()->alsa_in_device();
 		std::string out_device = config::Instance()->alsa_out_device();

 		if ((err = snd_pcm_open(&p_handle_, out_device.c_str(), SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
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

 		snd_pcm_hw_params_t *hw_params;

 		if ((err = snd_pcm_open(&c_handle_, in_device.c_str(), SND_PCM_STREAM_CAPTURE, 0)) < 0) {
 			std::cerr << "snd_pcm_open capture error: " << snd_strerror(err) << std::endl;
 			close();
 			throw audiosourceexception("snd_pcm_open");
 		}

#if 0
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
#endif


	if ((err = snd_pcm_hw_params_malloc (&hw_params)) < 0) {
		std::cerr << "cannot allocate hardware parameter structure " << snd_strerror (err) << std::endl;
		throw audiosourceexception("snd_pcm_hw_params_malloc");
	}

	if ((err = snd_pcm_hw_params_any (c_handle_, hw_params)) < 0) {
		std::cerr << "cannot initialize hardware parameter structure " << snd_strerror (err) << std::endl;
		throw audiosourceexception("snd_pcm_hw_params_any");
	}

	if ((err = snd_pcm_hw_params_set_access (c_handle_, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
		std::cerr << "alsa error " << snd_strerror (err) << std::endl;
		throw audiosourceexception("snd_pcm_hw_params_set_access");
	}

	if ((err = snd_pcm_hw_params_set_format (c_handle_, hw_params, SND_PCM_FORMAT_S16)) < 0) {
		std::cerr << "alsa error " << snd_strerror (err) << std::endl;
		throw audiosourceexception("snd_pcm_hw_params_set_format");
	}

	unsigned int rate = get_sample_rate();

	if ((err = snd_pcm_hw_params_set_rate_near (c_handle_, hw_params, &rate, 0)) < 0) {
		std::cerr << "alsa error " << snd_strerror (err) << std::endl;
		throw audiosourceexception("snd_pcm_hw_params_set_rate_near");
	}

	if ((int)rate != get_sample_rate()) {
		std::cerr << "requested rate " << get_sample_rate() << " not available got " << rate << std::endl;
		throw audiosourceexception("error");
	}

	if ((err = snd_pcm_hw_params_set_channels (c_handle_, hw_params, get_in_channel_count())) < 0) {
		std::cerr << "alsa error " << snd_strerror (err) << std::endl;
		throw audiosourceexception("snd_pcm_hw_params_set_channels");
	}

	if ((err = snd_pcm_hw_params (c_handle_, hw_params)) < 0) {
		std::cerr << "alsa error " << snd_strerror (err) << std::endl;
		throw audiosourceexception("snd_pcm_hw_params");
	}

	snd_pcm_hw_params_free (hw_params);
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
	thread_play_ = boost::thread(boost::bind(&audiosource_alsa::loop_async_thread_play, this));
	thread_rec_ = boost::thread(boost::bind(&audiosource_alsa::loop_async_thread_rec, this));
}

void audiosource_alsa::loop_async_thread_rec() {
	config* cfg = config::Instance();
	std::vector<short> buffer;
	std::vector<float> bufferf;
	snd_pcm_sframes_t frames;
	int ret;
	int buf_size = (cfg->frames_per_buff() > 0) ? cfg->frames_per_buff() : (get_sample_rate());
	bool restart = true;

	buffer.resize(buf_size);
	bufferf.resize(buf_size);

	if ((ret = snd_pcm_prepare(c_handle_)) < 0) {
		std::cerr << "snd_pcm_prepare capture error: " << snd_strerror(ret) << std::endl;
		close();
		return;
	}

	/* FIXME Break the loop */

	for (;;) {
		if (restart) {
			if (cfg->debug()) {
				std::cerr << "ALSA restart" << std::endl;
			}
			restart = false;
			snd_pcm_drop(c_handle_);
			snd_pcm_prepare(c_handle_);
		}

		/* Read data from the soundcard */
		for (;;) {
			frames = snd_pcm_readi(c_handle_, buffer.data(), buffer.size());
			if (frames == -EAGAIN) {
				if (cfg->debug()) {
					std::cerr << "ALSA EAGAIN" << std::endl;
				}
				continue;
			} else {
				if (frames < 0) {
					if (cfg->debug()) {
						std::cerr << "ALSA snd_pcm_readi < 0 = " << frames << std::endl;
					}
					restart = true;
				}
				break;
			}
		}

		if (restart) {
			continue;
		}

		if (cfg->debug() && frames != (snd_pcm_sframes_t)buffer.size()) {
			std::cerr << "ALSA short read, expected " << buffer.size() << " wrote " << frames << std::endl;
		}

		for (int i = 0; i < frames; ++i) {
			bufferf[i] = buffer[i] * 1.0 / 32768.0f;
		}

		get_listener()->input_callback(this, bufferf.data(), frames);
	}
}

void audiosource_alsa::loop_async_thread_play() {
	config* cfg = config::Instance();
	std::vector<short> buffer;
	std::vector<float> bufferf;
	snd_pcm_sframes_t frames;
	int ret;
	int buf_size = (cfg->frames_per_buff() > 0) ? cfg->frames_per_buff() : (get_sample_rate());
	bool restart = true;

	buffer.resize(buf_size);
	bufferf.resize(buf_size);

	if ((ret = snd_pcm_prepare(p_handle_)) < 0) {
		std::cerr << "snd_pcm_prepare playback error: " << snd_strerror(ret) << std::endl;
		close();
		return;
	}

	/* FIXME Break the loop */

	for (;;) {
		if (restart) {
			if (cfg->debug()) {
				std::cerr << "ALSA restart" << std::endl;
			}
			restart = false;
			snd_pcm_drop(c_handle_);
			snd_pcm_prepare(c_handle_);
		}

		/*
		for (size_t i = 0 ; i < bufferf.size(); ++i) {
			bufferf[i] = 0;
		}
		*/

		/* Write data to the soundcard */
		get_listener()->output_callback(this, bufferf.data(), bufferf.size());

		for (size_t i = 0 ; i < bufferf.size(); ++i) {
			buffer[i] = static_cast<short>( bufferf[i] * 32767 );
		}

		for (;;) {
			frames = snd_pcm_writei(p_handle_, buffer.data(), buffer.size());
			if (frames == -EAGAIN) {
				if (cfg->debug()) {
					std::cerr << "ALSA EAGAIN" << std::endl;
				}
				continue;
			} else {
				if (frames < 0) {
					if (cfg->debug()) {
						std::cerr << "ALSA snd_pcm_readi < 0 = " << frames << std::endl;
					}
					restart = true;
				}
				break;
			}
		}

		if (restart) {
			continue;
		}

		if (frames != (snd_pcm_sframes_t)buffer.size()) {
			if (cfg->debug()) {
				std::cerr << "ALSA short write, expected " << buffer.size() << " wrote " << frames << std::endl;
			}
		}
	}

	close();
}

void audiosource_alsa::list_devices() {
	std::cerr << "UNIMPLEMENTED. Use in the command line: " << std::endl;
	std::cerr << "  aplay -L # to list output devices" << std::endl;
	std::cerr << "  arecord -L # to list input devices" << std::endl;
}

} /* namespace extmodem */

#endif
