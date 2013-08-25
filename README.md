
extmodem
========

  http://extradio.sourceforge.net/extmodem.html

Sound card modem for Amateur Radio AX25.

Windows Files:
==============

Precompiled Windows binaries (EXE) can be found here:

  http://sourceforge.net/projects/extradio/files/extmodem/

You may need the VC++ 2010 runtime, available here:

  http://www.microsoft.com/en-us/download/details.aspx?id=5555

Build from source:
==================

LINUX, GET THE SOURCE CODE:
---------------------------

On your Linux terminal write:

  git clone https://github.com/alejolp/extmodem.git

The source code should be at the "extmodem" directory.

COMPILE:
--------

On Linux systems you can compile with g++ using the "build.py" script. You need:

 - Python to run the build script
 - Boost C++ library development files
 - PortAudio VERSION 19 library development files

On Debian and Ubuntu you can install the required packages easily:

  aptitude install build-essential g++ portaudio19-dev libboost-system-dev libboost-thread-dev

Then just type, at the "extmodem" directory:

  python build.py

The executable should be at bin/extradio

Usage
=====

The program prints the received packets on the screen.

It also opens a TCP KISS interface running at port 6666. You can use any TCP KISS capable program
to receive packets, ie "aprx":

  <interface>
    tcp-device 127.0.0.1 6666 KISS
    callsign LU0EXT-1
    tx-ok false
  </interface>


Licence
=======

This program borrows code from Sivan Toledo's javAX25 and Thomas Sailer's multimon. Thank you folks!

Copyright (C) 2013 Alejandro Santos LU4EXT, alejolp@gmail.com.

     This program is free software; you can redistribute it and/or modify
     it under the terms of the GNU General Public License as published by
     the Free Software Foundation; either version 2 of the License, or
     (at your option) any later version.

     This program is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     GNU General Public License for more details.

     You should have received a copy of the GNU General Public License
     along with this program; if not, write to the Free Software
     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

