//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-10-14 17:26:19 taubin>
//------------------------------------------------------------------------
//
// SceneGraphProcessor.cpp
//
// Software developed for the course
// Digital Geometry Processing
// Copyright (c) 2025, Gabriel Taubin
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

#include <math.h>
// #include <iostream>
#include "SceneGraphProcessor.hpp"
#include "SceneGraphTraversal.hpp"
#include "Shape.hpp"
#include "IndexedFaceSet.hpp"
#include "IndexedLineSet.hpp"
#include "Appearance.hpp"
#include "Material.hpp"
#include "core/Graph.hpp"

const int SceneGraphProcessor::_hexGridEdge[12][2] = {
  {0,4}, {1,5}, {2,6}, {3,7},
  {0,2}, {1,3}, {4,6}, {5,7},
  {0,1}, {2,3}, {4,5}, {6,7}
};

SceneGraphProcessor::SceneGraphProcessor(SceneGraph& wrl):
  _wrl(wrl),
  _nGrid(0),
  _nPoints(0),
  _next(nullptr),
  _first(nullptr) {
}

SceneGraphProcessor::~SceneGraphProcessor() {
  _deletePartition();
}

int SceneGraphProcessor::_nCells() {
  return _nGrid*_nGrid*_nGrid;
}

int SceneGraphProcessor::_createPartition
(Vec3f& min, Vec3f& max, int depth, vector<float>&coord) {
  int nPointsInPartition = 0;
  _deletePartition();
  float dx = max.x-min.x;
  float dy = max.y-min.y;
  float dz = max.z-min.z;
  if(dx>0.0f && dy>0.0f && dz>0.0f) {
    _nPoints = static_cast<int>((coord.size()/3));
    if(depth<0) depth = 0;
    _nGrid = 1<<depth;
    int nCells  = _nCells();
    _next    = new int[_nPoints];
    _first   = new int[nCells];
    int iCell,iPoint,ix,iy,iz;
    float x,y,z,nG=static_cast<float>(_nGrid);
    for(iCell=0;iCell<nCells;iCell++)
      _first[iCell] = -1;
    for(iPoint=0;iPoint<_nPoints;iPoint++) {  
      if((x = coord[static_cast<size_t>(3*iPoint  )])<min.x || x>max.x) continue;
      if((y = coord[static_cast<size_t>(3*iPoint+1)])<min.y || y>max.y) continue;
      if((z = coord[static_cast<size_t>(3*iPoint+2)])<min.z || z>max.z) continue;
      ix = static_cast<int>((nG*(x-min.x))/dx);
      iy = static_cast<int>((nG*(y-min.y))/dy);
      iz = static_cast<int>((nG*(z-min.z))/dz);
      iCell = ix+_nGrid*(iy+_nGrid*iz);
      _next[iPoint] = _first[iCell];
    _first[iCell] = iPoint;
    nPointsInPartition++;
    }
  }
  return nPointsInPartition;
}

void SceneGraphProcessor::_deletePartition() {
  if(_first!=nullptr) delete [] _first;
  if(_next !=nullptr) delete [] _next;
  _nGrid   = 0;
  _nPoints = 0;
  _next    = nullptr;
  _first   = nullptr;
}

int SceneGraphProcessor::numberOfShapeNodes() {
  int nShapes = 0;
  SceneGraphTraversal sgt(_wrl);
  Node* node;
  while((node=sgt.next())!=(Node*)0) {
    if(node->isShape()) nShapes++;
  }
  return nShapes;
}

void SceneGraphProcessor::normalClear() {
  _applyToIndexedFaceSet(_normalClear);
}

void SceneGraphProcessor::normalInvert() {
  _applyToIndexedFaceSet(_normalInvert);
}

void SceneGraphProcessor::computeNormalPerFace() {
  _applyToIndexedFaceSet(_computeNormalPerFace);
}

void SceneGraphProcessor::computeNormalPerVertex() {
  _applyToIndexedFaceSet(_computeNormalPerVertex);
}

void SceneGraphProcessor::computeNormalPerCorner() {
  _applyToIndexedFaceSet(_computeNormalPerCorner);
}

void SceneGraphProcessor::_applyToIndexedFaceSet(IndexedFaceSet::Operator o) {
  SceneGraphTraversal sgt(_wrl);
  Node* node;
  while((node=sgt.next())!=(Node*)0) {
    if(node->isShape()) {
      Shape* shape = (Shape*)node;
      node = shape->getGeometry();
      if(node!=(Node*)0 && node->isIndexedFaceSet()) {
        IndexedFaceSet& ifs = *((IndexedFaceSet*)node);
        o(ifs);
      }
    }
  }
}

void SceneGraphProcessor::_normalClear(IndexedFaceSet& ifs) {
  vector<float>& normal      = ifs.getNormal();
  vector<int>&   normalIndex = ifs.getNormalIndex();
  ifs.setNormalPerVertex(true);
  normal.clear();
  normalIndex.clear();
}

void SceneGraphProcessor::_normalInvert(IndexedFaceSet& ifs) {
  vector<float>& normal = ifs.getNormal();
  for(int i=0;i<(int)normal.size();i++)
    normal[i] = -normal[i];
}

void SceneGraphProcessor::_computeFaceNormal
(vector<float>& coord, vector<int>& coordIndex,
 int i0, int i1, Vec3f& n, bool normalize) {
  int niF,iV,i;
  Vec3f p,pi,ni,v1,v2;
  niF = i1-i0; // number of face corners
  // n << 0,0,0;
  n[0] = n[1] = n[2] = 0.0f;
  if(niF==3) { // triangle
    // triangle
    iV = coordIndex[i0];
    // p << coord[3*iV  ],coord[3*iV+1],coord[3*iV+2];
    p[0] = coord[3*iV  ]; p[1] = coord[3*iV+1]; p[2] = coord[3*iV+2];
    iV = coordIndex[i0+1];
    // v1 << coord[3*iV  ],coord[3*iV+1],coord[3*iV+2];
    v1[0] = coord[3*iV  ]; v1[1] = coord[3*iV+1]; v1[2] = coord[3*iV+2];
    iV = coordIndex[i0+2];
    // v2 << coord[3*iV  ],coord[3*iV+1],coord[3*iV+2];
    v2[0] = coord[3*iV  ]; v2[1] = coord[3*iV+1]; v2[2] = coord[3*iV+2];

    // v1 -= p;
    v1[0] -= p[0]; v1[1] -= p[1]; v1[2] -= p[2];
    // v2 -= p;
    v2[0] -= p[0]; v2[1] -= p[1]; v2[2] -= p[2];
    // n = v1.cross(v2);
    n[0] = v1[1]*v2[2]-v1[2]*v2[1];
    n[1] = v1[2]*v2[0]-v1[0]*v2[2];
    n[2] = v1[0]*v2[1]-v1[1]*v2[0];

    if(normalize) {
      // n.normalize();
      float nn = n[0]*n[0]+n[1]*n[1]+n[2]*n[2];
      if(nn>0.0f) {
        nn = (float)sqrt(nn);
        n[0] /= nn; n[1] /= nn; n[2] /= nn;
      }
    }
  } else if(niF>3) { // polygon
    // compute face centroid
    // p << 0,0,0;
    p[0] = p[1] = p[2] = 0.0f;
    for(i=i0;i<i1;i++) {
      iV = coordIndex[i];
      // pi << coord[3*iV  ],coord[3*iV+1],coord[3*iV+2];
      pi[0] = coord[3*iV  ]; pi[1] = coord[3*iV+1]; pi[2] = coord[3*iV+2];
      // p += pi;
      p[0] += pi[0]; p[1] += pi[1]; p[2] += pi[2];
    }
    // p /= ((float)niF);
    p[0] /= ((float)niF); p[1] /= ((float)niF); p[2] /= ((float)niF);
    // accumulate face normal
    iV = coordIndex[i1-1];
    // v1 << coord[3*iV  ],coord[3*iV+1],coord[3*iV+2];
    v1[0] = coord[3*iV  ]; v1[1] = coord[3*iV+1]; v1[2] = coord[3*iV+2];
    // v1 -= p;
    v1[0] -= p[0]; v1[1] -= p[1]; v1[2] -= p[2];
    for(i=i0;i<i1;i++) {
      iV = coordIndex[i];
      // v2 << coord[3*iV  ],coord[3*iV+1],coord[3*iV+2];
      v2[0] = coord[3*iV  ]; v2[1] = coord[3*iV+1]; v2[2] = coord[3*iV+2];
      // v2 -= p;
      v2[0] -= p[0]; v2[1] -= p[1]; v2[2] -= p[2];

      // ni = v1.cross(v2);
      ni[0] = v1[1]*v2[2]-v1[2]*v2[1];
      ni[1] = v1[2]*v2[0]-v1[0]*v2[2];
      ni[2] = v1[0]*v2[1]-v1[1]*v2[0];

      // n += ni;
      n[0] += ni[0]; n[1] += ni[1]; n[2] += ni[2];

      // v1.swap(v2);
      v1[0] = v2[0]; v1[1] = v2[1]; v1[2] = v2[2];
    }
    if(normalize) {
      // n.normalize();
      float nn = n[0]*n[0]+n[1]*n[1]+n[2]*n[2];
      if(nn>0.0f) {
        nn = (float)sqrt(nn);
        n[0] /= nn; n[1] /= nn; n[2] /= nn;
      }
    }
  } else /* if(n<3) */{ // face with less than 3 vertices
    // throw exception ?
    // n == (0,0,0)
  }
}

void SceneGraphProcessor::_computeNormalPerFace(IndexedFaceSet& ifs) {
  if(ifs.getNormalBinding()==IndexedFaceSet::PB_PER_FACE) return;
  vector<float>& coord       = ifs.getCoord();
  vector<int>&   coordIndex  = ifs.getCoordIndex();
  vector<float>& normal      = ifs.getNormal();
  vector<int>&   normalIndex = ifs.getNormalIndex();
  ifs.setNormalPerVertex(false);
  normal.clear();
  normalIndex.clear();
  Vec3f n;
  int /*iF,*/ i0,i1;
  for(i0=i1=0;i1<(int)coordIndex.size();i1++) {
    if(coordIndex[i1]<0) {
      _computeFaceNormal(coord,coordIndex,i0,i1,n,true);
      normal.push_back((float)(n[0]));
      normal.push_back((float)(n[1]));
      normal.push_back((float)(n[2]));
      i0=i1+1; /* iF++; */
    }
  }
}

void SceneGraphProcessor::_computeNormalPerVertex(IndexedFaceSet& ifs) {
  if(ifs.getNormalBinding()==IndexedFaceSet::PB_PER_VERTEX) return;
  vector<float>& coord       = ifs.getCoord();
  vector<int>&   coordIndex  = ifs.getCoordIndex();
  vector<float>& normal      = ifs.getNormal();
  vector<int>&   normalIndex = ifs.getNormalIndex();
  ifs.setNormalPerVertex(true);
  normal.clear();
  normalIndex.clear();
  Vec3f n;
  int /*iF,*/ nV,i,i0,i1,iV;
  float x0,x1,x2;
  nV = (int)(coord.size()/3);
  // initialize accumulators
  normal.insert(normal.end(),coord.size(),0.0f);
  // accumulate face normals
  for(i0=i1=0;i1<(int)coordIndex.size();i1++) {
    if(coordIndex[i1]<0) {
      _computeFaceNormal(coord,coordIndex,i0,i1,n,false);
      // accumulate
      for(i=i0;i<i1;i++) {
        iV = coordIndex[i];
        x0 = normal[3*iV  ];
        x1 = normal[3*iV+1];
        x2 = normal[3*iV+2];
        normal[3*iV  ] = x0+((float)(n[0]));
        normal[3*iV+1] = x1+((float)(n[1]));
        normal[3*iV+2] = x2+((float)(n[2]));
      }
      i0=i1+1; /* iF++; */
    }
  }
  for(iV=0;iV<nV;iV++) {
    n[0] = normal[3*iV  ];
    n[1] = normal[3*iV+1];
    n[2] = normal[3*iV+2];
    float nn = n[0]*n[0]+n[1]*n[1]+n[2]*n[2];
    if(nn>0.0f) {
      nn = (float)sqrt(nn);
      n[0] /= nn; n[1] /= nn; n[2] /= nn;
    }
    normal[3*iV  ] = n[0];
    normal[3*iV+1] = n[1];
    normal[3*iV+2] = n[2];
  }
}

void SceneGraphProcessor::_computeNormalPerCorner(IndexedFaceSet& ifs) {
  if(ifs.getNormalBinding()==IndexedFaceSet::PB_PER_CORNER) return;

  vector<float>& coord       = ifs.getCoord();
  vector<int>&   coordIndex  = ifs.getCoordIndex();
  vector<float>& normal      = ifs.getNormal();
  vector<int>&   normalIndex = ifs.getNormalIndex();
  ifs.setNormalPerVertex(true);
  normal.clear();
  normalIndex.clear();

  Vec3f pP,p0,pN,vP,vN,n;
  int /*iF,*/ i,i0,i1,ip,in,nFC,iN,iVp/*,iV0,iVn*/;
  for(/* iF= */ i0=i1=0;i1<(int)coordIndex.size();i1++) {
    if(coordIndex[i1]<0) {
      nFC = i1-i0; // number of face corners
      // n << 0,0,0;
      n[0]=n[1]=n[2]=0.0f;
      if(nFC>=3) { // polygon
        for(i=i0;i<i1;i++) {
          if((ip=i-1)< i0) ip=i1-1;
          if((in=i+1)==i1) in=i0  ;

          iVp = coordIndex[ip];
          // iV0 = coordIndex[i ];
          // iVn = coordIndex[in];

          // pP << coord[3*iVp  ],coord[3*iVp+1],coord[3*iVp+2];
          pP[0] = coord[3*iVp  ]; pP[1] = coord[3*iVp+1]; pP[2] = coord[3*iVp+2];
          // p0 << coord[3*iVp  ],coord[3*iVp+1],coord[3*iVp+2];
          p0[0] = coord[3*iVp  ]; p0[1] = coord[3*iVp+1]; p0[2] = coord[3*iVp+2];
          // pN << coord[3*iVp  ],coord[3*iVp+1],coord[3*iVp+2];
          pN[0] = coord[3*iVp  ]; pN[1] = coord[3*iVp+1]; pN[2] = coord[3*iVp+2];
          // vP = pP-p0;
          vP[0] = pP[0]-p0[0]; vP[1] = pP[1]-p0[1]; vP[2] = pP[2]-p0[2];
          // vN = pN-p0;
          vN[0] = pN[0]-p0[0]; vN[1] = pN[1]-p0[1]; vN[2] = pN[2]-p0[2];

          // n = vN.cross(vP);
          n[0] = vN[1]*vP[2]-vN[2]*vP[1];
          n[1] = vN[2]*vP[0]-vN[0]*vP[2];
          n[2] = vN[0]*vP[1]-vN[1]*vP[0];

          // n.normalize();
          float nn = n[0]*n[0]+n[1]*n[1]+n[2]*n[2];
          if(nn>0.0f) {
            nn = (float)sqrt(nn);
            n[0] /= nn; n[1] /= nn; n[2] /= nn;
          }

          iN = (int)(normal.size()/3);
          normal.push_back(n[0]);
          normal.push_back(n[1]);
          normal.push_back(n[2]);
          normalIndex.push_back(iN);

        }
        normalIndex.push_back(-1);

      } else /* if(nFC<3) */{ // face with less than 3 vertices
        // throw exception ?
        // n << 0,0,0;
        n[0]=n[1]=n[2]=0.0f;
        for(i=i0;i<i1;i++) {
          iN = (int)(normal.size()/3);
          normal.push_back(n[0]);
          normal.push_back(n[1]);
          normal.push_back(n[2]);
          normalIndex.push_back(iN);
        }
        normalIndex.push_back(-1);
      }

      i0=i1+1; /* iF++; */
    }
  }
}

void SceneGraphProcessor::gridAdd
(int depth, float scale, bool isCube) {
  const string name = "GRID";
  Shape* shape = (Shape*)0;
  const Node*  node = _wrl.getChild(name);
  if(node==(Node*)0) {
    shape = new Shape();
    shape->setName(name);
    Appearance* appearance = new Appearance();
    shape->setAppearance(appearance);
    Material* material = new Material();
    // colors should be stored in WrlViewerData
    Color gridColor(0.5f,0.3f,0.0f);
    material->setDiffuseColor(gridColor);
    appearance->setMaterial(material);
    _wrl.addChild(shape);
  } else if(node->isShape()) {
    shape = (Shape*)node;
  }
  if(shape==(Shape*)0) { /* throw exception ??? */ return; }

  IndexedLineSet* ils = (IndexedLineSet*)0;
  node = shape->getGeometry();
  if(node==(Node*)0) {
    ils = new IndexedLineSet();
    shape->setGeometry(ils);
  } else if(node->isIndexedLineSet()) {
    ils = (IndexedLineSet*)node;
  }
  if(ils==(IndexedLineSet*)0) { /* throw exception ??? */ return; }

  vector<float>& coord      = ils->getCoord();
  vector<int>&   coordIndex = ils->getCoordIndex();
  vector<float>& color      = ils->getColor();
  vector<int>&   colorIndex = ils->getColorIndex();
  coord.clear();
  coordIndex.clear();
  color.clear();
  colorIndex.clear();
  ils->setColorPerVertex(true);

  _wrl.updateBBox();
  Vec3f& center = _wrl.getBBoxCenter();
  Vec3f& size   = _wrl.getBBoxSize();

  float dx = size.x/2.0f;
  float dy = size.y/2.0f;
  float dz = size.z/2.0f;
  if(isCube) {
    float dMax = dx; if(dy>dMax) dMax=dy; if(dz>dMax) dMax=dz;
    dx = dMax; dy = dMax; dz = dMax;
  }
  if(scale>0.0f) {
    dx *= scale; dy *= scale; dz *= scale;
  }

  float x0 = center.x-dx; float y0 = center.y-dy; float z0 = center.z-dz;
  float x1 = center.x+dx; float y1 = center.y+dy; float z1 = center.z+dz;

  if(depth==0) {
    
    // vertices
    coord.push_back(x0); coord.push_back(y0); coord.push_back(z0);
    coord.push_back(x0); coord.push_back(y0); coord.push_back(z1);
    coord.push_back(x0); coord.push_back(y1); coord.push_back(z0);
    coord.push_back(x0); coord.push_back(y1); coord.push_back(z1);
    coord.push_back(x1); coord.push_back(y0); coord.push_back(z0);
    coord.push_back(x1); coord.push_back(y0); coord.push_back(z1);
    coord.push_back(x1); coord.push_back(y1); coord.push_back(z0);
    coord.push_back(x1); coord.push_back(y1); coord.push_back(z1);
    
    // edges
    coordIndex.push_back(0); coordIndex.push_back(1); coordIndex.push_back(-1);
    coordIndex.push_back(2); coordIndex.push_back(3); coordIndex.push_back(-1);
    coordIndex.push_back(4); coordIndex.push_back(5); coordIndex.push_back(-1);
    coordIndex.push_back(6); coordIndex.push_back(7); coordIndex.push_back(-1);
    //
    coordIndex.push_back(0); coordIndex.push_back(2); coordIndex.push_back(-1);
    coordIndex.push_back(1); coordIndex.push_back(3); coordIndex.push_back(-1);
    coordIndex.push_back(4); coordIndex.push_back(6); coordIndex.push_back(-1);
    coordIndex.push_back(5); coordIndex.push_back(7); coordIndex.push_back(-1);
    // iz=0
    coordIndex.push_back(0); coordIndex.push_back(4); coordIndex.push_back(-1);
    coordIndex.push_back(1); coordIndex.push_back(5); coordIndex.push_back(-1);
    coordIndex.push_back(2); coordIndex.push_back(6); coordIndex.push_back(-1);
    coordIndex.push_back(3); coordIndex.push_back(7); coordIndex.push_back(-1);

  } else {

    int N = 1<<depth;

    // vertices
    float x,y,z;
    int ix,iy,iz,jx,jy,jz,iV0,iV1;

    for(iz=0,jz=N;iz<=N;iz++,jz--) {
      z = (((float)jz)*z0+((float)iz)*z1)/((float)N);
      for(iy=0,jy=N;iy<=N;iy++,jy--) {
        y = (((float)jy)*y0+((float)iy)*y1)/((float)N);
        for(ix=0,jx=N;ix<=N;ix++,jx--) {
          x = (((float)jx)*x0+((float)ix)*x1)/((float)N);
          coord.push_back(x); coord.push_back(y); coord.push_back(z);
        }
      }
    }

    // edges
    for(iz=0;iz<N;iz++) {
      for(iy=0;iy<=N;iy++) {
        for(ix=0;ix<=N;ix++) {
          iV0 = (ix  )+(N+1)*((iy  )+(N+1)*(iz  ));
          iV1 = (ix  )+(N+1)*((iy  )+(N+1)*(iz+1));
          coordIndex.push_back(iV0);
          coordIndex.push_back(iV1);
          coordIndex.push_back(-1);
        }
      }
    }
    for(iz=0;iz<=N;iz++) {
      for(iy=0;iy<N;iy++) {
        for(ix=0;ix<=N;ix++) {
          iV0 = (ix  )+(N+1)*((iy  )+(N+1)*(iz  ));
          iV1 = (ix  )+(N+1)*((iy+1)+(N+1)*(iz  ));
          coordIndex.push_back(iV0);
          coordIndex.push_back(iV1);
          coordIndex.push_back(-1);
        }
      }
    }
    for(iz=0;iz<=N;iz++) {
      for(iy=0;iy<=N;iy++) {
        for(ix=0;ix<N;ix++) {
          iV0 = (ix  )+(N+1)*((iy  )+(N+1)*(iz  ));
          iV1 = (ix+1)+(N+1)*((iy  )+(N+1)*(iz  ));
          coordIndex.push_back(iV0);
          coordIndex.push_back(iV1);
          coordIndex.push_back(-1);
        }
      }
    }

  }
  
}

void SceneGraphProcessor::gridAdd(HexGridPartition& hgp) {
  const string name = "GRID";
  Shape* shape = (Shape*)0;
  const Node*  node = _wrl.getChild(name);
  if(node==(Node*)0) {
    shape = new Shape();
    shape->setName(name);
    Appearance* appearance = new Appearance();
    shape->setAppearance(appearance);
    Material* material = new Material();
    // colors should be stored in WrlViewerData
    Color gridColor(0.5f,0.3f,0.0f);
    material->setDiffuseColor(gridColor);
    appearance->setMaterial(material);
    _wrl.addChild(shape);
  } else if(node->isShape()) {
    shape = (Shape*)node;
  }
  if(shape==(Shape*)0) { /* throw exception ??? */ return; }

  IndexedLineSet* ils = (IndexedLineSet*)0;
  node = shape->getGeometry();
  if(node==(Node*)0) {
    ils = new IndexedLineSet();
    shape->setGeometry(ils);
  } else if(node->isIndexedLineSet()) {
    ils = (IndexedLineSet*)node;
  }
  if(ils==(IndexedLineSet*)0) { /* throw exception ??? */ return; }

  vector<float>& coord      = ils->getCoord();
  vector<int>&   coordIndex = ils->getCoordIndex();
  vector<float>& color      = ils->getColor();
  vector<int>&   colorIndex = ils->getColorIndex();
  coord.clear();
  coordIndex.clear();
  color.clear();
  colorIndex.clear();
  ils->setColorPerVertex(true);
  
  Vec3f gridMin = hgp.getMin();
  Vec3f gridMax = hgp.getMax();
  int   N       = hgp.getResolution();

  float x,x0,x1,y,y0,y1,z,z0,z1;
  x0 = gridMin.x; y0 = gridMin.y; z0 = gridMin.z;
  x1 = gridMax.x; y1 = gridMax.y; z1 = gridMax.z;

  map<int,int> vMap;
  Graph graph;

  // int iPointFirst;
  int h,iCell,iC,iCx,iCy,iCz,iVx,iVy,iVz,iVertex,iV[8],iV0,iV1,iE;
  map<int,int> first =  hgp.getFirstMap();
  map<int,int>::iterator iMap;

  for(iMap=first.begin();iMap!=first.end();iMap++) {
    iCell       = iMap->first;
    iC = iCell; iCx = iC%N; iC/=N; iCy = iC%N; iC/=N; iCz = iC;

    // iPointFirst = iMap->second;

    // vertices
    //
    // 0 ---- 1
    // |\     |\
    // | 2 ---- 3
    // 4 +--- 5 |
    //  \|     \|
    //   6 ---- 7
    //
    // 0 : (iCx  ,iCy  ,iCz  )
    // 1 : (iCx+1,iCy  ,iCz  )
    // 2 : (iCx  ,iCy+1,iCz  )
    // 3 : (iCx+1,iCy+1,iCz  )
    // 4 : (iCx  ,iCy  ,iCz+1)
    // 5 : (iCx+1,iCy  ,iCz+1)
    // 6 : (iCx  ,iCy+1,iCz+1)
    // 7 : (iCx+1,iCy+1,iCz+1)
    for(h=0;h<8;h++) {
      iVx   = iCx+((h  )%2);
      iVy   = iCy+((h/2)%2);
      iVz   = iCz+((h/4)%2);      
      iVertex = iVx+(N+1)*(iVy+(N+1)*iVz);
      if(vMap.count(iVertex)==0) {
        iV[h] = static_cast<int>(coord.size()/3);
        x = (((float)(N-iVx))*x0+((float)iVx)*x1)/((float)N);
        coord.push_back(x);
        y = (((float)(N-iVy))*y0+((float)iVy)*y1)/((float)N);
        coord.push_back(y);
        z = (((float)(N-iVz))*z0+((float)iVz)*z1)/((float)N);
        coord.push_back(z);
        vMap[iVertex] = iV[h];
      } else /* if(vMap.count(iVertex])==0) */ {
        iV[h] = vMap[iVertex];
      }
    }

    for(h=0;h<12;h++) {
      iV0 = iV[_hexGridEdge[h][0]];
      iV1 = iV[_hexGridEdge[h][1]];
      iE  = graph.getEdge(iV0,iV1);
      if(iE<0) {
        graph.insertEdge(iV0,iV1,true);
        coordIndex.push_back(iV0);
        coordIndex.push_back(iV1);
        coordIndex.push_back(-1);
      }
    }
    
  } // for(iMap=first.begin();iMap!=first.end();iMap++)
}

void SceneGraphProcessor::gridRemove() {
  vector<pNode>& children = _wrl.getChildren();
  vector<pNode>::iterator i;
  for(i=children.begin();i!=children.end();i++)
    if((*i)->nameEquals("GRID"))
      break;
  if(i!=children.end())
    children.erase(i);
}

void SceneGraphProcessor::edgesAdd() {
  SceneGraphTraversal sgt(_wrl);
  const Node* node;
  while((node=sgt.next())!=(Node*)0) {
    if(node->isShape()) {
      Shape* shape  = (Shape*)node;
      const Node* parent = shape->getParent();
      Group* group = (Group*)parent;

      node = shape->getGeometry();
      if(node!=(Node*)0 && node->isIndexedFaceSet()) {
        IndexedFaceSet* ifs = (IndexedFaceSet*)node;

        shape->setShow(false);

        // compose the node name ???
        string name = "EDGES";
        node = group->getChild(name);
        if(node==(Node*)0) {
          shape = new Shape();
          shape->setName(name);
          Appearance* appearance = new Appearance();
          shape->setAppearance(appearance);
          Material* material = new Material();
          // colors should be stored in WrlViewerData
          Color edgeColor(1.0f,0.5f,0.0f);
          material->setDiffuseColor(edgeColor);
          appearance->setMaterial(material);
          group->addChild(shape);
        } else if(node->isShape()) {
          shape = (Shape*)node;
        } else /* if(node!=(Node*)0 && node->isShape()==false */ {
          // throw exception ???
        }
        if(shape==(Shape*)0) { /* throw exception ??? */ return; }

        IndexedLineSet* ils = (IndexedLineSet*)0;
        node = shape->getGeometry();
        if(node==(Node*)0) {
          ils = new IndexedLineSet();
          shape->setGeometry(ils);
        } else if(node->isIndexedLineSet()) {
          ils = (IndexedLineSet*)node;
        } else /* if(node!=(Node*)0 && node->isIndexedLineSet()==false) */ {
          // throw exception ???
        }
        
        if(ils==(IndexedLineSet*)0) { /* throw exception ??? */ return; }

        ils->clear();

        vector<float>& coordIfs      = ifs->getCoord();
        vector<int>&   coordIndexIfs = ifs->getCoordIndex();

        vector<float>& coordIls      = ils->getCoord();
        vector<int>&   coordIndexIls = ils->getCoordIndex();

        coordIls.insert(coordIls.end(),
                        coordIfs.begin(),coordIfs.end());

        int i,i0,i1,nV,iV,iV0,iV1/*,iF*/;
        nV = static_cast<int>(coordIfs.size()/3);
        for(iV=0;iV<nV;iV++) {
          coordIls.push_back(coordIfs[3*iV  ]);
          coordIls.push_back(coordIfs[3*iV+1]);
          coordIls.push_back(coordIfs[3*iV+2]);
        }

        for(/* iF= */ i0=i1=0;i1<(int)coordIndexIfs.size();i1++) {
          if(coordIndexIfs[i1]<0) {
            iV0 = coordIndexIfs[i1-1];
            for(i=i0;i<i1;i++) {
              iV1 = coordIndexIfs[i];
              coordIndexIls.push_back(iV0);
              coordIndexIls.push_back(iV1);
              coordIndexIls.push_back(-1);
              iV0 = iV1;
            }
            i0 = i1+1; /* iF++; */
          }
        }


      }
    }
  }
}

void SceneGraphProcessor::edgesRemove() {
  SceneGraphTraversal sgt(_wrl);
  Node* node;
  while((node=sgt.next())!=(Node*)0) {
    if(node->isShape()) {
      Shape* shape  = (Shape*)node;
      const Node* parent = shape->getParent();
      Group* group = (Group*)parent;
      vector<pNode>& children = group->getChildren();
      vector<pNode>::iterator i;
      do {
        for(i=children.begin();i!=children.end();i++)
          if((*i)->nameEquals("EDGES"))
            break;
        if(i!=children.end()) {
          children.erase(i);
          i=children.begin();
        }
      } while(i!=children.end());
    }
  }
}

void SceneGraphProcessor::shapeIndexedFaceSetShow() {
  SceneGraphTraversal sgt(_wrl);
  Node* node;
  while((node=sgt.next())!=(Node*)0) {
    if(node->isShape()) {
      Shape* shape = (Shape*)node;
      node = shape->getGeometry();
      if(node!=(Node*)0 && node->isIndexedFaceSet()) {
        shape->setShow(true);
      }
    }
  }
}

void SceneGraphProcessor::shapeIndexedFaceSetHide() {
  SceneGraphTraversal sgt(_wrl);
  Node* node;
  while((node=sgt.next())!=(Node*)0) {
    if(node->isShape()) {
      Shape* shape = (Shape*)node;
      node = shape->getGeometry();
      if(node!=(Node*)0 && node->isIndexedFaceSet()) {
        shape->setShow(false);
      }
    }
  }
}

void SceneGraphProcessor::shapeIndexedLineSetShow() {
  SceneGraphTraversal sgt(_wrl);
  Node* node;
  while((node=sgt.next())!=(Node*)0) {
    if(node->isShape()) {
      Shape* shape = (Shape*)node;
      node = shape->getGeometry();
      if(node!=(Node*)0 && node->isIndexedLineSet()) {
        shape->setShow(true);
      }
    }
  }
}

void SceneGraphProcessor::shapeIndexedLineSetHide() {
  SceneGraphTraversal sgt(_wrl);
  Node* node;
  while((node=sgt.next())!=(Node*)0) {
    if(node->isShape()) {
      Shape* shape = (Shape*)node;
      node = shape->getGeometry();
      if(node!=(Node*)0 && node->isIndexedLineSet()) {
        shape->setShow(false);
      }
    }
  }
}

bool SceneGraphProcessor::hasGrid() {
  return _wrl.getChild("GRID")!=(Node*)0;
}

bool SceneGraphProcessor::_hasShapeProperty(Shape::Property p) {
  bool value = false;
  SceneGraphTraversal sgt(_wrl);
  Node* node;
  while(value==false && (node=sgt.next())!=(Node*)0) {
    if(node->isShape()) {
      Shape& shape = *(Shape*)node;
      value = p(shape);
    }
  }
  return value;
}

bool SceneGraphProcessor::_hasIndexedFaceSetProperty(IndexedFaceSet::Property p) {
  bool value = false;
  SceneGraphTraversal sgt(_wrl);
  Node* node;
  while(value==false && (node=sgt.next())!=(Node*)0) {
    if(node->isShape()) {
      Shape* shape = (Shape*)node;
      if(shape->hasGeometryIndexedFaceSet()) {
        IndexedFaceSet& ifs = *(IndexedFaceSet*)(shape->getGeometry());
        value = p(ifs);
      }
    }
  }
  return value;
}

bool SceneGraphProcessor::_hasIndexedLineSetProperty(IndexedLineSet::Property p) {
  bool value = false;
  SceneGraphTraversal sgt(_wrl);
  Node* node;
  while(value==false && (node=sgt.next())!=(Node*)0) {
    if(node->isShape()) {
      Shape* shape = (Shape*)node;
      if(shape->hasGeometryIndexedLineSet()) {
        IndexedLineSet& ils = *(IndexedLineSet*)(shape->getGeometry());
        value = p(ils);
      }
    }
  }
  return value;
}

bool SceneGraphProcessor::_hasFaces(IndexedFaceSet& ifs) {
  return (ifs.getNumberOfCoord()>0 && ifs.getNumberOfFaces()>0);
}

bool SceneGraphProcessor::_hasNormalNone(IndexedFaceSet& ifs) {
  return
    (ifs.getNumberOfCoord()==0 && ifs.getNumberOfFaces()==0) ||
    (ifs.getNormalBinding()==IndexedFaceSet::PB_NONE);
}

bool SceneGraphProcessor::_hasNormalPerFace(IndexedFaceSet& ifs) {
  return 
    (ifs.getNumberOfFaces()>0) &&
    (ifs.getNormalBinding()==IndexedFaceSet::PB_PER_FACE);
}

bool SceneGraphProcessor::_hasNormalPerVertex(IndexedFaceSet& ifs) {
  return
    (ifs.getNumberOfCoord()>0) &&
    (ifs.getNormalBinding()==IndexedFaceSet::PB_PER_VERTEX);
}

bool SceneGraphProcessor::_hasNormalPerCorner(IndexedFaceSet& ifs) {
  return
    (ifs.getNumberOfFaces()>0) &&
    (ifs.getNormalBinding()==IndexedFaceSet::PB_PER_CORNER);
}

bool SceneGraphProcessor::hasIndexedFaceSetFaces() {
  return _hasIndexedFaceSetProperty(_hasFaces);
}

bool SceneGraphProcessor::hasIndexedFaceSetNormalNone() {
  return _hasIndexedFaceSetProperty(_hasNormalNone);
}

bool SceneGraphProcessor::hasIndexedFaceSetNormalPerFace() {
  return _hasIndexedFaceSetProperty(_hasNormalPerFace);
}

bool SceneGraphProcessor::hasIndexedFaceSetNormalPerVertex() {
  return _hasIndexedFaceSetProperty(_hasNormalPerVertex);
}

bool SceneGraphProcessor::hasIndexedFaceSetNormalPerCorner() {
  return _hasIndexedFaceSetProperty(_hasNormalPerCorner);
}

// VRML'97
//
// If the color field is not NULL, it shall contain a Color node, and
// the colours are applied to the line(s) as follows:
//
// If colorPerVertex is FALSE:
//
// If the colorIndex field is not empty, then one colour is used for
// each polyline of the IndexedLineSet. There must be at least as many
// indices in the colorIndex field as there are polylines in the
// IndexedLineSet. If the greatest index in the colorIndex field is N,
// then there must be N+1 colours in the Color node. The colorIndex
// field must not contain any negative entries.
//
// If the colorIndex field is empty, then the colours from the Color
// node are applied to each polyline of the IndexedLineSet in
// order. There must be at least as many colours in the Color node as
// there are polylines.
//
// If colorPerVertex is TRUE:
//
// If the colorIndex field is not empty, then colours are applied to
// each vertex of the IndexedLineSet in exactly the same manner that
// the coordIndex field is used to supply coordinates for each vertex
// from the Coordinate node. The colorIndex field must contain at
// least as many indices as the coordIndex field and must contain
// end-of-polyline markers (-1) in exactly the same places as the
// coordIndex field. If the greatest index in the colorIndex field is
// N, then there must be N+1 colours in the Color node.
//
// If the colorIndex field is empty, then the coordIndex field is used
// to choose colours from the Color node. If the greatest index in the
// coordIndex field is N, then there must be N+1 colours in the Color
// node.
//
// If the color field is NULL and there is a Material defined for the
// Appearance affecting this IndexedLineSet, the emissiveColor of the
// Material shall be used to draw the lines.

 bool SceneGraphProcessor::_hasColorNone(IndexedLineSet& ils) {
  vector<float>& color         = ils.getColor();
  return (color.size()==0);
}

 bool SceneGraphProcessor::_hasColorPerVertex(IndexedLineSet& ils) {
  vector<float>& color         = ils.getColor();
  // vector<int>&   colorIndex    = ils.getColorIndex();
  bool           colorPerVerex = ils.getColorPerVertex();
  // not testing for errors, but
  // if(colorIndex.size()==0)
  //   we should have color.size()/3 == ils.getNumberOfCoord()
  // else
  //   we should have colorIndex.size() == ils.getNumberOfCoord()
  return color.size()>0 && colorPerVerex==false;
}

 bool SceneGraphProcessor::_hasColorPerPolyline(IndexedLineSet& ils) {
  vector<float>& color         = ils.getColor();
  // vector<int>&   colorIndex    = ils.getColorIndex();
  bool           colorPerVerex = ils.getColorPerVertex();
  // not testing for errors, but
  // if(colorIndex.size()==0)
  //   we should have color.size()/3 == ils.getNumberOfPolylines()
  // else
  //   we should have colorIndex.size() == ils.getNumberOfPolylines()
  return color.size()>0 && colorPerVerex==false;
}

bool SceneGraphProcessor::hasIndexedLineSetColorNone() {
  return _hasIndexedLineSetProperty(_hasColorNone);
}

bool SceneGraphProcessor::hasIndexedLineSetColorPerVertex() {
  return _hasIndexedLineSetProperty(_hasColorPerVertex);
}

bool SceneGraphProcessor::hasIndexedLineSetColorPerPolyline() {
  return _hasIndexedLineSetProperty(_hasColorPerPolyline);
}

 bool SceneGraphProcessor::_hasEdges(Shape& shape) {
   return shape.nameEquals("EDGES");
}

 bool SceneGraphProcessor::_hasIndexedFaceSetShown(Shape& shape) {
  return shape.hasGeometryIndexedFaceSet() && shape.getShow()==true;
}

 bool SceneGraphProcessor::_hasIndexedFaceSetHidden(Shape& shape) {
  return shape.hasGeometryIndexedFaceSet() && shape.getShow()==false;
}

 bool SceneGraphProcessor::_hasIndexedLineSetShown(Shape& shape) {
  return shape.hasGeometryIndexedLineSet() && shape.getShow()==true;
}

 bool SceneGraphProcessor::_hasIndexedLineSetHidden(Shape& shape) {
  return shape.hasGeometryIndexedLineSet() && shape.getShow()==false;
}

bool SceneGraphProcessor::hasEdges() {
  return _hasShapeProperty(_hasEdges);
}

bool SceneGraphProcessor::hasIndexedFaceSetShown() {
  return _hasShapeProperty(_hasIndexedFaceSetShown);
}

bool SceneGraphProcessor::hasIndexedFaceSetHidden() {
  return _hasShapeProperty(_hasIndexedFaceSetHidden);
}

bool SceneGraphProcessor::hasIndexedLineSetShown() {
  return _hasShapeProperty(_hasIndexedLineSetShown);
}

bool SceneGraphProcessor::hasIndexedLineSetHidden() {
  return _hasShapeProperty(_hasIndexedLineSetHidden);
}

void SceneGraphProcessor::removeSceneGraphChild(const string& name) {
  vector<pNode>& children = _wrl.getChildren();
  vector<pNode>::iterator i;
  for(i=children.begin();i!=children.end();i++)
    if((*i)->nameEquals(name))
      break;
  if(i!=children.end())
    children.erase(i);
}

void SceneGraphProcessor::pointsRemove() {
  removeSceneGraphChild("POINTS");
}

void SceneGraphProcessor::surfaceRemove() {
  removeSceneGraphChild("SURFACE");
}

IndexedFaceSet* SceneGraphProcessor::_getNamedShapeIFS
(const string& name, bool create) {
  IndexedFaceSet* ifs = (IndexedFaceSet*)0;
  Node* node = _wrl.getChild(name);
  if(node!=(Node*)0 && node->isShape()) {
    Shape* shape = (Shape*)node;
    node = shape->getGeometry();
    if(node!=(Node*)0 && node->isIndexedFaceSet()) {
      ifs = (IndexedFaceSet*)node;
    }
  }
  if(ifs==(IndexedFaceSet*)0 && create) {
    Shape* shape = new Shape();
    shape->setName(name);
    _wrl.addChild(shape);
    Appearance* appearance = new Appearance();
    shape->setAppearance(appearance);
    Material* material = new Material();
    appearance->setMaterial(material);
    ifs = new IndexedFaceSet();
    shape->setGeometry(ifs);
  }
  return ifs;
}

