//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-08-05 23:13:01 taubin>
//------------------------------------------------------------------------
//
// HalfEdges.cpp (Assignment 3)
//
// Written by: <Your Name>
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

#include <math.h>
#include "HalfEdges.hpp"
// #include "Graph.hpp"

// TODO Mon Mar 6 2023
// - merge your code from Assignment 2

HalfEdges::HalfEdges(const int nVertices, const vector<int>&  coordIndex):
  Edges(nVertices),
  _coordIndex(coordIndex),
  _twin()
{
  // TODO
}

int HalfEdges::getNumberOfCorners() const {
  return static_cast<int>(_coordIndex.size());
}

int HalfEdges::getFace(const int iC) const {
  // TODO
  return -1;
}

int HalfEdges::getSrc(const int iC) const {
  // TODO
  return -1;
}

int HalfEdges::getDst(const int iC) const {
  // TODO
  return -1;
}

int HalfEdges::getNext(const int iC) const {
  // TODO
  return -1;
}

int HalfEdges::getPrev(const int iC) const {
  // TODO
  return -1;
}

int HalfEdges::getTwin(const int iC) const {
  // TODO
  return -1;
}

// represent the half edge as an array of lists, with one list
// associated with each edge

int HalfEdges::getNumberOfEdgeHalfEdges(const int iE) const {
  // TODO
  return 0;  
}

int HalfEdges::getEdgeHalfEdge(const int iE, const int j) const {
  // TODO
  return -1;
}

// TODO Mon Mar 6 2023
// - new functions to implement

bool HalfEdges::isOriented(const int iC) const {
  // iC     : iV00->iV01
  // iCtwin : iV10->iV11

  // if (twin half edges are consistently oriented)
  /* \                  / */
  /*  \ iV01 <-- iV00  /  */
  /*   X ------------ X   */
  /*  / iV10 --> iV11  \  */
  /* /                  \ */
  // return true;
  
  // else if (twin half edges are not consistently oriented)
  /* \                  / */
  /*  \ iV01 <-- iV00  /  */
  /*   X ------------ X   */
  /*  / iV11 <-- iV10  \  */
  /* /                  \ */
  // return false;
}

// half-edge method getFaceSize()
int HalfEdges::getFaceSize(const int iC) const {
  // TODO
  return 0;
}
  
int HalfEdges::getNumberOfFacesEdge(const int iE) const {
  // TODO
  return 0;
}

bool HalfEdges::isBoundaryEdge(const int iE) const {
  // TODO
  return false;
}

bool HalfEdges::isRegularEdge(const int iE) const {
  // TODO
  return false;
}

bool HalfEdges::isSingularEdge(const int iE) const {
  // TODO
  return false;
}

// HINTS

// - it is best to determine the return values of these methods in the
//   constructor and save them in private variables

bool HalfEdges::hasBoundaryEdges() const {
  // TODO
  return false;
}

bool HalfEdges::hasRegularEdges() const {
  // TODO
  return false;
}

bool HalfEdges::hasSingularEdges() const {
  // TODO
  return false;
}
