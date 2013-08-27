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
#include <cstring>

#include <boost/cstdint.hpp>

#include "tcpserver_agwpe.h"
#include "extconfig.h"

namespace extmodem {

/*
 * AGWPE Protocol reference is available here:
 *
 *  http://www.sv2agw.com/downloads/develop.zip
 * 
 */

struct agwpe_tcp_frame {
	/* port:
	[0..n] the least significant value comes in the 
	first byte while the most significant in the 
	second. I.E. Port 2 would be expressed as 0x01 ([4])
	*/
	uint8_t port;

	/* dataKind:
	Is the frame code, reflects the purpose of the frame. 
	The meaning of the DataKind DO VARY depending on 
	whether the frame flows from the application to 
	AGWPE or viceversa.
	*/
	uint8_t dataKind;

	/* pid:
	Frame PID, it’s usage is valid only under certain 
	frames only. Should be 0x00 when not used.
	*/
	uint8_t pid;

	/* callFrom:
	CallSign FROM of the packet, in ASCII, using the 
	format {CALLSIGN}-{SSID} (i.e. LU7DID-8)
	it is “null terminated” (it ends with 0x00). ([5])
	The field ALWAYS is 10 bytes long.
	It’s filled on packets where it has some meaning.
	*/
	char callFrom[11];

	/* callTo:
	CallSign TO of the packet, same as above.
	*/
	char callTo[11];

	/* dataLen:
	Data Length as a 32 bits unsigned integer.
	If zero means no data follows the header.
	*/
	uint32_t dataLen;

	/* userReserved:
	32 bits unsigned integer, not used. Reserved
	for future use.
	*/
	uint32_t userReserved;

	union {
		struct {
			char username[256];
			char password[256];
		} P_type;
	} data;
};

/* Minimum AGWPE frame size. DOES NOT contain "userReserved" field. */
#define AGWPE_MIN_HEADER_SIZE (1+3+1+1+1+1+10+10+4)

namespace {

int agwpe_decode_frame(const unsigned char* data, std::size_t len, agwpe_tcp_frame* frame) {
	if (len < AGWPE_MIN_HEADER_SIZE)
		return 0;

	const unsigned char* p = data;
	::memset(static_cast<void*>(frame), 0, sizeof(agwpe_tcp_frame));

	frame->port = *p;
	p += 4;

	frame->dataKind = *p;
	p += 2;

	frame->pid = *p;
	p += 2;

	std::strncpy(frame->callFrom, reinterpret_cast<const char*>(p), sizeof(frame->callFrom) - 1);
	p += 10;

	std::strncpy(frame->callTo, reinterpret_cast<const char*>(p), sizeof(frame->callTo) - 1);
	p += 10;

	frame->dataLen = *(reinterpret_cast<const uint32_t*>(p));
	//frame->userReserved = *(reinterpret_cast<const uint32_t*>(&data[26]));

	switch (frame->dataKind) {
	case 'P': /* Application Login (‘P’ frame) */
		if (frame->dataLen != 512)
			return -1;
		if ((len < AGWPE_MIN_HEADER_SIZE + 512))
			return 0;
		std::strncpy(frame->data.P_type.username, reinterpret_cast<const char*>(p), sizeof(frame->data.P_type.username) - 1);
		p += 255;
		std::strncpy(frame->data.P_type.password, reinterpret_cast<const char*>(p), sizeof(frame->data.P_type.password) - 1);
		p += 255;
		return (p - data);

	case 'X': /* Register CallSign (‘X’ frame) */
		return -1;

	case 'x': /* Unregister CallSign (‘x’ frame) */
		return -1;

	case 'G': /* Ask Port Information (‘G’ frame) */
		return -1;

	case 'm': /* Enable Reception of Monitoring Frames (‘m’ frame) */
		return -1;

	case 'R': /* AGWPE Version Info (‘R’ frame) */
		return -1;

	case 'g': /* Ask Port Capabilities (‘g’ frame) */
		return -1;

	case 'H': /* Callsign Heard on a Port (‘H’ frame) */
		return -1;

	case 'y': /* Ask Outstanding frames waiting on a Port (‘y’ Frame) */
		return -1;

	case 'Y': /* Ask Outstanding frames waiting for a connection (‘Y’ frame) */
		return -1;

	case 'M': /* Send UNPROTO Information (‘M’ frame) */
		return -1;

	case 'C': /* Connect, Start an AX.25 Connection (‘C’ frame) */
		return -1;

	case 'D': /* Send Connected Data (‘D’ frame) */
		return -1;

	case 'd': /* Disconnect, Terminate an AX.25 Connection (‘d’ frame) */
		return -1;

	case 'v': /* Connect VIA, Start an AX.25 circuit thru digipeaters (‘v’ frame) */
		return -1;

	case 'V': /* Send UNPROTO VIA (‘V’ frame) */
		return -1;

	case 'c': /* Non-Standard Connections, Connection with PID (‘c’ frame) */
		return -1;

	case 'K': /* Send Data in “raw” AX.25 format (‘K’ frame) */
		return -1;

	case 'k': /* Activate reception of Frames in “raw” format (‘k’ Frame) */
		return -1;

	default:
		return -1;
	}
}

}

void agwpe_session::handle_connect() {
	if (config::Instance()->debug())
		std::cout << "AGWPE New TCP connection, " << get_agwpe_server()->get_clients().size() << " total." << std::endl;
}

void agwpe_session::handle_close() {
	if (config::Instance()->debug())
		std::cout << "AGWPE Closed TCP connection." << std::endl;
}

void agwpe_session::handle_incoming_data(const unsigned char* buffer, std::size_t length) {
	if (config::Instance()->debug())
		std::cout << "AGWPE new data " << length << std::endl;
}

agwpe_server* agwpe_session::get_agwpe_server() {
	return 0;
}

basic_asio_session* agwpe_server::new_session_instance(boost::asio::io_service& io_service_) {
	return 0;
}



}

