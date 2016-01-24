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

#include "tcpserver_kiss.h"
#include "extconfig.h"

#include "kiss.h"
#include "frame.h"
#include "extmodem.h"


namespace extmodem {

/**********************************************/
/* KISS */

void kiss_session::handle_incoming_data(const unsigned char* buffer, std::size_t length) {
	if (config::Instance()->debug())
		std::cout << "data: " << length << std::endl;

	inbuff_.insert(inbuff_.end(), buffer, buffer + length);

	unsigned int i = 0, start, end, last_good_pos = -1;

	/*
	 * A KISS frame begins and ends with a KISS_FEND, not having any KISS_FEND in the middle.
	 */

	while (i < inbuff_.size()) {
		for (; i < inbuff_.size(); ++i)
			if (inbuff_[i] == KISS_FEND)
				break;

		last_good_pos = i - 1;

		if (i < inbuff_.size() && inbuff_[i] == KISS_FEND) {
			start = i;
			i++;

			for (; i < inbuff_.size(); ++i)
				if (inbuff_[i] == KISS_FEND)
					break;

			if (i < inbuff_.size() && inbuff_[i] == KISS_FEND) {
				// Found a KISS frame.
				end = i;
				last_good_pos = i;
				i++;
				frame_ptr new_frame(new frame());
				int ret = kiss_decode(inbuff_.data() + start, end - start + 1, &(new_frame->get_data()));

				if (ret && (new_frame->get_data().size() > 0)) {
					if (config::Instance()->debug())
						std::cout << "NEW FRAME FROM TCP " << start << ", " << end << std::endl;
					else
						std::cout << "New local frame" << std::endl;

					new_frame->print("TCP");

					get_kiss_server()->get_modem()->output_packet_to_sc(new_frame);
				}
			}
		}
	}

	inbuff_.erase(inbuff_.begin(), inbuff_.begin() + last_good_pos + 1);

}

void kiss_session::write(frame_ptr fp) {
	std::vector<unsigned char> kiss_data;
	kiss_encode(fp->get_data().data(), fp->get_data().size(), &kiss_data);
	write_raw(kiss_data.data(), kiss_data.size());
}

void kiss_session::handle_close() {
	if (config::Instance()->debug())
		std::cout << "TCP close" << std::endl;
}

void kiss_session::handle_connect() {
	if (config::Instance()->debug())
		std::cout << "connect, hay clientes: " << get_server()->get_clients().size() << std::endl;
}

kiss_server* kiss_session::get_kiss_server() {
	 return dynamic_cast<kiss_server*>(get_server());
}

basic_asio_session* kiss_server::new_session_instance(boost::asio::io_service& io_service_) {
	return new kiss_session(io_service_, this);
}

}

