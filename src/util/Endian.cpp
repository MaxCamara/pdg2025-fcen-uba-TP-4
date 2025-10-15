//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin / 3D Shape Tech LLC
//  Time-stamp: <2025-08-06 09:11:40 taubin>
//------------------------------------------------------------------------
//
// Endian.cpp
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

#include "Endian.hpp"

bool Endian::toBool(const char b[/*1*/]) {
  return (b[0] != 0);
}

bool Endian::toBool(const unsigned char b[/*1*/]) {
  return (b[0] != 0);
}

bool Endian::toBool(SingleValueBuffer& buff) {
  return (buff.uc[0]!=0);
}

Endian::SingleValueBuffer& Endian::swap2
(Endian::SingleValueBuffer& buff, const bool all) {
  uchar tmp;
  tmp = buff.uc[0]; buff.uc[0] = buff.uc[1]; buff.uc[1] = tmp;
  if(all) {
    tmp = buff.uc[2]; buff.uc[2] = buff.uc[3]; buff.uc[3] = tmp;
    tmp = buff.uc[4]; buff.uc[4] = buff.uc[5]; buff.uc[5] = tmp;
    tmp = buff.uc[6]; buff.uc[6] = buff.uc[7]; buff.uc[7] = tmp;
  }
  return buff;
}

Endian::SingleValueBuffer& Endian::swap4
(Endian::SingleValueBuffer& buff, const bool all) {
  uchar tmp;
  tmp = buff.uc[0]; buff.uc[0] = buff.uc[3]; buff.uc[3] = tmp;
  tmp = buff.uc[1]; buff.uc[1] = buff.uc[2]; buff.uc[2] = tmp;
  if(all) {
    tmp = buff.uc[4]; buff.uc[4] = buff.uc[7]; buff.uc[7] = tmp;
    tmp = buff.uc[5]; buff.uc[5] = buff.uc[6]; buff.uc[6] = tmp;
  }
  return buff;
}

Endian::SingleValueBuffer& Endian::swap8
(Endian::SingleValueBuffer& buff) {
  uchar tmp;
  tmp = buff.uc[0]; buff.uc[0] = buff.uc[7]; buff.uc[7] = tmp;
  tmp = buff.uc[1]; buff.uc[1] = buff.uc[6]; buff.uc[6] = tmp;
  tmp = buff.uc[2]; buff.uc[2] = buff.uc[5]; buff.uc[5] = tmp;
  tmp = buff.uc[3]; buff.uc[3] = buff.uc[4]; buff.uc[4] = tmp;
  return buff;
}


//////////////////////////////////////////////////////////////////////
// static
bool Endian::isLittleEndianSystem() {
  union {
    int  i;
    char c[sizeof(int)];
  } x ;
  x.i = 1;
  return (x.c[0]==1);
}
