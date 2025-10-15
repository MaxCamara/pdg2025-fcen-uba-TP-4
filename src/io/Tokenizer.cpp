//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin / 3D Shape Tech LLC
//  Time-stamp: <2025-08-06 09:57:36 taubin>
//------------------------------------------------------------------------
//
// Tokenizer.cpp
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

#include <stdio.h>
#include "Tokenizer.hpp"
#include "StrException.hpp"

Tokenizer::Tokenizer(): _skip(true) {
}

void Tokenizer::setSkipComments(const bool value) {
  _skip = value;
}

bool Tokenizer::get() {
  char c='\0';
  do {
    clear();
    // skip blank space
    while((c=getc())!=EOF)
      if(!(c==' ' || c=='\t' || c=='\n' || c=='\r' || c==',' || c=='\015'))
        { push_back(c); break; }
    // collect token characters
    while(((c=getc())!=EOF) &&
          !(c==' ' || c=='\t' || c=='\n' || c=='\r' || c==',' || c=='\015'))
      push_back(c);
    // if comment, get the rest of the line, including blank spaces
    if(size()>0 && (*this)[0]=='#') {
      // last blank space character read is not in tkn yet
      if(c!='\n')
        push_back(c);
      while((c=getc())!=EOF && c!='\n')
        push_back(c);
    }
    // terminate the string
    // push_back('\0');
  } while(_skip && length()>0 && *(begin())=='#');
  
  return (length()>0)?true:false;
}

void Tokenizer::get(const string& errMsg) /* throw(StrException *) */ {
  if(get()==false) throw new StrException(errMsg);
}

bool Tokenizer::getline() {
  clear();
  char c='\0';
  while((c=getc())!=EOF && c!='\n') {
    push_back(c);
  }
  return (length()>0)?true:false;
}

void Tokenizer::nextline() {
  char c='\0';
  while((c=getc())!=EOF && c!='\n');
}

bool Tokenizer::getBool(bool& b) {
  bool success = false;
  if(get()) {
    if(this->equals("t") || this->equals("true") ||
       this->equals("T") || this->equals("TRUE")) {
      b = true;
      success = true;
       } else if(this->equals("f") || this->equals("false") ||
                 this->equals("F") || this->equals("FALSE")) {
      b = false;
      success = true;
    }
  }
  return success;
}

bool Tokenizer::getInt(int& i) {
  bool success =
    (get() && (sscanf(&((*this)[0]),"%d",&i)==1));
  return success;
}

bool Tokenizer::getUInt(unsigned int& ui) {
  bool success =
    (get() && (sscanf(&((*this)[0]),"%u",&ui)==1));
  return success;
}

bool Tokenizer::getFloat(float& f) {
  bool success =
    (get() && (sscanf(&((*this)[0]),"%f",&f)==1));
  return success;
}

bool Tokenizer::getColor(Color& c) {
  bool success =
    (get() && (sscanf(&((*this)[0]),"%f",&(c.r))==1)) &&
    (get() && (sscanf(&((*this)[0]),"%f",&(c.g))==1)) &&
    (get() && (sscanf(&((*this)[0]),"%f",&(c.b))==1));
  return success;
}

bool Tokenizer::getVec4f(Vec4f& v) {
  bool success =
    (get() && (sscanf(&((*this)[0]),"%f",&(v.x))==1)) &&
    (get() && (sscanf(&((*this)[0]),"%f",&(v.y))==1)) &&
    (get() && (sscanf(&((*this)[0]),"%f",&(v.z))==1)) &&
    (get() && (sscanf(&((*this)[0]),"%f",&(v.w))==1));
  return success;
}

bool Tokenizer::getVec3f(Vec3f& v) {
  bool success =
    (get() && (sscanf(&((*this)[0]),"%f",&(v.x))==1)) &&
    (get() && (sscanf(&((*this)[0]),"%f",&(v.y))==1)) &&
    (get() && (sscanf(&((*this)[0]),"%f",&(v.z))==1));
  return success;
}

bool Tokenizer::getVec2f(Vec2f& v) {
  bool success =
    (get() && (sscanf(&((*this)[0]),"%f",&(v.x))==1)) &&
    (get() && (sscanf(&((*this)[0]),"%f",&(v.y))==1));
  return success;
}

bool Tokenizer::equals(const char* str) {
  return ((*this)==str);
}

bool Tokenizer::expecting(const string& str) {
  return get() && this->equals(str.c_str());
}

bool Tokenizer::expecting(const char* str) {
  return get() && this->equals(str);
}
