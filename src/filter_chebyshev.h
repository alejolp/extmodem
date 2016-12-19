
#ifndef __FILTER_CHEBYSHEV_H__
#define __FILTER_CHEBYSHEV_H__

#include <vector>
#include <cstdlib>
#include <cstdint>

void apply_filter(
    const std::vector<float> &X, 
    const std::vector<float> &A, 
    const std::vector<float> &B, 
    size_t number_of_poles, 
    std::vector<float> &Y);

void chebyshev_filter_calc(
    float FC, bool LH, float PR, size_t NP,
    std::vector<float> &A,
    std::vector<float> &B);

/* 
 * Chebyshev/Butterworth filter implementation
 *
 * From: http://www.dspguide.com/ch20.htm
 */
class chebyshev_filter {
public:

    //freq: Cut Frequency, in rads/s [0.0, 0.5]
    //highpass: false=Low Pass, true=High Pass
    //ripple: Percentage Ripple, [0, 99]
    //number_of_poles: Number of Poles

    chebyshev_filter(float freq, bool highpass, float ripple, size_t number_of_poles);

    float sample(float v);

private:
    float freq_;
    bool highpass_;
    float ripple_;
    size_t number_of_poles_;
    std::vector<float> A_;
    std::vector<float> B_;
    std::vector<float> X_;
    std::vector<float> Y_;
    int idx_;
};

#endif // __FILTER_CHEBYSHEV_H__
