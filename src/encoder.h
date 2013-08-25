/*
 * encoder.h
 *
 *  Created on: 23/08/2013
 *      Author: alejo
 */

#ifndef ENCODER_H_
#define ENCODER_H_

#include "frame.h"


namespace extmodem {

class audiosource;

class encoder {
public:
	encoder();
	virtual ~encoder();

	virtual void output_callback(audiosource* a, float* buffer, unsigned long length) = 0;
	virtual void init(audiosource* a) = 0;
	virtual void send(frame_ptr fp) = 0;
};

} /* namespace extmodem */
#endif /* ENCODER_H_ */
