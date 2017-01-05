#!/bin/bash

g++ -Wall -Wextra -std=c++11 -ggdb -DFILTER_CHEB_TEST=1 -D_GLIBCXX_DEBUG -fstack-protector filter_chebyshev.cpp

