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

#include <iostream>
#include <cstdlib>
#include <exception>

#ifdef __unix__
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#endif

#include "ptt.h"

namespace extmodem {

ptt::ptt() {

}

ptt::~ptt() {

}

#ifdef __unix__
static void unix_print_error(int err) {
	char buf[1024];
	strerror_r(err, buf, sizeof(buf));
	std::cerr << "Error: open() " << buf << std::endl;
	throw std::exception();
}

ptt_serial_unix::~ptt_serial_unix() {
	set_tx(0);
	if (fd_ > 0)
		close(fd_);
}

int ptt_serial_unix::init(const char* fname) {
	fd_ = open(fname, O_RDWR, 0);
	if (fd_ < 0) {
		unix_print_error(errno);
		return 0;
	}
	set_tx(0);
	return 1;
}

void ptt_serial_unix::set_tx(int tx) {
	state_ = !!tx;
	set_rts(state_);
	set_dtr(state_);
}

int ptt_serial_unix::get_tx() {
	return state_;
}

void ptt_serial_unix::set_rts(int tx) {
	unsigned int y;
	ioctl(fd_, TIOCMGET, &y);
	if (tx)
	    y |= TIOCM_RTS;
	else
	    y &= ~TIOCM_RTS;
	ioctl(fd_, TIOCMSET, &y);
}

void ptt_serial_unix::set_dtr(int tx) {
	unsigned int y;
	ioctl(fd_, TIOCMGET, &y);
	if (tx)
	    y |= TIOCM_DTR;
	else
	    y &= ~TIOCM_DTR;
	ioctl(fd_, TIOCMSET, &y);
}
#endif /* __unix__ */

} /* namespace extmodem */
