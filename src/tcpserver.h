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

#ifndef TCPSERVER_H_
#define TCPSERVER_H_

#include <deque>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>

#include "frame.h"


namespace extmodem {

class kiss_server;
class agwpe_server;
class modem;

class tcpserver {
public:
	tcpserver(modem* em);
	virtual ~tcpserver();

	void run();
	/*
	void write_to_all_safe(const unsigned char* buffer, std::size_t length);
	void write_to_all_safe(const std::vector<unsigned char>& buffer);
	 */
	void write_to_all_safe(frame_ptr fp);

private:
	void flush_output_queue();

	boost::asio::io_service io_service_;

	boost::shared_ptr<kiss_server> kiss_srv_;
	boost::shared_ptr<agwpe_server> agwpe_srv_;
	boost::mutex output_queue_mutex_;
	std::deque<frame_ptr> output_queue_;
};

} /* namespace extmodem */
#endif /* TCPSERVER_H_ */
