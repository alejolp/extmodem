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

#include "ax25_utils.h"
#include <algorithm>
#include <sstream>

namespace extmodem {

    std::string ax25_utils::encode_addr(const std::string address, bool flags_first_bit, bool flags_last_bit) {

        // check SSID presence
        int ssid = 0;
        std::string::size_type sep = address.find('-', 0);
        if (sep != std::string::npos && sep != address.length() - 1)
            ssid = std::atoi(address.substr(sep + 1, address.length() - sep).c_str());
        else
            sep = address.length();

        // shift address bits
        std::string out = std::string(7, ' ' << 1);
        for (unsigned i = 0; i < std::min(sep, (std::string::size_type) 6); ++i)
            out.at(i) = address.at(i) << 1;

        // last octet with SSID and flags
        char octet = (char) ssid;
        octet <<= 1;
        octet ^= (-flags_first_bit ^ octet) & (1 << 7);
        octet |= 1 << 6;
        octet |= 1 << 5;
        octet ^= (-flags_last_bit ^ octet) & (1 << 0);
        out.at(6) = octet;

        return out;
    }

    std::string ax25_utils::encode_source_addr(const std::string address, bool is_cmd_res, bool has_repeaters) {

        return encode_addr(address, is_cmd_res, !has_repeaters);
    }

    std::string ax25_utils::encode_destination_addr(const std::string address, bool is_cmd_res) {

        return encode_addr(address, is_cmd_res, 0);
    }

    std::string
    ax25_utils::encode_repeater_addr(const std::string address, bool has_been_repeated, bool is_last_repeater) {

        return encode_addr(address, has_been_repeated, is_last_repeater);
    }

    std::string
    ax25_utils::encode_frame_data_repeater_mode(const std::string source_addr, const std::string destination_addr,
                                                const std::vector<std::string> *repeaters_addrs,
                                                const std::string data) {

        unsigned long nreps = repeaters_addrs->size();

        // compose frame data with destination and source
        std::stringstream ss;
        ss << encode_destination_addr(destination_addr, true);
        ss << encode_source_addr(source_addr, true, nreps > 0);

        // add repeaters
        for (unsigned long k = 0; k < nreps; k++)
            ss << encode_repeater_addr(repeaters_addrs->at(k), false, k == nreps - 1);

        // control field, protocol and information
        ss << (char) AX25_UI_FRAME;
        ss << (char) AX25_PID_NO_LAYER_3;
        ss << data;

        return ss.str();
    }
}
