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
#include <sstream>


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
#include <stdio.h>
#endif

#ifdef __linux
#include <linux/parport.h>
#include <linux/ppdev.h>
#endif

#include "ptt_serpar.h"
#include "extexception.h"


namespace extmodem {


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
static void unix_print_error(int err, const std::string& context) {
	char buf[1024];
	std::stringstream sss;
	strerror_r(err, buf, sizeof(buf));
	sss << buf;
	std::string message = sss.str();
	std::cerr << "Error: " << context << ": " << buf << std::endl;
	throw extexception(context);
}

ptt_serial_unix::~ptt_serial_unix() {
	set_tx(0);
	if (fd_ > 0)
		close(fd_);
}

int ptt_serial_unix::init(const char* fname) {
	fd_ = open(fname, O_RDWR, 0);
	if (fd_ < 0) {
		std::string ctx = "open() fname: ";
		ctx += fname;
		unix_print_error(errno, ctx);
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


ptt_parallel_unix::~ptt_parallel_unix() {
	set_tx(0);
	if (fd_ > 0)
		close(fd_);
}

int ptt_parallel_unix::init(const char* fname) {
	fd_ = open(fname, O_RDWR, 0);
	if (fd_ < 0) {
		std::string ctx = "open() fname: ";
		ctx += fname;
		unix_print_error(errno, ctx);
		return 0;
	}

	if (ioctl(fd_, PPCLAIM, 0)) {
		std::string ctx = "ioctl() 1 fname: ";
		ctx += fname;
		unix_print_error(errno, ctx);
		close (fd_);
		return 0;
	}

	// Set the Mode
	int mode = IEEE1284_MODE_BYTE;
	if (ioctl(fd_, PPSETMODE, &mode)) {
		std::string ctx = "ioctl() 2 fname: ";
		ctx += fname;
		unix_print_error(errno, ctx);
		ioctl(fd_, PPRELEASE);
		close (fd_);
		return 0;
	}

	// Set data pins to output
	int dir = 0x00;
	if (ioctl(fd_, PPDATADIR, &dir)) {
		std::string ctx = "ioctl() 3 fname: ";
		ctx += fname;
		unix_print_error(errno, ctx);
		ioctl(fd_, PPRELEASE);
		close (fd_);
		return 1;
	}

	set_tx(0);
	return 1;
}

void ptt_parallel_unix::set_tx(int tx) {
	state_ = !!tx;
	unsigned char reg1, reg2;

	if (fd_ < 0)
		return;

	reg1 = state_ ? 0xff : 0x00;
	reg2 = 0;
        
        
#ifdef __FreeBSD__
        ioctl(fd_, PPISDATA, &reg1); // High
        ioctl(fd_, PPISDATA, &reg2); // Low
#else
        ioctl(fd_, PPWDATA, &reg1); // High
        ioctl(fd_, PPWDATA, &reg2); // Low
#endif
}
        
        
int ptt_parallel_unix::get_tx() {
      return state_;
}


ptt_gpio_unix::~ptt_gpio_unix() {
}

int ptt_gpio_unix::init(const char* fname) {
  gpio_pin_ = "/sys/class/gpio/";
  gpio_pin_ += fname;
  gpio_pin_ += "/value";
  return 1;
}

void ptt_gpio_unix::set_tx(int tx) {
  state_ = !!tx;
  int fd = open(gpio_pin_.c_str(), O_WRONLY);
  if (fd < 0) {
    std::string ctx = "open() fname: ";
    ctx += gpio_pin_;
    unix_print_error(errno, ctx);
    return;
  }
  
  if (state_) 
    write(fd, "1", 1); 
  else
    write(fd, "0", 1);
  close(fd);
}

int ptt_gpio_unix::get_tx() {
  return state_;
}



#endif /* __unix__ */

} /* namespace extmodem */
