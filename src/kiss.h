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

#ifndef KISS_H_
#define KISS_H_

#include <vector>
#include <cstdlib>

/*
 * KISS specs:
 *  - http://www.ka9q.net/papers/kiss.html
 *  - http://www.ax25.net/kiss.aspx
 */

#define KISS_FEND   0xC0 /* Frame  End */
#define KISS_FESC   0xDB /* Frame  Escape */
#define KISS_TFEND  0xDC /* Transposed Frame End */
#define KISS_TFESC  0xDD /* Transposed Frame Escape */

namespace extmodem {

void kiss_encode(const unsigned char* buffer, std::size_t length, std::vector<char>* dst);

}

#endif /* KISS_H_ */
