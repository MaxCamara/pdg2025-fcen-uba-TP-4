//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin / 3D Shape Tech LLC
//  Time-stamp: <2025-08-06 09:57:36 taubin>
//------------------------------------------------------------------------
//
// Tokenizer.hpp
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

#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <wrl/Types.hpp>
#include <wrl/Node.hpp>

// abstract class
// use TokenizerFile or TokenizerString instead
class Tokenizer : public string {

private:

  bool _skip;

  virtual char getc() = 0;

public:

  Tokenizer();

  bool get();
  void get(const string& errMsg);
  bool getline();
  void nextline();
  bool getBool(bool& b);
  bool getInt(int& i);
  bool getUInt(unsigned int& ui);
  bool getFloat(float& f);
  bool getColor(Color& c);
  bool getVec3f(Vec3f& v);
  bool getVec4f(Vec4f& v);
  bool getVec2f(Vec2f& v);
  bool equals(const char* str);
  bool expecting(const string& str);
  bool expecting(const char* str);
  void setSkipComments(const bool value);

};

#endif // TOKENIZER_HPP
