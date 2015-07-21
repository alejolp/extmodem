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

#include "ptt.h"
#include "ptt_serpar.h"
#include "ptt_hamlib.h"
#include "extexception.h"

namespace extmodem {

ptt::ptt() {

}

ptt::~ptt() {

}

ptt_ptr ptt::factory(const std::string& mode) {
	if (mode == "null") {
	return ptt_ptr(new ptt_dummy());
	} else if (mode == "serial") {
#ifdef __unix__
	return ptt_ptr(new ptt_serial_unix());
#endif  
#ifdef _MSC_VER
	return ptt_ptr(new ptt_serial_windows());
#endif
	} else if (mode == "parallel") {
#ifdef __unix__
	return ptt_ptr(new ptt_parallel_unix());
#endif
#ifdef _MSC_VER       
        throw extexception("ptt::factory unsupported mode '" + mode + "'"); 
        /* return ptt_ptr(new ptt_parallel_windows()); */
#endif        
        } else if (mode == "gpio") {
#ifdef __unix__
        return ptt_ptr(new ptt_gpio_unix());          
#endif
#ifdef _MSC_VER
	throw extexception("ptt::factory unsupported mode '" + mode + "'");
#endif
	} else if (mode == "hamlib") {
#ifdef HAMLIB_FOUND
		return ptt_ptr(new ptt_hamlib());
#endif
	}
	throw extexception("ptt::factory unknown mode '" + mode + "'");
	//return ptt_ptr();
}

ptt_dummy::~ptt_dummy() {}

int ptt_dummy::init(const char* fname) {
	(void)fname;
	state_ = 0;
	return 1;
}

void ptt_dummy::set_tx(int tx) {
	state_ = !!tx;
}

int ptt_dummy::get_tx() {
	return state_;
}

} /* namespace extmodem */
