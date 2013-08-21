/*
 * tcpserver.h
 *
 *  Created on: 21/08/2013
 *      Author: alejo
 */

#ifndef TCPSERVER_H_
#define TCPSERVER_H_

#include <deque>
#include <set>

#include <boost/bind.hpp>
#include <boost/asio.hpp>

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

private:
	boost::asio::io_service io_service_;

	kiss_server kiss_srv_;
};

} /* namespace extmodem */
#endif /* TCPSERVER_H_ */
