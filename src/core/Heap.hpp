//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-08-06 09:56:43 taubin>
//------------------------------------------------------------------------
//
// Heap.hpp
//
// Software developed for the course
// Digital Geometry Processing
// Copyright (c) 2015, Gabriel Taubin
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//     * Redistributions of source code must retain the above copyright
//       notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above copyright
//       notice, this list of conditions and the following disclaimer in the
//       documentation and/or other materials provided with the distribution.
//     * Neither the name of the Brown University nor the
//       names of its contributors may be used to endorse or promote products
//       derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL GABRIEL TAUBIN BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef _HEAP_HPP_
#define _HEAP_HPP_

#include <vector>

using namespace std;

class Heap {

private: // static

  static int LEFT_SON(int i);
  static int RIGHT_SON(int i);

public:

  Heap();

  void  reset();
  void  swap(Heap& H);
  int   length();
  void  del(int indx);
  int   replaceKey(int indx, float fKey, int iKey);
  int   delMin();
  int   getMin();
  float getLastFKey();
  int   getLastIKey();
  int   add(float fKey, int iKey);
  void  build();
  void  sort();

private:

  void  _switch(int i, int j);
  void  _down(int father);
  void  _up();
  int   _n();
  bool  _less(int i, int j);
  void  _del(int i);

private:

  int           _last;
  vector<float> _fKey;
  vector<int>   _iKey;
  vector<int>   _perm;
  vector<int>   _invp;

};

#endif // _HEAP_HPP_
