
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

#ifndef __FILTER_CHEBYSHEV_H__
#define __FILTER_CHEBYSHEV_H__


#include <cmath>
#include <vector>
#include <cstdlib>
#include <cstdint>

template<typename FT>
void apply_filter(
    const std::vector<FT> &X, 
    const std::vector<FT> &A, 
    const std::vector<FT> &B, 
    size_t number_of_poles, 
    std::vector<FT> &Y);

template<typename FT>
void chebyshev_filter_calc(
    FT FC, bool LH, FT PR, size_t NP,
    std::vector<FT> &A,
    std::vector<FT> &B);

/* 
 * Chebyshev/Butterworth filter implementation
 *
 * From: http://www.dspguide.com/ch20.htm
 */
template<typename FT>
class chebyshev_filter {
public:

    //freq: Cut Frequency, in rads/s [0.0, 0.5]
    //highpass: false=Low Pass, true=High Pass
    //ripple: Percentage Ripple, [0, 99]
    //number_of_poles: Number of Poles

    chebyshev_filter(FT freq, bool highpass, FT ripple, size_t number_of_poles);

    FT sample(FT v);

private:
    FT freq_;
    bool highpass_;
    FT ripple_;
    size_t number_of_poles_;
    std::vector<FT> A_;
    std::vector<FT> B_;
    std::vector<FT> X_;
    std::vector<FT> Y_;
};


template<typename FT>
size_t count_number_of_poles(
    const std::vector<FT> &A, 
    const std::vector<FT> &B) 
{
    size_t k = 0;

    for (; k < std::min(A.size(), B.size());) {
        k += 1;
        if (! (std::abs(A[k]) > 1e-16 || std::abs(B[k]) > 1e-16))
            break;
    }

    return k;
}

template<typename FT>
void apply_filter(
    const std::vector<FT> &X, 
    const std::vector<FT> &A, 
    const std::vector<FT> &B, 
    size_t number_of_poles, 
    std::vector<FT> &Y) 
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

template<typename FT>
void chebyshev_filter_calc_coeffs(
    FT FC, bool LH, FT PR, int NP, int P,
    FT& A0, FT& A1, FT& A2, FT& B1, FT& B2) 
{
    // # From: http://www.dspguide.com/ch20.htm

    FT RP = - std::cos(M_PI / (NP*2.0) + (P-1.0) * M_PI/NP);
    FT IP =   std::sin(M_PI / (NP*2.0) + (P-1.0) * M_PI/NP);
    FT ES = 0, VX = 0, KX = 0;
    FT T, W, M, D, X0, X1, X2, Y1, Y2;
    FT K;

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

template<typename FT>
void chebyshev_filter_calc(
    FT FC, bool LH, FT PR, size_t NP,
    std::vector<FT> &A,
    std::vector<FT> &B) 
{
    //
    //# FC: Cut Frequency, [0.0, 0.5]
    //# LH: 0=Low Pass, 1=High Pass
    //# PR: Percentage Ripple, [0, 99]
    //# NP: Number of Poles
    //#
    //# From: http://www.dspguide.com/ch20.htm

    std::vector<FT> TA;
    std::vector<FT> TB;

    A.resize(23);
    B.resize(23);
    TA.resize(23);
    TB.resize(23);

    A[2] = 1.0;
    B[2] = 1.0;

    //for P in range(1, NP/2 + 1):
    for (int P = 1; P <= (int)NP / 2; ++P) {
        FT A0, A1, A2, B1, B2;

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

    FT SA = 0.0;
    FT SB = 0.0;
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

    FT Gain = SA / (1.0 - SB);

    for (int I = 0; I < 21; ++I) {
        A[I] = A[I] / Gain;
    }

    // return A, B
}

template<typename FT>
chebyshev_filter<FT>::chebyshev_filter(
    FT freq, bool highpass, FT ripple, size_t number_of_poles) 
 : freq_(freq), 
   highpass_(highpass), 
   ripple_(ripple), 
   number_of_poles_(number_of_poles)
{
    chebyshev_filter_calc(freq_, highpass_, ripple_, number_of_poles_, A_, B_);
    X_.resize(number_of_poles_ + 1);
    Y_.resize(number_of_poles_ + 1);
}

template<typename FT>
FT chebyshev_filter<FT>::sample(FT v) {
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


#endif // __FILTER_CHEBYSHEV_H__
