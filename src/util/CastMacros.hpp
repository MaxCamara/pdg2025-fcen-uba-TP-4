//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin / 3D Shape Tech LLC
//  Time-stamp: <2025-08-06 09:11:39 taubin>
//------------------------------------------------------------------------
//
// CastMacros.hpp
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

#ifndef CAST_MACROS_HPP
#define CAST_MACROS_HPP

#define C(x)   static_cast<char>(x)
#define UC(x)  static_cast<uchar>(x)
#define S(x)   static_cast<short>(x)
#define US(x)  static_cast<ushort>(x)
#define I(x)   static_cast<int>(x)
#define UI(x)  static_cast<uint>(x)
#define L(x)   static_cast<long>(x)
#define UL(x)  static_cast<unsigned long>(x)
#define ULL(x) reinterpret_cast<unsigned long long>(x)
#define F(x)   static_cast<float>(x)
#define D(x)   static_cast<double>(x)

#define RUL(x) reinterpret_cast<unsigned long>(x)

#endif // CAST_MACROS_HPP
