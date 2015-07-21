/*
 * audiosource_alsa.h
 *
 *  Created on: 16/11/2013
 *      Author: alejo
 */

#ifndef AUDIOSOURCE_ALSA_H_
#define AUDIOSOURCE_ALSA_H_

#ifdef ALSA_FOUND

#include <alsa/asoundlib.h>

#include <boost/thread.hpp>

#include "audiosource.h"

namespace extmodem {

class audiosource_alsa : public audiosource {
public:
	explicit audiosource_alsa(int sample_rate);
	virtual ~audiosource_alsa();

	virtual void loop_async();

private:

	void loop_async_thread_proc();

	void init();
	void close();

private:
	snd_pcm_t *p_handle_;
	snd_pcm_t *c_handle_;
	boost::thread thread_;
};

} /* namespace extmodem */

#endif

#endif /* AUDIOSOURCE_ALSA_H_ */
