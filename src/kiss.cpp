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

namespace extmodem {

void kiss_encode(const unsigned char* buffer, std::size_t length, std::vector<char> *dst) {
	dst->reserve(length+2);
	dst->push_back(KISS_FEND);

	for (std::size_t k = 0; k < length; ++k) {
		if (buffer[k] == KISS_FEND) {
			/* If a FEND ever appears in the data, it is translated into
			 * the two byte sequence FESC TFEND */
			dst->push_back(KISS_FESC);
			dst->push_back(KISS_TFEND);
		} else if (buffer[k] == KISS_FESC) {
			/* Likewise, if the FESC character ever appears in the user
			 * data, it is replaced with the two character sequence
			 * FESC TFESC (Frame Escape, Transposed Frame Escape). */
			dst->push_back(KISS_FESC);
			dst->push_back(KISS_TFESC);
		} else {
			dst->push_back(buffer[k]);
		}
	}

	dst->push_back(KISS_FEND);
}


}

