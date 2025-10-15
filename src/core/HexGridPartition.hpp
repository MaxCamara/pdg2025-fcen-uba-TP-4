//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-08-07 19:55:02 taubin>
//------------------------------------------------------------------------
//
// HexGridPartition.hpp
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

#ifndef HEX_GRID_PARTITION_HPP
#define HEX_GRID_PARTITION_HPP

#include <vector>
#include <map>
#include "wrl/Types.hpp"

using namespace std;

class HexGridPartition {
public:

  HexGridPartition
  (const Vec3f& min, const Vec3f& max, const int resolution);

  HexGridPartition
  (const Vec3f& center, const Vec3f& size,
   const int resolution, const float scale, const bool cube);

  bool   insertPoints
  (const vector<float>& coord);
  bool   sample
  (vector<float>& coordSample, vector<int>* vMap=nullptr);

  bool   insertPoints
  (const vector<float>& coord, const vector<float>& normal);
  bool   sample
  (vector<float>& coordSample, vector<float>& normalSample,
   vector<int>* vMap=nullptr);

  int    getNumberOfCells() { return static_cast<int>(_first.size()); }
  int    getNumberOfVertices();

  int    getNumberOfPoints();
  int    getFirst(const int ix, const int iy, const int iz);
  map<int,int>& getFirstMap() { return _first; }

  int    getNext(const int iPoint);

  Vec3f& getMin()   { return   _min; }
  Vec3f& getMax()   { return   _max; }

  int    getResolution() { return _resolution; }

private:
  Vec3f        _min;
  Vec3f        _max;
  int          _resolution;

  int          _nPointsInsideBox;
  int          _nPointsOutsideBox;

  vector<int>  _next;
  map<int,int> _first;

  const vector<float>* _coord;
  const vector<float>* _normal;
};

#endif // HEX_GRID_PARTITION_HPP
