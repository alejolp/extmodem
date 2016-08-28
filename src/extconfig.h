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

	const std::string audio_backend() const { return audio_backend_; }
	int kiss_tcp_port() const { return kiss_tcp_port_; }
	int agwpe_tcp_port() const { return agwpe_tcp_port_; }
	const std::string& ptt_port() const { return ptt_port_; }
	const std::string& ptt_mode() const { return ptt_mode_; }
	int tx_delay() const { return tx_delay_; }
	int tx_tail() const { return tx_tail_; }
	bool debug() const { return debug_; }
	bool list_devices() const { return listdevices_; }
	int sample_rate() const { return sample_rate_; }
	int in_chan_count() const { return in_channels_count_; }
	int out_chan_count() const { return out_channels_count_; }
	std::string alsa_in_device() const { return alsa_in_device_; }
	std::string alsa_out_device() const { return alsa_out_device_; }
	int frames_per_buff() const { return frames_per_buff_; }
	int hamlib_model() const { return hamlib_model_; }
	float audio_mult_factor() const { return audio_mult_factor_; }
	int enabled_channels() const { return enabled_channels_; }
	std::string wave_file_in() const { return wave_file_in_; }
	bool debugaudio() const { return debugaudio_; }
	int portaudio_input_device() const { return portaudio_input_device_; }
	int portaudio_output_device() const { return portaudio_output_device_; }

private:
	po::options_description desc;
	po::variables_map vm;
	po::positional_options_description p_;

	std::string audio_backend_;
	int kiss_tcp_port_{};
	int agwpe_tcp_port_{};
	std::string ptt_port_;
	std::string ptt_mode_;
	int tx_delay_{};
	int tx_tail_{};
	bool debug_{};
	bool listdevices_{};
	int sample_rate_{};
	int in_channels_count_{};
	int out_channels_count_{};
	std::string alsa_in_device_;
	std::string alsa_out_device_;
	int frames_per_buff_{};
	int hamlib_model_{};
	float audio_mult_factor_{};
	int enabled_channels_{};
	std::string wave_file_in_;
	bool debugaudio_{};
	int portaudio_input_device_{};
	int portaudio_output_device_{};
};

} /* namespace extmodem */
#endif /* CONFIG_H_ */
