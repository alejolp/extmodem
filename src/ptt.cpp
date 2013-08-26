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
/*
 *      Some code was taken from Thomas Sailer's soundmodem:
 *
 *      Copyright (C) 1999-2000
 *        Thomas Sailer (sailer@ife.ee.ethz.ch)
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
 *
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

ptt_ptr ptt::factory() {
#ifdef __unix__
	return ptt_ptr(new ptt_serial_unix());
#endif
#ifdef _MSC_VER
	return ptt_ptr(new ptt_serial_windows());
#endif
	return ptt_ptr();
}

#ifdef _MSC_VER
#define IOCTL_SERIAL_SET_DTR            CTL_CODE(FILE_DEVICE_SERIAL_PORT, 9,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_SERIAL_CLR_DTR            CTL_CODE(FILE_DEVICE_SERIAL_PORT,10,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_SERIAL_SET_RTS            CTL_CODE(FILE_DEVICE_SERIAL_PORT,12,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_SERIAL_CLR_RTS            CTL_CODE(FILE_DEVICE_SERIAL_PORT,13,METHOD_BUFFERED,FILE_ANY_ACCESS)


ptt_serial_windows::~ptt_serial_windows() {
	set_tx(0);
	::CloseHandle(hnd_);
}

int ptt_serial_windows::init(const char* fname) {
	BOOLEAN res;
	DWORD val;

	hnd_ = ::CreateFileA(fname, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hnd_ == INVALID_HANDLE_VALUE) {
		std::cerr << "PTT Error: CreateFileA" << std::endl;
		throw std::exception();
	}

	res = ::DeviceIoControl(hnd_, IOCTL_SERIAL_CLR_RTS, NULL, 0, NULL, 0, &val, NULL);
	if (!res) {
		std::cerr << "PTT Error: DeviceIoControl" << std::endl;
		::CloseHandle(hnd_);
		throw std::exception();
	}
	return 1;
}

void ptt_serial_windows::set_tx(int tx) {
	state_ = !!tx;
	set_rts(state_);
	set_dtr(state_);
}

int ptt_serial_windows::get_tx() {
	return state_;
}

void ptt_serial_windows::set_rts(int tx) {
	DWORD val;
	DeviceIoControl(hnd_, tx ? IOCTL_SERIAL_SET_RTS : IOCTL_SERIAL_CLR_RTS, NULL, 0, NULL, 0, &val, NULL);
}

void ptt_serial_windows::set_dtr(int tx) {
	DWORD val;
	DeviceIoControl(hnd_, tx ? IOCTL_SERIAL_SET_DTR : IOCTL_SERIAL_CLR_DTR, NULL, 0, NULL, 0, &val, NULL);
}
#endif

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
