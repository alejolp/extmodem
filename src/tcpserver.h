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
#include <set>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>


namespace extmodem {

class basic_asio_server;

class basic_asio_session {
public:
	explicit basic_asio_session(boost::asio::io_service& io_service, basic_asio_server* server);
	virtual ~basic_asio_session();

	boost::asio::ip::tcp::socket& socket() {
		return socket_;
	}

	basic_asio_server* get_server() { return server_; }

	void start();
	void write(const char* buffer, std::size_t length);

protected:
	virtual void handle_connect();
	virtual void handle_close();
	virtual void handle_incoming_data(const char* buffer, std::size_t length) = 0;

private:
	void start_read();
	void start_write();

	void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
	void handle_write(const boost::system::error_code& error);

	void on_error();

	boost::asio::ip::tcp::socket socket_;
	enum {
		max_length = 4096
	};
	basic_asio_server* server_;
	char in_data_[max_length];
	std::deque<std::vector<char> > out_data_queue_;
	bool waiting_write_;
	bool waiting_read_;
	bool closing_;
};

class basic_asio_server {
public:
	explicit basic_asio_server(boost::asio::io_service& io_service, short port);
	virtual ~basic_asio_server();

	void start();
	void session_closed(basic_asio_session* s);

	std::set<basic_asio_session*>& get_clients() { return clients_; }

	void write_to_all(const char* buffer, std::size_t length);

protected:
	virtual basic_asio_session* new_session_instance(boost::asio::io_service& io_service_) = 0;

private:
	void start_accept();
	void handle_accept(basic_asio_session* new_session, const boost::system::error_code& error);

	boost::asio::io_service& io_service_;
	boost::asio::ip::tcp::acceptor acceptor_;
	std::set<basic_asio_session*> clients_;
};

class kiss_session : public basic_asio_session {
public:
	explicit kiss_session(boost::asio::io_service& io_service, basic_asio_server* server) : basic_asio_session(io_service, server) {}
	virtual ~kiss_session() {}

protected:
	virtual void handle_connect();
	virtual void handle_close();
	virtual void handle_incoming_data(const char* buffer, std::size_t length);

};

class kiss_server : public basic_asio_server {
public:
	explicit kiss_server(boost::asio::io_service& io_service, short port) : basic_asio_server(io_service, port) {}
	virtual ~kiss_server() {}

protected:
	virtual basic_asio_session* new_session_instance(boost::asio::io_service& io_service_);

};


class tcpserver {
public:
	tcpserver();
	virtual ~tcpserver();

	void run();
	void write_to_all_safe(const unsigned char* buffer, std::size_t length);
	void write_to_all_safe(const std::vector<unsigned char>& buffer);

private:
	void flush_output_queue();

	boost::asio::io_service io_service_;

	kiss_server kiss_srv_;
	boost::mutex output_queue_mutex_;
	std::deque<std::vector<unsigned char> > output_queue_;
};

} /* namespace extmodem */
#endif /* TCPSERVER_H_ */
