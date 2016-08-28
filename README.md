
[![Build status](https://ci.appveyor.com/api/projects/status/nm79pmxbjytquyx0?svg=true)](https://ci.appveyor.com/project/alejolp/extmodem)

[![Build Status](https://travis-ci.org/alejolp/extmodem.svg?branch=master)](https://travis-ci.org/alejolp/extmodem)

[![Coverity Scan Build Status](https://scan.coverity.com/projects/9834/badge.svg)](https://scan.coverity.com/projects/alejolp-extmodem)

Sound card modem for Amateur Radio AX25
=======================================

By Alejandro Santos, LU4EXT / HB3YNQ. alejolp@gmail.com.

Web page (old): http://extradio.sourceforge.net/extmodem.html

This is a modem for AFSK AX25 packet (1200 bps APRS compatible). It is capable to both send and receive packets. The main feature of this program is that it is currently running three different demodulators in parallel, increasing the quality of reception. The first modem is Thomas Sailer's multimon, the other two are described by Sivan Toledo in this QEX article: http://www.tau.ac.il/~stoledo/Bib/Pubs/QEX-JulAug-2012.pdf

Since the core of the extmodem runs the same algorithm as javAX25, the number of demodulated packets should be better than soundmodem. 

Windows Files
-------------

Windows 32-bit and 64-bit binaries can be found at: http://files.dakaraonline.org/extmodem/

Usage
-----

Open a new terminal (Start-Execute-"cmd" on windows) and ask for the program's help:

    extmodem.exe --help
    
    Allowed options:
      -h [ --help ]                     produce help message
      -d [ --debug ]                    produce debug messages
      -a [ --debugaudio ]               produce audio debug information
      -L [ --list-devices ]             list audio source devices and exit
      --config-file arg                 configuration file name
      --audio-backend arg (=portaudio)  Audio backend: alsa,loopback,portaudio,wave
                                        ,null
      --kiss-tcp-port arg (=6666)       set KISS TCP listening port
      --agwpe-tcp-port arg (=8000)      set AGWPE emulator TCP listening port
      --ptt-mode arg (=serial)          PTT mode: serial,parallel,gpio,hamlib,null
      --ptt-port arg (=/dev/ttyS0)      set serial/parallel port PTT file name
      --tx-delay arg (=200)             set tx-delay in ms
      --tx-tail arg (=50)               set tx-tail in ms
      -s [ --sample-rate ] arg (=22050) sample rate
      --in-chan arg (=2)                input channel count
      --out-chan arg (=2)               output channel count
      --frames-per-buffer arg (=8192)   frames per buffer, bigger increases latency
      --hamlib-model arg (=-1)          hamlib Rig Model, -1 for auto detection
      --audio-mult-factor arg (=1)      Audio multiplication factor as float
      --enabled-in-channels arg (=-1)   Enabled input channels as a bitfield (1: 
                                        left, 2: right, 3: both), -1 for all
      --backend-wave-in-file arg        Backend WAVE: input file
      --alsa-in-dev arg (=default)      ALSA device string, only for ALSA backend 
                                        (not PortAudio!)
      --alsa-out-dev arg (=default)     ALSA device string, only for ALSA backend 
                                        (not PortAudio!)
      --pa-in-dev arg (=-1)             PortAudio input device id number. See 
                                        --list-devices
      --pa-out-dev arg (=-1)            PortAudio output device id number. See 
                                        --list-devices


You should at least set the PTT Port, Usually COMx on Windows and /dev/ttySX on Linux and Unix.

The program prints the received packets on the screen having the -d switch.

### Testing with DTMF

Extmodem provides a DTMF decoder for testing. You can use your handheld radio to test the decoder by sending DTMF codes. You should be able to see on the screen the decoded DTMF code.

### Selecting the sound device to use.

Using the ALSA backend you can use the --alsa-in-dev and --alsa-out-dev command line options to select specific input and output sound devices, ie, hw:1,0. This option only works with the ALSA backend, not PortAudio backend. To get a list of ALSA PCM input and output devices you can use the aplay and arecord ALSA command line utilities:

    $ arecord -L
    $ aplay -L

Use the complete line header as a device string. For example:

    $ arecord -L
    plughw:CARD=C920,DEV=0
        HD Pro Webcam C920, USB Audio
        Hardware device with all software conversions
    
    # ./extmodem --audio-backend=alsa --alsa-in-dev "plughw:CARD=C920,DEV=0"

The Portaudio backend uses by default the default sound device, which is usually the default ALSA device. To select a different PortAudio device you can first get a list of devices with the -L option:

    $ ./extmodem --audio-backend=portaudio --list-devices

Take note of the Device NUMBER and then use that number with the --pa-in-dev and --pa-out-dev options:

    $ ./extmodem --audio-backend=portaudio --pa-in-dev=44 --pa-out-dev=45

### APRX - TCP Kiss Support

Extmodem opens a TCP KISS interface running at port 6666. You can use any TCP KISS capable program to send and receive packets, ie "aprx":

    <interface>
      tcp-device 127.0.0.1 6666 KISS
      callsign LU0EXT-1
      tx-ok false
    </interface>

### AGWPE Support

You can use UI-VIEW32 with extmodem. This program supports a preliminary version of the AGWPE protocol, effectively emulating its behaviour. It opens a TCP socket on port 8000, the AGWPE default. 

### Example on the RPi

This is my command line on the Raspberry Pi. I am using an external USB sound card interface:

    ./extmodem --ptt-mode null --audio-backend alsa --alsa-in-dev "hw:1" -s 44100 --out-chan 2 --in-chan 1 --debug

Build from source
-----------------

### Linux, get the source

On your Linux terminal write:

    git clone https://github.com/alejolp/extmodem.git

The source code should be at the "extmodem" directory.

### Compile

On Linux systems you can compile with g++ using cmake. You need:

* Modern C++ compiler with C++11 support
* CMAKE
* Boost C++ library development files
* PortAudio VERSION 19 library development files

On Debian and Ubuntu you can install the required packages easily:

    apt-get install cmake build-essential g++ portaudio19-dev libboost-system-dev libboost-thread-dev libboost-program-options-dev libhamlib-dev

Then just type, at the "extmodem" directory:

    mkdir build
    cd build
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j4

The executable should be at  `build/extmodem`

### Raspberry Pi Support

I'm running extmodem on my RPi. To compile with cmake you can:

    mkdir build
    cd build
    CXX=$(which g++-4.8) CC=$(which gcc-4.8) CFLAGS=-O2 CXXFLAGS=-O2 cmake ..
    make -j4

License
-------

This program borrows code from Sivan Toledo's javAX25 and Thomas Sailer's multimon. Thank you folks!

> 
>     Copyright (C) 2013 Alejandro Santos LU4EXT, alejolp@gmail.com.
> 
>     This program is free software; you can redistribute it and/or modify
>     it under the terms of the GNU General Public License as published by
>     the Free Software Foundation; either version 2 of the License, or
>     (at your option) any later version.
> 
>     This program is distributed in the hope that it will be useful,
>     but WITHOUT ANY WARRANTY; without even the implied warranty of
>     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
>     GNU General Public License for more details.
> 
>     You should have received a copy of the GNU General Public License
>     along with this program; if not, write to the Free Software
>     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
> 

