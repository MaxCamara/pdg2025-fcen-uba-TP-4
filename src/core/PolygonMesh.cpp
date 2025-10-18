//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-08-05 23:13:04 taubin>
//------------------------------------------------------------------------
//
// PolygonMesh.cpp
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

#include <iostream>
#include "PolygonMesh.hpp"
#include "Partition.hpp"
#include "Faces.hpp"

// TODO Mon Mar 6 2023
// - merge your code from Assignment 2

PolygonMesh::PolygonMesh(const int nVertices, const vector<int>& coordIndex):
  HalfEdges(nVertices,coordIndex),
  _nPartsVertex(),
  _isBoundaryVertex()
{
  int nV = getNumberOfVertices();
  int nE = getNumberOfEdges(); // Edges method
  // int nF = getNumberOfFaces();
  int nC = getNumberOfCorners();

  int iV;
  for(iV=0;iV<nV;iV++)
    _isBoundaryVertex.push_back(false);

  for(int iE=0; iE<nE; iE++){
    int nIncidentFaces = getNumberOfEdgeHalfEdges(iE);
    if(nIncidentFaces == 1){      //La arista es borde sii tiene una sola cara incidente (i.e. un solo half-edge incidente)
      int V0 = getVertex0(iE);
      int V1 = getVertex1(iE);
      _isBoundaryVertex[V0] = true;
      _isBoundaryVertex[V1] = true;
    }
  }

  Partition partition(nC);

  for(int iE=0; iE<nE; iE++){
    int nIncidentFaces = getNumberOfEdgeHalfEdges(iE);
    if(nIncidentFaces == 2){      //La arista es regular sii tiene dos caras incidentes
      int C0 = getEdgeHalfEdge(iE, 0);
      int C1 = getEdgeHalfEdge(iE, 1);
      if(coordIndex[C0] == coordIndex[getNext(C1)]){
        // Las esquinas incidentes a iE, C0 y C1, estan consistentemente orientadas
        partition.join(C0, getNext(C1));
        partition.join(C1, getNext(C0));
      } else {
        //Las esquinas incidentes a iE, C0 y C1, no están consistentemente orientadas
        partition.join(C0, C1);
        partition.join(getNext(C0), getNext(C1));
      }

    }
  }

  for(int i=0; i<nV; i++){
    _nPartsVertex.push_back(0);
  }
  for(int iC=0; iC<nC; iC++){
    if(partition.find(iC)==iC && coordIndex[iC]>=0){
      iV = coordIndex[iC];
      _nPartsVertex[iV]++;
    }
  }
}


int PolygonMesh::getNumberOfFaces() const {
  int nF = 0;
  for(int iC=0; iC<getNumberOfCorners(); iC++){
    if(_coordIndex[iC]<0) nF++;
  }
  return nF;
}

int PolygonMesh::getNumberOfEdgeFaces(const int iE) const {
  return getNumberOfEdgeHalfEdges(iE);
}

int PolygonMesh::getEdgeFace(const int iE, const int j) const {
  int iC = getEdgeHalfEdge(iE, j);
  return getFace(iC);               //Si los parámetros son inválidos, getEdgeHalfEdge retorna -1 y getFace(-1) retorna -1
}

bool PolygonMesh::isEdgeFace(const int iE, const int iF) const {
  bool result = false;
  int nHE = getNumberOfEdgeHalfEdges(iE);
  for(int i=0; i<nHE; i++){
    int iC = getEdgeHalfEdge(iE, i);
    if(getFace(iC)==iF){
      result = true;
      break;
    }
  }
  return result;
}

// classification of vertices

bool PolygonMesh::isBoundaryVertex(const int iV) const {
  int nV = getNumberOfVertices();
  return (0<=iV && iV<nV)?_isBoundaryVertex[iV]:false;
}

bool PolygonMesh::isSingularVertex(const int iV) const {
  int nV = getNumberOfVertices();
  return (0<=iV && iV<nV && _nPartsVertex.size()>0 && _nPartsVertex[iV]>1);
}

// properties of the whole mesh

bool PolygonMesh::isRegular() const {
  bool result = true;
  int nE = getNumberOfEdges();
  int nV = getNumberOfVertices();
  for(int iE=0; iE<nE; iE++){
    if(isSingularEdge(iE)){
      result = false;
      break;
    }
  }
  if(result){
    for(int iV=0; iV<nV; iV++){
      if(isSingularVertex(iV)){
        result = false;
        break;
      }
    }
  }
  return result;
}

bool PolygonMesh::hasBoundary() const {
  bool result = false;
  int nE = getNumberOfEdges();
  for(int iE=0; iE<nE; iE++){
    if(isBoundaryEdge(iE)){
      result = true;
      break;
    }
  }
  return result;
}

//////////////////////////////////////////////////////////////////////
// CONNECTED COMPONENTS

// connected components of the primal graph
// - returns number of connected components nCC
// - fills the faceLabel array with connected component number iCC
// - for each face; 0<=iCC<nCC
int PolygonMesh::computeConnectedComponentsPrimal(vector<int>& faceLabel) const {
  int nCCprimal = 0;
  faceLabel.clear();

  int nV = getNumberOfVertices();
  int nE = getNumberOfEdges();
  int nF = getNumberOfFaces();
  Partition vertexPartition(nV);

  for(int iE=0; iE<nE; iE++){
    int V0 = getVertex0(iE);
    int V1 = getVertex1(iE);
    vertexPartition.join(V0, V1);
  }

  nCCprimal = vertexPartition.getNumberOfParts();

  vector<int> vToCC(nV, -1);
  int componentNumber = 0;
  for (int iV=0; iV<nV; iV++) {
    if (vertexPartition.find(iV) == iV) {
      //iV es el vértice representativo de su partición
      vToCC[iV] = componentNumber;
      componentNumber++;
    }
  }
  for (int iV=0; iV<nV; iV++) {
    int representative = vertexPartition.find(iV);
    if (representative != iV) {
      //iV no es el índice representativo de su partición
      vToCC[iV] = vToCC[representative];
    }
  }

  Faces faces(nV, _coordIndex);
  for (int iF=0; iF<nF; iF++) {
    int vertex = faces.getFaceVertex(iF, 0); //Obtengo el primer vértice de la cara
    faceLabel.push_back(vToCC[vertex]);
  }

  return nCCprimal;
}

// connected components of the dual graph
// - returns number of connected components nCC
// - fills the faceLabel array with connected component number iCC
// - for each face; 0<=iCC<nCC
int PolygonMesh::computeConnectedComponentsDual(vector<int>& faceLabel) const {
  int nCCdual = 0;
  faceLabel.clear();

  int iF,iFt,iFR,iC0,iC1,iC,iCt,iP;

  int nC = getNumberOfCorners();
  int nF = getNumberOfFaces();
  Partition facePartition(nF);
  for(iF=iC0=iC1=0;iC1<nC;iC1++) {
    if(_coordIndex[iC1]>=0) continue;
    for(iC=iC0; iC<iC1; iC++){
      int vSrc = getSrc(iC);
      int vDst = getDst(iC);
      int iE = getEdge(min(vSrc, vDst), max(vSrc, vDst));
      bool regularEdge = isRegularEdge(iE);
      if (regularEdge) {
        iCt = getTwin(iC);
        iFt = getFace(iCt);
        facePartition.join(iF, iFt);
      }
    }
    iC0 = iC1+1;
    iF++;
  }

  nCCdual = facePartition.getNumberOfParts();

  vector<int> faceToCC(nF, -1);
  int componentNumber = 0;
  for (int iF=0; iF<nF; iF++) {
    if (facePartition.find(iF) == iF) {
      //iF es la cara representativa de su partición
      faceToCC[iF] = componentNumber;
      componentNumber++;
    }
  }
  for (int iF=0; iF<nF; iF++) {
    int representative = facePartition.find(iF);
    if (representative != iF) {
      //iF no es la cara representativa de su partición
      faceToCC[iF] = faceToCC[representative];
    }
  }

  for (int iF=0; iF<nF; iF++) {
    faceLabel.push_back(faceToCC[iF]);
  }

  return nCCdual;
}


// ORIENTATION
  
// determines if the mesh is oriented
// - a mesh is oriented if all regular edges are consistently oriented
// - by definition, a mesh with singular edges is not oriented
// - note that isolated singular vertices play no role in this
//   definition (since cuting through them does not affect
//   orientation)
bool PolygonMesh::isOriented() const {
  if(hasSingularEdges()) return false;

  bool oriented = true;
  int nC = getNumberOfCorners();
  for (int iC=0; iC<nC; iC++) {
    if (_coordIndex[iC]==-1) continue;
    int vSrc = getSrc(iC);
    int vDst = getDst(iC);
    int iE = getEdge(min(vSrc, vDst), max(vSrc, vDst));
    bool regularEdge = isRegularEdge(iE);
    if (regularEdge) {
      int iCtwin = getTwin(iC);
      oriented = (getDst(iCtwin) == getSrc(iC));
      if (!oriented) break;
    }
  }
  return oriented;
}


// determines if the mesh is orientable
// - a mesh is orientable if a choice of orientation can be made for
//   each face so that, after the face orientation changes are made,
//   the resulting mesh is oriented
// - by definition, a mesh with singular edges is not orientable
// - note that isolated singular vertices play no role in this
//   definition (since cuting through them does not affect
//   orientation)
bool PolygonMesh::isOrientable() const {
  if(hasSingularEdges()) return false;
  if(isOriented()) return true;

  int nC = getNumberOfCorners();
  int nF = getNumberOfFaces();
  vector<bool> face_was_visited(nF,false);
  vector<bool> invert_face(nF,false);
  vector<int> face_root;
  vector<int> corner_stack;

  int nV = getNumberOfVertices();
  Faces faces(nV, _coordIndex);

  int iF_root = 0;

  while (iF_root < nF) {
    face_was_visited[iF_root] = true;

    //Agrego todas las esquinas de la cara al stack
    int iC0 = faces.getFaceFirstCorner(iF_root);
    corner_stack.push_back(iC0);
    int iCNext = getNext(iC0);
    while (iCNext != iC0) {
      corner_stack.push_back(iCNext);
      iCNext = getNext(iCNext);
    }

    while (!corner_stack.empty()) {
      //Saco la siguiente esquina del stack y me fijo si tiene una esquina twin válida (es decir, si es un half-edge incidente a una arista regular)
      int iC = corner_stack[corner_stack.size()-1];
      corner_stack.pop_back();
      int iCt = getTwin(iC);
      if (iCt >= 0) {
        int iF = getFace(iC);
        int iFt = getFace(iCt);

        bool consistentlyOriented = (getSrc(iC) == getDst(iCt));

        //Como iF es la cara de la esquina obtenida del stack, sabemos que ya la visitamos y por lo tanto ya decidimos si se tiene que invertir o no
        //Si la cara iF se tiene que invertir, entonces la cara iFt se tiene que invertir solo si actualmente estan consistentemente orientadas
        //Si la cara iF no se tiene que invertir, entonces la cara iFt se tiene que invertir solo si no están consistentemente orientadas
        bool shouldInvert;
        if (invert_face[iF]) {
          shouldInvert = consistentlyOriented;
        } else {
          shouldInvert = !consistentlyOriented;
        }

        if (face_was_visited[iFt]) {
          if (shouldInvert != invert_face[iFt]) return false;
        } else {
          face_was_visited[iFt] = true;

          invert_face[iFt] = shouldInvert;

          corner_stack.push_back(iCt);
          iCNext = getNext(iCt);
          while (iCNext != iCt) {
            corner_stack.push_back(iCNext);
            iCNext = getNext(iCNext);
          }
        }
      }
    }

    //Busco un nuevo valor iF_root de una cara que no se haya visitado. Si todas las caras se visitaron, iF_root queda con valor nF y se rompe el ciclo
    for (iF_root=0; iF_root<nF; iF_root++) {
      if (face_was_visited[iF_root] == false) break;
    }
  }

  //Si llego a este punto, ya verifiqué que la malla es orientable
  return true;
}


// orient
// - implementation requires a dual graph traversal algorithm
// - the number of connected components nCC of the dual graph are
//   determined as a by product
// - if multiple orientations are posible choose one
// - fills the ccIndex array, of size equal to the number of faces
//   nF, with the connected component number iCC assigned to each face
// - the values stored in the ccIndex shold be in the range 0<=iCC<nCC
// - fills the output invert_face, with values required to produce
//   an oriented mesh
// - the size of output invert_face array should equal to the number
//   of faces
// - returns the number of connected components nCC if successful,
//   and 0 if the mesh is not orientable
// - if not successful, the output arrays should be empty as well
int PolygonMesh::orient(vector<int>& ccIndex, vector<bool>& invert_face) {
  int nCC = 0;
  ccIndex.clear();
  invert_face.clear();
  if(hasSingularEdges()) return 0;
  // if(isOriented()) return true;
  // mesh is not oriented but only has regular and boundary edges
  int nC = getNumberOfCorners();
  int nF = getNumberOfFaces();
  vector<bool> face_was_visited(nF,false);
  vector<int>& face_root = ccIndex;
  vector<int> corner_stack;

  //inicializo ccIndex e invert_face para que tengan tamaño nF
  for (int iF=0; iF<nF; iF++) {
    ccIndex.push_back(-1);
    invert_face.push_back(false);
  }

  int nV = getNumberOfVertices();
  Faces faces(nV, _coordIndex);

  int iF_root = 0;

  while (iF_root < nF) {
    //En cada iteración del while exploro las caras de una nueva componente conexa
    nCC++;
    face_was_visited[iF_root] = true;
    //El ccIndex de cada cara visitada en el while va a ser igual a la cantidad de componentes conexas descubiertas en el momento menos uno
    ccIndex[iF_root] = nCC-1;


    //Agrego todas las esquinas de la cara al stack
    int iC0 = faces.getFaceFirstCorner(iF_root);
    corner_stack.push_back(iC0);
    int iCNext = getNext(iC0);
    while (iCNext != iC0) {
      corner_stack.push_back(iCNext);
      iCNext = getNext(iCNext);
    }

    while (!corner_stack.empty()) {
      //Saco la siguiente esquina del stack y me fijo si tiene una esquina twin válida (es decir, si es un half-edge incidente a una arista regular)
      int iC = corner_stack[corner_stack.size()-1];
      corner_stack.pop_back();
      int iCt = getTwin(iC);
      if (iCt >= 0) {
        int iF = getFace(iC);
        int iFt = getFace(iCt);

        bool consistentlyOriented = (getSrc(iC) == getDst(iCt));

        //Como iF es la cara de la esquina obtenida del stack, sabemos que ya la visitamos y por lo tanto ya decidimos si se tiene que invertir o no
        //Si la cara iF se tiene que invertir, entonces la cara iFt se tiene que invertir solo si actualmente estan consistentemente orientadas
        //Si la cara iF no se tiene que invertir, entonces la cara iFt se tiene que invertir solo si no están consistentemente orientadas
        bool shouldInvert;
        if (invert_face[iF]) {
          shouldInvert = consistentlyOriented;
        } else {
          shouldInvert = !consistentlyOriented;
        }

        if (face_was_visited[iFt]) {
          if (shouldInvert != invert_face[iFt]) {
            //Si encuentro que la malla no es orientable, vacío los vectores del output y devuelvo 0
            ccIndex.clear();
            invert_face.clear();
            return 0;
          }
        } else {
          face_was_visited[iFt] = true;

          //Cuando visito una cara, reflejo en ccIndex que forma parte de la componente conexa que se está explorando actualmente
          ccIndex[iFt] = nCC-1;

          invert_face[iFt] = shouldInvert;

          corner_stack.push_back(iCt);
          iCNext = getNext(iCt);
          while (iCNext != iCt) {
            corner_stack.push_back(iCNext);
            iCNext = getNext(iCNext);
          }
        }
      }
    }

    //Busco un nuevo valor iF_root de una cara que no se haya visitado. Si todas las caras se visitaron, iF_root queda con valor nF y se rompe el ciclo
    for (iF_root=0; iF_root<nF; iF_root++) {
      if (face_was_visited[iF_root] == false) break;
    }
  }

  return nCC;
}

//////////////////////////////////////////////////////////////////////
// MANIFOLD

// determine how many isolated vertices the mesh has
int PolygonMesh::numberOfIsolatedVertices() {
  int nV_isolated = 0;

  vector<int> isolatedVertices = {};
  getIsolatedVertices(isolatedVertices);
  nV_isolated = isolatedVertices.size();

  return nV_isolated;
}

// get array of isolated vertex indices
void PolygonMesh::getIsolatedVertices(vector<int>& isolated_vertex) {

  int nV = getNumberOfVertices();
  int nE = getNumberOfEdges();
  vector<bool> isIsolated(nV, true);

  for (int iE=0; iE<nE; iE++) {
    int V0 = getVertex0(iE);
    int V1 = getVertex1(iE);
    isIsolated[V0] = false;
    isIsolated[V1] = false;
  }

  for (int iV=0; iV<nV; iV++) {
    if (isIsolated[iV]) isolated_vertex.push_back(iV);
  }
}

// remove isolated vertices
// - the new number of vertices nVout should be <= that the original
//   number of vertices nV
// - fills the coordMap array, of size nVout, with an input vertex
//   input in the range 0<=iV<nV for each output vertex index
//   0<=iVout<nVout
// - fills the output coordIndexOut array with vertex indices in the
//   output range 0<=iVout<nVout
// - the output coordIndexOut should be of the same size as the
//   input coordIndex array
// - returns true if one or more isolated vertices have been removed,
//   and false if no isolated vertices have been found
// - if no isolated vertices are found, the output arrays should be
//   empty as well
bool PolygonMesh::removeIsolatedVertices
(vector<int>& coordMap, vector<int>& coordIndexOut) {
  coordMap.clear();
  coordIndexOut.clear();

  vector<int> isolatedVertices = {};
  getIsolatedVertices(isolatedVertices);
  if (isolatedVertices.empty()) return false;

  int nV = getNumberOfVertices();
  vector<int> newVertex(nV, 0);

  for (int iV : isolatedVertices) {
    newVertex[iV] = -1;
  }

  int vertexCounter = 0;
  for (int iV = 0; iV<nV; iV++) {
    if (newVertex[iV]==-1) continue;
    newVertex[iV] = vertexCounter;
    coordMap.push_back(iV);
    vertexCounter++;
  }

  int nC = getNumberOfCorners();
  for (int iC=0; iC<nC; iC++) {
    if (_coordIndex[iC]==-1) {
      coordIndexOut.push_back(-1);
    } else {
      int iV = _coordIndex[iC];
      int newiV = newVertex[iV];
      coordIndexOut.push_back(newiV);
    }
  }

  return true;
}


// cut through singular vertices
// - should only cut through singular vertices which belong to
//   different connected components of the dual graph
// - it should also remove isolated vertices
// - singular edges should not be modified
// - it should work on non-orientable meshes of any kind
// - the new number of vertices nVout should be => that the original
//   number of vertices nV
// - if nVout==nV, should return empty vIndexMap and coordIndexOut
// - otherwise
// - fills the vIndexMap array, of size nVout, with an input vertex
//   input in the range 0<=iV<nV for each output vertex index
//   0<=iVout<nVout
// - fills the output coordIndexOut array with vertex
//   indices in the output range 0<=iVout<nVout
// - the output coordIndexOut should be of the same size as the
//   input coordIndex array
void PolygonMesh::cutThroughSingularVertices
(vector<int>& vIndexMap, vector<int>& coordIndexOut) {
  vIndexMap.clear();
  coordIndexOut.clear();

  int nC = getNumberOfCorners();
  int nE = getNumberOfEdges();
  Partition partition(nC);
  vector<int> firstIncidentHalfEdge(nE, -1);

  for (int iC=0; iC<nC; iC++) {
    if (_coordIndex[iC] == -1) continue;
    int vSrc = getSrc(iC);
    int vDst = getDst(iC);
    int iE = getEdge(min(vSrc, vDst), max(vSrc, vDst));
    if (firstIncidentHalfEdge[iE] == -1) {
      firstIncidentHalfEdge[iE] = iC;
    } else {
      int iCt = firstIncidentHalfEdge[iE];
      if (getSrc(iC) == getDst(iCt)) {
        partition.join(iC, getNext(iCt));
        partition.join(getNext(iC), iCt);
      } else {
        partition.join(iC, iCt);
        partition.join(getNext(iC), getNext(iCt));
      }
    }
  }

  int nF = getNumberOfFaces();
  int nV = getNumberOfVertices();
  int nI = numberOfIsolatedVertices();
  int nVout = partition.getNumberOfParts() - nF;
  //Si la cantidad de partes en la partición de esquinas es igual a la cantidad de vértices sin contar los vértices aislados,
  //entonces no hay vértices singulares que separan componentes conexas.
  if (nVout == (nV-nI)) return;

  for (int iC=0; iC<nC; iC++) {
    coordIndexOut.push_back(-1);
  }

  int vertexCounter=0;
  for (int iC=0; iC<nC; iC++) {
    if (_coordIndex[iC] == -1) continue;
    if (partition.find(iC) == iC) {
      coordIndexOut[iC] = vertexCounter;
      vIndexMap.push_back(_coordIndex[iC]);
      vertexCounter++;
    }
  }

  for (int iC=0; iC<nC; iC++) {
    if (_coordIndex[iC] == -1) continue;
    int iCroot = partition.find(iC);
    if (iCroot != iC) {
      coordIndexOut[iC] = coordIndexOut[iCroot];
    }
  }
}


// convert to manifold
// - removes isolated vertices, cuts through singular vertices and
//   through singular edges
// - the new number of vertices nVout should be => that the original
//   number of vertices nV
// - if nVout==nV, should return empty vIndexMap and coordIndexOut
// - otherwise
// - fills the vIndexMap array, of size nVout, with an input vertex
//   input in the range 0<=iV<nV for each output vertex index
//   0<=iVout<nVout
// - fills the output coordIndexOut array with vertex
//   indices in the output range 0<=iVout<nVout
// - the output coordIndexOut should be of the same size as the
//   input coordIndex array

void PolygonMesh::convertToManifold
(vector<int>& vIndexMap, vector<int>& coordIndexOut) {
  bool success = false;
  vIndexMap.clear();
  coordIndexOut.clear();

  int nC = getNumberOfCorners();
  int nE = getNumberOfEdges();
  Partition partition(nC);
  vector<int> firstIncidentHalfEdge(nE, -1);

  for (int iC=0; iC<nC; iC++) {
    if (_coordIndex[iC] == -1) continue;
    int vSrc = getSrc(iC);
    int vDst = getDst(iC);
    int iE = getEdge(min(vSrc, vDst), max(vSrc, vDst));
    if (isSingularEdge(iE)) continue;
    if (firstIncidentHalfEdge[iE] == -1) {
      firstIncidentHalfEdge[iE] = iC;
    } else {
      int iCt = firstIncidentHalfEdge[iE];
      if (getSrc(iC) == getDst(iCt)) {
        partition.join(iC, getNext(iCt));
        partition.join(getNext(iC), iCt);
      }
    }
  }

  int nF = getNumberOfFaces();
  int nV = getNumberOfVertices();
  int nI = numberOfIsolatedVertices();
  int nVout = partition.getNumberOfParts() - nF;
  if (nVout == (nV-nI)) return;

  for (int iC=0; iC<nC; iC++) {
    coordIndexOut.push_back(-1);
  }

  int vertexCounter=0;
  for (int iC=0; iC<nC; iC++) {
    if (_coordIndex[iC] == -1) continue;
    if (partition.find(iC) == iC) {
      coordIndexOut[iC] = vertexCounter;
      vIndexMap.push_back(_coordIndex[iC]);
      vertexCounter++;
    }
  }

  for (int iC=0; iC<nC; iC++) {
    if (_coordIndex[iC] == -1) continue;
    int iCroot = partition.find(iC);
    if (iCroot != iC) {
      coordIndexOut[iC] = coordIndexOut[iCroot];
    }
  }
}
