//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-08-06 09:11:02 taubin>
//------------------------------------------------------------------------
//
// PolygonMeshTest.cpp
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
#include "PolygonMeshTest.hpp"
#include <wrl/Shape.hpp>
#include <wrl/Appearance.hpp>
#include <wrl/Material.hpp>
#include <wrl/IndexedFaceSet.hpp>
#include <wrl/SceneGraphTraversal.hpp>

PolygonMeshTest::PolygonMeshTest
(SceneGraph& wrl, const string& indent, ostream& ostr):_ostr(ostr) {
  _ostr << indent << "PolygonMeshTest {" << endl;

  int nIndexedFaceSet = 0;

  SceneGraphTraversal traversal(wrl);
  Node* node = (Node*)0;
  while((node=traversal.next())!=(Node*)0) {
    if(node->isShape()) {
      _ostr << indent << "  Shape {" << endl;
      Shape* shape = (Shape*)node;

      _ostr << indent << "    name = \"" << shape->getName() << "\"" << endl;    

      node = shape->getAppearance();
      if(node==(Node*)0) {
        _ostr << indent << "    appearance = NULL" << endl;    
      } else if(node->isAppearance()) {
        Appearance* appearance = (Appearance*)node;
        node = appearance->getMaterial();
        if(node==(Node*)0) {
          _ostr << indent << "    appearance->material = NULL" << endl;    
        } else if(node->isMaterial()) {
          Material* material = (Material*)node;
          const Color& dc = material->getDiffuseColor();
          _ostr << indent << "    diffuseColor = [ "
                << dc.r << " " << dc.g << " " << dc.b << " ]" << endl;    
        }
      }
      
      node = shape->getGeometry();
      if(node==(Node*)0) {
        _ostr << indent << "    geometry = NULL" << endl;    
      } else if(node->isIndexedFaceSet()) {
        _ostr << indent << "    geometry IndexedFaceSet["
              << nIndexedFaceSet << "] {" << endl;
        IndexedFaceSet* ifs = (IndexedFaceSet*)node;

        int nVifs = ifs->getNumberOfCoord();
        vector<int>& coordIndex = ifs->getCoordIndex();

        _ostr << indent << "      nV(ifs) = " << nVifs << endl;

        _ostr << indent << "      PolygonMesh(nV,coordIndex) {" << endl;

        PolygonMesh pMesh(nVifs,coordIndex);

        int nV = pMesh.getNumberOfVertices();
        int nE = pMesh.getNumberOfEdges();
        int nF = pMesh.getNumberOfFaces();
        int nC = pMesh.getNumberOfCorners();

        _ostr << indent << "        nV          = " << nV << endl;
        _ostr << indent << "        nE          = " << nE << endl;
        _ostr << indent << "        nF          = " << nF << endl;
        _ostr << indent << "        nC          = " << nC << endl;

        // print info about the polygon mesh

        int nV_boundary  = 0;
        int nV_internal  = 0;
        int nV_singular  = 0;
        int nV_regular   = 0;
        int nE_boundary  = 0;
        int nE_regular   = 0;
        int nE_singular  = 0;
        int nE_other     = 0;

        int iE,iV;

        for(iE=0;iE<nE;iE++) {
          if(pMesh.isBoundaryEdge(iE)) {
            nE_boundary++;
          } else if(pMesh.isRegularEdge(iE)) {
            nE_regular++;
          } else if(pMesh.isSingularEdge(iE)) {
            nE_singular++;
          } else {
            nE_other++;
          }
        }

        for(iV=0;iV<nV;iV++) {
          if(pMesh.isBoundaryVertex(iV))
            nV_boundary++;
          if(pMesh.isSingularVertex(iV))
            nV_singular++;
        }

        nV_internal = nV-nV_boundary;
        nV_regular  = nV-nV_singular;

        _ostr << indent << "        nV_boundary = " << nV_boundary << endl;
        _ostr << indent << "        nV_internal = " << nV_internal << endl;
        _ostr << indent << "        nV_regular  = " << nV_regular  << endl;
        _ostr << indent << "        nV_singular = " << nV_singular << endl;
        _ostr << indent << "        nE_boundary = " << nE_boundary << endl;
        _ostr << indent << "        nE_regular  = " << nE_regular  << endl;
        _ostr << indent << "        nE_singular = " << nE_singular << endl;
        _ostr << indent << "        nE_other    = " << nE_other    << endl;
        _ostr << indent << "        isRegular   = " << pMesh.isRegular() << endl;
        _ostr << indent << "        hasBoundary = " << pMesh.hasBoundary() << endl;

        _ostr << indent << "      } PolygonMesh" << endl;
        _ostr << indent << "    } IndexedFaceSet" << endl;
        nIndexedFaceSet++;
      } else {
        _ostr << indent << "    geometry " << node->getType() << endl;    
      }

      _ostr << indent << "  } Shape" << endl;
    }
  }

  _ostr << indent << "} PolygonMeshTest" << endl;
}
