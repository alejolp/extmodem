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


#ifndef AUDIOSOURCE_H_
#define AUDIOSOURCE_H_

#include <exception>
#include <string>

#include <boost/noncopyable.hpp>

#include "extexception.h"


namespace extmodem {

class audiosource;

class audiosourcelistener {
public:
	virtual void input_callback(audiosource* a, const float* input, unsigned long frameCount) = 0;
	virtual void output_callback(audiosource* a, float* buffer, unsigned long length) = 0;
};

class audiosource: private boost::noncopyable {
public:
	explicit audiosource(int sample_rate);
	virtual ~audiosource();

	virtual void loop_async() = 0;

	virtual void list_devices();
	
	void set_listener(audiosourcelistener* p) { listener_ = p; }
	audiosourcelistener* get_listener() const { return listener_; }

	int get_sample_rate() const { return sample_rate_; }
	int get_in_channel_count();
	int get_out_channel_count();

private:
	int sample_rate_;
	audiosourcelistener* listener_;
};

} /* namespace extmodem */
#endif /* AUDIOSOURCE_H_ */
