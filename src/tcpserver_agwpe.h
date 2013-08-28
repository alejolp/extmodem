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

#ifndef TCPSERVER_AGWPE_H_
#define TCPSERVER_AGWPE_H_

#include "tcpserver_base.h"

namespace extmodem {

class agwpe_server;
class modem;

class agwpe_tcp_frame;
typedef boost::shared_ptr<agwpe_tcp_frame> agwpe_tcp_frame_ptr;

class agwpe_session : public basic_asio_session {
public:
	agwpe_session(boost::asio::io_service& io_service, basic_asio_server* server);
	virtual ~agwpe_session() {}

	virtual void write(frame_ptr fp);

protected:
	virtual void handle_connect();
	virtual void handle_close();
	virtual void handle_incoming_data(const unsigned char* buffer, std::size_t length);

	agwpe_server* get_agwpe_server();

private:
	void handle_agwpe_frame(agwpe_tcp_frame_ptr new_frame);
	std::vector<unsigned char> inbuff_;
	bool want_raw_frames_;
};

class agwpe_server : public basic_asio_server {
public:
	explicit agwpe_server(boost::asio::io_service& io_service, unsigned short port, modem* em) : basic_asio_server(io_service, port), em_(em) {}
	virtual ~agwpe_server() {}

	modem* get_modem() { return em_; }

protected:
	virtual basic_asio_session* new_session_instance(boost::asio::io_service& io_service_);

private:
	modem* em_;
};

}

#endif
