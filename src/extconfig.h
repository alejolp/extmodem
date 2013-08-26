/*
 * config.h
 *
 *  Created on: 26/08/2013
 *      Author: alejo
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

private:
	po::options_description desc;
	po::variables_map vm;
	po::positional_options_description p_;

	int kiss_tcp_port_;
	std::string ptt_port_;
	int tx_delay_;
	int tx_tail_;
};

} /* namespace extmodem */
#endif /* CONFIG_H_ */
