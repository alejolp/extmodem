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
 
#include "audiosource.h"

#include "extconfig.h"


/* FIXME */

namespace extmodem {

audiosource::audiosource(int sample_rate) : sample_rate_(sample_rate), listener_(0) {}

audiosource::~audiosource() {}

int audiosource::get_in_channel_count() {
	return config::Instance()->in_chan_count();
}

int audiosource::get_out_channel_count() {
	return config::Instance()->out_chan_count();
}

void audiosource::list_devices() {
	std::cerr << "UNIMPLEMENTED" << std::endl;
}

} /* namespace extmodem */
