#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Alejandro Santos LU4EXT alejolp@gmail.com
# I hate the esoteric syntax of standard builders.

import os, sys, multiprocessing

FILES=['src/audiosource.cpp',
    'src/audiosource_portaudio.cpp',
    'src/decoder_af1200mm.cpp',
    'src/decoder_af1200stj.cpp',
    'src/decoder.cpp',
    'src/decoder_dtmf.cpp',
    'src/encoder_af1200stj.cpp',
    'src/encoder.cpp',
    'src/extconfig.cpp',
    'src/extmodem.cpp',
    'src/frame.cpp',
    'src/hdlc.cpp',
    'src/kiss.cpp',
    'src/main.cpp',
    'src/multimon_utils.cpp',
    'src/ptt.cpp',
    'src/ptt_serpar.cpp',
    'src/tcpserver_agwpe.cpp',
    'src/tcpserver_base.cpp',
    'src/tcpserver.cpp',
    'src/tcpserver_kiss.cpp']

OBJ_DIR = 'bin'
CC = 'g++'
CFLAGS = '-O3 -Wall'
LDFLAGS = '-lportaudio -lboost_system -lboost_program_options'
EXECUTABLE = 'extmodem'
JOBS_COUNT = multiprocessing.cpu_count()

# g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/multimon_utils.d" -MT"src/multimon_utils.d" -o "src/multimon_utils.o" "../src/multimon_utils.cpp"

# g++  -o "extmodem"  ./src/audiosource.o ./src/decoder.o ./src/decoder_af1200mm.o ./src/decoder_af1200stj.o ./src/decoder_dtmf.o ./src/extmodem.o ./src/hdlc.o ./src/kiss.o ./src/main.o ./src/multimon_utils.o ./src/tcpserver.o   -lportaudio -lboost_system

def proc_func(p):
    out_bin_path, f = p
    cmd = CC + " " + CFLAGS + " -c -o \"" + out_bin_path + "\" \"" + f + "\""
    print(cmd)
    os.system(cmd)

def main():
    objs = []
    pool = multiprocessing.Pool(JOBS_COUNT)
    queue = []

    if not os.path.exists(OBJ_DIR):
        os.mkdir(OBJ_DIR)

    for f in FILES:
        out_bin_path = os.path.join(OBJ_DIR, \
            os.path.basename(f[:f.rfind('.')] + ".o"))
        objs.append(out_bin_path)
        if not os.path.exists(out_bin_path) or os.path.getmtime(out_bin_path) < os.path.getmtime(f):
            queue.append((out_bin_path, f))

    pool.map(proc_func, queue)

    cmd = CC + " -o \"" + os.path.join(OBJ_DIR, EXECUTABLE) + "\" " + ' '.join(objs) + " " + LDFLAGS
    print(cmd)
    os.system(cmd)

if __name__ == '__main__':
    main()

