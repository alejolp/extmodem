/*
 * ptt.cpp
 *
 *  Created on: 25/08/2013
 *      Author: alejo
 */

#include <iostream>
#include <cstdlib>

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
