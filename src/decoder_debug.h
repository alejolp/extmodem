/*
 * Sound card modem for Amateur Radio AX25.
 *
 * Copyright (C) Alejandro Santos, 2013-2017, alejolp@gmail.com.
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

#ifndef DECODER_DEBUG_H
#define DECODER_DEBUG_H

#include <vector>
#include <cstdlib>

#include "decoder.h"


namespace extmodem {

class modem;
class audiosource;

class decoder_debug : public decoder {
public:
    decoder_debug(modem* em);
    virtual ~decoder_debug();

    virtual void input_callback(audiosource* a, const float* input, unsigned long frameCount);

private:
    int sample_rate;
    size_t idx;

    std::vector<float> history;
};

}

#endif

