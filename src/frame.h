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

#ifndef FRAME_H_
#define FRAME_H_

#include <vector>
#include <cstdlib>
#include <boost/smart_ptr.hpp>


namespace extmodem {

class frame;
typedef boost::shared_ptr<frame> frame_ptr;

class frame {
public:
	/**  Constructs a new frame without the CRC at the end of the buffer.
	 *
	 * @param buffer
	 * @param length
	 * @param crc
	 */
	explicit frame() : crc_(0) {};
	explicit frame(const unsigned char* buffer, std::size_t length, unsigned int crc);
	virtual ~frame();

	std::vector<unsigned char>& get_data() { return data_; }
	unsigned int get_crc() const { return crc_; }

	void print();

private:
	std::vector<unsigned char> data_;
	unsigned int crc_;
};

} /* namespace extmodem */
#endif /* FRAME_H_ */
