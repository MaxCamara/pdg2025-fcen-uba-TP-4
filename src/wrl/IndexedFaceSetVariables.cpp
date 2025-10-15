//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-08-06 09:59:17 taubin>
//------------------------------------------------------------------------
//
// IndexedFaceSetVariables.cpp
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
#include "IndexedFaceSetVariables.hpp"

IndexedFaceSetVariables::IndexedFaceSetVariables(IndexedFaceSet& ifs):
  _ifs(ifs) {
  Node* node = ifs.getParent();
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
        _ifs.setVariable(var);
      }
    }
  }
}

void IndexedFaceSetVariables::deletePolygonMesh() {
  _ifs.eraseVariable("PolygonMesh");
}

PolygonMesh* IndexedFaceSetVariables::getPolygonMesh(const bool rebuild) {
  Variable* var = _ifs.getVariable("PolygonMesh");
  if(var==(Variable*)0) { // not found
    if(rebuild) {
      int nV = _ifs.getNumberOfVertices();
      vector<int>& coordIndex = _ifs.getCoordIndex();
      var = new VariablePolygonMesh(nV,coordIndex);
      _ifs.setVariable(var);
    }
  }
  if(var==(Variable*)0) return nullptr;
  return (PolygonMesh*)(var->getValue());
}

int IndexedFaceSetVariables::getNumberOfEdges() {
  PolygonMesh* pmesh = getPolygonMesh(true);
  return pmesh->getNumberOfEdges();
}

Material** IndexedFaceSetVariables::getMaterial() {
  string name = "material";
  Variable* var  = _ifs.getVariable(name);
  if(var==(Variable*)0) {
    var = new VariablePointer(name);
    _ifs.setVariable(var);
  }
  return (Material**)(var->getValue());
}

void IndexedFaceSetVariables::setMaterial(Material *material) {
  Material** pMaterial = getMaterial();
  *pMaterial = material;
}

Color& IndexedFaceSetVariables::getMaterialColor() {
  string   name = "materialColor";
  Variable* var  = _ifs.getVariable(name);
  if(var==(Variable*)0) {
    var = new VariableColor(name);
    _ifs.setVariable(var);
  }
  return *((Color*)(var->getValue()));
}

bool IndexedFaceSetVariables::hasVertexSelection() {
  return (getNumberOfSelectedVertices()>0);
}

bool IndexedFaceSetVariables::hasEdgeSelection() {
  return (getNumberOfSelectedEdges()>0);
}

bool IndexedFaceSetVariables::hasFaceSelection() {
  return (getNumberOfSelectedFaces()>0);
}

bool IndexedFaceSetVariables::hasCornerSelection() {
  return (getNumberOfSelectedCorners()>0);
}

int IndexedFaceSetVariables::getNumberOfSelectedVertices() {
  int nVsel = 0;
  Variable* var  = _ifs.getVariable("vertexSelection");
  if(var!=(Variable*)0) {
    vector<int>& vertexSelection = getVertexSelection();
    for(int i=0;i<static_cast<int>(vertexSelection.size());i++)
      if(vertexSelection[i]>=0) nVsel++;
  }
  return nVsel;
}

int IndexedFaceSetVariables::getNumberOfSelectedEdges() {
  int nEsel = 0;
  Variable* var  = _ifs.getVariable("edgeSelection");
  if(var!=(Variable*)0) {
    vector<int>& edgeSelection = getEdgeSelection();
    for(int i=0;i<static_cast<int>(edgeSelection.size());i++)
      if(edgeSelection[i]>=0) nEsel++;
  }
  return nEsel;
}

int IndexedFaceSetVariables::getNumberOfSelectedFaces() {
  int nFsel = 0;
  Variable* var  = _ifs.getVariable("faceSelection");
  if(var!=(Variable*)0) {
    vector<int>& faceSelection = getFaceSelection();
    for(int i=0;i<static_cast<int>(faceSelection.size());i++)
      if(faceSelection[i]>=0) nFsel++;
  }
  return nFsel;
}

int IndexedFaceSetVariables::getNumberOfSelectedCorners() {
  int nCsel = 0;
  Variable* var  = _ifs.getVariable("cornerSelection");
  if(var!=(Variable*)0) {
    vector<int>& cornerSelection = getCornerSelection();
    for(int i=0;i<static_cast<int>(cornerSelection.size());i++)
      if(cornerSelection[i]>=0) nCsel++;
  }
  return nCsel;
}

vector<int>& IndexedFaceSetVariables::getVertexSelection() {
  size_t    nV   = _ifs.getNumberOfCoord();
  string   name = "vertexSelection";
  Variable* var  = _ifs.getVariable(name);
  if(var==(Variable*)0) {
    var = new VariableVecInt(name,nV,-1);
    _ifs.setVariable(var);
  }
  vector<int>& vecIntRef = *((vector<int>*)(var->getValue()));
  // adjust size
  while(vecIntRef.size()>nV) vecIntRef.pop_back();
  while(vecIntRef.size()<nV) vecIntRef.push_back(-1);
  // return reference
  return vecIntRef;
}

vector<int>& IndexedFaceSetVariables::getEdgeSelection() {
  size_t    nE   = getNumberOfEdges();
  string   name = "edgeSelection";
  Variable* var  = _ifs.getVariable(name);
  if(var==(Variable*)0) {
    var = new VariableVecInt(name,nE,-1);
    _ifs.setVariable(var);
  }
  vector<int>& vecIntRef = *((vector<int>*)(var->getValue()));
  // adjust size
  while(vecIntRef.size()>nE) vecIntRef.pop_back();
  while(vecIntRef.size()<nE) vecIntRef.push_back(-1);
  // return reference
  return vecIntRef;
}

vector<int>& IndexedFaceSetVariables::getFaceSelection() {
  size_t    nF   = _ifs.getNumberOfFaces();
  string   name = "faceSelection";
  Variable* var  = _ifs.getVariable(name);
  if(var==(Variable*)0) {
    var = new VariableVecInt(name,nF,-1);
    _ifs.setVariable(var);
  }
  vector<int>& vecIntRef = *((vector<int>*)(var->getValue()));
  // adjust size
  while(vecIntRef.size()>nF) vecIntRef.pop_back();
  while(vecIntRef.size()<nF) vecIntRef.push_back(-1);
  // return reference
  return vecIntRef;
}

vector<int>& IndexedFaceSetVariables::getCornerSelection() {
  size_t    nC   = _ifs.getNumberOfCorners();
  string   name = "cornerSelection";
  Variable* var  = _ifs.getVariable(name);
  if(var==(Variable*)0) {
    var = new VariableVecInt(name,nC,-1);
    _ifs.setVariable(var);
  }
  vector<int>& vecIntRef = *((vector<int>*)(var->getValue()));
  // adjust size
  while(vecIntRef.size()>nC) vecIntRef.pop_back();
  while(vecIntRef.size()<nC) vecIntRef.push_back(-1);
  // return reference
  return vecIntRef;
}

void IndexedFaceSetVariables::clearVertexSelection() {
  vector<int>& vSel = getVertexSelection();
  int nV = static_cast<int>(vSel.size());
  for(int i=0;i<nV;i++)
    vSel[i] = -1;
}

void IndexedFaceSetVariables::clearEdgeSelection() {
  vector<int>& eSel = getEdgeSelection();
  int nE = static_cast<int>(eSel.size());
  for(int i=0;i<nE;i++)
    eSel[i] = -1;
}

void IndexedFaceSetVariables::clearFaceSelection() {
  vector<int>& fSel = getFaceSelection();
  int nF = static_cast<int>(fSel.size());
  for(int i=0;i<nF;i++)
    fSel[i] = -1;
}

void IndexedFaceSetVariables::clearCornerSelection() {
  vector<int>& cSel = getCornerSelection();
  int nC = static_cast<int>(cSel.size());
  for(int i=0;i<nC;i++)
    cSel[i] = -1;
}

void IndexedFaceSetVariables::clearAllSelection() {
  clearVertexSelection();
  clearEdgeSelection();
  clearFaceSelection();
  clearCornerSelection();
}
