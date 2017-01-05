
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

#include "filter_chebyshev.h"


#ifdef FILTER_CHEB_TEST

#include <iostream>

typedef float float_type;

int main(int argc, char const *argv[])
{
    (void)argc;
    (void)argv;

    std::vector<float_type> X;
    int i = 0;

    for (i = 0; i < 30; ++i)
        X.push_back(0);
    
    X.push_back(1);     

    for (i = 0; i < 300; ++i)
        X.push_back(0);

    /* ************ **/

    std::vector<float_type> Y;
    std::vector<float_type> A;
    std::vector<float_type> B;

    chebyshev_filter_calc<float_type>(0.1, 0, 0.0, 5, A, B);

    Y.resize(X.size());

    apply_filter<float_type>(X, A, B, 5, Y);

    for (i=0; i < (int)Y.size(); ++i)
        std::cout << Y[i] << std::endl;

    chebyshev_filter<float_type> F(0.1, false, 0, 5);

    std::cout << "************************************************" << std::endl;

    i = 0;
    float_type sq = 0;

    for (i = 0; i < (int)X.size(); ++ i) {
        float_type e = F.sample(X[i]);
        sq += (e - Y[i]) * (e - Y[i]);
        std::cout << e << std::endl;
    }

    std::cout << "************************************************" << std::endl;

    std::cout << "error: " << sq << std::endl;

    return 0;
}

#endif
