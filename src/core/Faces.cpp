//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-08-05 23:12:59 taubin>
//------------------------------------------------------------------------
//
// Faces.cpp
//
// Written by: <Your Name>
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
#include "Faces.hpp"
#include "io/StrException.hpp"
  
Faces::Faces(const int nV, const vector<int>& coordIndex) {
  try{
    if(nV<=0) throw new StrException("Cantidad incorrecta de vértices");

    int faceNumber = 1;
    _faceFirstCorner = {};
    vector<int> newCoordIndex = {};

    for(size_t i=0; i<coordIndex.size(); i++){
      if(coordIndex[i] < -1 || coordIndex[i] >= nV) throw new StrException("Índice de vértice incorrecto");
      if(i == 0 || coordIndex[i-1] == -1) _faceFirstCorner.push_back(i);
      if(coordIndex[i] != -1){
        newCoordIndex.push_back(coordIndex[i]);
      } else {
        newCoordIndex.push_back(-faceNumber);
        faceNumber++;
      }
    }

    _nV = nV;
    _coordIndex = newCoordIndex;
    _faceFirstCorner.push_back(coordIndex.size()); //Agrego un face index que apunta después del último separador

  } catch(StrException* e) {
    fprintf(stderr,"Faces | ERROR | %s\n",e->what());
    delete e;
  }
}

int Faces::getNumberOfVertices() const {
  return _nV;
}

int Faces::getNumberOfFaces() const {
  return _faceFirstCorner.size()-1; //Le resto 1 ya que la última posición no apunta a una cara real
}

int Faces::getNumberOfCorners() const {
  return _coordIndex.size();
}

int Faces::getFaceSize(const int iF) const {
  int result = -1;
  try{
    if(iF < 0 || iF >= getNumberOfFaces()){
      throw new StrException("Índice de cara incorrecto");
    }
    int firstIndexFace = getFaceFirstCorner(iF);
    int indexFaceSeparator;
    if(iF == getNumberOfFaces() - 1){
      indexFaceSeparator = getNumberOfCorners() - 1;
    } else {
      indexFaceSeparator = getFaceFirstCorner(iF+1) - 1;
    }
    result = indexFaceSeparator - firstIndexFace;

  } catch(StrException* e) {
    fprintf(stderr,"Faces | ERROR | %s\n",e->what());
    delete e;
  }

  return result;
}

int Faces::getFaceFirstCorner(const int iF) const {
  int result = -1;
  try{
    if(iF < 0 || iF >= getNumberOfFaces()){
      throw new StrException("Índice de cara incorrecto");
    }
    result = _faceFirstCorner[iF];

  } catch(StrException* e) {
    fprintf(stderr,"Faces | ERROR | %s\n",e->what());
    delete e;
  }

  return result;
}

int Faces::getFaceVertex(const int iF, const int j) const {
  int result = -1;
  try{
    if(iF < 0 || iF >= getNumberOfFaces()){
      throw new StrException("Índice de cara incorrecto");
    }
    if(j < 0 || j >= getFaceSize(iF)){
      throw new StrException("Número de esquina fuera del tamaño de la cara");
    }
    int vertexIndex = getFaceFirstCorner(iF) + j;
    result = _coordIndex[vertexIndex];

  } catch(StrException* e) {
    fprintf(stderr,"Faces | ERROR | %s\n",e->what());
    delete e;
  }

  return result;
}

int Faces::getCornerFace(const int iC) const {
  int result = -1;
  try{
    if(iC < 0 || iC >= getNumberOfCorners()){
      throw new StrException("Índice de esquina incorrecto");
    }
    if(_coordIndex[iC] >= 0){
      int i = iC+1;
      while(_coordIndex[i] >= 0){i++;}
      result = -_coordIndex[i] - 1;
    }
  } catch(StrException* e) {
    fprintf(stderr,"Faces | ERROR | %s\n",e->what());
    delete e;
  }

  return result;
}

int Faces::getNextCorner(const int iC) const {
  int result = -1;
  try{
    if(iC < 0 || iC >= getNumberOfCorners()){
      throw new StrException("Índice de esquina incorrecto");
    }
    if(_coordIndex[iC] >= 0){
      int cornerFace = getCornerFace(iC);
      if(_coordIndex[iC+1] >= 0){
        result = iC+1;
      } else {
        result = getFaceFirstCorner(cornerFace);  //Si la posición siguiente es el separador, la siguiente esquina es la primer esquina de la cara
      }
    }

  } catch(StrException* e) {
    fprintf(stderr,"Faces | ERROR | %s\n",e->what());
    delete e;
  }

  return result;
}
