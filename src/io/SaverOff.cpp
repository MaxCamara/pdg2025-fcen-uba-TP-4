//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-08-06 09:57:31 taubin>
//------------------------------------------------------------------------
//
// SaverOff.cpp
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

#include <cstring>

#include "SaverOff.hpp"
#include "StrException.hpp"

#include "wrl/Shape.hpp"
#include "wrl/IndexedFaceSet.hpp"
// #include "wrl/Appearance.hpp"
// #include "wrl/Material.hpp"
// #include "core/Faces.hpp"

const char* SaverOff::_ext = "off";

//////////////////////////////////////////////////////////////////////
bool SaverOff::save(const char* filename, SceneGraph& wrl) const {
  bool success = false;
  FILE* fp = (FILE*)0;
  try {
    // Check these conditions
    if(filename==(char*)0)
      throw new StrException("empty filename");
    // 1) the SceneGraph should have a single child
    if(wrl.getNumberOfChildren()!=1)
      throw new StrException("number of SceneGraph children != 1");
    // 2) the child should be a Shape node
    Node* child_0 = wrl[0];
    Shape* shape = dynamic_cast<Shape*>(child_0);
    if(shape==(Shape*)0)
      throw new StrException("first SceneGraph child not a Shape node");
    // 3) the geometry of the Shape node should be an IndexedFaceSet node
    Node* geometry = shape->getGeometry();
    IndexedFaceSet* ifs = dynamic_cast<IndexedFaceSet*>(geometry);
    if(ifs==(IndexedFaceSet*)0)
      throw new StrException("Shape geometry not an IndexedFaceSet");

    // if try to open the file
    fp = fopen(filename,"w");
    if( fp==(FILE*)0)
      throw new StrException("unable to open ASCII OFF output file");

    int nV = ifs->getNumberOfVertices();
    int nF = ifs->getNumberOfFaces();

    vector<float>& coord       = ifs->getCoord();
    vector<int>&   coordIndex  = ifs->getCoordIndex();
    
    fprintf(fp,"OFF %d %d 0\n",nV,nF);

    int iV,niF,iC,iC0,iC1;
    float x,y,z;
    for(iV=0;iV<nV;iV++) {
      x = coord[3*iV  ];
      y = coord[3*iV+1];
      z = coord[3*iV+2];
      fprintf(fp,"%e %e %e ",x,y,z);
      fprintf(fp,"\n");
    }

    int nC = coordIndex.size();
    for(iC0=iC1=0;iC1<nC;iC1++) {
      if(coordIndex[iC1]>=0) continue;
      niF = iC1-iC0;
      fprintf(fp,"%d ",niF);
      for(iC=iC0;iC<iC1;iC++) {
        fprintf(fp,"%d ",coordIndex[iC]);
      }
      fprintf(fp,"\n");

      iC0 = iC1+1;
    }

    fclose(fp);

    success = true;
    
  } catch(StrException* e) { 
    
    if(fp!=(FILE*)0) fclose(fp);
    fprintf(stderr,"SaverOff | ERROR | %s\n",e->what());
    delete e;
  }
  return success;
}
