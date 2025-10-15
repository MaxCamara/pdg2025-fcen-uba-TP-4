//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-10-14 17:12:28 taubin>
//------------------------------------------------------------------------
//
// Edges.cpp
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
#include "Edges.hpp"

// public methods

Edges::Edges(const int nV):
  _first(),
  _edge() {
  _reset(nV);
}

int Edges::getNumberOfVertices() const {
  return static_cast<int>(_first.size());
}

// the _edge array contains a triple (iV0,iV1,next) for inserted edge
int Edges::getNumberOfEdges() const {
  return static_cast<int>(_edge.size()/3);
}

int Edges::getEdge(int iV0, int iV1) const {
  // edges with the same ends are not allowed
  if(iV0==iV1) return -1;
  // check that vertices are not out of range
  int nV = getNumberOfVertices();
  if(iV0<0 || nV<=iV0) return -1;
  if(iV1<0 || nV<=iV1) return -1;
  // make sure that iV0<iV1
  if(iV0>iV1) { int iV=iV0; iV0=iV1; iV1=iV; }
  // look for iV1 in the list of iV0
  for(int j=_first[iV0];j>=0;j=/*next*/_edge[j+2])
    if(/* _edge[j]==iV0 && */ _edge[j+1]==iV1)
      // edge index is location of triple (iV0,iV1,next) in _edge
      // array, regarded as an array of triples
      return (j/3);
  return -1;
}

int Edges::getVertex0(const int iE) const {
  int nE = getNumberOfEdges();
  if(iE<0 || iE>=nE) return -1;
  return _edge[3*iE  ];
}

int Edges::getVertex1(const int iE) const {
  int nE = getNumberOfEdges();
  if(iE<0 || iE>=nE) return -1;
  return _edge[3*iE+1];
}

// protected methods

void Edges::_reset(const int nV) {
  _edge.clear();
  _first.clear();
  for(int iV=0;iV<nV;iV++)
    _first.push_back(-1);
}

int Edges::_insertEdge(int iV0, int iV1, bool grow) {
  // edges with the same ends are not allowed
  if(iV0==iV1) return -1;
  // check that vertices are not out of range
  int nV = getNumberOfVertices();
  if(grow) {
    if(iV0>=nV) nV = iV0+1;
    if(iV1>=nV) nV = iV1+1;
    while(static_cast<int>(_first.size())<nV)
      _first.push_back(-1);
  } else {
    if(iV0<0 || nV<=iV0) return -1;
    if(iV1<0 || nV<=iV1) return -1;
  }
  // make sure that iV0<iV1
  if(iV0>iV1) { int iV=iV0; iV0=iV1; iV1=iV; }
  // if the edges has already been inserted, return the previously
  // assigned edge index
  int iE = getEdge(iV0,iV1); if(iE>=0) return iE;
  // get the index of the next edge to be created
  int j = static_cast<int>(_edge.size()); iE = j/3;
  // append a new triple (iV0,iV1,*) to the _edge array 
  // and link it to the list of iV0 as the first node
  _edge.push_back(iV0);
  _edge.push_back(iV1);
  _edge.push_back(_first[iV0]);
  // _first[iV0] ->  ... (.,.,-1)
  _first[iV0] = j;
  // return the index of the new edge
  return iE;
}

// makes an array of arrays
// eFirst.size() == nV+1;
// eFirst[0] == 0
// eFirst[nV] == 2*nE
// eFirst[iV+1]-eFirst[iV] == number of 
// eStar.size() == 2*nE;
// each edge iE shows twice in the eStar array,
//    with indices 2*iE for iV0, and 2*iE+1 for iV1;
// subarray [eStar[iV+1]:eStar[iV]) contains the edge indices (2*iE,
//    or 2*iE+1) corresponding to the edges incident to iV;
void Edges::makeEdgeStars(vector<int>& eFirst, vector<int>& eStar) {
  eFirst.clear();
  eStar.clear();

  int nV = getNumberOfVertices();
  int nE = getNumberOfEdges();

  int iV,iV0,iV1,iE;

  // initialize
  eFirst.reserve(nV+1);
  for(iV=0;iV<=nV;iV++)
    eFirst.push_back(0);
  eStar.reserve(2*nE);
  for(iE=0;iE<nE;iE++) {
    eStar.push_back(-1);
    eStar.push_back(-1);
  }

  // count number of edges incident to each vertex
  vector<int> valence(nV,0);
  for(iE=0;iE<nE;iE++) {
    iV0 = _edge[3*iE  ];
    iV1 = _edge[3*iE+1];
    valence[iV0]++;
    valence[iV1]++;
  }
  // convert into initial indices
  for(iV=0;iV<nV;iV++) {
    eFirst[iV+1] = eFirst[iV]+valence[iV];
    // reset valence
    valence[iV] = 0;
  }
  // fill the eStar array
  for(iE=0;iE<nE;iE++) {
    iV0 = _edge[3*iE  ];
    eStar[eFirst[iV0]+valence[iV0]] = 2*iE  ;
    valence[iV0]++;
    iV1 = _edge[3*iE+1];
    eStar[eFirst[iV1]+valence[iV1]] = 2*iE+1;
    valence[iV1]++;
  }
}
