
Sound card modem for Amateur Radio AX25
=======================================

By Alejandro Santos, LU4EXT / HB3YNQ. alejolp@gmail.com.

Web page (old): http://extradio.sourceforge.net/extmodem.html

This is a modem for AFSK AX25 packet (1200 bps APRS compatible). It is capable to both send and receive packets. The main feature of this program is that it is currently running three different demodulators in parallel, increasing the quality of reception. The first modem is Thomas Sailer's multimon, the other two are described by Sivan Toledo in this QEX article: http://www.tau.ac.il/~stoledo/Bib/Pubs/QEX-JulAug-2012.pdf

Since the core of the extmodem runs the same algorithm as javAX25, the number of demodulated packets should be better than soundmodem. 

Windows Files
-------------

I no longer provide Windows binaries. The program should compile cleanly on VC2010. Help needed.

Usage
-----

Open a new terminal (Start-Execute-"cmd" on windows) and ask for the program's help:

    extmodem.exe --help
    
    Allowed options:
      -h [ --help ]                     produce help message
      -d [ --debug ]                    produce debug messages
      --config-file arg                 configuration file name
      --audio-backend arg (=portaudio)  Audio backend: portaudio,alsa
      --kiss-tcp-port arg (=6666)       set KISS TCP listening port
      --agwpe-tcp-port arg (=8000)      set AGWPE emulator TCP listening port
      --ptt-mode arg (=serial)          PTT mode: serial,parallel,gpio,hamlib,null
      --ptt-port arg (=/dev/ttyS0)      set serial/parallel port PTT file name
      --tx-delay arg (=200)             set tx-delay in ms
      --tx-tail arg (=50)               set tx-tail in ms
      -s [ --sample-rate ] arg (=22050) sample rate
      --in-chan arg (=2)                input channel count
      --out-chan arg (=2)               output channel count
      --alsa-device arg (=default)      ALSA device string
      --frames-per-buffer arg (=8192)   frames per buffer, bigger increases latency
      --hamlib-model arg (=-1)          hamlib Rig Model, -1 for auto detection

You should at least set the PTT Port, Usually COMx on Windows and /dev/ttySX on Linux and Unix.

The program prints the received packets on the screen having the -d switch.

### Testing with DTMF

Extmodem provides a DTMF decoder for testing. You can use your handheld radio to test the decoder by sending DTMF codes. You should be able to see on the screen the decoded DTMF code.

### Selecting the sound device to use.

Using the alsa backend you can use the --alsa-device command line option to select another sound device, ie, hw:1. This option only works with the ALSA backend.

The portaudio backend always use the default sound card, which in turn by default uses the default ALSA device. To select a different ALSA device you should edit your asoundrc ALSA file. For example:

    # cat /etc/asound.conf 

    pcm.!default {
            type hw
            card 1
    }

    ctl.!default {
            type hw           
            card 1
    }

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

    ./extmodem --ptt-mode null --audio-backend alsa --alsa-device "hw:1" -s 44100 --out-chan 2 --in-chan 1 --debug

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

    aptitude install cmake build-essential g++ portaudio19-dev libboost-system-dev libboost-thread-dev libboost-program-options-dev

Then just type, at the "extmodem" directory:

    mkdir build
    cd build
    CFLAGS=-O2 CXXFLAGS=-O2 cmake ..
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

