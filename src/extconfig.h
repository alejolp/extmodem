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

#ifndef CONFIG_H_
#define CONFIG_H_

#include <string>

#include <boost/program_options.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

namespace po = boost::program_options;

namespace extmodem {

class config : boost::noncopyable {
public:
	static config* Instance();

private:
	static boost::shared_ptr<config> config_instance_;

public:
	config();
	virtual ~config();

	void init(int argc, char** argv);
	bool is_help();
	po::variables_map& get_vm() { return vm; }

	int kiss_tcp_port() const { return kiss_tcp_port_; }
	const std::string& ptt_port() const { return ptt_port_; }
	int tx_delay() const { return tx_delay_; }
	int tx_tail() const { return tx_tail_; }
	bool debug() const { return debug_; }

private:
	po::options_description desc;
	po::variables_map vm;
	po::positional_options_description p_;

	int kiss_tcp_port_;
	std::string ptt_port_;
	int tx_delay_;
	int tx_tail_;
	bool debug_;
};

} /* namespace extmodem */
#endif /* CONFIG_H_ */
