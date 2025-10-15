//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-10-14 17:12:28 taubin>
//------------------------------------------------------------------------
//
// Geometry.cpp
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

// #include <iostream>
// #include <iomanip>
#include <math.h>
#include "Geometry.hpp"

// private static
void Geometry::_computeFaceNormal
(const vector<float>& coord, const vector<int>& coordIndex,
 const int i0, const int i1, float& w0, float& w1, float& w2) {

  w0 = w1 = w2 = 0.0f;

  int   i,j,niF,iV0,iV1,iV2;
  float u0,u1,u2,v0,v1,v2,x0,x1,x2;

  niF = i1-i0;
  if(niF==3) {

    iV0 = coordIndex[i0+0];
    iV1 = coordIndex[i0+1];
    iV2 = coordIndex[i0+2];

    u0 = coord[3*iV1+0]-coord[3*iV0+0];
    u1 = coord[3*iV1+1]-coord[3*iV0+1];
    u2 = coord[3*iV1+2]-coord[3*iV0+2];

    v0 = coord[3*iV2+0]-coord[3*iV0+0];
    v1 = coord[3*iV2+1]-coord[3*iV0+1];
    v2 = coord[3*iV2+2]-coord[3*iV0+2];

    // w = u x v
    w0 = u1*v2-v1*u2;
    w1 = u2*v0-v2*u0;
    w2 = u0*v1-v0*u1;

  } else if(niF>3) {

    // compute face centroid
    x0=x1=x2=0.0f;
    for(i=i0;i<i1;i++) {
      iV0 = coordIndex[i];
      x0 += coord[3*iV0+0];
      x1 += coord[3*iV0+1];
      x2 += coord[3*iV0+2];
    }
    x0 /= ((float)niF);
    x1 /= ((float)niF);
    x2 /= ((float)niF);

    // accumulate normals defined by centroid and boundary edges
    w0 = w1 = w2 = 0.0f;
    for(i=i0;i<i1;i++) {
      if((j=i+1)==i1) j=i0;
      iV1 = coordIndex[i];
      iV2 = coordIndex[j];

      u0 = coord[3*iV1+0]-x0;
      u1 = coord[3*iV1+1]-x1;
      u2 = coord[3*iV1+2]-x2;

      v0 = coord[3*iV2+0]-x0;
      v1 = coord[3*iV2+1]-x1;
      v2 = coord[3*iV2+2]-x2;

      // w = u x v
      w0 += u1*v2-v1*u2;
      w1 += u2*v0-v2*u0;
      w2 += u0*v1-v0*u1;
    }
  }

}


// public static
void Geometry::computeFaceCentroids
(const vector<float>& coord, const vector<int>& coordIndex,
 vector<float>& faceCentroid) {
  faceCentroid.clear();
  int nC = static_cast<int>(coordIndex.size());
  int i,i0,i1,/*iF,*/iV;
  float x0,x1,x2,w;
  for(/*iF=*/i0=i1=0;i1<nC;i1++) {
    if(coordIndex[i1]>=0) continue;
    w = static_cast<float>(i1-i0);
    x0=x1=x2=0.0f;
    for(i=i0;i<i1;i++) {
      iV = coordIndex[i];
      x0 += coord[3*iV  ];
      x1 += coord[3*iV+1];
      x2 += coord[3*iV+2];
    }
    x0 /= w; x1 /= w; x2 /= w;
    faceCentroid.push_back(x0);
    faceCentroid.push_back(x1);
    faceCentroid.push_back(x2);
    i0=i1+1;/*iF++;*/
  }

}

// public static
void Geometry::computeNormalsPerFace
  (const vector<float>& coord, const vector<int>& coordIndex,
   vector<float>& normal) {

  int   i0,i1,iF;
  float w0,w1,w2,ww;

  normal.clear();
  int nC = static_cast<int>(coordIndex.size());
  for(i1=0;i1<nC;i1++) {
    if(coordIndex[i1]>=0) continue;
    normal.push_back(0.0f);
    normal.push_back(0.0f);
    normal.push_back(0.0f);
  }

  for(iF=i0=i1=0;i1<nC;i1++) {
    if(coordIndex[i1]>=0) continue;
    // face iF = coordIndex[i0<=i<i1]

    // compute face normal * face area
    _computeFaceNormal(coord, coordIndex, i0, i1, w0, w1, w2);
    
    // normalize
    ww = w0*w0+w1*w1+w2*w2; // face area
    // Mon Mar 11 18:52:14 2019
    // note that it may generate zero vectors
    if(ww>0.0f) {
      ww = sqrt(ww);
      w0/=ww; w1/=ww; w2/=ww;
      // save
      normal[3*iF+0] = w0;
      normal[3*iF+1] = w1;
      normal[3*iF+2] = w2;
    }
    
    i0 = i1+1; iF++; // next face
  }
}

// public static
void Geometry::computeNormalsPerVertex
  (const vector<float>& coord, const vector<int>& coordIndex,
   vector<float>& normal) {

  int   i,i0,i1,iV,nV;
  float w0,w1,w2,ww;

  nV = static_cast<int>(coord.size()/3);
  normal.clear();
  for(iV=0;iV<nV;iV++) {
    normal.push_back(0.0f);
    normal.push_back(0.0f);
    normal.push_back(0.0f);
  }

  int nC = static_cast<int>(coordIndex.size());
  for(i0=i1=0;i1<nC;i1++) {
    if(coordIndex[i1]<0) {

      // compute face normal * face area
      _computeFaceNormal(coord, coordIndex, i0, i1, w0, w1, w2);

      // accumulate
      for(i=i0;i<i1;i++) {
        iV = coordIndex[i];
        normal[3*iV+0] += w0;
        normal[3*iV+1] += w1;
        normal[3*iV+2] += w2;
      }

      i0 = i1+1; // iF++;
    }
  }

  // normalize
  for(iV=0;iV<nV;iV++) {
    w0 = normal[3*iV+0];
    w1 = normal[3*iV+1];
    w2 = normal[3*iV+2];

    ww = w0*w0+w1*w1+w2*w2;
    // Mon Mar 11 18:52:14 2019
    // note that it may generate zero vectors
    if(ww>0.0f) {
      ww = sqrt(ww);
      w0/=ww; w1/=ww; w2/=ww;

      normal[3*iV+0] = w0;
      normal[3*iV+1] = w1;
      normal[3*iV+2] = w2;
    }
  }
}

// public static
void Geometry::computeEdgeLengths
(const vector<float>& coord, const Edges& edges, vector<float>& edgeLengths) {
  int   nE,iE,iV0,iV1;
  float dx0,dx1,dx2,eLength;
  nE = edges.getNumberOfEdges();
  edgeLengths.clear();
  for(iE=0;iE<nE;iE++) {
    iV0 = edges.getVertex0(iE);
    iV1 = edges.getVertex1(iE);
    dx0 = coord[3*iV1+0]-coord[3*iV0+0];
    dx1 = coord[3*iV1+1]-coord[3*iV0+1];
    dx2 = coord[3*iV1+2]-coord[3*iV0+2];
    eLength = sqrt(dx0*dx0+dx1*dx1+dx2*dx2);
    edgeLengths.push_back(eLength);
  }
}

// public static
float Geometry::computeAverageEdgeLength
(const vector<float>& coord, const Edges& edges) {
  float avrgEdgeLength = 0.0;
  int   nE,iE,iV0,iV1;
  float dx0,dx1,dx2,eLength;
  nE = edges.getNumberOfEdges();
  for(iE=0;iE<nE;iE++) {
    iV0 = edges.getVertex0(iE);
    iV1 = edges.getVertex1(iE);
    dx0 = coord[3*iV1+0]-coord[3*iV0+0];
    dx1 = coord[3*iV1+1]-coord[3*iV0+1];
    dx2 = coord[3*iV1+2]-coord[3*iV0+2];
    eLength = sqrt(dx0*dx0+dx1*dx1+dx2*dx2);
    avrgEdgeLength += eLength;
  }
  avrgEdgeLength /= static_cast<float>(nE);
  return avrgEdgeLength;
}


// public static
float Geometry::computeDiameter(const vector<float>& coord) {
  float coordDiameter = 0.0f;
  int nPoints = static_cast<int>(coord.size()/3);
  if(nPoints>1) {
    float min0,min1,min2,max0,max1,max2,p0,p1,p2,d0,d1,d2,diamSq;
    min0 = max0 = coord[0];
    min1 = max1 = coord[1];
    min2 = max2 = coord[2];
    for(int i=1;i<nPoints;i++) {
      p0 = coord[3*i  ];
      p1 = coord[3*i+1];
      p2 = coord[3*i+2];
      if(p0<min0) min0 = p0; else if(p0>max0) max0 = p0;
      if(p1<min1) min1 = p1; else if(p1>max1) max1 = p1;
      if(p2<min2) min2 = p2; else if(p2>max2) max2 = p2;
    }
    diamSq = 0.0f;
    d0 = max0-min0; diamSq += d0*d0;
    d1 = max1-min1; diamSq += d1*d1;
    d2 = max2-min2; diamSq += d2*d2;
    coordDiameter = std::sqrt(diamSq);
  }
  return coordDiameter;
}

// public static
void Geometry::computeValences
(const Edges& edges, vector<int>& valence) {
  int nV,nE,iE,iV0,iV1;

  nV = edges.getNumberOfVertices();
  nE = edges.getNumberOfEdges();

  valence.clear();
  for(iV0=0;iV0<nV;iV0++)
    valence.push_back(0);

  for(iE=0;iE<nE;iE++) {
    iV0 = edges.getVertex0(iE);
    iV1 = edges.getVertex1(iE);
    valence[iV0]++;
    valence[iV1]++;
  }
}

// public static
void Geometry::invertVector(vector<float>& values) {
    int nValues= static_cast<int>(values.size());
  for(int i=0;i<nValues;i++)
    values[i] = -values[i];
}

// public static
void Geometry::deleteUnusedCoord
(vector<float>& coord, vector<int>& coordIndex) {

  int nV,iV,i,nC;

  nV = static_cast<int>(coord.size()/3);
  vector<int> vertexRefs;
  for(iV=0;iV<nV;iV++)
    vertexRefs.push_back(0);
  nC = static_cast<int>(coordIndex.size());
  for(i=0;i<nC;i++)
    if((iV=coordIndex[i])>=0 && iV<nV)
      vertexRefs[iV]++;
  vector<float> newCoord;
  vector<int>& vertexMap = vertexRefs; // can use the same array
  for(iV=0;iV<nV;iV++) {
    if(vertexRefs[iV]==0) {
      vertexMap[iV] = -1;
    } else /* if(vertexRefs[iV]>0) */ {
      vertexMap[iV] = static_cast<int>(newCoord.size()/3);
      newCoord.push_back(coord[3*iV+0]);
      newCoord.push_back(coord[3*iV+1]);
      newCoord.push_back(coord[3*iV+2]);
    }
  }
  nC = static_cast<int>(coordIndex.size());
  for(i=0;i<nC;i++)
    if((iV=coordIndex[i])>=0 && iV<nV)
      coordIndex[i] = vertexMap[iV];

  coord.clear();
  coord.insert(coord.end(),newCoord.begin(),newCoord.end());
}

// public static
bool Geometry::isTriangulated(vector<int>& coordIndex) {
  bool isTri = true;
  int iC0,iC1,nC=static_cast<int>(coordIndex.size());
  for(iC0=iC1=0;isTri && iC1<nC;iC1++) {
    if(coordIndex[iC1]>=0) continue;
    if(iC1-iC0 != 3) isTri = false;
    iC0=iC1+1;
  }
  return isTri;
}

// public static
void Geometry::triangulate
(vector<float>& coord, vector<int>& coordIndex) {
  
  // int nV = coord.size()/3;
  vector<float> newCoord;
  newCoord.insert(newCoord.end(),coord.begin(),coord.end());
  vector<int> newCoordIndex;

  int i,i0,i1,nVF,iV,iV0,iV1,iVF;
  float x0,x1,x2;
  int nC = static_cast<int>(coordIndex.size());
  for(i0=i1=0;i1<nC;i1++) {
    if(coordIndex[i1]>=0) continue;
    nVF = i1-i0;
    if(nVF==3) {
      newCoordIndex.push_back(coordIndex[i0+0]);
      newCoordIndex.push_back(coordIndex[i0+1]);
      newCoordIndex.push_back(coordIndex[i0+2]);
      newCoordIndex.push_back(-1);
    } else if(nVF>3) {
      // compute face centroid
      x0=x1=x2=0.0f;
      for(i=i0;i<i1;i++) {
        iV = coordIndex[i];
        x0 += coord[3*iV+0];
        x1 += coord[3*iV+1];
        x2 += coord[3*iV+2];
      }
      x0 /= ((float)nVF);
      x1 /= ((float)nVF);
      x2 /= ((float)nVF);
      // new vertex index
      iVF = static_cast<int>(newCoord.size()/3);
      // save new coords
      newCoord.push_back(x0);
      newCoord.push_back(x1);
      newCoord.push_back(x2);
      // split the face into nVF triangles
      iV0 = coordIndex[i1-1];
      for(i=i0;i<i1;i++) {
        iV1 = coordIndex[i];
        newCoordIndex.push_back(iV0);
        newCoordIndex.push_back(iV1);
        newCoordIndex.push_back(iVF);
        newCoordIndex.push_back(-1);
        iV0 = iV1;
      }
    }
    i0 = i1+1; // iF++;
  }

  deleteUnusedCoord(newCoord,newCoordIndex);

  coord.clear();
  coord.insert(coord.end(),newCoord.begin(),newCoord.end());

  coordIndex.clear();
  coordIndex.insert(coordIndex.end(),newCoordIndex.begin(),newCoordIndex.end());
}

// public static 
void Geometry::invertOrientation(vector<int>& propertyIndex) {
  int i0,i1,j0,j1,iV;
  int nPI = static_cast<int>(propertyIndex.size());
  for(i0=i1=0;i1<nPI;i1++) {
    if(propertyIndex[i1]>=0) continue;
    for(j0=i0,j1=i1-1;j0<j1;j0++,j1--) {
      iV = propertyIndex[j0];
      propertyIndex[j0] = propertyIndex[j1];
      propertyIndex[j1] = iV;
    }
    i0 = i1+1; // iF++;
  }
}
