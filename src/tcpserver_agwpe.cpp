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
#include <boost/static_assert.hpp>

#include "tcpserver_agwpe.h"
#include "frame.h"
#include "extmodem.h"
#include "extconfig.h"

#ifdef _MSC_VER
#define EXTMODEM_VCPP_ALIGN(x) __declspec(align(x))
#else
#define EXTMODEM_VCPP_ALIGN(x)
#endif

#ifdef __GNUC__
#define EXTMODEM_GCC_ALIGN(x) __attribute__ ((__aligned__ (x)))
#else
#define EXTMODEM_GCC_ALIGN(x)
#endif

namespace extmodem {

/*
 * AGWPE Protocol reference is available here:
 *
 *  http://www.sv2agw.com/downloads/develop.zip
 * 
 */

EXTMODEM_VCPP_ALIGN(1) struct agwpe_tcp_frame {

	/** Header that is sent and received bit-by-bit.
	 *
	 */
	EXTMODEM_VCPP_ALIGN(1) struct {
		/* port:
		[0..n] the least significant value comes in the
		first byte while the most significant in the
		second. I.E. Port 2 would be expressed as 0x01 ([4])
		*/
		uint8_t port;
		uint8_t reserved1[3];

		/* dataKind:
		Is the frame code, reflects the purpose of the frame.
		The meaning of the DataKind DO VARY depending on
		whether the frame flows from the application to
		AGWPE or viceversa.
		*/
		uint8_t dataKind;
		uint8_t reserved2[1];

		/* pid:
		Frame PID, it’s usage is valid only under certain
		frames only. Should be 0x00 when not used.
		*/
		uint8_t pid;
		uint8_t reserved3[1];

		/* callFrom:
		CallSign FROM of the packet, in ASCII, using the
		format {CALLSIGN}-{SSID} (i.e. LU7DID-8)
		it is “null terminated” (it ends with 0x00). ([5])
		The field ALWAYS is 10 bytes long.
		It’s filled on packets where it has some meaning.
		*/
		char callFrom[10];

		/* callTo:
		CallSign TO of the packet, same as above.
		*/
		char callTo[10];

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
	} EXTMODEM_GCC_ALIGN(1) header;

	/** Data field that is sent and received bit-by-bit.
	 *
	 */
	EXTMODEM_VCPP_ALIGN(1) union {
		/* INPUT FRAMES */
		char raw_data[512];

		struct {
			char username[255];
			char password[255];
		} in_P_frame;

		/* OUTPUT FRAMES */

		struct {
			uint16_t major;
			uint16_t unsed1;
			uint16_t minor;
			uint16_t unsed2;
		} out_R_frame;
		struct {
			uint8_t status;
		} out_X_frame;
		struct {
			char data[512];
		} out_G_frame;
		struct {
			uint8_t baudRate;
			uint8_t trafficLevel;
			uint8_t txDelay;
			uint8_t txTail;
			uint8_t persist;
			uint8_t slotTime;
			uint8_t maxFrame;
			uint8_t connsCount;
			uint32_t bytesRX;
		} out_g_frame;

	} EXTMODEM_GCC_ALIGN(1) data;
} EXTMODEM_GCC_ALIGN(1);

/* Minimum AGWPE frame size. */
#define AGWPE_MIN_HEADER_SIZE (1+3+1+1+1+1+10+10+4+4)

BOOST_STATIC_ASSERT(sizeof(((agwpe_tcp_frame*)(0))->header) == AGWPE_MIN_HEADER_SIZE);

/** Decodes an AGWPE frame.
 *
 * @param data
 * @param len
 * @param frame
 * @return Returns -1 on error, 0 on insufficient data, else the read size of the new frame.
 */
int agwpe_decode_frame(const unsigned char* data, std::size_t len, agwpe_tcp_frame* frame) {
	if (len < sizeof(frame->header))
		return 0;

	const unsigned char* p = data;
	::memset(static_cast<void*>(frame), 0, sizeof(agwpe_tcp_frame));

	::memcpy(&(frame->header), data, sizeof(frame->header));

	p += sizeof(frame->header);

	switch (frame->header.dataKind) {
	case 'P': /* Application Login (‘P’ frame) */
		if (frame->header.dataLen != 512)
			return -1;
		return sizeof(frame->header) + 512;

	case 'X': /* Register CallSign (‘X’ frame) */
		if (frame->header.dataLen != 0)
			return -1;
		return (p - data);

	case 'x': /* Unregister CallSign (‘x’ frame) */
		if (frame->header.dataLen != 0)
			return -1;
		return (p - data);

	case 'G': /* Ask Port Information (‘G’ frame) */
		if (frame->header.dataLen != 0)
			return -1;
		return (p - data);

	case 'm': /* Enable Reception of Monitoring Frames (‘m’ frame) */
		if (frame->header.dataLen != 0)
			return -1;
		return (p - data);

	case 'R': /* AGWPE Version Info (‘R’ frame) */
		if (frame->header.dataLen != 0)
			return -1;
		return (p - data);

	case 'g': /* Ask Port Capabilities (‘g’ frame) */
		if (frame->header.dataLen != 0)
			return -1;
		return (p - data);

	case 'H': /* Callsign Heard on a Port (‘H’ frame) */
		if (frame->header.dataLen != 0)
			return -1;
		return (p - data);

	case 'y': /* Ask Outstanding frames waiting on a Port (‘y’ Frame) */
		if (frame->header.dataLen != 0)
			return -1;
		return (p - data);

	case 'Y': /* Ask Outstanding frames waiting for a connection (‘Y’ frame) */
		if (frame->header.dataLen != 0)
			return -1;
		return (p - data);

	case 'M': /* Send UNPROTO Information (‘M’ frame) */
		if (frame->header.dataLen >= sizeof(frame->data.raw_data))
			return -1;
		std::memcpy(&(frame->data), p, frame->header.dataLen);
		p += frame->header.dataLen;
		return (p - data);

	case 'C': /* Connect, Start an AX.25 Connection (‘C’ frame) */
		if (frame->header.dataLen != 0)
			return -1;
		return (p - data);

	case 'D': /* Send Connected Data (‘D’ frame) */
		if (frame->header.dataLen >= sizeof(frame->data.raw_data))
			return -1;
		std::memcpy(&(frame->data), p, frame->header.dataLen);
		p += frame->header.dataLen;
		return (p - data);

	case 'd': /* Disconnect, Terminate an AX.25 Connection (‘d’ frame) */
		if (frame->header.dataLen != 0)
			return -1;
		return (p - data);

	case 'v': /* Connect VIA, Start an AX.25 circuit thru digipeaters (‘v’ frame) */
		if (frame->header.dataLen >= sizeof(frame->data.raw_data))
			return -1;
		std::memcpy(&(frame->data), p, frame->header.dataLen);
		p += frame->header.dataLen;
		return (p - data);

	case 'V': /* Send UNPROTO VIA (‘V’ frame) */
		if (frame->header.dataLen >= sizeof(frame->data.raw_data))
			return -1;
		std::memcpy(&(frame->data), p, frame->header.dataLen);
		p += frame->header.dataLen;
		return (p - data);

	case 'c': /* Non-Standard Connections, Connection with PID (‘c’ frame) */
		if (frame->header.dataLen != 0)
			return -1;
		return (p - data);

	case 'K': /* Send Data in “raw” AX.25 format (‘K’ frame) */
		if (frame->header.dataLen >= sizeof(frame->data.raw_data))
			return -1;
		std::memcpy(&(frame->data), p, frame->header.dataLen);
		p += frame->header.dataLen;
		return (p - data);

	case 'k': /* Activate reception of Frames in “raw” format (‘k’ Frame) */
		if (frame->header.dataLen != 0)
			return -1;
		return (p - data);

	default:
		return -1;
	}
}

int agwpe_encode_frame(agwpe_tcp_frame* frame, std::vector<unsigned char>* out) {
	/* fill DataLen field */

	switch (frame->header.dataKind) {
	case 'R': /* Version Number (‘R’ frame) */
		frame->header.dataLen = 8;
		break;

	case 'X': /* Callsign Registration (‘X’ Frame) */
		frame->header.dataLen = 1;
		break;

	case 'G': /* Port Information (‘G’ Frame) */
		frame->header.dataLen = std::strlen(frame->data.out_G_frame.data) + 1;
		break;

	case 'g': /* Capabilities of a Port (‘g’ Frame) */
		frame->header.dataLen = 12;
		break;

	case 'y': /* Frames Outstanding on a Port (‘y’ Frame) */
		frame->header.dataLen = 4;
		break;

	case 'Y': /* Frames Outstanding on a Connection (‘Y’ Frame) */
		frame->header.dataLen = 4;
		break;

	case 'H': /* Heard Stations on a Port (‘H’ Frame) */
		frame->header.dataLen = 0; // FIXME
		break;

	case 'C': /* AX.25 Connection Received (‘C’ Frame)      */
		frame->header.dataLen = 0; // FIXME
		break;

	case 'D': /* Connected AX.25 Data (‘D’ Frame) */
		frame->header.dataLen = 0; // FIXME
		break;

	case 'I': /* Monitored Connected Information (‘I’ Frame) */
		frame->header.dataLen = 0; // FIXME
		break;

	case 'S': /* Monitored Supervisory Information (‘S’ Frames) */
		frame->header.dataLen = 0; // FIXME
		break;

	case 'U': /* Monitored Unproto Information (‘U’ Frames) */
		frame->header.dataLen = 0; // FIXME
		break;

	case 'T': /* Monitoring Own Information (‘T’ Frames) */
		frame->header.dataLen = 0; // FIXME
		break;

	case 'K': /* Monitored Information in Raw Format (‘K’ Frames) */
		/* dataLen should already contain the raw size */
		break;

	default:
		return -1;
	}

	const unsigned char* p;

	p = reinterpret_cast<const unsigned char*>(&(frame->header));
	out->insert(out->end(), p, p + sizeof(frame->header));

	p = reinterpret_cast<const unsigned char*>(&(frame->data));
	out->insert(out->end(), p, p + frame->header.dataLen);

	return sizeof(frame->header) + frame->header.dataLen;
}

int agwpe_decode_frame(const unsigned char* data, std::size_t len, agwpe_tcp_frame_ptr frame) {
	return agwpe_decode_frame(data, len, frame.get());
}

int agwpe_encode_frame(agwpe_tcp_frame_ptr frame, std::vector<unsigned char>* out) {
	return agwpe_encode_frame(frame.get(), out);
}

agwpe_session::agwpe_session(boost::asio::io_service& io_service, basic_asio_server* server)
 : basic_asio_session(io_service, server), want_raw_frames_(false)
{

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

	inbuff_.insert(inbuff_.end(), buffer, buffer + length);
	const unsigned char* p = inbuff_.data();
	int ret = 0;

	for (;;) {
		agwpe_tcp_frame_ptr new_frame(new agwpe_tcp_frame());
		ret = agwpe_decode_frame(p, length - (p - inbuff_.data()), new_frame);
		if (ret <= -1) {
			close();
			return;
		} else if (ret == 0) {
			break;
		} else {
			handle_agwpe_frame(new_frame);
			p += ret;
		}
	}

	inbuff_.erase(inbuff_.begin(), inbuff_.begin() + (p - inbuff_.data()));
}

void agwpe_session::handle_agwpe_frame(agwpe_tcp_frame_ptr new_agwpe_frame) {
	if (config::Instance()->debug())
			std::cout << "AGWPE new frame kind: " << new_agwpe_frame->header.dataKind << std::endl;

	switch (new_agwpe_frame->header.dataKind)
	{
	case 'R': /* AGWPE Version Info (‘R’ frame) */
	{
		agwpe_tcp_frame_ptr reply_frame(new agwpe_tcp_frame());
		std::vector<unsigned char> reply_out_bytes;
		std::memset(reply_frame.get(), 0, sizeof(agwpe_tcp_frame));
		reply_frame->header.dataKind = 'R';
		reply_frame->data.out_R_frame.major = 2000;
		reply_frame->data.out_R_frame.minor = 99;
		agwpe_encode_frame(reply_frame, &reply_out_bytes);
		write_raw(reply_out_bytes.data(), reply_out_bytes.size());
	}
	break;

	case 'G': /* Ask Port Information (‘G’ frame) */
	{
		agwpe_tcp_frame_ptr reply_frame(new agwpe_tcp_frame());
		std::vector<unsigned char> reply_out_bytes;
		std::memset(reply_frame.get(), 0, sizeof(agwpe_tcp_frame));
		reply_frame->header.dataKind = 'G';
		std::strcpy(reply_frame->data.out_G_frame.data, "1; Port 1 EXTMODEM;");
		agwpe_encode_frame(reply_frame, &reply_out_bytes);
		write_raw(reply_out_bytes.data(), reply_out_bytes.size());
	}
	break;

	case 'K': /* Send Data in “raw” AX.25 format (‘K’ frame) */
	{
		frame_ptr new_ax25_frame (
			new frame(reinterpret_cast<const unsigned char*>(new_agwpe_frame->data.raw_data + 1),
					new_agwpe_frame->header.dataLen - 1));

		std::cout << "AGWPE New local frame" << std::endl;
		new_ax25_frame->print();

		get_agwpe_server()->get_modem()->output_packet_to_sc(new_ax25_frame);
	}
	break;

	case 'k': /* Activate reception of Frames in “raw” format (‘k’ Frame) */
	{
		want_raw_frames_ = !want_raw_frames_;
	}
	break;

#if 0
	case 'P': /* Application Login (‘P’ frame) */
		break;

	case 'X': /* Register CallSign (‘X’ frame) */
		break;

	case 'm': /* Enable Reception of Monitoring Frames (‘m’ frame) */
		break;

#endif

	default:
		std::cerr << "AGWPE UNHANDLED PACKET type: " << new_agwpe_frame->header.dataKind << std::endl;
		break;
	}
}

agwpe_server* agwpe_session::get_agwpe_server() {
	return reinterpret_cast<agwpe_server*>(get_server());
}

void agwpe_session::write(frame_ptr fp) {
	if (want_raw_frames_) {
		agwpe_tcp_frame_ptr reply_frame(new agwpe_tcp_frame());
		std::vector<unsigned char> reply_out_bytes;
		std::memset(reply_frame.get(), 0, sizeof(agwpe_tcp_frame));
		reply_frame->header.dataKind = 'K';
		reply_frame->header.dataLen = fp->get_data().size() + 1;
		std::memcpy(reply_frame->data.raw_data + 1, fp->get_data().data(), fp->get_data().size());
		agwpe_encode_frame(reply_frame, &reply_out_bytes);
		write_raw(reply_out_bytes.data(), reply_out_bytes.size());
	}
}

basic_asio_session* agwpe_server::new_session_instance(boost::asio::io_service& io_service_) {
	return new agwpe_session(io_service_, this);
}



}

