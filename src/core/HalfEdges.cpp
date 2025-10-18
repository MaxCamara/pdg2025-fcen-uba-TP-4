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
#include "io/StrException.hpp"

// TODO Mon Mar 6 2023
// - merge your code from Assignment 2

HalfEdges::HalfEdges(const int nVertices, const vector<int>&  coordIndex):
  Edges(nVertices), // a graph with no edges is created here
  _coordIndex(coordIndex),
  _twin(),
  _face(),
  _firstCornerEdge(),
  _cornerEdge(),
  _hasBoundaryEdge(false),
  _hasRegularEdge(false),
  _hasSingularEdge(false)
{
  int nV = nVertices;
  int nC = static_cast<int>(_coordIndex.size()); // number of corners

  try{
      for(int iC=0; iC < nC; iC++){
          int iV = coordIndex[iC];
          if(iV < -1) throw new StrException("Índice de vértice inválido en coordIndex");
          if(iV >= nV) {
              nV = iV;
              _reset(nV);
          }
      }

  } catch(StrException* e) {
      fprintf(stderr,"Faces | ERROR | %s\n",e->what());
      delete e;
  }

  vector<int> nFacesEdge;

  int iV0,iV1,iF,iE,iC,iC0,iC1;
  for(iF=iC0=iC1=0;iC1<nC;iC1++) {
    if(_coordIndex[iC1]>=0) continue;
    // face iF comprises corners iC0<=iC<iC1
    // - each corner in this range corresponds to one half edge
    for(iC=iC0; iC<iC1; iC++){
        if(iC == iC1-1){
            iV0 = min(coordIndex[iC0], coordIndex[iC]);
            iV1 = max(coordIndex[iC0], coordIndex[iC]);
        } else {
            iV0 = min(coordIndex[iC], coordIndex[iC+1]);
            iV1 = max(coordIndex[iC], coordIndex[iC+1]);
        }
        iE = _insertEdge(iV0, iV1);
        if(iE>=nFacesEdge.size()){
            nFacesEdge.push_back(1);
        } else {
            nFacesEdge[iE]++;
        }
        _face.push_back(iF);
    }
    _face.push_back(-1);

    // increment variables to continue processing next face
    iC0 = iC1+1; iF++;
  }

  int nF = iF;
  int nE = getNumberOfEdges();

  vector<int> twinCorner;
  for(int i=0; i<nE; i++){
      twinCorner.push_back(-1);
  }

  for(int i=0; i<nC; i++){
      _twin.push_back(-1);
  }

  int iVsrc;
  int iVdst;
  for(iC0=iC1=0;iC1<nC;iC1++) {
      if(_coordIndex[iC1]>=0) continue;
      for(iC=iC0; iC<iC1; iC++){
          iVsrc = coordIndex[iC];
          if(iC == iC1-1){
              iVdst = coordIndex[iC0];
          } else {
              iVdst = coordIndex[iC+1];
          }
          if(iVsrc < iVdst){
              iE = getEdge(iVsrc, iVdst);
          } else {
              iE = getEdge(iVdst, iVsrc);
          }
          if(twinCorner[iE]==-1){
              twinCorner[iE] = iC;
          } else {
              _twin[iC] = twinCorner[iE];
              _twin[_twin[iC]] = iC;
          }
      }
      _twin[iC1] = -(iC1 - iC0); //En la posición de _twin correspondiente a cada separador de cara almaceno el tamaño
                                 //de la cara (negativo para no confundirlo con un índice iC)
      iC0 = iC1+1;
  }

  _firstCornerEdge.push_back(0);
  for(iE=0; iE<nE; iE++){
      int nextEdgePosition = _firstCornerEdge[iE] + nFacesEdge[iE];
      _firstCornerEdge.push_back(nextEdgePosition);
  }

  for(int i=0; i<nC-nF; i++){
      _cornerEdge.push_back(-1);
  }

  for(iC0=iC1=0;iC1<nC;iC1++) {
      if(_coordIndex[iC1]>=0) continue;
      for(iC=iC0; iC<iC1; iC++){
          iVsrc = coordIndex[iC];
          if(iC == iC1-1){
              iVdst = coordIndex[iC0];
          } else {
              iVdst = coordIndex[iC+1];
          }
          if(iVsrc < iVdst){
              iE = getEdge(iVsrc, iVdst);
          } else {
              iE = getEdge(iVdst, iVsrc);
          }
          int firstCornerIndex = _firstCornerEdge[iE];
          if(_cornerEdge[firstCornerIndex] == -1){
              _cornerEdge[firstCornerIndex] = iC;
              if(_twin[iC] != -1){
                  _cornerEdge[firstCornerIndex + 1] = _twin[iC];
              }
          }
      }
      iC0 = iC1+1;
  }

  //Me fijo si hay aristas borde, regulares o singulares en el grafo. Esto lo podría hacer en cualquiera de los loops sobre los edges para optimizar, pero lo hago
  //por separado para visualizar mejor cada paso de la creación de la instancia de la clase
  for(iE=0; iE<nE; iE++){
    if(nFacesEdge[iE]==1) _hasBoundaryEdge = true;
    if(nFacesEdge[iE]==2) _hasRegularEdge = true;
    if(nFacesEdge[iE]>=3) _hasSingularEdge = true;
  }
}

int HalfEdges::getNumberOfCorners() const {
  return static_cast<int>(_coordIndex.size());
}

int HalfEdges::getFace(const int iC) const {
    if(iC < 0 || iC >= getNumberOfCorners() || _coordIndex[iC] == -1) return -1;
    return _face[iC];
}

int HalfEdges::getSrc(const int iC) const {
    if(iC < 0 || iC >= getNumberOfCorners() || _coordIndex[iC] == -1) return -1;
    return _coordIndex[iC];
}

int HalfEdges::getDst(const int iC) const {
    if(iC < 0 || iC >= getNumberOfCorners() || _coordIndex[iC] == -1) return -1;
    return _coordIndex[getNext(iC)];
}

int HalfEdges::getNext(const int iC) const {
    if(iC < 0 || iC >= getNumberOfCorners() || _coordIndex[iC] == -1) return -1;
    int next;
    if(_coordIndex[iC+1]>=0){
        next = iC+1;
    } else {
        int faceSize = -(_twin[iC+1]);
        next = iC - faceSize + 1;
    }
    return next;
}

int HalfEdges::getPrev(const int iC) const {
    if(iC < 0 || iC >= getNumberOfCorners() || _coordIndex[iC] == -1) return -1;
    int prev;
    if(iC == 0 || _coordIndex[iC-1] < 0){
        int faceSize = getFaceSize(iC);
        prev = iC + faceSize - 1;
    } else {
        prev = iC-1;
    }
    return prev;
}

int HalfEdges::getTwin(const int iC) const {
    if(iC < 0 || iC >= getNumberOfCorners() || _coordIndex[iC] == -1) return -1;
    return _twin[iC];
}

// represent the half edge as an array of lists, with one list
// associated with each edge

int HalfEdges::getNumberOfEdgeHalfEdges(const int iE) const {
    if(iE < 0 || iE >= getNumberOfEdges()) return 0;
    return (_firstCornerEdge[iE+1] - _firstCornerEdge[iE]);
}

int HalfEdges::getEdgeHalfEdge(const int iE, const int j) const {
    if(iE < 0 || iE >= getNumberOfEdges()) return -1;
    int targetIndex = _firstCornerEdge[iE]+j;
    if(targetIndex >= _firstCornerEdge[iE+1]) return -1; //Si la arista iE no tiene j half-edges incidentes, devuelvo -1
    return _cornerEdge[targetIndex];
}

// TODO Mon Mar 6 2023
// - new functions to implement

bool HalfEdges::isOriented(const int iC) const {
    if(iC < 0 || iC >= getNumberOfCorners() || _coordIndex[iC] == -1) return false;
    int vSrc = getSrc(iC);
    int vDst = getDst(iC);
    int iE = getEdge(min(vSrc, vDst), max(vSrc, vDst));
    bool regularEdge = isRegularEdge(iE);
    bool oriented;
    if (regularEdge) {
        int iCtwin = getTwin(iC);
        oriented = (getDst(iCtwin) == getSrc(iC));
    } else {
        oriented = false;
    }
    return oriented;
}

// half-edge method getFaceSize()
int HalfEdges::getFaceSize(const int iC) const {
    if(iC < 0 || iC >= getNumberOfCorners() || _coordIndex[iC] == -1) return -1;
    int separatorIndex = iC+1;
    while (_twin[separatorIndex]>=-1){separatorIndex++;} //El tamaño de la cara se almacena negado en la posición de su separador en _twin.
                                                         //Como cada cara tiene tamaño >=3, no se confunde con un valor twin válido
    return -(_twin[separatorIndex]);
}
  
int HalfEdges::getNumberOfFacesEdge(const int iE) const {
    if(iE < 0 || iE >= getNumberOfEdges()) return -1;  //En la página web dice que debería retornar 0, lo que lo haría consistente con getNumberOfEdgeHalfEdges
    return (_firstCornerEdge[iE+1] - _firstCornerEdge[iE]);
}

bool HalfEdges::isBoundaryEdge(const int iE) const {
    return (getNumberOfEdgeHalfEdges(iE)==1);
}

bool HalfEdges::isRegularEdge(const int iE) const {
    return (getNumberOfEdgeHalfEdges(iE)==2);
}

bool HalfEdges::isSingularEdge(const int iE) const {
    return (getNumberOfEdgeHalfEdges(iE)>=3);
}

bool HalfEdges::hasBoundaryEdges() const {
    return _hasBoundaryEdge;
}

bool HalfEdges::hasRegularEdges() const {
    return _hasRegularEdge;
}

bool HalfEdges::hasSingularEdges() const {
    return _hasSingularEdge;
}
