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

#ifndef DECODER_AF1200FSK_H_
#define DECODER_AF1200FSK_H_

#include <vector>
#include <memory>

#include "decoder.h"
#include "hdlc.h"

#include "filter_chebyshev.h"



namespace extmodem {

class modem;
class audiosource;

class decoder_af1200fsk : public decoder {
public:
    decoder_af1200fsk(modem* em);
    virtual ~decoder_af1200fsk();

    virtual void input_callback(audiosource* a, const float* input, unsigned long frameCount);

private:

    void calc_delay_len();

    hdlc hdlc_;

    std::vector<float> delayline;
    int delaylength;
    size_t delayidx;

    size_t sample_counter;
    size_t sample_last_change;
    float last_filt_samp;

    float f1;
    float f2;
    float fcfilt;
    float bps;
    int sample_rate;

    std::unique_ptr< chebyshev_filter<float> > lpf;
};

}

#endif

