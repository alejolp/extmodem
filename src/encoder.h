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
