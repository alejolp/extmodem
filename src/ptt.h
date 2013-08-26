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

#ifndef PTT_H_
#define PTT_H_

#include <boost/shared_ptr.hpp>

#ifdef _MSC_VER
#include <windows.h>
#include <winioctl.h>
#endif

namespace extmodem {

class ptt;
typedef boost::shared_ptr<ptt> ptt_ptr;

class ptt {
public:
	static ptt_ptr factory();

public:
	ptt();
	virtual ~ptt();

	virtual int init(const char* fname) = 0;
	virtual void set_tx(int tx) = 0;
	virtual int get_tx() = 0;
};

#ifdef _MSC_VER
class ptt_serial_windows: public ptt {
public:
	ptt_serial_windows() : hnd_(INVALID_HANDLE_VALUE) {}
	virtual ~ptt_serial_windows();

	virtual int init(const char* fname);
	virtual void set_tx(int tx);
	virtual int get_tx();

	void set_dtr(int tx);
	void set_rts(int tx);

private:
	HANDLE hnd_;
	int state_;
};
#endif

#ifdef __unix__
class ptt_serial_unix: public ptt {
public:
	ptt_serial_unix() : fd_(-1) {}
	virtual ~ptt_serial_unix();

	virtual int init(const char* fname);
	virtual void set_tx(int tx);
	virtual int get_tx();

	void set_dtr(int tx);
	void set_rts(int tx);

private:
	int fd_;
	int state_;
};
#endif

} /* namespace extmodem */
#endif /* PTT_H_ */
