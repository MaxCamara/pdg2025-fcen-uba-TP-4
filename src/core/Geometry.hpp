//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-10-14 17:12:28 taubin>
//------------------------------------------------------------------------
//
// Geometry.hpp
//
// Written by: Gabriel Taubin
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

#ifndef _GEOMETRY_HPP_
#define _GEOMETRY_HPP_

#include <vector>
#include "Edges.hpp"

using namespace std;

class Geometry {

private:

  static void _computeFaceNormal
  (const vector<float>& coord, const vector<int>& coordIndex,
   const int i0, const int i1, float& w0, float& w1, float& w2);

public:

  static void computeFaceCentroids
  (const vector<float>& coord, const vector<int>& coordIndex,
   vector<float>& faceCentroid);

  static void computeNormalsPerFace
  (const vector<float>& coord, const vector<int>& coordIndex,
   vector<float>& normalPerFace);

  static void computeNormalsPerVertex
  (const vector<float>& coord, const vector<int>& coordIndex,
   vector<float>& normalPerVertex);

  static void computeEdgeLengths
  (const vector<float>& coord, const Edges& edges,
   vector<float>& edgeLengths);

  static float computeAverageEdgeLength
  (const vector<float>& coord, const Edges& edges);
  static float computeDiameter
  (const vector<float>& coord);
  
  static void computeValences
  (const Edges& edges,
   vector<int>& valences);

  static void invertVector(vector<float>& values);

  static void deleteUnusedCoord
  (vector<float>& coord, vector<int>& coordIndex);

  static bool isTriangulated(vector<int>& coordIndex);
  
  static void triangulate
  (vector<float>& coord, vector<int>& coordIndex);

  static void removeValence3
  (vector<float>& coord, vector<int>& coordIndex);

  static void invertOrientation
  (vector<int>& propertyIndex);

};

#endif /* _GEOMETRY_HPP_ */
