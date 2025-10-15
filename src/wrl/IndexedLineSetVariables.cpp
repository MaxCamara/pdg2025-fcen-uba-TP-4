//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-08-06 09:59:19 taubin>
//------------------------------------------------------------------------
//
// IndexedLineSetVariables.cpp
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

#include "Shape.hpp"
#include "Appearance.hpp"
#include "Material.hpp"
#include "IndexedLineSetVariables.hpp"

IndexedLineSetVariables::IndexedLineSetVariables(IndexedLineSet& ils):
  _ils(ils) {
  Node* node = ils.getParent();
  Shape* shape = dynamic_cast<Shape*>(node);
  if(shape!=nullptr) {
      node = shape->getAppearance();
    Appearance* appearance = dynamic_cast<Appearance*>(node);
    if(appearance!=nullptr) {
      node = appearance->getMaterial();
      Material* material = dynamic_cast<Material*>(node);
      if(material!=(Material*)0) {
        setMaterial(material);
        Color& dc = material->getDiffuseColor();
        VariableColor* var = new VariableColor("materialColor",dc);
        _ils.setVariable(var);
      }
    }
  }
}

int IndexedLineSetVariables::getNumberOfEdges() {
  int nEdges = 0;
  vector<int>& coordIndex = _ils.getCoordIndex();
  int nC = static_cast<int>(coordIndex.size());
  for(int i0=0,i1=0;i1<nC;i1++)
    if(coordIndex[i1]<0) {
      nEdges += (i1-i0-1);
      i0 = i1+1;
    }
  return nEdges;
}

Material** IndexedLineSetVariables::getMaterial() {
  string name = "material";
  Variable* var  = _ils.getVariable(name);
  if(var==(Variable*)0) {
    var = new VariablePointer(name);
    _ils.setVariable(var);
  }
  return (Material**)(var->getValue());
}

void IndexedLineSetVariables::setMaterial(Material *material) {
  Material** pMaterial = getMaterial();
  *pMaterial = material;
}

Color& IndexedLineSetVariables::getMaterialColor() {
  string   name = "materialColor";
  Variable* var  = _ils.getVariable(name);
  if(var==(Variable*)0) {
    var = new VariableColor(name);
    _ils.setVariable(var);
  }
  return *((Color*)(var->getValue()));
}

bool IndexedLineSetVariables::hasVertexSelection() {
  return (getNumberOfSelectedVertices()>0);
}

bool IndexedLineSetVariables::hasEdgeSelection() {
  return (getNumberOfSelectedEdges()>0);
}

bool IndexedLineSetVariables::hasPolylineSelection() {
  return (getNumberOfSelectedPolylines()>0);
}

int IndexedLineSetVariables::getNumberOfSelectedVertices() {
  int nVsel = 0;
  Variable* var  = _ils.getVariable("vertexSelection");
  if(var!=(Variable*)0) {
    vector<int>& vertexSelection = getVertexSelection();
    for(int i=0;i<static_cast<int>(vertexSelection.size());i++)
      if(vertexSelection[i]>=0) nVsel++;
  }
  return nVsel;
}

int IndexedLineSetVariables::getNumberOfSelectedEdges() {
  int nEsel = 0;
  Variable* var  = _ils.getVariable("edgeSelection");
  if(var!=(Variable*)0) {
    vector<int>& edgeSelection = getEdgeSelection();
    for(int i=0;i<static_cast<int>(edgeSelection.size());i++)
      if(edgeSelection[i]>=0) nEsel++;
  }
  return nEsel;
}

int IndexedLineSetVariables::getNumberOfSelectedPolylines() {
  int nFsel = 0;
  Variable* var  = _ils.getVariable("polylineSelection");
  if(var!=(Variable*)0) {
    vector<int>& polylineSelection = getPolylineSelection();
    for(int i=0;i<static_cast<int>(polylineSelection.size());i++)
      if(polylineSelection[i]>=0) nFsel++;
  }
  return nFsel;
}

vector<int>& IndexedLineSetVariables::getVertexSelection() {
  size_t    nV   = _ils.getNumberOfCoord();
  string   name = "vertexSelection";
  Variable* var  = _ils.getVariable(name);
  if(var==(Variable*)0) {
    var = new VariableVecInt(name,nV,-1);
    _ils.setVariable(var);
  }
  vector<int>& vecIntRef = *((vector<int>*)(var->getValue()));
  // adjust size
  while(vecIntRef.size()>nV) vecIntRef.pop_back();
  while(vecIntRef.size()<nV) vecIntRef.push_back(-1);
  // return reference
  return vecIntRef;
}

vector<int>& IndexedLineSetVariables::getEdgeSelection() {
  size_t    nE   = getNumberOfEdges();
  string   name = "edgeSelection";
  Variable* var  = _ils.getVariable(name);
  if(var==(Variable*)0) {
    var = new VariableVecInt(name,nE,-1);
    _ils.setVariable(var);
  }
  vector<int>& vecIntRef = *((vector<int>*)(var->getValue()));
  // adjust size
  while(vecIntRef.size()>nE) vecIntRef.pop_back();
  while(vecIntRef.size()<nE) vecIntRef.push_back(-1);
  // return reference
  return vecIntRef;
}

vector<int>& IndexedLineSetVariables::getPolylineSelection() {
  size_t    nF   = _ils.getNumberOfPolylines();
  string   name = "polylineSelection";
  Variable* var  = _ils.getVariable(name);
  if(var==(Variable*)0) {
    var = new VariableVecInt(name,nF,-1);
    _ils.setVariable(var);
  }
  vector<int>& vecIntRef = *((vector<int>*)(var->getValue()));
  // adjust size
  while(vecIntRef.size()>nF) vecIntRef.pop_back();
  while(vecIntRef.size()<nF) vecIntRef.push_back(-1);
  // return reference
  return vecIntRef;
}

void IndexedLineSetVariables::clearVertexSelection() {
  vector<int>& vSel = getVertexSelection();
  int nV = static_cast<int>(vSel.size());
  for(int i=0;i<nV;i++)
    vSel[i] = -1;
}

void IndexedLineSetVariables::clearEdgeSelection() {
  vector<int>& eSel = getEdgeSelection();
  int nE = static_cast<int>(eSel.size());
  for(int i=0;i<nE;i++)
    eSel[i] = -1;
}

void IndexedLineSetVariables::clearPolylineSelection() {
  vector<int>& fSel = getPolylineSelection();
  int nF = static_cast<int>(fSel.size());
  for(int i=0;i<nF;i++)
    fSel[i] = -1;
}

void IndexedLineSetVariables::clearAllSelection() {
  clearVertexSelection();
  clearEdgeSelection();
  clearPolylineSelection();
}
