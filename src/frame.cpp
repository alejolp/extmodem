/*
 * frame.cpp
 *
 *  Created on: 23/08/2013
 *      Author: alejo
 */

#include "frame.h"
#include "hdlc.h"

namespace extmodem {

frame::frame(const unsigned char* buffer, std::size_t length, unsigned int crc) : data_(buffer, buffer + length), crc_(crc) {

}

frame::~frame() {

}

void frame::print() {
	ax25_print_packet(data_.data(), data_.size(), "AF1200");
}


} /* namespace extmodem */
