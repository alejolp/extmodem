


#include <iostream>
#include <cmath>
#include <algorithm>

#include "decoder_debug.h"
#include "audiosource.h"
#include "extmodem.h"

namespace extmodem {


decoder_debug::decoder_debug(modem* em)
{
    sample_rate = em->get_audiosource()->get_sample_rate();
    idx = 0;

    history.resize(sample_rate * 3); // 2 seconds 
}

decoder_debug::~decoder_debug()
{

}

void decoder_debug::input_callback(
    audiosource* a, const float* input, unsigned long frameCount)
{
    (void)a;

    for (size_t i = 0; i < frameCount; ++i) {
        history[idx] = input[i];

        idx++;

        if (idx >= history.size()) {
            idx = 0;
            float avg = 0, stddev = 0, smin = 10, smax = -10;

            for (size_t k = 0; k < history.size(); ++k) {
                avg += history[k];
                smin = std::min(smin, history[k]);
                smax = std::max(smax, history[k]);
            }

            avg /= history.size();

            for (size_t k = 0; k < history.size(); ++k) {
                stddev += (history[k] - avg) * (history[k] - avg);
            }

            stddev = std::sqrt(stddev / history.size());

            std::cout << "[DEBUG] avg=" << avg 
                << " stddev=" << stddev 
                << " min=" << smin 
                << " max=" << smax 
                << std::endl;
        }
    }
}

}