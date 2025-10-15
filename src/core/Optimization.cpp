//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-10-14 17:12:28 taubin>
//------------------------------------------------------------------------
//
// Optimization.cpp
//
// Software developed for the course
// Digital Geometry Processing
// Copyright (c) 2015, Gabriel Taubin
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
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

// Assignment 4
//
// SMOOTHING TASKS
//
// 1) void Optimization::laplacianSmoothingVertexCoordinatesRun();
//
// 2) void Optimization::laplacianSmoothingFaceNormalsRun(const bool normalize);
//
// 3) void Optimization::jacobiRun();
//
// 4) void Optimization::integrateNormalsRun(const bool recomputeFaceNormals=false);
//
// REMESHING TASKS
//
// 5) void Optimization::_collapseEdgesSelect
//    (const EdgeCollapseErrorMetric errorMetric,
//     const EdgeCollapseIndependentSet indepSet,
//     vector<int>& edgeSelection,
//     bool colorIncidentFaces);
//
//    Optimization::_collapseEdgesSelect is partially implemented
//
// 6) void Optimization::_initializeGarlandHeckbert();
//
// 7) float Optimization::_errorGarlandHeckbert
//    (const float M0[/*10*/], const float M1[/*10*/],
//    float M[/*10*/], float x[/*3*/]);
//
// 8) void Optimization::collapseEdgesApply
//    (const EdgeCollapseErrorMetric errorMetric,
//     const EdgeCollapseIndependentSet indepSet);
//
// 9) void Optimization::_adaptiveSubdivisionSelect
//    (vector<int>& vertexSelection,
//     const SplitEdgesMode mode, bool colorIncidentFaces);
//
// 10) void Optimization::adaptiveSubdivisionApply
//     (const SplitEdgesMode mode, const bool colorFaces);
//
// You can get some ideas for the previous tasks from these examples
//
// ALREADY IMPLEMENTED
//     void Optimization::_equalizeValencesSelect
//     (vector<int>& edgeSelection, bool colorIncidentFaces);
//
// ALREADY IMPLEMENTED
//     void Optimization::equalizeValencesApply();

// #include <iostream>
// #include <iomanip>
#include <cmath>
#include <cassert>
#include "Optimization.hpp"
#include "Geometry.hpp"
// #include "Partition.hpp"
#include "Heap.hpp"
#include "wrl/IndexedFaceSetVariables.hpp"

//////////////////////////////////////////////////////////////////////
Optimization::Optimization():
  _ifsInput(nullptr),
  _ifsOptimized(nullptr),
  /* _stop(false), */
  _steps(2),
  _lambda(0.5f),
  _mu(0.525f),
  _jacobiWeightSmoothing(0.5f),
  _integrateNormalsWeightData(1.0f),
  _integrateNormalsWeightLink(1.0f),
  _integrateNormalsWeightSmoothing(1.0f),
  _edgeLengths(),
  _minEdgeLength(0.0f),
  _maxEdgeLength(0.0f),
  _targetEdgeLength(0.0f),
  _vSelIndex(0),
  _eSelIndex(0),
  _fSelIndex(0) {
}

//////////////////////////////////////////////////////////////////////
void Optimization::clear() {
  _ifsInput     = (IndexedFaceSet*)0;
  _ifsOptimized = (IndexedFaceSet*)0;
  _lambda = 0.5f;
  _mu = 0.5f;
  _steps = 2;
  _edgeLengths.clear();
  _minEdgeLength = 0.0f;
  _maxEdgeLength = 0.0f;
  _targetEdgeLength = 0.0f;
}

//////////////////////////////////////////////////////////////////////
IndexedFaceSet* Optimization::getInput() {
  return _ifsInput;
}

//////////////////////////////////////////////////////////////////////
IndexedFaceSet* Optimization::getOptimized() {
  return _ifsOptimized;
}

//////////////////////////////////////////////////////////////////////
void Optimization::setInput(IndexedFaceSet* ifsInput) {
  _ifsInput = ifsInput;
}

//////////////////////////////////////////////////////////////////////
void Optimization::saveOptimized() {
  if(_ifsInput!=(IndexedFaceSet*)0 && _ifsOptimized!=(IndexedFaceSet*)0) {
    (*_ifsInput) = (*_ifsOptimized);
  }
}

//////////////////////////////////////////////////////////////////////
void Optimization::setOptimized
(IndexedFaceSet* ifsOptimized, const bool reset) {

  _ifsOptimized = ifsOptimized;
  if(_ifsOptimized!=(IndexedFaceSet*)0 && _ifsInput!=(IndexedFaceSet*)0) {

    vector<int>&   coordIndex      = _ifsOptimized->getCoordIndex();
    vector<float>& coord           = _ifsOptimized->getCoord();

    if(reset || coord.size()==0) {
      _ifsOptimized->clear();

      // make _ifsOptimized a copy of _ifsInput

      vector<int>&   coordIndexInput = _ifsInput->getCoordIndex();
      vector<float>& coordInput      = _ifsInput->getCoord();

      coordIndex.insert
        (coordIndex.end(), coordIndexInput.begin(),coordIndexInput.end());
      coord.insert
        (coord.end(), coordInput.begin(),coordInput.end());
    }

    // all the algorithms in this class assume that the mesh is a triangle mesh
    if(Geometry::isTriangulated(coordIndex)==false) {
      Geometry::triangulate(coord,coordIndex);
      if(_ifsOptimized->hasColorPerFace() || _ifsOptimized->hasColorPerCorner())
        _ifsOptimized->clearColor();
      if(_ifsOptimized->hasNormalPerFace() || _ifsOptimized->hasNormalPerCorner())
        _ifsOptimized->clearNormal();
      if(_ifsOptimized->hasTexCoordPerCorner())
        _ifsOptimized->clearTexCoord();
    }
    
    // and that it has normals per face
    if(_ifsOptimized->hasNormalPerFace()==false) {
      _ifsOptimized->clearNormal();
      _ifsOptimized->setNormalPerVertex(false); // default value is TRUE
      Geometry::computeNormalsPerFace
        (coord,coordIndex,_ifsOptimized->getNormal());
    }

    // construct the half edges and save them as a privaye variable
    IndexedFaceSetVariables ifsv(*_ifsOptimized);
    PolygonMesh* pmesh = ifsv.getPolygonMesh(true);

    // compute all the mesh edge lengths
    Geometry::computeEdgeLengths(coord,*pmesh,_edgeLengths);

    // compute some edge length statistics
    _minEdgeLength = 0.0f;
    _maxEdgeLength = 0.0f;
    int nE = pmesh->getNumberOfEdges();
    for(int iE=0;iE<nE;iE++) {
      float eLength = _edgeLengths[iE];
      if(iE==0 || eLength<_minEdgeLength) _minEdgeLength = eLength;
      if(iE==0 || eLength>_maxEdgeLength) _maxEdgeLength = eLength;
    }
    _targetEdgeLength = (_minEdgeLength+_maxEdgeLength)/2.0f;
  }
}

//////////////////////////////////////////////////////////////////////

void Optimization::setSelectedVertexIndex(const int value) {
  _vSelIndex = (value<0)?0:(value%64);
}

void Optimization::setSelectedEdgeIndex(const int value) {
  _eSelIndex = (value<0)?0:(value%64);
}

void Optimization::setSelectedFaceIndex(const int value) {
  _fSelIndex = (value<0)?0:(value%64);
}

int Optimization::getSteps() {
  return _steps;
}

void Optimization::setSteps(const int value) {
  _steps = (value>0)?value:0;
}

float Optimization::getLambda() {
  return _lambda;
}

void  Optimization::setLambda(const float value) {
  _lambda = value;
}

float Optimization::getMu() {
  return _mu;
}

void  Optimization::setMu(const float value) {
  _mu = value;
}

// kappa : pass-band frequency
float Optimization::getKappa() {
  float kappa = (_lambda+_mu)/(_lambda*_mu);
  return kappa;
}

// kappa : pass-band frequency
void Optimization::setKappa(const float value) {
  float kappa = (0.0f<value)?value:0.001;
  if(_lambda*kappa>=1.0f) _lambda = (1.0f/kappa)-0.001f;
  _mu = _lambda/(_lambda*kappa-1.0f);
}

float Optimization::getJacobiWeightData() {
  return 1.0f-_jacobiWeightSmoothing;
}

void  Optimization::setJacobiWeightData(const float value) {
  _jacobiWeightSmoothing = (value<0.0f)?1.0f:(value>1.0f)?0.0f:(1.0f-value);
}

float Optimization::getJacobiWeightSmoothing() {
  return _jacobiWeightSmoothing;
}

void  Optimization::setJacobiWeightSmoothing(const float value) {
  _jacobiWeightSmoothing = (value<0.0f)?0.0f:(value>1.0f)?1.0f:value;
}

float Optimization::getIntegrateNormalsWeightData() {
  return _integrateNormalsWeightData;
}

void  Optimization::setIntegrateNormalsWeightData(const float value) {
  float valueData = (value<0.0f)?0.0f:value;
  _integrateNormalsWeightData = valueData;
}

float Optimization::getIntegrateNormalsWeightLink() {
  return _integrateNormalsWeightLink;
}

void  Optimization::setIntegrateNormalsWeightLink(const float value) {
  float valueLink = (value<0.0f)?0.0f:value;
  _integrateNormalsWeightLink = valueLink;
}

float Optimization::getIntegrateNormalsWeightSmoothing() {
  return _integrateNormalsWeightSmoothing;
}

void  Optimization::setIntegrateNormalsWeightSmoothing(const float value) {
  float valueSmoothing = (value<0.0f)?0.0f:value;
  _integrateNormalsWeightSmoothing = valueSmoothing;
}

//////////////////////////////////////////////////////////////////////
// TASK 1)
//

void Optimization::laplacianSmoothingVertexCoordinatesRun() {

  // Jacobi iteration for energy function
  // E(x) = \sum_{0<=iE<nE} {1} \| x_{iV0}-x_{iV1}^0\|^2

  // std::cout << "void Optimization::laplacianSmoothingVertexCoordinatesRun() {\n";
  // std::cout << "  lambda = " << _lambda << "\n";
  // std::cout << "  mu     = " << _mu     << "\n";
  // std::cout << "  steps  = " << _steps  << "\n";

  IndexedFaceSetVariables ifsv(*_ifsOptimized);
  PolygonMesh* pmesh = ifsv.getPolygonMesh(true);
  int nV = pmesh->getNumberOfVertices();
  int nE = pmesh->getNumberOfEdges();

  // this is the signal that we want to smooth over the primal graph
  vector<float>& x = _ifsOptimized->getCoord();

  // allocate local arrays to accumulate displacements and weights
  vector<float> dx(3*nV,0.0f);
  vector<float> wx(nV,0.0f);

  for(int step=0;step<_steps;step++) {

    // zero accumulators dx[] and w[]

    // accumulate displacement vectors and weights
    //
    // for each edge iE (iV0,iV1) {
    //   let dx_iV0_iV1  be the displacement vector from x_iV0 to x_iV1
    //   let  w_iE>0 be the weight assigned to the edge (such as 1.0)
    //   add  dx_iV0_iV1 multiplied by w_iE to the accumulator dx_iV0
    //   add  w_iE to the accumulator wx_iV0
    //   add -dx_iV0_iV1 multiplied by w_iE to the accumulator dx_iV1
    //   add  w_iE to the accumulator wx_iV1
    // }
    //
    // normalize the displacement vectors
    // (make sure that you do not divide by zero!)
    //
    // for each vertex iV {
    //   dx_iV /= wx_iV
    // }

    // alternate between _lambda and _mu for even and odd step values
    float lambda = (step%2==0)?_lambda:_mu;

    // apply displacement
    //
    // for each vertex iV {
    //   x_iV += lambda * dx_iV
    // }

  }

  // since the vertex coordinates have changed, recompute face normals
  _ifsOptimized->clearNormal();
  _ifsOptimized->setNormalPerVertex(false);
  vector<float>& coord      = _ifsOptimized->getCoord(); // == x
  vector<float>& normal     = _ifsOptimized->getNormal();
  vector<int>&   coordIndex = _ifsOptimized->getCoordIndex();
  Geometry::computeNormalsPerFace(coord,coordIndex,normal);

  // Remember that none of the smoothing methods modify the
  // connectivity of the mesh
  // - as a result, if the mesh had any other properties, they are
  //   still valid after smoothing
  // - same thing with respect to selections
  
  // it would be useful to preserve the face colors set by some of the
  // remeshing methods
  // _ifsOptimized->clearColor();

  // it is very unlikely that any of the meshes that we use will have
  // texture coordinates
  // _ifsOptimized->clearTexCoord();

  // it would be useful to preserve the selections set by some of the
  // remeshing methods
  // ifsv.clearAllSelection();

  // std::cout << "}\n";
}

//////////////////////////////////////////////////////////////////////
// TASK 2)
//
void Optimization::laplacianSmoothingFaceNormalsRun(const bool normalize) {

  // Jacobi iteration for energy function
  // E(n) = \sum_{0<=iE<nED} {1} \| n_{iF0}-n_{iF1}^0\|^2

  // std::cout << "void Optimization::laplacianSmoothingFaceNormalsRun() {\n";
  // std::cout << "  normalize         = " << ((normalize)?"true":"false") << "\n";
  // std::cout << "  lambda            = " << _lambda << "\n";
  // std::cout << "  mu                = " << _mu     << "\n";
  // std::cout << "  steps             = " << _steps  << "\n";

  IndexedFaceSetVariables ifsv(*_ifsOptimized);
  PolygonMesh* pmesh = ifsv.getPolygonMesh(true);
  int nF = pmesh->getNumberOfFaces();
  int nE = pmesh->getNumberOfEdges();

  // we need normals per face not indexed
  if(_ifsOptimized->getNormalBinding()!=IndexedFaceSet::Binding::PB_PER_FACE) {
    // recompute face normals
    _ifsOptimized->clearNormal();
    _ifsOptimized->setNormalPerVertex(false);
    vector<float>& coord      = _ifsOptimized->getCoord();
    vector<float>& normal     = _ifsOptimized->getNormal();
    vector<int>&   coordIndex = _ifsOptimized->getCoordIndex();
    Geometry::computeNormalsPerFace(coord,coordIndex,normal);
  }

  // this is the signal that we want to smooth over the dual graph
  vector<float>& n = _ifsOptimized->getNormal();

  // allocate local arrays to accumulate the normal displacements and weights
  vector<float> dn(3*nF,0.0f);
  vector<float> wn(nF,0.0f);

  for(int step=0;step<_steps;step++) {

    // zero accumulators dn[] and 

    // accumulate displacement vectors
    //
    // for each regular edge iE {
    //   find the two faces iF0 and iF1 incident to the edge
    //   let dn_iF0_iF1  be the displacement vector from n_iF0 to n_iF1
    //   let w_iE>0 be the weight assigned to the edge (such as 1.0)
    //   add  dn_iF0_iF1 multiplied by w_iE to the accumulator dn_iF0
    //   add  w_iE to the accumulator wn_iF0
    //   add -dx_iF0_iF1 multiplied by w_iE to the accumulator dn_iF1
    //   add  w_iE to the accumulator wn_iF1
    // }
    //
    // normalize the displacement vectors
    // (make sure that you do not divide by zero!)
    //
    // for each face iF {
    //   dn_iF /= wn_iF
    // }

    // alternate between _lambda and _mu for even and odd step values
    float lambda = (step%2==0)?_lambda:_mu;

    // apply displacement
    //
    // for each face iF {
    //   n_iF += lambda * dn_iF
    // }

  }

  if(normalize) {
    // normalize face normals to unit length
  }

  // clear colors (or maybe not?)
  _ifsOptimized->clearColor();

  // clear all selection buffers (or maybe not?)
  ifsv.clearAllSelection();
}

//////////////////////////////////////////////////////////////////////
// TASK 3)
//

void  Optimization::jacobiRun() {

  // This method is very similar to
  // Optimization::laplacianSmoothingVertexCoordinatesRun() 

  // Jacobi iteration for energy function
  // E(x) = (1-sigma)*\sum_{0<=iV<nV} \| x_{iV}-x0_{iV}\|^2 +
  //        (  sigma)*\sum_{0<=iE<nE} \| x_{iV0}-x_{iV1}\|^2

  // std::cout << "void Optimization::laplacianSmoothingRun() {\n";
  // std::cout << "  lambda = " << _lambda << "\n";
  // std::cout << "  mu     = " << _mu     << "\n";
  // std::cout << "  steps  = " << _steps  << "\n";

  IndexedFaceSetVariables ifsv(*_ifsOptimized);
  PolygonMesh* pmesh = ifsv.getPolygonMesh(true);
  int nV = pmesh->getNumberOfVertices();
  int nE = pmesh->getNumberOfEdges();

  // these are the target coordinates 
  vector<float>& x0 = _ifsInput->getCoord();
  
  // this is the signal that we want to smooth over the primal graph
  vector<float>& x = _ifsOptimized->getCoord();

  // allocate local arrays to accumulate displacements and weights
  vector<float> dx(3*nV,0.0f);
  vector<float> wx(nV,0.0f);

  float sigma = _jacobiWeightSmoothing;
  // setJacobiWeightSmoothing(const float value);
  // constraints this value to 0<=sigma<=1
  // note that if sigma==1.0f then this method produces the same result as
  // Optimization::laplacianSmoothingVertexCoordinatesRun() 
  // and if sigma==0 the regularization term is ignored 

  for(int step=0;step<_steps;) {

    // zero accumulators dx[] and wx[]

    // accumulate displacement vectors and weights
    //
    // 1) accumulate the contribution of the data term
    //
    // w = 1.0f-sigma;
    // for each vertex iV {
    //   let dx_iV be the displacement vector from x_iV to x0_iV
    //   add dx_iV multiplied by w to the accumulator dx_iV
    //   add the weight w to the accumulator wx_iV
    // 
    // }

    // 2) accumulate the contribution of the regularization term
    //    - note that this part can be copied from
    //      Optimization::laplacianSmoothingVertexCoordinatesRun() 
    //
    // w = sigma;
    // for each edge iE (iV0,iV1) {
    //   let dx_iV0_iV1  be the displacement vector from x_iV0 to x_iV1
    //   add  dx_iV0_iV1 multiplied by w to the accumulator dx_iV0
    //   add  w to the accumulator wx_iV0
    //   add -dx_iV0_iV1 multiplied by w to the accumulator dx_iV1
    //   add  w to the accumulator wx_iV1
    // }
    //
    // normalize the displacement vectors
    // (make sure that you do not divide by zero!)
    //
    // for each vertex iV {
    //   dx_iV /= wx_iV
    // }

    // alternate between _lambda and _mu for even and odd step values
    float lambda = (step%2==0)?_lambda:_mu;

    // apply displacement
    //
    // for each vertex iV {
    //   x_iV += lambda * dx_iV
    // }
  }

  // since the vertex coordinates have changed, recompute face normals
  _ifsOptimized->clearNormal();
  _ifsOptimized->setNormalPerVertex(false);
  vector<float>& coord      = _ifsOptimized->getCoord();
  vector<float>& normal     = _ifsOptimized->getNormal();
  vector<int>&   coordIndex = _ifsOptimized->getCoordIndex();
  Geometry::computeNormalsPerFace(coord,coordIndex,normal);

  // clear colors ???
  _ifsOptimized->clearColor();

  // clear all selection buffers ???
  ifsv.clearAllSelection();

  // std::cout << "}\n";
}

//////////////////////////////////////////////////////////////////////
// TASK 4)
//

//////////////////////////////////////////////////////////////////////
void  Optimization::integrateNormalsRun(const bool recomputeFaceNormals) {
  
  // std::cout << "void Optimization::integrateNormalsRun() {\n";

  static float epsA = 0.95f;
  static float epsI = 0.05f;

  // Jacobi iteration for energy function
  //
  // E(x) = E_D(x) + E_L(x) + E_S(n)
  //
  // E_D(x) = (sigmaData)*
  //            \sum_{vertex iV} \| x_{iV}-x_{iV}^0\|^2 +
  // E_L(x) = (sigmaLink)*
  //            \sum_{half edge (iF,iV0,iV1)} (n_{iF}^t(x_{iV0}-x_{iV1}))^2
  // E_S(x) = (sigmaSmoothing)*
  //            \sum_{edge (iV0,iV1)} \| x_{iV0}-x_{iV1} \|^2
  //
  // where the face normal vectors n_{iF}
  // and the target vertex positions x_{iV}^0
  // are regarded as known constants

  // Note that E(x) is a quadratic function of the vertex coordinates x
  //
  // E(x) = x^t * W * x + 2*b^t*x + c
  //
  // where x is represented here as a N=3*nV dimensional vector,
  // W is a symmetric non-negative NxN matrix, b is a N-dimensional
  // vector, and c is a scalar
  //
  // Furthermore, the matrix W is a diagonal block matrix, with a 3x3
  // block corresponding to each vertex. And while the three energy
  // terms contribute to the W matrix, only the data term E_D(x)
  // contribute to the vector b and the scalar c.

  IndexedFaceSetVariables ifsv(*_ifsOptimized);
  PolygonMesh* pmesh = ifsv.getPolygonMesh(true);

  // get target vertex coordinates
  vector<float>& coord0     = _ifsInput->getCoord();
  
  // get output vertex coordinates
  vector<float>& coord      = _ifsOptimized->getCoord();

  // make sure that _ifsOptimized has normals per face not indexed
  if(_ifsOptimized->getNormalBinding()!=IndexedFaceSet::Binding::PB_PER_FACE) {
    // if not, delete existing normals
    _ifsOptimized->clearNormal();
    // and recompute from the vertex coordinates
    _ifsOptimized->setNormalPerVertex(false);
    vector<int>& coordIndex = _ifsOptimized->getCoordIndex();
    Geometry::computeNormalsPerFace(coord,coordIndex,_ifsOptimized->getNormal());
  }
  vector<float>& normal     = _ifsOptimized->getNormal();

  int   nV,nE,nF,nC,nHE,iV,iV0,iV1,iF,iE,iC,k;
  float dW[6],dxe[3];
  float xk,dxk,n0,n1,n2,dx0,dx1,dx2,u0,u1,u2,v0,v1,v2;
  float W0,W1,W2,W3,W4,W5,Wd,L0,L1,L2,L3,L4,L5;
  bool not_positive_definite;

  nV = pmesh->getNumberOfVertices(); // number of vertices
  nE = pmesh->getNumberOfEdges();    // number of edges
  nF = pmesh->getNumberOfFaces();    // number of faces
  nC = pmesh->getNumberOfCorners();   // number of corners
  // numberOfHalfEdges
  for(nHE=iC=0;iC<nC;iC++) { if(pmesh->getFace(iC)>=0) nHE++; }

  // throw exception if not true ?
  // assert(static_cast<int>(coord.size())==3*nV);
  // assert(static_cast<int>(normal.size())==3*nF);

  vector<float>& x0 = coord0; // target vertex coordinates
  vector<float>& x  = coord;  // variable vertex coordinates
  vector<float>& n  = normal; // fixed face normals
  vector<float>  dx(3*nV,0.0f); // displacement vectors
  
  // coord weights are 3x3 symmetric matrices
  //            [ 6*iV+0 6*iV+1 6*iV+3 ]
  // W_{iV} =  [ 6*iV+1 6*iV+2 6*iV+4 ]
  //            [ 6*iV+3 6*iV+4 6*iV+5 ]
  vector<float>  W(6*nV,0.0f);

  if(_integrateNormalsWeightData     <0.0f) _integrateNormalsWeightData      = 0.0f;
  if(_integrateNormalsWeightLink     <0.0f) _integrateNormalsWeightLink      = 0.0f;
  if(_integrateNormalsWeightSmoothing<0.0f) _integrateNormalsWeightSmoothing = 0.0f;

  // std::cout << "  weigthData      = " << _integrateNormalsWeightData      << "\n";
  // std::cout << "  weightLink      = " << _integrateNormalsWeightLink      << "\n";
  // std::cout << "  weightSmoothing = " << _integrateNormalsWeightSmoothing << "\n";

  // normalize the weights by the number of elements contributing to
  // each energy term
  float totalWeight =
    _integrateNormalsWeightData+
    _integrateNormalsWeightLink+
    _integrateNormalsWeightSmoothing;

  // totalWeight should be >0
  // throw exception if not true ?
  // assert(totalWeight>0.0);

  float sigmaData = (totalWeight==0.0f)?0.0f:
    100.0f*_integrateNormalsWeightData/totalWeight/static_cast<float>(nV);
  float sigmaLink = (totalWeight==0.0f)?0.0f:
    100.0f*_integrateNormalsWeightLink/static_cast<float>(nHE);
  float sigmaSmoothing = (totalWeight==0.0f)?0.0f:
    100.0f*_integrateNormalsWeightSmoothing/static_cast<float>(nE);

  // std::cout << "  sigmaData      = " << sigmaData  << "\n";
  // std::cout << "  sigmaLink      = " << sigmaLink  << "\n";
  // std::cout << "  sigmaSmoothing = " << sigmaSmoothing  << "\n";

  // std::cout << "  loop {\n";

  for(int step=0;step<_steps;) {

    // std::cout << "    step = " << step << "\n";

    //  while(_stop) {
    //    // wait here until _stop = false
    //  }

    // zero accumulators
    std::fill(dx.begin(), dx.end(), 0.0f);
    std::fill(W.begin(), W.end(), 0.0f);

    // accumulate contributions to displacement vectors from first energy term
    //
    // (sigmaData)*\sum_{vertex iV} \| x_{iV}-x_{iV}^0\|^2

    if(sigmaData>0.0f) {
      for(iV=0;iV<nV;iV++) {
        // ...
      }
    }

    // accumulate contributions to displacement vectors from the second energy term
    //
    // \sum_{half edge (iF,iV0,iV1)} (sigmaLink)*(n_{iF}^t(x_{iV0}-x_{iV1}))^2

    if(sigmaLink>0.0f) {
      // for each half edge iC (i.e., corner) ...
      for(iC=0;iC<nC;iC++) {
        // get the face index iF corresponding to the half edge iC
        // get the two vertex indices iV0,iV1  corresponding to the half edge iC
        // compute the current displacement from iV0 to iV1
        // retrieve the face normal corresponding to face iF
        // accumulate contributions to W matrix
        // accumulate contributions to displacement vectors
      }
    }

    // accumulate contributions to displacement vectors from smoothing term
    //
    // \sum_{edge (iV0,iV1)} (sigmaSmoothing) * \| x_{iV0}-n_{iV1} \|^2

    if(sigmaSmoothing>0.0f) {
      // for each edge ...
      for(iE=0;iE<nE;iE++) {
        // get the two vertex indices iV0,iV1  corresponding to the edge iE
        // compute the edge displacement vector from iV0 to iV1
        // accumulate contributions to W matrix
        // accumulate contributions to displacement vectors
      }
    }

    // normalize vertex coord displacements
    // for each vertex iV ...
    for(iV=0;iV<nV;iV++) {

      // get 3x3 vertex weight matrix W corresponding to vertex iV (6 coefficients)
      //
      // [ W0 W1 W3 ]
      // [ W1 W2 W4 ]
      // [ W3 W4 W5 ]

      // std::cout << "W["<<iV<<"] = [\n";
      // std::cout << " " << W0 << " " << W1 << " " << W3 << "\n";
      // std::cout << " " << W1 << " " << W2 << " " << W4 << "\n";
      // std::cout << " " << W3 << " " << W4 << " " << W5 << "\n";
      // std::cout << "];\n";

      // since this matrix must be positive definite,
      // compute its Cholesky Decomposition W = L*L^t
      //
      // [ L0  0  0 ]   [ L0 L1 L3 ]   [ W0 W1 W3 ]
      // [ L1 L2  0 ] * [  0 L2 L4 ] = [ W1 W2 W4 ]
      // [ L3 L4 L5 ]   [  0  0 L5 ]   [ W3 W4 W5 ]
      //
      // no need to call a library function for a 3x3 matrix !
      //
      // L0*L0             = W0 => L0 = sqrt(W0);
      // L1*L0             = W1 => L1 = W1/L0;
      // L1*L1+L2*L2       = W2 => L2 = sqrt(W2-L1*L1);
      // L3*L0             = W3 => L3 = W3/L0;
      // L3*L1+L4*L2       = W4 => L4 = (W4-L3*L1)/L2;
      // L3*L3+L4*L4+L5*L5 = W5 => L5 = sqrt(W5-L3*L3-L4*L4);

      not_positive_definite = false;
      if(W0<=0.0f) {
        not_positive_definite = true;
      } else {
        L0 = std::sqrt(W0);
        L1 = W1/L0;
        W2 -= L1*L1;
        if(W2<0.0f) {
          not_positive_definite = true;
        } else {
          L2 = std::sqrt(W2);
          L3 = W3/L0;
          L4 = (W4-L3*L1)/L2;
          W5 -= L3*L3;
          W5 -= L4*L4;
          if(W5<=0.0f) {
            not_positive_definite = true;
          } else {
            L5 = sqrt(W5);
          }
        }
      }

      // std::cout << "Lx["<<iV<<"] = [\n";
      // std::cout << " " << L0 << " " <<  0 << " " <<  0 << "\n";
      // std::cout << " " << L1 << " " << L2 << " " <<  0 << "\n";
      // std::cout << " " << L3 << " " << L4 << " " << L5 << "\n";
      // std::cout << "];\n";

      if(not_positive_definite) {
        // error handling : the matrix can be singular in patological cases

        dx[3*iV  ] = 0.0f;
        dx[3*iV+1] = 0.0f;
        dx[3*iV+2] = 0.0f;

      } else {

        // get accumulated displacement for vertex iV
        // u0 = ... ;
        // u1 = ... ;
        // u2 = ... ;
        
        // solve for v : u = L*v
        //
        // [ u0 ]   [ L0  0  0 ]   [ v0 ]
        // [ u1 ] = [ L1 L2  0 ] * [ v1 ]
        // [ u2 ]   [ L3 L4 L5 ]   [ v2 ]
        //
        // u0 = L0 * v0
        // u1 = L1 * v0 + L2 * v1
        // u2 = L3 * v0 + L4 * v1 + L5 * v2

        // solve for u : v = L^t*u
        //
        // [ v0 ]   [ L0 L1 L3 ]   [ u0 ]
        // [ v1 ] = [  0 L2 L4 ] * [ u1 ]
        // [ v2 ]   [  0  0 L5 ]   [ u2 ]
        //
        // v0 = L0 * u0 + L1 * u1 + L3 * u2
        // v1 =           L2 * u1 + L4 * u2
        // v2 =                     L5 * u2

        // set displacement vector for vertex iV
      }
    }

    // apply displacement using the lambda-mu method (mu can be equal to lambda)
    float lambda = (step%2==0)?_lambda:_mu;

    float errV = 0.0;
    if(true) {
      for(iV=0;iV<nV;iV++) {
        for(k=0;k<3;k++) {
          dxk = lambda*dx[3*iV+k];
          xk  =         x[3*iV+k];
          x[3*iV+k] = xk+dxk;
          errV += dxk*dxk;
        }
      }
      errV /= static_cast<float>(nV);
      errV = std::sqrt(errV);
    }

    // std::cout << std::setprecision(6)
    //           << "    errV[" << step << "] = "<< errV <<"\n";

    ++step;
  } //   for(int step=0;step<_steps;)

  // std::cout << "  }\n";

  // clear colors
  _ifsOptimized->clearColor();

  // recompute face normals
  if(recomputeFaceNormals) {
    _ifsOptimized->clearNormal();
    _ifsOptimized->setNormalPerVertex(false);
    vector<int>& coordIndex = _ifsOptimized->getCoordIndex();
    Geometry::computeNormalsPerFace(coord,coordIndex,normal);
  }

  // clear all selection buffers
  ifsv.clearAllSelection();

  // std::cout << "}\n";
}

//////////////////////////////////////////////////////////////////////
// SIMPLIFICATION BY EDGE COLLAPSES
// REFINEMENT BY SPLITTING EDGES
// VALENCE OPTIMIZATION BY EDGE FLIPS

vector<float>& Optimization::getEdgeLengths() {
  return _edgeLengths;
}

float Optimization::getMinEdgeLength() {
  return _minEdgeLength;
}

float Optimization::getMaxEdgeLength() {
  return _maxEdgeLength;
}

float Optimization::getTargetEdgeLength() {
  return _targetEdgeLength;
}

void Optimization::setMinEdgeLength(const float value) {
  _minEdgeLength = (value<0.0f)?0.0f:value;
}

void Optimization::setMaxEdgeLength(const float value) {
  _maxEdgeLength = (value<_minEdgeLength)?_minEdgeLength:value;
}

void Optimization::setTargetEdgeLength(const float value) {
  _targetEdgeLength =
    (value<_minEdgeLength)?_minEdgeLength:
    (value>_maxEdgeLength)?_maxEdgeLength:
    value;
}

//////////////////////////////////////////////////////////////////////
// TASK 5)
//

void Optimization::_collapseEdgesSelect
(const EdgeCollapseErrorMetric    errorMetric,
 const EdgeCollapseIndependentSet indepSet,
 vector<int>&                     edgeSelection,
 bool                             colorIncidentFaces) {

  int   i,iV,iF,iE,iV0,iV1,iV2,iV3,nEF;
  int   iC,iC0,iC1,iC0n,iC0p,iC1n,iC1p;
  float M[10];

  IndexedFaceSetVariables ifsv(*_ifsOptimized);
  PolygonMesh* pmesh = ifsv.getPolygonMesh(true);

  int nV = pmesh->getNumberOfVertices();
  int nE = pmesh->getNumberOfEdges();
  int nF = pmesh->getNumberOfFaces();

  vector<float>* qVMatrix4x4 = nullptr;
  if(errorMetric==EdgeCollapseErrorMetric::GARLAND_HECKBERT) {
    bool initialize = false;
    Variable* var  = _ifsOptimized->getVariable("qVMatrix4x4");
    if(var==(Variable*)0) {
      var = new VariableVecFloat("qVMatrix4x4",10*nV,0.0f);
      _ifsOptimized->setVariable(var);
      initialize = true;
    }
    qVMatrix4x4 = (vector<float>*)(var->getValue());
    if(static_cast<int>(qVMatrix4x4->size())!=10*nV) {
      qVMatrix4x4->clear();
      qVMatrix4x4->reserve(10*nV);
      for(int i=0;i<10*nV;i++)
        qVMatrix4x4->push_back(0.0f);
      initialize = true;
    }

    if(initialize) _initializeGarlandHeckbert();
  } // if(errorMetric==EdgeCollapseErrorMetric::GARLAND_HECKBERT)
  
  // clear output edge selection array
  edgeSelection.clear();
  edgeSelection.resize(nE,-1); // no edge is initially selected

  // color faces for visualization purposes
  vector<int>*   colorIndex = (vector<int>*)0;
  vector<float>* color      = (vector<float>*)0;
  int noChangesColorIndex     = 0;
  int collapseEdgesColorIndex = 1;
  if(colorIncidentFaces) {

    // set _ifsOptimized
    // as color per face indexed with two colors
    colorIndex = &(_ifsOptimized->getColorIndex());
    colorIndex->clear();
    color      = &(_ifsOptimized->getColor());
    color->clear();
    _ifsOptimized->setColorPerVertex(false);

    // color 0
    Color noChangesColor(0.8f,0.8f,0.8f);
    noChangesColorIndex =
      static_cast<int>(color->size()/3); // ==0
    color->push_back(noChangesColor.r);
    color->push_back(noChangesColor.g);
    color->push_back(noChangesColor.b);

    // color 1
    Color collapseEdgesColor(0.9f,0.5f,0.7f);
    collapseEdgesColorIndex =
      static_cast<int>(color->size()/3); // ==1
    color->push_back(collapseEdgesColor.r);
    color->push_back(collapseEdgesColor.g);
    color->push_back(collapseEdgesColor.b);

    // initially paint all the faces with color noChangesColorIndex
    for(int iF=0;iF<nF;iF++)
      colorIndex->push_back(noChangesColorIndex);
  }

  // compute error metric for all edges and insert them into a
  Heap heap;
  if(errorMetric==EdgeCollapseErrorMetric::GARLAND_HECKBERT) {
    float *M0,*M1,x[3];
    for(iE=0;iE<nE;iE++) {
      iV0 = pmesh->getVertex0(iE);
      iV1 = pmesh->getVertex1(iE);
      M0  = &((*qVMatrix4x4)[10*iV0]);
      M1  = &((*qVMatrix4x4)[10*iV1]);
      float err_iE = _errorGarlandHeckbert(M0,M1,M,x);
      heap.add(err_iE,iE);
    }
  } else /* if(errorMetric==EdgeCollapseErrorMetric::EDGE_LENGTH) */ {
    for(iE=0;iE<nE;iE++) {
      heap.add(_edgeLengths[iE],iE);
    }
  }

  // creating an independent set ef edges to split ...

  // initially mark all the vertices as not used
  vector<bool> usedVertex(nV,false);

  while(heap.length()>0) {
    /* iH = */  heap.delMin();
    iE      =  heap.getLastIKey();

    // you may want to set up a threshold so that only edges shorter
    // than the threshold are collapsed
    //
    // float eLength = heap.getLastFKey();
    // if(eLength>=_lowEdgeLength) break;

    // if edge is not regular, continue

    // if either one of the two vertices iV0 and iV1 of the edge iE
    // are marked as used, continue

    // mark the edge as selected 
    edgeSelection[iE] = _eSelIndex;

    // mark the two vertices, and depending on the value of indepSet,
    // some neighboring vertices as used
    //
    // do you need to take relative orientation into account?
    //
    //        iV3                      iV3
    //     /       \                /       \
    //    / <-iC1-- \              / --iC1-> \
    // iV0 --------- iV1   or   iV0 --------- iV1
    //    \ --iC0-> /              \ --iC0-> /
    //     \       /                \       /
    //        iV2                      iV2
    //
    // mark the x vertices as used
    switch(indepSet) {
    case EdgeCollapseIndependentSet::VERTICES_8:
      //     x - x - x
      //      \ / \ /
      // iV0-> x - x <- iV1
      //      / \ / \
      //     x - x - x
      break;
    case EdgeCollapseIndependentSet::VERTICES_4:
      //          x
      //         / \
      // iV0 -> x - x <- iV1
      //         \ /
      //          x
      break;
    case EdgeCollapseIndependentSet::VERTICES_2:
      //          o
      //         / \
      // iV0 -> x - x <- iV1
      //         \ /
      //          o
      break;
    }

    // for visualization purposes
    // set the colorIndex of the two faces incident to the edge to
    // collapseEdgesColorIndex;
    nEF = pmesh->getNumberOfEdgeFaces(iE);
    for(i=0;i<nEF;i++) {
      iF = pmesh->getEdgeFace(iE,i);
      if(colorIncidentFaces) {
        (*colorIndex)[iF] = collapseEdgesColorIndex;
      }
    }

  }
}

//////////////////////////////////////////////////////////////////////
// TASK 6)
//

// TODO 20250808 : convert into task

void Optimization::_initializeGarlandHeckbert() {

  IndexedFaceSetVariables ifsv(*_ifsOptimized);
  PolygonMesh* pmesh = ifsv.getPolygonMesh(true);

  Variable* var = _ifsOptimized->getVariable("qVMatrix4x4");
  vector<float>* qVMatrix4x4 = (vector<float>*)(var->getValue());

  vector<int>&   coordIndex  = _ifsOptimized->getCoordIndex();
  vector<float>& coord       = _ifsOptimized->getCoord();
  vector<int>&   normalIndex = _ifsOptimized->getNormalIndex();
  vector<float>& normal      = _ifsOptimized->getNormal();

  int nF = pmesh->getNumberOfFaces();
  int nC = pmesh->getNumberOfCorners();

  // we need normals per face, indexed or not
  bool indexed=false;
  if(_ifsOptimized->hasNormalPerFace()==false) {
    // if _ifsOptimized does not have normals per face
    // delete and recompute
    _ifsOptimized->clearNormal();
    _ifsOptimized->setNormalPerVertex(false);
    Geometry::computeNormalsPerFace(coord,coordIndex,normal);
  } else if(static_cast<int>(normalIndex.size())==nF) {
    indexed = true;
  }

  int j,iF,iC,iC0,iC1,iN,iV;
  float nf0,nf1,nf2,df,p0,p1,p2,M[10];

  // for each face iF, statrting at corner iC, and ending at corner iC1-1 ...
  for(iF=iC0=iC1=0;iC1<nC;iC1++) {
    if(coordIndex[iC1]>=0) continue;
    // get face iF normal vector
    // nf0 = normal[...];
    // nf1 = normal[...];
    // nf2 = normal[...];

    // get one vertex of the face, and its coordinates
    iV  = coordIndex[iC0];
    // p0  = coord[...];
    // p1  = coord[...];
    // p2  = coord[...];

    // compute the fourth df parameter of the implicit equation
    // f(p) = nf^t(p-p0) = nf^t p + (-nf^t p0) =  nf^t p + df

    // build the face rank-1 4x4 matrix
    // [ M[0] M[1] M[3] M[6] ]
    // [ M[1] M[2] M[4] M[7] ]
    // [ M[3] M[4] M[5] M[8] ]
    // [ M[6] M[7] M[8] M[9] ]

    // accumulate on qVMatrix4x4 for the face vertices

    // look for next face
    iC0 = iC1+1; iF++;
  }

}

//////////////////////////////////////////////////////////////////////
// TASK 7)
//

float Optimization::_errorGarlandHeckbert
(const float M0[/*10*/], const float M1[/*10*/],
 float M[/*10*/], float x[/*3*/]) {
  float a00,a10,a11,a20,a21,a22,b0,b1,b2,c,n_planes,l0,l1,l2,l3,l4,l5;

  for(int j=0;j<10;j++) {
    M[j] = M0[j] + M1[j];
  }

  //     [ M[0] M[1] M[3] ]     [ M[6] ]
  // A = [ M[1] M[2] M[4] ] b = [ M[7] ] c = M[9]
  //     [ M[3] M[4] M[5] ]     [ M[8] ]

  a00 = M[0];
  a10 = M[1]; a11 = M[2];
  a20 = M[3]; a21 = M[4]; a22 = M[5];
  b0  = M[6];  b1 = M[7];  b2 = M[8]; c = M[9];
  n_planes = a00+a11+a22; // number of planes in distance

  // minimum of
  //
  // q(x) = x^t*A*x+2*b^t*x+c
  //
  // attained at
  //
  // x_min = -A^{-1}*b
  //
  // minimum value attained @ x_min is
  //
  // q(x_min) = (-b^t*A^{-1})*A*(-A^{-1}*b) + 2*b^t*(-A^{-1}*b) + c
  //          = c - b^t*{A^-1}*b
  //          = c + b^t*x_min
  //
  //  will be used as error metric associated with edge iE

  // to compute x_min
  //
  // compute Cholesky decomposition of A = L*L^t
  //
  //     [ l0  0  0 ]       [ l0 l1 l3 ]
  // L = [ l1 l2  0 ] L^t = [  0 l2 l4 ]
  //     [ l3 l4 l5 ]       [  0  0 l5 ]
  //

  // solve A*x = L*(L^t*x) = -b;

  // compute edge error metric :
  float err_iE = c + b0*x[0] + b1*x[1] + b2*x[2];
  err_iE /= n_planes;

  return err_iE;
}

void Optimization::collapseEdgesShow
(const EdgeCollapseErrorMetric errorMetric,
 const EdgeCollapseIndependentSet indepSet) {
  if(_ifsOptimized==nullptr) return;
  IndexedFaceSetVariables ifsv(*_ifsOptimized);
  vector<int>& edgeSelection = ifsv.getEdgeSelection();
  _collapseEdgesSelect(errorMetric,indepSet,edgeSelection,true);
}

//////////////////////////////////////////////////////////////////////
// TASK 8)
//

void Optimization::collapseEdgesApply
(const EdgeCollapseErrorMetric errorMetric,
 const EdgeCollapseIndependentSet indepSet) {
  if(_ifsOptimized==nullptr) return;

  // select an independent set of edges
  //
  // use the _ifsOptimized edgeSelection array instead ?
  // vector<int>& edgeSelection = _ifsOptimized->getEdgeSelection();
  vector<int> edgeSelection;
  _collapseEdgesSelect(errorMetric,indepSet,edgeSelection,false);
  // the variable "qVMatrix4x4" should have been created
  vector<float>* qVMatrix4x4 = nullptr;
  if(errorMetric==EdgeCollapseErrorMetric::GARLAND_HECKBERT) {
    Variable* var  = _ifsOptimized->getVariable("qVMatrix4x4");
    qVMatrix4x4 = (vector<float>*)(var->getValue());
  }

  IndexedFaceSetVariables ifsv(*_ifsOptimized);
  PolygonMesh* pmesh = ifsv.getPolygonMesh(true);
  vector<float>& coord      = _ifsOptimized->getCoord();
  vector<int>&   coordIndex = _ifsOptimized->getCoordIndex();
  
  int nV = pmesh->getNumberOfVertices();
  int nE = pmesh->getNumberOfEdges();
  int nF = pmesh->getNumberOfFaces();
  int nC = pmesh->getNumberOfCorners();

  // map old vertices to new vertices and create new coord array
  vector<int>    vertexMap(nV,-1);
  vector<float>  newCoord;
  vector<float>  newQMatrix4x4;

  // iVnew = 0
  //
  // assign a new vertex index to each collapsed edge
  //
  // for each selected edge iE {
  //    assign new index iVnew to iV0 and iV1
  //    compute newCoord_iVnew as the midpoint of coord_iV0 and coord_iV1
  //    if(errorMetric==EdgeCollapseErrorMetric::GARLAND_HECKBERT) {
  //      save old values of qVMatrix4x4
  //    }
  //    increment iVnew
  // }
  //
  // assign new vertex indices to vertices which are not ends of
  // collapsed edges
  //
  // for each vertex iV {
  //   if vertex iV is not end of a selected edge {
  //      assign new index iVnew to iV
  //      copy coord_iV onto newCoord_iVnew
  //      if(errorMetric==EdgeCollapseErrorMetric::GARLAND_HECKBERT) {
  //        use values stored in newQMatrix4x4 and _errorGarlandHeckbert()
  //        to generate new matrix for the new vertex and to compute
  //        coordinates of the new vertex
  //      } else {
  //        set coordinates of the new vertex as edge midpoint
  //      }
  //      increment iVnew
  //   }
  // }

  // since faces incident to selected edges must be deleted, you may
  // want to create a data structure to identify them before the next
  // loop

  // create the new coordIndex array
  vector<int> newCoordIndex;
  // traverse the coordIndex array
  int iF,iC0,iC1,iC;
  for(iF=iC0=iC1=0;iC1<nC;iC1++) {
    if(coordIndex[iC1]>=0) continue;
    //
    // if face iF is incident to a selected edge, it has to be
    // deleted, continue
    //
    // for each corner iC of face iF {
    //   using the vertexMap array determine the new vertex index iVnew
    //   push_back that value onto the new coordIndex array
    // }
    // don't forget to add a face separator
    //
    // advance to next face
    iC0 = iC1+1; iF++;
  }

  // fix _ifsRendering
  coord.swap(newCoord);
  coordIndex.swap(newCoordIndex);

  // the PolygonMesh is no longer valid
  ifsv.deletePolygonMesh();

  // since the number of vertices and faces has changed colors and
  // are no longer valid
  _ifsOptimized->clearColor();
  // if the mesh had colors and you want to preserve them, a better
  // solution would be to map old colors onto new colors

  // since a lot of vertices and faces may have changed, recompute
  // face normals
  _ifsOptimized->clearNormal();
  _ifsOptimized->setNormalPerVertex(false);
  Geometry::computeNormalsPerFace(coord,coordIndex,_ifsOptimized->getNormal());

  // clear all selection buffers
  ifsv.clearAllSelection();

  _ifsOptimized->printInfo("  ");
}

//////////////////////////////////////////////////////////////////////
// TASK 9)
//

void Optimization::_adaptiveSubdivisionSelect
(vector<int>& vertexSelection,
 vector<int>& edgeSelection,
 const SplitEdgesMode mode,
 bool colorIncidentFaces) {

  if(_ifsOptimized==nullptr) return;
  vector<int>& coordIndex = _ifsOptimized->getCoordIndex();
  if(Geometry::isTriangulated(coordIndex)==false) return;

  IndexedFaceSetVariables ifsv(*_ifsOptimized);
  PolygonMesh* pmesh = ifsv.getPolygonMesh(true);
  
  int nV = pmesh->getNumberOfVertices();
  int nE = pmesh->getNumberOfEdges();
  int nF = pmesh->getNumberOfFaces();
  int nC = pmesh->getNumberOfCorners();

  // for visualization purposes
  // set color per face indexed with three colors
  vector<int>*   colorIndex             = (vector<int>*)0;
  vector<float>* color                  = (vector<float>*)0;
  int            noSplitColorIndex      = 0;
  int            split2ColorIndex       = 1;
  int            split4ColorIndex       = 2;
  if(colorIncidentFaces) {

    colorIndex = &(_ifsOptimized->getColorIndex());
    colorIndex->clear();
    colorIndex->resize(nF,noSplitColorIndex);

    color = &(_ifsOptimized->getColor());
    color->clear();

    _ifsOptimized->setColorPerVertex(false);

    Color noSplitColor(0.8f,0.8f,0.8f); // index 0
    color->push_back(noSplitColor.r);
    color->push_back(noSplitColor.g);
    color->push_back(noSplitColor.b);

    Color split2EdgesColor(0.6f,0.9f,0.3f); // index 1
    color->push_back(split2EdgesColor.r);
    color->push_back(split2EdgesColor.g);
    color->push_back(split2EdgesColor.b);

    Color split4EdgesColor(0.3f, 0.6f,0.9f); // index 2
    color->push_back(split4EdgesColor.r);
    color->push_back(split4EdgesColor.g);
    color->push_back(split4EdgesColor.b);
  }

  // select vertices of edges which have to be split

  // int nVselected = 0;
  vertexSelection.clear();
  vertexSelection.resize(nV,-1);
  // use
  // vertexSelection[iV] = _vSelIndex;
  // to select vertex iV

  if(mode==SplitEdgesMode::ALL) {

    // select al the vertices

  } else if(mode==SplitEdgesMode::SELECTED) {

    vector<int>& eSel = ifsv.getEdgeSelection();
    // select the vertices iV0 and iV1 of each selected edge (eSel[iE]!=-1)

  } else if(mode==SplitEdgesMode::LONG) {

    // using the array _edgeLengths of pre-computed edge lengths
    // select all vertices of edges longer than highEdgeLength
    float highEdgeLength = _targetEdgeLength;
  }

  // std::cout << "nVselected = " << nVselected << std::endl;

  // select all the edges with the two ends selected
  int iE,iF,iV;

  edgeSelection.clear();
  edgeSelection.resize(nE,-1);
  for(iE=0;iE<nE;iE++) {
    // if(_edgeLengths[iE]<=_highEdgeLength) continue;
    iV = pmesh->getVertex0(iE);
    if(vertexSelection[iV]==-1) continue;
    iV = pmesh->getVertex1(iE);
    if(vertexSelection[iV]==-1) continue;
    edgeSelection[iE] = _eSelIndex;
  }

  // color faces for visualization purposes
  if(colorIncidentFaces) {
      int iF,iC0,iC1,nVsel;

    for(iF=iC0=iC1=0;iC1<nC;iC1++) {
      if(coordIndex[iC1]>=0) continue;
      // remember that the faces are all triangles
      // assert(iC1-iC0==3);
      //
      // count number of selected vertices of the triangle
      nVsel = 0;
      // ...
      // painT the face accordingly
      (*colorIndex)[iF] =
        (nVsel<=1)?noSplitColorIndex:
        (nVsel==2)?split2ColorIndex:
        split4ColorIndex;

      // advance to next face
      iC0=iC1+1; iF++;
    }
  }
}

void Optimization::adaptiveSubdivisionShow(const SplitEdgesMode mode) {
  IndexedFaceSetVariables ifsv(*_ifsOptimized);
    vector<int>& vertexSelection = ifsv.getVertexSelection();
    vector<int>& edgeSelection = ifsv.getEdgeSelection();
  _adaptiveSubdivisionSelect(vertexSelection,edgeSelection,mode,true);
}

//////////////////////////////////////////////////////////////////////
// TASK 10)
//

void Optimization::adaptiveSubdivisionApply
(const SplitEdgesMode mode, const bool colorFaces) {

  vector<int> vertexSelection;
  vector<int> edgeSelection;
  _adaptiveSubdivisionSelect(vertexSelection,edgeSelection,mode,true);

  vector<float>& coord      = _ifsOptimized->getCoord();
  vector<int>&   coordIndex = _ifsOptimized->getCoordIndex();

  // _ifsOptimized should have color per face indexed if colorFaces==true
  if(colorFaces==false) _ifsOptimized->clearColor();
  // vector<float>& color = _ifsOptimized->getColor();
  vector<int>&   colorIndex = _ifsOptimized->getColorIndex();

  // remove normal vectors & texCoords
  // normal vectors will be recomputed at the end
  _ifsOptimized->clearNormal();
  _ifsOptimized->clearTexCoord();

  IndexedFaceSetVariables ifsv(*_ifsOptimized);
  PolygonMesh* pmesh = ifsv.getPolygonMesh(true);
  int nV = pmesh->getNumberOfVertices();
  int nE = pmesh->getNumberOfEdges();
  int nF = pmesh->getNumberOfFaces();
  int nC = pmesh->getNumberOfCorners();

  // a new vertex must be created for each selected edge
  
  // assign new vertex indices to edges and compute coordinates of new
  // vertices
  //
  // since all the original vertices are preserved, new vertex indices
  // start with iVnew=nV
  //
  // when you create new coordinates for a new vertices, you can just
  // push them back onto the original coord array.

  vector<int> edgeToNewVertex(nE,-1);
  // for each edge iE {
  //   if either iV0 or iV1 is not selected, continue;
  //   create new vertex at edge midpoint and save them
  //   save the new vertex index into the edgeToNewVertex array
  // }

  // now split the triangles
  // for each triangle iF connecting vertices iV0, iV1, and iV2 {
  //   count the number of selected vertices nVsel=0,1,2,or 3
  //   if nVsel==0 or nVsel==1 {
  //     keep the triangle unchanged 
  //   } else if nVsel==2 {
  //     for example, if the selected vertices are iV1 and iV2
  //     find the edge iE defined by iV1 and iV2
  //     find the new vertex index iV12 associated with the edge 
  //
  //     split the input triangle into two output triangles
  //
  //           iV0
  //        /   |    \
  //       /    |     \
  //     iV1 - iV12 - iV2
  //
  //   } else if nVsel==3 {
  //
  //     split into 3 triangles
  //      
  //           iV2
  //         /      \
  //       iV02 -- iV12
  //       /  \    /   \
  //     iV0 - iV01 - iV1
  //   }
  // }

  // clear all selection buffers
  ifsv.clearAllSelection();

  // delete pmesh
  ifsv.deletePolygonMesh();
  pmesh = nullptr;

  // recompute face normals
  _ifsOptimized->setNormalPerVertex(false);
  Geometry::computeNormalsPerFace(coord,coordIndex,_ifsOptimized->getNormal());
}

// TODO 20250807 : color !

void Optimization::_equalizeValencesSelect
(vector<int>& edgeSelection, bool colorIncidentFaces) {

  // for each edge e do
  // let a, b, c, d be the vertices of the two triangles adjacent to e
  // deviation_pre =
  //   abs(valence(a)-target_val(a)) +
  //   abs(valence(b)-target_val(b)) +
  //   abs(valence(c)-target_val(c)) +
  //   abs(valence(d)-target_val(d))
  // flip(e)
  // deviation_post =
  //   abs(valence(a)-target_val(a)) +
  //   abs(valence(b)-target_val(b)) +
  //   abs(valence(c)-target_val(c)) +
  //   abs(valence(d)-target_val(d))
  // if deviation_pre <= deviation_post do
  // flip(e)

  // vector<float>& coord      = _ifsOptimized->getCoord();
  // vector<int>&   coordIndex = _ifsOptimized->getCoordIndex();
  IndexedFaceSetVariables ifsv(*_ifsOptimized);
  PolygonMesh* pmesh = ifsv.getPolygonMesh(true);

  int nV,nE,nF,nEF,iE,iV0,iV1,iV2,iV3,iC0,iC1,iC0n,iC1n,iF,i;
  int targetValence0,targetValence1,targetValence2,targetValence3;
  int errBefore,errAfter,errDiff;

  nV = pmesh->getNumberOfVertices();
  nE = pmesh->getNumberOfEdges();
  nF = pmesh->getNumberOfFaces();

  vector<int> valence;
  Geometry::computeValences(*pmesh,valence);

  // clear edge selection
  edgeSelection.clear();
  edgeSelection.resize(nE,-1);

  // set color per face
  _ifsOptimized->clearColor();
  _ifsOptimized->setColorPerVertex(false);
  vector<int>&   colorIndex               = _ifsOptimized->getColorIndex();
  vector<float>& color                    = _ifsOptimized->getColor();
  int            noChangesColorIndex      = 0;
  int            flipEdgesColorIndex = 1;
  if(colorIncidentFaces) {
    // make a two color table
    
    Color noChangesColor(0.8f,0.8f,0.8f); // index 0
    noChangesColorIndex = static_cast<int>(color.size()/3);
    color.push_back(noChangesColor.r);
    color.push_back(noChangesColor.g);
    color.push_back(noChangesColor.b);

    Color flipEdgesColor(0.7f,0.5f,0.9f); // index 1
    flipEdgesColorIndex = static_cast<int>(color.size()/3);
    color.push_back(flipEdgesColor.r);
    color.push_back(flipEdgesColor.g);
    color.push_back(flipEdgesColor.b);

    // paint all faces as not affected by flips
    colorIndex.resize(nF,noChangesColorIndex);
  }

  // for each regular edge {
  //   compute a flip error
  //   insert the edge in a priority queue
  // }
  Heap heap;
  for(iE=0;iE<nE;iE++) {
    if(pmesh->isRegularEdge(iE)) {
      // pmesh->getNumberOfEdgeFaces(iE)==2
      // pmesh->getNumberOfEdgeHalfEdges(iE)==2

      // iV0 = pmesh->getVertex0(iE);
      // iV1 = pmesh->getVertex1(iE);

      iC0  = pmesh->getEdgeHalfEdge(iE,0);
      iC0n = pmesh->getNext(iC0);
      iC1  = pmesh->getEdgeHalfEdge(iE,1);
      iC1n = pmesh->getNext(iC1);

      //      iV1
      //    /     \
      // iV0 ----- iV2
      //    \     /
      //      iV3

      iV0  = pmesh->getSrc(iC0n);
      iV1  = pmesh->getDst(iC0n);
      iV2  = pmesh->getSrc(iC1n);
      iV3  = pmesh->getDst(iC1n);

      targetValence0 = (pmesh->isBoundaryVertex(iV0))?4:6;
      targetValence1 = (pmesh->isBoundaryVertex(iV1))?4:6;
      targetValence2 = (pmesh->isBoundaryVertex(iV2))?4:6;
      targetValence3 = (pmesh->isBoundaryVertex(iV3))?4:6;

      // current
      // (iV0,iV1,iV2) & (iV2,iV3,iV0)
      errBefore =
        abs(valence[iV0]-targetValence0) +
        abs(valence[iV1]-targetValence1) +
        abs(valence[iV2]-targetValence2) +
        abs(valence[iV3]-targetValence3);

      // swap
      // (iV0,iV1,iV3) & (iV1,iV2,iV3)
      errAfter =
        abs(valence[iV0]-1-targetValence0) +
        abs(valence[iV1]+1-targetValence1) +
        abs(valence[iV2]-1-targetValence2) +
        abs(valence[iV3]+1-targetValence3);

      if((errDiff=errAfter-errBefore)<0) {
        heap.add((float)errDiff,iE);
      }

    }
  }

  // create an independent set ef edges to flip
  vector<bool> usedVertex(nV,false);
  while(heap.length()>0) {
    /* iH = */ heap.delMin();
    iE      =  heap.getLastIKey();
    // errDiff = -heap.getLastFKey();

    iC0  = pmesh->getEdgeHalfEdge(iE,0);  // iV2->iV0
    iC0n = pmesh->getNext(iC0);           // iV0->iV1
    iC1  = pmesh->getEdgeHalfEdge(iE,1);  // iV0->iV2
    iC1n = pmesh->getNext(iC1);           // iV2->iV3

    iV0  = pmesh->getSrc(iC0n);
    iV1  = pmesh->getDst(iC0n);
    iV2  = pmesh->getSrc(iC1n);
    iV3  = pmesh->getDst(iC1n);

    if(usedVertex[iV0] || usedVertex[iV1] ||
       usedVertex[iV2] || usedVertex[iV3] ) {
      // iE REJECTED
      continue;
    }

    if(pmesh->getEdge(iV1,iV3)>=0) {
      // may happen in some cases, for eaxmple for a tetrahedron, and a flip will
      // result in double faces, singular edges and vertices
      //
      // iE REJECTED (DIAGONAL)
      continue;
    }

    // mark the four vertices as used
    usedVertex[iV0]   = true;
    usedVertex[iV1]   = true;
    usedVertex[iV2]   = true;
    usedVertex[iV3]   = true;
    // mark the edge as selected for a flip
    edgeSelection[iE] = _eSelIndex;

    // for visualization purposes
    // paint the two faces incident to the edge with the flip color
    if(colorIncidentFaces) {
      nEF = pmesh->getNumberOfEdgeFaces(iE);
      for(i=0;i<nEF;i++) {
        iF = pmesh->getEdgeFace(iE,i);
        colorIndex[iF] = flipEdgesColorIndex;
      }
    }
  }
}

void Optimization::equalizeValencesShow() {
  IndexedFaceSetVariables ifsv(*_ifsOptimized);
  vector<int>& edgeSelection = ifsv.getEdgeSelection();
  _equalizeValencesSelect(edgeSelection,true);
}

void Optimization::equalizeValencesApply() {

  vector<int> edgeSelection;
  _equalizeValencesSelect(edgeSelection,false);

  vector<float>& coord      = _ifsOptimized->getCoord();
  vector<int>&   coordIndex = _ifsOptimized->getCoordIndex();

  IndexedFaceSetVariables ifsv(*_ifsOptimized);
  PolygonMesh* pmesh = ifsv.getPolygonMesh(true);
  int nE = pmesh->getNumberOfEdges();

  int iV0,iV1,iV2,iV3,iE,iC0,iC1,iC0n,iC1n,iF0,iF1;
  for(iE=0;iE<nE;iE++) {
    if(edgeSelection[iE]<0) continue; // edge not selected for flipping

    iC0  = pmesh->getEdgeHalfEdge(iE,0);  // iV2->iV0
    iC0n = pmesh->getNext(iC0);           // iV0->iV1
    iC1  = pmesh->getEdgeHalfEdge(iE,1);  // iV0->iV2
    iC1n = pmesh->getNext(iC1);           // iV2->iV3

    iV0  = pmesh->getSrc(iC0n); // == pmesh->getDst(iC0) == pmesh->getSrc(iC1);
    iV1  = pmesh->getDst(iC0n);
    iV2  = pmesh->getSrc(iC1n); // == pmesh->getDst(iC1) == pmesh->getSrc(iC0);
    iV3  = pmesh->getDst(iC1n);

    // we know that the edge is regular
    iF0  = pmesh->getFace(iC0); // (iV0,iV1,iV2)
    iF1  = pmesh->getFace(iC1); // (iV2,iV3,iV0)

    //      iV1
    //    / iF0 \
    // iV0 ----- iV2
    //    \ iF1 /
    //      iV3

    // iF0 : (iV0,iV1,iV2) -> (iV0,iV1,iV3)
    coordIndex[4*iF0+0] = iV0;
    coordIndex[4*iF0+1] = iV1;
    coordIndex[4*iF0+2] = iV3;

    // iF1 : (iV2,iV3,iV0) -> (iV2,iV3,iV1)
    coordIndex[4*iF1+0] = iV2;
    coordIndex[4*iF1+1] = iV3;
    coordIndex[4*iF1+2] = iV1;
  }

  // PolygonMesh is no longer valid
  ifsv.deletePolygonMesh();
  // pmesh = ifsv.getPolygonMesh(true);

  // clear colors ???
  // _ifsOptimized->clearColor();

  // recompute face normals
  _ifsOptimized->clearNormal();
  _ifsOptimized->setNormalPerVertex(false);
  Geometry::computeNormalsPerFace(coord,coordIndex,_ifsOptimized->getNormal());

  // clear all selection buffers ???
  ifsv.clearAllSelection();

  _ifsOptimized->printInfo("  ");
}
