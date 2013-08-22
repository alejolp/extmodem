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


namespace extmodem {

basic_asio_session::basic_asio_session(boost::asio::io_service& io_service, basic_asio_server* server)
: socket_(io_service), server_(server), waiting_write_(false), waiting_read_(false), closing_(false) { }

basic_asio_session::~basic_asio_session() {}

void basic_asio_session::start() {
	start_read();
	handle_connect();
}

void basic_asio_session::write(const char* buffer, std::size_t length) {
	out_data_queue_.push_back(std::vector<char>(buffer, buffer + length));
	start_write();
}

void basic_asio_session::start_read() {
	if (!waiting_read_) {
		waiting_read_ = true;
		socket_.async_read_some(boost::asio::buffer(in_data_, max_length),
				boost::bind(&basic_asio_session::handle_read, this,
						boost::asio::placeholders::error,
						boost::asio::placeholders::bytes_transferred));
	}
}

void basic_asio_session::start_write() {
	if (out_data_queue_.size() > 0) {
		if (!waiting_write_) {
			waiting_write_ = true;

			std::vector<char> &front = out_data_queue_.front();

			boost::asio::async_write(socket_,
					boost::asio::buffer(front.data(), front.size()),
					boost::bind(&basic_asio_session::handle_write, this,
							boost::asio::placeholders::error));
		}
	}
}

void basic_asio_session::handle_connect() {

}

void basic_asio_session::handle_close() {

}

void basic_asio_session::on_error() {
	if (!closing_) {
		closing_ = true;
		handle_close();
		server_->session_closed(this);
		socket_.close();
	}

	if (!waiting_read_ && !waiting_write_) {
		delete this;
	}
}

void basic_asio_session::handle_read(const boost::system::error_code& error, size_t bytes_transferred) {
	waiting_read_ = false;

	if (!error) {
		handle_incoming_data(in_data_, bytes_transferred);
		start_read();
	} else {
		on_error();
	}
}

void basic_asio_session::handle_write(const boost::system::error_code& error) {
	waiting_write_ = false;

	if (!error) {
		if (out_data_queue_.size() > 0) {
			out_data_queue_.pop_front();
		}
		start_write();
	} else {
		on_error();
	}
}

basic_asio_server::basic_asio_server(boost::asio::io_service& io_service, short port) :
		io_service_(io_service), acceptor_(io_service,
				boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port))
{ }

basic_asio_server::~basic_asio_server() {}

#if 0
basic_asio_session* basic_asio_server::new_session_instance(boost::asio::io_service& io_service_) {
	return 0; /* new basic_asio_session(io_service_); */
}
#endif

void basic_asio_server::start() {
	/* Do not call virtual methods from the constructor, they tend to blow up :P */
	start_accept();
}

void basic_asio_server::start_accept() {
	basic_asio_session* new_session = this->new_session_instance(io_service_);

	acceptor_.async_accept(new_session->socket(),
			boost::bind(&basic_asio_server::handle_accept, this, new_session,
					boost::asio::placeholders::error));
}

void basic_asio_server::handle_accept(basic_asio_session* new_session,
		const boost::system::error_code& error) {
	if (!error) {
		clients_.insert(new_session);
		new_session->start();
	} else {
		delete new_session;
	}

	start_accept();
}

void basic_asio_server::session_closed(basic_asio_session* s) {
	clients_.erase(s);
}

void basic_asio_server::write_to_all(const char* buffer, std::size_t length) {
	std::set<basic_asio_session*>::iterator it;

	for (it = clients_.begin(); it != clients_.end(); ++it) {
		(*it)->write(buffer, length);
	}
}


/**********************************************/
/* KISS */

void kiss_session::handle_incoming_data(const char* buffer, std::size_t length) {
	std::cout << "data: " << length << std::endl;
}

void kiss_session::handle_close() {
	std::cout << "close" << std::endl;
}

void kiss_session::handle_connect() {
	std::cout << "connect, hay clientes: " << get_server()->get_clients().size() << std::endl;
}

basic_asio_session* kiss_server::new_session_instance(boost::asio::io_service& io_service_) {
	return new kiss_session(io_service_, this);
}

/**********************************************/
/* tcpserver */

tcpserver::tcpserver() : kiss_srv_(io_service_, 6666) {

}

tcpserver::~tcpserver() {

}

void tcpserver::run() {
	try {
		kiss_srv_.start();
		io_service_.run();
	} catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}
}

void tcpserver::flush_output_queue() {
	boost::lock_guard<boost::mutex> guard_(output_queue_mutex_);

	while (!output_queue_.empty()) {
		std::vector<unsigned char> &data = output_queue_.front();
		std::vector<char> kiss_data;
		kiss_encode(data.data(), data.size(), &kiss_data);
		kiss_srv_.write_to_all(kiss_data.data(), kiss_data.size());
		output_queue_.pop_front();
	}
}

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



} /* namespace extmodem */
