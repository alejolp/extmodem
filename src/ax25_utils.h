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

#ifndef EXTMODEM_AX25_UTILS_H
#define EXTMODEM_AX25_UTILS_H

#include <string>
#include <vector>

/*
 * AX.25 specs:
 * - https://www.tapr.org/pub_ax25.html
 */

#define AX25_UI_FRAME 3             /* Control field value */
#define AX25_PID_NO_LAYER_3 0xf0    /* Protocol Identifier (PID) for no layer 3 */

namespace extmodem {

    class ax25_utils {

    public:
        static std::string encode_addr(const std::string address, bool flags_first_bit, bool flags_last_bit);

        static std::string encode_source_addr(const std::string address, bool is_cmd_res, bool has_repeaters);

        static std::string encode_destination_addr(const std::string address, bool is_cmd_res);

        static std::string
        encode_repeater_addr(const std::string address, bool has_been_repeated, bool is_last_repeater);

        static std::string
        encode_frame_data_repeater_mode(const std::string source_addr, const std::string destination_addr,
                                        const std::vector<std::string> *repeaters_addrs,
                                        const std::string data);
    };

} /* namespace extmodem */

#endif //EXTMODEM_AX25_UTILS_H
