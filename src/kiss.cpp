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


#include "kiss.h"

/*
 * KISS specs:
 *  - http://www.ka9q.net/papers/kiss.html
 *  - http://www.ax25.net/kiss.aspx
 */

namespace extmodem {

void kiss_encode(const unsigned char* buffer, std::size_t length, std::vector<unsigned char> *dst) {
	unsigned char frame_type;

	dst->reserve(length+2);
	dst->push_back((char)KISS_FEND);

	/*  the first byte of each asynchronous frame between host and TNC is a "type" indicator */
	/*
		Command        Function         Comments
		   0           Data frame       The  rest  of the frame is data to
										be sent on the HDLC channel.
	 */
	frame_type = 0;
	dst->push_back(frame_type);

	for (std::size_t k = 0; k < length; ++k) {
		if (buffer[k] == KISS_FEND) {
			/* If a FEND ever appears in the data, it is translated into
			 * the two byte sequence FESC TFEND */
			dst->push_back((char)KISS_FESC);
			dst->push_back((char)KISS_TFEND);
		} else if (buffer[k] == KISS_FESC) {
			/* Likewise, if the FESC character ever appears in the user
			 * data, it is replaced with the two character sequence
			 * FESC TFESC (Frame Escape, Transposed Frame Escape). */
			dst->push_back((char)KISS_FESC);
			dst->push_back((char)KISS_TFESC);
		} else {
			dst->push_back(buffer[k]);
		}
	}

	dst->push_back((char)KISS_FEND);
}

int kiss_decode(const unsigned char* buffer, std::size_t length, std::vector<unsigned char>* dst) {
	if (length < 2 || buffer[0] != KISS_FEND || buffer[length-1] != KISS_FEND)
		return 0;

	for (std::size_t k = 1; k < length - 1; ++k) {
		if (buffer[k] == KISS_FESC) {
			if (buffer[k+1] == KISS_TFEND) {
				dst->push_back(KISS_FEND);
				++k;
			} else {
				return 0; // ERROR
			}
		} else if (buffer[k] == KISS_FESC) {
			if (buffer[k+1] == KISS_TFESC) {
				dst->push_back(KISS_FESC);
				++k;
			} else {
				return 0; // ERROR
			}
		} else {
			dst->push_back(buffer[k]);
		}
	}

	return 1; // OK
}

}

