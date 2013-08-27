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

#include <cstdlib>
#include <iostream>

#include "tcpserver.h"
#include "kiss.h"
#include "extmodem.h"
#include "extconfig.h"

#include "tcpserver_kiss.h"
#include "tcpserver_agwpe.h"


namespace extmodem {


/**********************************************/
/* tcpserver */

tcpserver::tcpserver(modem* em)
	: kiss_srv_(new kiss_server(io_service_, config::Instance()->kiss_tcp_port(), em)),
	  agwpe_srv_(new agwpe_server(io_service_, config::Instance()->agwpe_tcp_port(), em)){
	
}

tcpserver::~tcpserver() {

}

void tcpserver::run() {
	try {
		kiss_srv_->start();
		io_service_.run();
	} catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << "\n";
		throw;
	}
}

void tcpserver::flush_output_queue() {
	boost::lock_guard<boost::mutex> guard_(output_queue_mutex_);

	while (!output_queue_.empty()) {
		frame_ptr fp = output_queue_.front();
		std::vector<unsigned char> kiss_data;

		output_queue_.pop_front();

		kiss_encode(fp->get_data().data(), fp->get_data().size(), &kiss_data);
		kiss_srv_->write_to_all(kiss_data.data(), kiss_data.size());
	}
}

void tcpserver::write_to_all_safe(frame_ptr fp) {
	{
		boost::lock_guard<boost::mutex> guard_(output_queue_mutex_);
		output_queue_.push_back(fp);
	}

	io_service_.dispatch(boost::bind(&tcpserver::flush_output_queue, this));
}

/*
void tcpserver::write_to_all_safe(const unsigned char* buffer, std::size_t length) {
	write_to_all_safe(std::vector<unsigned char>(buffer, buffer + length));
}

void tcpserver::write_to_all_safe(const std::vector<unsigned char>& buffer) {
	{
		boost::lock_guard<boost::mutex> guard_(output_queue_mutex_);
		output_queue_.push_back(buffer);
	}

	io_service_.dispatch(boost::bind(&tcpserver::flush_output_queue, this));
}
*/


} /* namespace extmodem */
