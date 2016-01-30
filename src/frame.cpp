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

#include "frame.h"
#include "hdlc.h"

namespace extmodem {

frame::frame(const unsigned char* buffer, std::size_t length)
 : data_(buffer, buffer + length), crc_(calc_crc_ccitt(buffer, length)) {

}

frame::frame(const unsigned char* buffer, std::size_t length, unsigned int crc)
 : data_(buffer, buffer + length), crc_(crc) {

}

frame::~frame() {

}

void frame::print() {
	ax25_print_packet(data_.data(), data_.size(), "AF1200", 0);
}

void frame::print(const char* name) {
    ax25_print_packet(data_.data(), data_.size(), name, 0);
}


} /* namespace extmodem */
