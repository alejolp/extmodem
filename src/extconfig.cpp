/*
 * config.cpp
 *
 *  Created on: 26/08/2013
 *      Author: alejo
 */

#include <iostream>
#include <fstream>

#include "extconfig.h"

namespace extmodem {

boost::shared_ptr<config> config::config_instance_;

config* config::Instance() {
	if (!config_instance_.get())
		config_instance_.reset(new config());
	return config_instance_.get();
}

config::config() : desc("Allowed options") {}

config::~config() {}

bool config::is_help() {
	if (vm.count("help")) {
		std::cout << desc << std::endl;
		return true;
	}
	return false;
}

void config::init(int argc, char** argv) {
	desc.add_options()
	    ("help,h", "produce help message")
	    ("config-file", po::value<std::string>(), "configuration file name")
	    ("kiss-tcp-port", po::value<int>(&kiss_tcp_port_)->default_value(6666), "set KISS TCP listening port")
	    ("ptt-port", po::value<std::string>(&ptt_port_)->default_value("/dev/ttyS0"), "set serial port PTT name")
	    ("tx-delay", po::value<int>(&tx_delay_)->default_value(200), "set tx-delay in ms")
	    ("tx-tail", po::value<int>(&tx_tail_)->default_value(50), "set tx-tail in ms")
	;

	p_.add("config-file", -1);

	po::store(po::command_line_parser(argc, argv).options(desc).positional(p_).run(), vm);
	po::notify(vm);

	if (vm.count("config-file")) {
		std::ifstream input_file(vm["config-file"].as<std::string>().c_str());
		if (input_file) {
			po::store(po::parse_config_file(input_file, desc), vm);
			po::notify(vm);
		} else {
			throw std::exception(); // FIXME
		}
	}
}


} /* namespace extmodem */
