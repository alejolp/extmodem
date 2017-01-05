
#include <cmath>
#include <vector>
#include <cstdlib>
#include <cstdint>

#include "filter_chebyshev.h"


size_t count_number_of_poles(
    const std::vector<float> &A, 
    const std::vector<float> &B) 
{
    size_t k = 0;

    for (; k < std::min(A.size(), B.size());) {
        k += 1;
        if (! (std::abs(A[k]) > 1e-16 || std::abs(B[k]) > 1e-16))
            break;
    }

    return k;
}

void apply_filter(
    const std::vector<float> &X, 
    const std::vector<float> &A, 
    const std::vector<float> &B, 
    size_t number_of_poles, 
    std::vector<float> &Y) 
{
    size_t k = 0;

#if 0
    number_of_poles = count_number_of_poles(A, B);
#endif

    // #assert k == NP, (k, NP, A, B)
    // NP = k
    
    for (size_t i = number_of_poles; i < X.size(); ++i) {
        //for k in range(0, NP+1):
        for (k = 0; k <= number_of_poles; ++k)
            Y[i] += A[k] * X[i-k];
        //for k in range(1, NP+1):
        for (k = 1; k <= number_of_poles; ++k)
            Y[i] += B[k] * Y[i-k];
    }

    //return Y
}

void chebyshev_filter_calc_coeffs(
    float FC, bool LH, float PR, int NP, int P,
    float& A0, float& A1, float& A2, float& B1, float& B2) 
{
    // # From: http://www.dspguide.com/ch20.htm

    float RP = - std::cos(M_PI / (NP*2.0) + (P-1.0) * M_PI/NP);
    float IP =   std::sin(M_PI / (NP*2.0) + (P-1.0) * M_PI/NP);
    float ES = 0, VX = 0, KX = 0;
    float T, W, M, D, X0, X1, X2, Y1, Y2;
    float K;

    // # Warp from a circle to an ellipse
    if (PR > 0) {
        auto t0 = (100.0 / (100.0-PR));
        ES = std::sqrt( t0 * t0 -1 );
        VX = (1.0/NP) * std::log( (1.0/ES) + std::sqrt( (1.0/(ES*ES)) + 1.0) );
        KX = (1.0/NP) * std::log( (1.0/ES) + std::sqrt( (1.0/(ES*ES)) - 1.0) );
        KX = (std::exp(KX) + std::exp(-KX))/2.0;
        RP = RP * ( (std::exp(VX) - std::exp(-VX) ) /2.0 ) / KX;
        IP  = IP * ( (std::exp(VX) + std::exp(-VX) ) /2.0 ) / KX;
    }

    // #print RP, IP, ES, VX, KX

    // # s-domain to z-domain conversion
    T  = 2.0 * std::tan(1.0 / 2.0);
    W  = 2.0 * M_PI*FC;
    M  = RP*RP + IP*IP;
    D = 4.0 - 4.0*RP*T + M*T*T;
    X0 = (T*T)/D;
    X1 = (2.0*T*T)/D;
    X2 = (T*T)/D;
    Y1 = (8.0 - 2.0*M*T*T)/D;
    Y2 = (-4.0 - 4.0*RP*T - M*T*T)/D;

    // #print T, W, M, D, X0, X1, X2, Y1, Y2

    // # LP TO LP, or LP TO HP transform
    if (LH) {
        K = -std::cos(W/2.0 + 1.0/2.0) / std::cos(W/2.0 - 1.0/2.0);
    } else {
        K =  std::sin(1.0/2.0 - W/2.0) / std::sin(1.0/2.0 + W/2.0);
    }
    
    D = 1.0 + Y1*K - Y2*K*K;
    A0 = (X0 - X1*K + X2*K*K)/D;
    A1 = (-2.0*X0*K + X1 + X1*K*K - 2.0*X2*K)/D;
    A2 = (X0*K*K - X1*K + X2)/D;
    B1 = (2.0*K + Y1 + Y1*K*K - 2.0*Y2*K)/D;
    B2 = (-(K*K) - Y1*K + Y2)/D;
    
    if (LH) {
        A1 = -A1;
        B1 = -B1;
    }

    // #print A0, A1, A2, B1, B2
    //return (A0, A1, A2, B1, B2)
}

void chebyshev_filter_calc(
    float FC, bool LH, float PR, size_t NP,
    std::vector<float> &A,
    std::vector<float> &B) 
{
    //
    //# FC: Cut Frequency, [0.0, 0.5]
    //# LH: 0=Low Pass, 1=High Pass
    //# PR: Percentage Ripple, [0, 99]
    //# NP: Number of Poles
    //#
    //# From: http://www.dspguide.com/ch20.htm

    std::vector<float> TA;
    std::vector<float> TB;

    A.resize(23);
    B.resize(23);
    TA.resize(23);
    TB.resize(23);

    A[2] = 1.0;
    B[2] = 1.0;

    //for P in range(1, NP/2 + 1):
    for (int P = 1; P <= (int)NP / 2; ++P) {
        float A0, A1, A2, B1, B2;

        chebyshev_filter_calc_coeffs(FC, LH, PR, NP, P, A0, A1, A2, B1, B2);

        //for I in range(0, 23):
        for (int I = 0; I < 23; ++I) {
            TA[I] = A[I];
            TB[I] = B[I];
        }

        //for I in range(2, 23):
        for (int I = 2; I < 23; ++I) {
            A[I] = A0 * TA[I] + A1 * TA[I-1] + A2 * TA[I-2];
            B[I] = TB[I] - B1 * TB[I-1] - B2 * TB[I-2];
        }
    }

    B[2] = 0.0;

    //for I in range(0, 21):
    for (int I = 0; I < 21; ++I) {
        A[I] = A[I+2];
        B[I] = -B[I+2];
    }

    float SA = 0.0;
    float SB = 0.0;
    //for I in range(0, 21):
    for (int I = 0; I < 21; ++I) {
        if (!LH) { 
            SA = SA + A[I];
            SB = SB + B[I];
        } else {
            SA = SA + A[I] * ((I % 2) ? (-1) : (1));
            SB = SB + B[I] * ((I % 2) ? (-1) : (1));
        }
    }

    float Gain = SA / (1.0 - SB);

    for (int I = 0; I < 21; ++I) {
        A[I] = A[I] / Gain;
    }

    // return A, B
}

chebyshev_filter::chebyshev_filter(
    float freq, bool highpass, float ripple, size_t number_of_poles) 
 : freq_(freq), 
   highpass_(highpass), 
   ripple_(ripple), 
   number_of_poles_(number_of_poles), 
   idx_(0)
{
    chebyshev_filter_calc(freq_, highpass_, ripple_, number_of_poles_, A_, B_);
    X_.resize(number_of_poles_ + 1);
    Y_.resize(number_of_poles_ + 1);
}

#if 0
float chebyshev_filter::sample(float v) {
    size_t k;

    idx_++;
    X_[idx_ % X_.size()] = v;
    Y_[idx_ % Y_.size()] = 0;

    for (int i = number_of_poles_; i < (int)X_.size(); ++i) {
        for (k = 0; k <= number_of_poles_; ++k) {
            Y_[(i - idx_ + (int)Y_.size()) % Y_.size()] += A_[k] * X_[(i - k - idx_ + (int)X_.size()) % X_.size()];
        }

        for (k = 1; k <= number_of_poles_; ++k) {
            Y_[(i - idx_ + (int)Y_.size()) % Y_.size()] += B_[k] * Y_[(i - k - idx_ + (int)Y_.size()) % Y_.size()];
        }
    }

    return Y_[idx_ % Y_.size()];
}
#endif

#if 1
float chebyshev_filter::sample(float v) {
    size_t k;

    for (k = 1; k < X_.size(); ++k) {
        X_[k-1] = X_[k];
        Y_[k-1] = Y_[k];
    }

    X_[X_.size() - 1] = v;
    Y_[X_.size() - 1] = 0;

    //for (int i = number_of_poles_; i < (int)X_.size(); ++i) 
    int i = number_of_poles_;
    {
        //for k in range(0, NP+1):
        for (k = 0; k <= number_of_poles_; ++k)
            Y_[i] += A_[k] * X_[i-k];
        //for k in range(1, NP+1):
        for (k = 1; k <= number_of_poles_; ++k)
            Y_[i] += B_[k] * Y_[i-k];
    }

    return Y_[X_.size() - 1];
}
#endif

#ifdef FILTER_CHEB_TEST

#include <iostream>

int main(int argc, char const *argv[])
{
    (void)argc;
    (void)argv;

    std::vector<float> X;
    int i = 0;

    for (i = 0; i < 30; ++i)
        X.push_back(0);
    
    X.push_back(1);     

    for (i = 0; i < 300; ++i)
        X.push_back(0);

    /* ************ **/

    std::vector<float> Y;
    std::vector<float> A;
    std::vector<float> B;

    chebyshev_filter_calc(0.1, 0, 0, 5, A, B);

    Y.resize(X.size());

    apply_filter(X, A, B, 5, Y);

    for (i=0; i < (int)Y.size(); ++i)
        std::cout << Y[i] << std::endl;

    chebyshev_filter F(0.1, false, 0, 5);

    std::cout << "************************************************" << std::endl;

    i = 0;
    float sq = 0;

    for (i = 0; i < (int)X.size(); ++ i) {
        float e = F.sample(X[i]);
        sq += (e - Y[i]) * (e - Y[i]);
        std::cout << e << std::endl;
    }

    std::cout << "************************************************" << std::endl;

    std::cout << "error: " << sq << std::endl;

    return 0;
}

#endif
