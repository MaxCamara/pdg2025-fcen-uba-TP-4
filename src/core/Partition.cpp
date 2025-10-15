//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-08-06 09:56:45 taubin>
//------------------------------------------------------------------------
//
// Partition.cpp
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

#include "Partition.hpp"

Partition::Partition(const int nElements):
  _nParts(0),
  _parent(),
  _size()
{
  reset(nElements);
}

void Partition::reset(const int nElements) {
  _nParts = 0;
  _parent.clear();
  _size.clear();
  if(nElements>0) {
    _nParts = nElements;
    for(int i=0;i<nElements;i++) {
      _parent.push_back(i);
      _size.push_back(1);
    }
  }
}

int Partition::getNumberOfElements() const {
  return static_cast<int>(_parent.size());
}

int Partition::getNumberOfParts() const {
  return _nParts;
}

int Partition::find(const int i) {
  if(i<0) return -1;
  if(i>=getNumberOfElements()) return -1;
  int Ri,Pj,j;
  // traverse path and find root node
  for(Ri=i;_parent[Ri]!=Ri;Ri=_parent[Ri]);
  // compress the path:
  // traverse the path again and point all the nodes to the root
  for(j=i;j!=Ri;Pj=_parent[j],_parent[j]=Ri,j=Pj);
  // return the root element as the part ID
  return Ri;
}

int Partition::join(const int i, const int j) {
  int Ri,Rj,part;
  Ri = find(i);
  Rj = find(j);
  if((part=Ri)!=Rj) {
    _nParts--;
    if(_size[Ri]>=_size[Rj]) {
      // make Ri the root of the joined part
      part = _parent[Rj] = Ri; _size[Ri] += _size[Rj]; _size[Rj] = 0;
    } else /* if(_size[Rj]>_size[Ri]) */ {
      // make Rj the root of the joined part
      part = _parent[Ri] = Rj; _size[Rj] += _size[Ri]; _size[Ri] = 0;
    }
  }
  return part;
}

int Partition::getSize(const int i) const {
  return (i<0 || i>=static_cast<int>(_parent.size()))?0:_size[i];

}
