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

#ifdef HAMLIB_FOUND

#include <string>
#include <cstring>
#include <cmath>
#include <iostream>

#include "ptt_hamlib.h"
#include "extconfig.h"
#include "extexception.h"

namespace extmodem {

ptt_hamlib::ptt_hamlib() : rig_(0), state_(0) {

}

ptt_hamlib::~ptt_hamlib() {
	close();
}

int ptt_hamlib::init(const char* fname) {
	config* cfg = config::Instance();
	rig_model_t myrig_model;
	int retcode;

	if (cfg->debug()) {
		rig_set_debug_level(RIG_DEBUG_VERBOSE);
	} else {
		rig_set_debug_level(RIG_DEBUG_NONE);
	}

	if (cfg->hamlib_model() < 0) {
		hamlib_port_t myport;
		/* may be overriden by backend probe */
		myport.type.rig = RIG_PORT_SERIAL;
		myport.parm.serial.rate = 9600;
		myport.parm.serial.data_bits = 8;
		myport.parm.serial.stop_bits = 1;
		myport.parm.serial.parity = RIG_PARITY_NONE;
		myport.parm.serial.handshake = RIG_HANDSHAKE_NONE;
		std::strncpy(myport.pathname, fname, FILPATHLEN - 1);

		rig_load_all_backends();
		myrig_model = rig_probe(&myport);
	} else {
		myrig_model = cfg->hamlib_model();
	}

	rig_ = rig_init(myrig_model);

	if (!rig_) {
		std::cerr << "Unknown rig num: " << myrig_model << std::endl;
		std::cerr << "Please check riglist.h" << std::endl;
		throw extexception("rig_init");
	}

	std::strncpy(rig_->state.rigport.pathname, fname, FILPATHLEN - 1);

	retcode = rig_open(rig_);

	if (retcode != RIG_OK) {
		std::cerr << "rig_open: error = " << rigerror(retcode) << std::endl;
		throw extexception("rig_open");
	}

	retcode = rig_set_vfo(rig_, RIG_VFO_A);

	if (retcode != RIG_OK ) {
		std::cerr << "rig_set_vfo: error = " << rigerror(retcode) << std::endl;
	}

	set_tx(0);
	return 1;
}

void ptt_hamlib::close() {
	rig_close(rig_);
	rig_cleanup(rig_);
	rig_ = 0;
}

void ptt_hamlib::set_tx(int tx) {
	if (tx != state_) {
		int retcode;

		 /* stand back ! */
		retcode = rig_set_ptt(rig_, RIG_VFO_CURR, (tx ? RIG_PTT_ON : RIG_PTT_OFF));

		if (retcode != RIG_OK) {
			std::cerr << "rig_set_ptt: error = " << rigerror(retcode) << std::endl;
		}

		state_ = !!tx;
	}
}

int ptt_hamlib::get_tx() {
	return state_;
}

} /* namespace extmodem */

#endif
