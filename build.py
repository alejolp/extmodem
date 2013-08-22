#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Alejandro Santos LU4EXT alejolp@gmail.com
# I hate the esoteric syntax of standard builders.

import os, sys

FILES=['./src/decoder_af1200mm.cpp',
    './src/extmodem.cpp',
    './src/decoder_dtmf.cpp',
    './src/multimon_utils.cpp',
    './src/tcpserver.cpp',
    './src/decoder_af1200stj.cpp',
    './src/kiss.cpp',
    './src/main.cpp',
    './src/hdlc.cpp',
    './src/decoder.cpp',
    './src/audiosource.cpp']
OBJ_DIR = 'bin'
CC = 'g++'
CFLAGS = '-O3 -Wall'
LDFLAGS = '-lportaudio -lboost_system'
EXECUTABLE = 'extmodem'

# g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/multimon_utils.d" -MT"src/multimon_utils.d" -o "src/multimon_utils.o" "../src/multimon_utils.cpp"

# g++  -o "extmodem"  ./src/audiosource.o ./src/decoder.o ./src/decoder_af1200mm.o ./src/decoder_af1200stj.o ./src/decoder_dtmf.o ./src/extmodem.o ./src/hdlc.o ./src/kiss.o ./src/main.o ./src/multimon_utils.o ./src/tcpserver.o   -lportaudio -lboost_system

def main():
    objs = []
    if not os.path.exists(OBJ_DIR):
        os.mkdir(OBJ_DIR)
    for f in FILES:
        out_bin_path = os.path.join(OBJ_DIR, \
            os.path.basename(f[:f.rfind('.')] + ".o"))
        objs.append(out_bin_path)
        if not os.path.exists(out_bin_path) or os.path.getmtime(out_bin_path) < os.path.getmtime(f):
            cmd = CC + " " + CFLAGS + " -c -o \"" + out_bin_path + "\" \"" + f + "\""
            print(cmd)
            os.system(cmd)

    cmd = CC + " " + LDFLAGS + " -o \"" + os.path.join(OBJ_DIR, EXECUTABLE) + "\" " + ' '.join(objs)
    print(cmd)
    os.system(cmd)

if __name__ == '__main__':
    main()

