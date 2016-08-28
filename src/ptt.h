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

#include <string>

#include <boost/shared_ptr.hpp>

namespace extmodem {

class ptt;
typedef boost::shared_ptr<ptt> ptt_ptr;

class ptt {
public:
	static ptt_ptr factory(const std::string& mode);

public:
	ptt();
	virtual ~ptt();

	virtual int init(const char* fname) = 0;
	virtual void set_tx(int tx) = 0;
	virtual int get_tx() = 0;
};

class ptt_dummy : public ptt {
public:
	ptt_dummy() : state_(0) {}
	virtual ~ptt_dummy();

	virtual int init(const char* fname);
	virtual void set_tx(int tx);
	virtual int get_tx();

private:
	int state_;
};

} /* namespace extmodem */
#endif /* PTT_H_ */
