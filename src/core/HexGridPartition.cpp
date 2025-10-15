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

#include <cmath>
#include <iostream>
#include "HexGridPartition.hpp"

HexGridPartition::HexGridPartition
(const Vec3f& min, const Vec3f& max, const int resolution):
  _min(min),
  _max(max),
  _resolution(resolution),
  _nPointsInsideBox(0),
  _nPointsOutsideBox(0),
  _next(),
  _first(),
  _coord(nullptr),
  _normal(nullptr) {

}

HexGridPartition::HexGridPartition
(const Vec3f& center, const Vec3f& size,
 const int resolution, const float scale, const bool cube):
  _min(),
  _max(),
  _resolution(resolution),
  _nPointsInsideBox(0),
  _nPointsOutsideBox(0),
  _next(),
  _first(),
  _coord(nullptr),
  _normal(nullptr) {
  float dx=size.x/2.0f, dy=size.y/2.0f, dz=size.z/2.0f;
  float dMax = dx; if(dy>dMax) dMax=dy; if(dz>dMax) dMax=dz;
  if(cube      ) { dx = dy = dz = dMax; }
  if(scale>0.0f) { dx *= scale; dy *= scale; dz *= scale; }
  _min.x = center.x-dx; _min.y = center.y-dy; _min.z = center.z-dz;
  _max.x = center.x+dx; _max.y = center.y+dy; _max.z = center.z+dz;
}

bool HexGridPartition::insertPoints(const vector<float>& coord) {
  _nPointsInsideBox = 0;
  _nPointsOutsideBox = 0;
  _next.clear();
  _first.clear();
  if(_resolution<1) return false; // failure
  int nPoints = static_cast<int>(coord.size()/3);
  float dx = _max.x-_min.x;
  float dy = _max.y-_min.y;
  float dz = _max.z-_min.z;
  if(nPoints<=0 || dx<=0.0f || dy<=0.0f || dz<=0.0f) {
    _coord = nullptr;
    _normal = nullptr;
    return false; // failure
  }

  _coord  = &coord;
  _next.resize(nPoints,-1);

  int N  = _resolution;
  int n0 = N;
  int n1 = N;
  int n2 = N;
  int iCell,iPoint,ix,iy,iz;
  float x,y,z;
  for(iPoint=0;iPoint<nPoints;iPoint++) {
    if((x = (*_coord)[3*iPoint  ])<_min.x || x>_max.x) continue;
    if((y = (*_coord)[3*iPoint+1])<_min.y || y>_max.y) continue;
    if((z = (*_coord)[3*iPoint+2])<_min.z || z>_max.z) continue;
    ix = static_cast<int>((n0*(x-_min.x))/dx);
    iy = static_cast<int>((n1*(y-_min.y))/dy);
    iz = static_cast<int>((n2*(z-_min.z))/dz);
    if(ix<0 || ix>=n0 || iy<0 || iy>=n0 || iz<0 || iz>=n0) {
      _nPointsOutsideBox++;
      continue;
    }
    _nPointsInsideBox++;
    iCell = ix+n0*(iy+n1*iz); // 0<=iCell<(N+1)*(N+1)*(N+1)
    if(_first.count(iCell)==1) {
      _next[iPoint] = _first[iCell];
    } /* else { _next[iPoint] = -1; } */
    _first[iCell] = iPoint;
  }

  return true; // success
}

bool HexGridPartition::sample
(vector<float>& coordSample, vector<int>* vMap) {

  if(_coord==nullptr) return false;

  coordSample.clear();

  if(vMap!=nullptr) {
    vMap->clear();
    int nPoints = static_cast<int>(_coord->size()/3);
    vMap->resize(nPoints,-1);
  }
  
  // int N=_resolution,n0=N,n1=N,n2=N,ix,iy,iz,iCell;
  int k,iPointFirst,iPoint,nPointsCell,iPointMin,iSamplePoint;
  float xMean,yMean,zMean,fPointsInCell,dx,dy,dz,dPoint2,dMin2;
  map<int,int>::iterator iMap = _first.begin();
  for(;iMap!=_first.end();iMap++) {
    // iCell = iMap->first;
    iPointFirst = iMap->second;
    iSamplePoint = static_cast<int>(coordSample.size()/3);

    // count number of points in cell
    // and mean coords of points contained in the cell
    nPointsCell = 0;
    xMean=yMean=zMean=0.0f;
    for(iPoint=iPointFirst;iPoint>=0;iPoint=_next[iPoint]) {
      xMean += (*_coord)[3*iPoint  ];
      yMean += (*_coord)[3*iPoint+1];
      zMean += (*_coord)[3*iPoint+2];
      nPointsCell++;
      if(vMap!=nullptr) (*vMap)[iPoint] = iSamplePoint;
    }
    fPointsInCell = static_cast<float>(nPointsCell);
    xMean /= fPointsInCell;
    yMean /= fPointsInCell;
    zMean /= fPointsInCell;

    // find amongst the points contained in the cell, the closest to the mean
    dMin2 = std::numeric_limits<float>::max();
    iPointMin = -1;
    for(iPoint=iPointFirst;iPoint>=0;iPoint=_next[iPoint]) {
      dx = (*_coord)[3*iPoint  ]-xMean;
      dy = (*_coord)[3*iPoint+1]-yMean;
      dz = (*_coord)[3*iPoint+2]-zMean;
      dPoint2 = dx*dx+dy*dy+dz*dz;
      if(dPoint2<dMin2) {
        dMin2 = dPoint2;
        iPointMin = iPoint;
      }
    }
    for(k=0;k<3;k++) {
      coordSample.push_back((*_coord)[3*iPointMin+k]);
    }
  }
  return true;
}

bool HexGridPartition::insertPoints
(const vector<float>& coord, const vector<float>& normal) {
  bool success = false;
  int nPoints = static_cast<int>(coord.size()/3);
  int nNormal = static_cast<int>(normal.size()/3);
  if(nPoints>0 && nNormal==nPoints) {
    success = insertPoints(coord);
  }
  if(success)  {
    _coord  = &coord;
    _normal = &normal;
  } else {
    _coord = nullptr;
    _normal = nullptr;
  }
  return success;
}

bool HexGridPartition::sample
(vector<float>& coordSample, vector<float>& normalSample, vector<int>* vMap) {

  if(_coord==nullptr || _normal==nullptr) return false;

  coordSample.clear();
  normalSample.clear();

  if(vMap!=nullptr) {
    vMap->clear();
    int nPoints = static_cast<int>(_coord->size()/3);
    vMap->resize(nPoints,-1);
  }
  
  // int N=_resolution,n0=N,n1=N,n2=N,ix,iy,iz,iCell;
  int k,iPointFirst,iPoint,nPointsCell,iPointMin,iSamplePoint;
  float xMean,yMean,zMean,fPointsInCell,dx,dy,dz,dPoint2,dMin2;
  map<int,int>::iterator iMap = _first.begin();
  for(;iMap!=_first.end();iMap++) {
    // iCell = iMap->first;
    iPointFirst = iMap->second;
    iSamplePoint = static_cast<int>(coordSample.size()/3);

    // count number of points in cell
    // and mean coords of points contained in the cell
    nPointsCell = 0;
    xMean=yMean=zMean=0.0f;
    for(iPoint=iPointFirst;iPoint>=0;iPoint=_next[iPoint]) {
      xMean += (*_coord)[3*iPoint  ];
      yMean += (*_coord)[3*iPoint+1];
      zMean += (*_coord)[3*iPoint+2];
      nPointsCell++;
      if(vMap!=nullptr) (*vMap)[iPoint] = iSamplePoint;
    }
    fPointsInCell = static_cast<float>(nPointsCell);
    xMean /= fPointsInCell;
    yMean /= fPointsInCell;
    zMean /= fPointsInCell;

    // find amongst the points contained in the cell, the closest to the mean
    dMin2 = std::numeric_limits<float>::max();
    iPointMin = -1;
    for(iPoint=iPointFirst;iPoint>=0;iPoint=_next[iPoint]) {
      dx = (*_coord)[3*iPoint  ]-xMean;
      dy = (*_coord)[3*iPoint+1]-yMean;
      dz = (*_coord)[3*iPoint+2]-zMean;
      dPoint2 = dx*dx+dy*dy+dz*dz;
      if(dPoint2<dMin2) {
        dMin2 = dPoint2;
        iPointMin = iPoint;
      }
    }
    for(k=0;k<3;k++) {
      coordSample.push_back((*_coord)[3*iPointMin+k]);
      normalSample.push_back((*_normal)[3*iPointMin+k]);
    }
  }
  return true;
}

int HexGridPartition::getNumberOfPoints() {
  if(_coord==nullptr || _normal==nullptr || _coord->size()!=_normal->size())
    return 0;
  else
    return static_cast<int>(_coord->size()/3);
}

int HexGridPartition::getFirst(const int ix, const int iy, const int iz) {
  int N  = _resolution, n0 = N, n1 = N /*, n2 = N*/;
  if(ix<0 || ix>=n0 || iy<0 || iy>=n0 || iz<0 || iz>=n0) return -1;
  int iCell = ix+n0*(iy+n1*iz); // 0<=iCell<(N+1)*(N+1)*(N+1)
  if(_first.count(iCell)==0) return -1; 
  return _first[iCell];
}

int HexGridPartition::getNext(const int iPoint) {
  int nPoints = getNumberOfPoints();
  if(iPoint<0 || iPoint>=nPoints) return -1;
  return _next[iPoint];
}


int HexGridPartition::getNumberOfVertices() {
  int nV = 0;
  int h,h0,h1,h2,iCell,iC,iCx,iCy,iCz,iVx,iVy,iVz,iVertex;
  int N = _resolution;
  map<int,int> vMap;
  map<int,int>::iterator iMap;
  for(iMap=_first.begin();iMap!=_first.end();iMap++) {
    iCell       = iMap->first;
    iC = iCell; iCx = iC%N; iC/=N; iCy = iC%N; iC/=N; iCz = iC;
    //
    // 0 ---- 1
    // |\     |\
    // | 2 ---- 3
    // 4 +--- 5 |
    //  \|     \|
    //   6 ---- 7
    //
    for(h=0;h<8;h++) {
      h0 = (h  )%2; iVx = iCx+h0;
      h1 = (h/2)%2; iVy = iCy+h1;
      h2 = (h/4)%2; iVz = iCz+h2;
      iVertex = iVx+(N+1)*(iVy+(N+1)*iVz);
      if(vMap.count(iVertex)==0) {
        vMap[iVertex] = nV++;
      }
    }
  }
  return nV;
}
