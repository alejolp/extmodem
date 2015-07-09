#!/usr/bin/env python
# -*- coding: utf-8 -*-

"""
Alejandro Santos LU4EXT/HB3YNQ alejolp@gmail.com

I hate the esoteric syntax of standard builders like make. We're in year 2014,
there's Python everywhere, even on your cellphone.

This is the very most simple C++ builder for small, medium and large projects
that have only one single target binary file. Much like make, this builder
recompiles a file only when needed, wheter the file was modified since last
compilation, or one of the .h header files were changed.

This script can be easily modified for different purposes. We can also decouple
configuration from this file to build the source code under different settings.
I don't need that at the moment, but since this is Python the changes should
be easy to be done.
"""

import os, sys, multiprocessing, glob

FILES = glob.glob("src/*.cpp")

OBJ_DIR = 'bin'
CC = 'g++'
CFLAGS = '-MMD -g -O2 -Wall -std=c++0x'
LDFLAGS = '-lportaudio -lboost_system -lboost_program_options -lasound -lpthread'
EXECUTABLE = 'extmodem'
JOBS_COUNT = multiprocessing.cpu_count()

# g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"src/multimon_utils.d" -MT"src/multimon_utils.d" -o "src/multimon_utils.o" "../src/multimon_utils.cpp"

# g++  -o "extmodem"  ./src/audiosource.o ./src/decoder.o ./src/decoder_af1200mm.o ./src/decoder_af1200stj.o ./src/decoder_dtmf.o ./src/extmodem.o ./src/hdlc.o ./src/kiss.o ./src/main.o ./src/multimon_utils.o ./src/tcpserver.o   -lportaudio -lboost_system

def change_ext(f, ext):
    return f[:f.rfind('.')] + ext

def deps_func(p):
    out_bin_path, f = p
    build = False
    f_mtime = os.path.getmtime(f)
    if not os.path.exists(out_bin_path):
        build = True
    else:
        bin_mtime = os.path.getmtime(out_bin_path)
        if bin_mtime < f_mtime:
            build = True
        else:
            depsfile_path = change_ext(out_bin_path, ".d")
            if os.path.exists(depsfile_path) and os.path.getmtime(depsfile_path) < f_mtime:
                with open(depsfile_path) as w:
                    deps = w.read().replace("\\", " ").replace("\n", " ").split()
                    assert deps[1][-1] == ':'
                    del deps[0]
                for d in deps:
                    if os.path.getmtime(d) >= bin_mtime:
                        build = True
                        break
            elif not os.path.exists(depsfile_path):
                build = True
    if build:
        return (out_bin_path, f)
    return None

def build_func(p):
    out_bin_path, f = p
    cmd = CC + " " + CFLAGS + " -o \"" + out_bin_path + "\" -c \"" + f + "\""
    print(cmd)
    os.system(cmd)

def main():
    objs = []
    pool = multiprocessing.Pool(JOBS_COUNT)

    if not os.path.exists(OBJ_DIR):
        os.mkdir(OBJ_DIR)

    for f in FILES:
        out_bin_path = os.path.join(OBJ_DIR, os.path.basename(change_ext(f, ".o")))
        objs.append((out_bin_path, f))

    queue = [x for x in pool.map(deps_func, objs) if x is not None]

    pool.map(build_func, queue)

    if all((os.path.exists(out_bin_path) for out_bin_path, f in objs)):
        cmd = CC 
        cmd += " -o \"" 
        cmd += os.path.join(OBJ_DIR, EXECUTABLE) 
        cmd += "\" "
        cmd += ' '.join([out_bin_path for out_bin_path, f in objs]) 
        cmd += " " + LDFLAGS
        print(cmd)
        os.system(cmd)

if __name__ == '__main__':
    main()

