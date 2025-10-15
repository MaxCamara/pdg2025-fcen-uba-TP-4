//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin / 3D Shape Tech LLC
//  Time-stamp: <2025-08-06 09:11:40 taubin>
//------------------------------------------------------------------------
//
// Endian.hpp
//
// Software developed for the course
// Digital Geometry Processing
// Copyright (c) 2025, Gabriel Taubin
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//     * Redistributions of source code must retain the above
//       copyright notice, this list of conditions and the following
//       disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials
//       provided with the distribution.
//     * Neither the name of the Brown University nor the names of its
//       contributors may be used to endorse or promote products
//       derived from this software without specific prior written
//       permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL GABRIEL
// TAUBIN BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
// USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.

#ifndef ENDIAN_HPP
#define ENDIAN_HPP

typedef unsigned char  uchar;
typedef unsigned short ushort;
typedef unsigned int   uint;
typedef unsigned long  ulong;

namespace Endian {

  union SingleValueBuffer {
    char    c[8];
    uchar  uc[8];
    short   s[4];
    ushort us[4];
    int     i[2];
    uint   ui[2];
    long    l[1];
    ulong  ul[1];
    float   f[2];
    double  d[1];
  };

  bool toBool(const char b[/*1*/]);
  bool toBool(const unsigned char b[/*1*/]);
  bool toBool(SingleValueBuffer& buff);

  // short & ushort
  SingleValueBuffer& swap2(SingleValueBuffer& buff, const bool all=false);

#define swapShort  swap2
#define swapUShort swap2

  // int, uint & float
  SingleValueBuffer& swap4(SingleValueBuffer& buff, const bool all=false);

#define swapInt    swap4
#define swapUInt   swap4
#define swapFloat  swap4

  // long & double
  SingleValueBuffer& swap8(SingleValueBuffer& buff);

#define swapLong   swap8
#define swapDouble swap8

  bool isLittleEndianSystem();

};

#endif // ENDIAN_HPP
