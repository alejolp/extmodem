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

#include <iostream>

#include "decoder_af1200fsk.h"
#include "audiosource.h"
#include "extmodem.h"
#include "extconfig.h"


namespace extmodem {

decoder_af1200fsk::decoder_af1200fsk(modem* em)
 : hdlc_(em)
{
    f1 = 1200;
    f2 = 2200;
    fcfilt = 600;
    bps = 1200;
    delaylength = 0;
    delayidx = 0;

    sample_counter = 0;
    sample_last_change = 0;
    last_filt_samp = 0;

    sample_rate = em->get_audiosource()->get_sample_rate();

    lpf.reset(new chebyshev_filter<fptype>(fcfilt / sample_rate, false, 0, 5));

    calc_delay_len();

    delayline.resize(delaylength, 0);

    hdlc_.set_name("af1200fsk");
}

decoder_af1200fsk::~decoder_af1200fsk() 
{

}

void decoder_af1200fsk::input_callback(
    audiosource* a, const float* input, unsigned long frameCount) 
{
    (void)a;

    size_t k;

    for (k = 0; k < frameCount; ++k, ++sample_counter) {
        fptype oldsample = delayline[delayidx % delayline.size()];
        fptype newsample = input[k];

        delayline[delayidx % delayline.size()] = newsample;
        delayidx++;

        fptype a = lpf->sample(oldsample * newsample);

        bool s1 = a > 0;
        bool s2 = last_filt_samp > 0;

        if (s1 != s2) {
            size_t d = (sample_counter - sample_last_change);
            int e = std::round(((fptype)d) * bps / sample_rate);

#if 0
            std::cout << " a " << a
                << " last_filt_samp " << last_filt_samp
                << " d " << d 
                << " e " << e 
                << " oldsample " << oldsample
                << " newsample " << newsample
                << std::endl;
#endif

            sample_last_change = sample_counter;
            last_filt_samp = a;

            for (size_t i = 1; i < e; ++i) {
                hdlc_.rxbit(1);
            }


            hdlc_.rxbit(0);
            
        }
    }
}

void decoder_af1200fsk::calc_delay_len() {
    fptype maxdiff = 0;
    int i;
    int bit_len = std::round(sample_rate / bps);

    for (i = 0; i < bit_len; ++i) {
        fptype a = -std::cos(2 * M_PI * f1 * (i) / sample_rate) 
            + std::cos(2 * M_PI * f2 * (i) / sample_rate);
        if (a > maxdiff) {
            maxdiff = a;
            delaylength = i;
        }
    }

    if (config::Instance()->debug()) {
        std::cout << "maxdiff " << maxdiff 
            << " delaylength " << delaylength 
            << " sample_rate " << sample_rate 
            << std::endl;
    }
}

}

