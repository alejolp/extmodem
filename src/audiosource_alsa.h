/*
 * audiosource_alsa.h
 *
 *  Created on: 16/11/2013
 *      Author: alejo
 */

#ifndef AUDIOSOURCE_ALSA_H_
#define AUDIOSOURCE_ALSA_H_

#include <alsa/asoundlib.h>

#include "audiosource.h"

namespace extmodem {

class audiosource_alsa : public audiosource {
public:
	explicit audiosource_alsa(int sample_rate);
	virtual ~audiosource_alsa();

	virtual void loop();

private:
	void init();
	void close();

private:
	snd_pcm_t *p_handle_;
	snd_pcm_t *c_handle_;
};

} /* namespace extmodem */
#endif /* AUDIOSOURCE_ALSA_H_ */
