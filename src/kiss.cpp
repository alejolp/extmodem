/*
 * kiss.cpp
 *
 *  Created on: 21/08/2013
 *      Author: alejo
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

