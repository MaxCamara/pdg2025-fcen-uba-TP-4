//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin / 3D Shape Tech LLC
//  Time-stamp: <2025-08-06 09:59:15 taubin>
//------------------------------------------------------------------------
//
// IndexedFaceSetPly.cpp
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

#include "IndexedFaceSetPly.hpp"
#include <util/Endian.hpp>
#include <util/CastMacros.hpp>
#include <io/StrException.hpp>

#include <iostream>
using namespace std;

IndexedFaceSetPly::IndexedFaceSetPly(Ply * ply, const string indent):
  IndexedFaceSet(),
  _ply(ply) {

  (void)indent;

  // APP->log(QString("%1IndexedFaceSetPly() {").arg(indent.c_str()));

  try {

    int iF,i,i0,i1;

    vector<float>& coord         = getCoord();
    vector<int>&   coordIndex    = getCoordIndex();
    vector<float>& normal        = getNormal();
    vector<int>&   normalIndex   = getNormalIndex();
    vector<float>& color         = getColor();
    vector<int>&   colorIndex    = getColorIndex();
    vector<float>& texCoord      = getTexCoord();
    vector<int>&   texCoordIndex = getTexCoordIndex();

    Ply::Element*  vertex        = ply->getElement("vertex");
    int            nVertices     = ply->getNumberOfElementRecords("vertex");
    // Ply::Element*  edge          = ply->getElement("edge");
    // int         nEdges        = ply->getNumberOfElementRecords("edge");
    Ply::Element*  face          = ply->getElement("face");
    int            nFaces        = ply->getNumberOfElementRecords("face");
    // Ply::Element*  material      = ply->getElement("material");
    int            nMaterial     = ply->getNumberOfElementRecords("material");

    // APP->log(QString("%1  nVertices = %2").arg(indent.c_str()).arg(nVertices));
    // APP->log(QString("%1  nEdges    = %2").arg(indent.c_str()).arg(nEdges));
    // APP->log(QString("%1  nFaces    = %2").arg(indent.c_str()).arg(nFaces));
    // APP->log(QString("%1  nMaterial = %2").arg(indent.c_str()).arg(nMaterial));

    if(nMaterial>0) { // element material 1

      // While not common nor standardized in PLY files, some
      // exporters have an option to output a "material" element per
      // PLY file.  The syntax for the "material" element is as
      // follows (this element defines "1" material in the PLY
      // file). The "specular_power" is assumed to lie between 0.0 and
      // 1.0. The opacity is assumed to lie between 0.0 and 1.0. Each
      // color coefficient must lie between 0.0 and 1.0.

      /*
        Ply::Element::Property* aR;// property uchar ambient_red     
        Ply::Element::Property* aG;// property uchar ambient_green 
        Ply::Element::Property* aB;// property uchar ambient_blue 
        Ply::Element::Property* aA;// property float ambient_coeff
        Ply::Element::Property* dR;// property uchar diffuse_red         
        Ply::Element::Property* dG;// property uchar diffuse_green 
        Ply::Element::Property* dB;// property uchar diffuse_blue 
        Ply::Element::Property* dA;// property float diffuse_coeff 
        Ply::Element::Property* sR;// property uchar specular_red
        Ply::Element::Property* sG;// property uchar specular_green 
        Ply::Element::Property* sB;// property uchar specular_blue 
        Ply::Element::Property* sA;// property float specular_coeff
        Ply::Element::Property* sP;// property float specular_power   
        Ply::Element::Property*  o;// property float opacity  
      */

      // TODO Sat Nov 30 14:34:51 2019
      // set Shape->Appearance->Material node
    }

    // vertex coordinates
    if(_ply->getWrlMode()) {

      Ply::Element::Property* coordP = vertex->getProperty("coord");
      if(coordP==nullptr)
        throw new StrException("  ply does not have vertex coordinates");
      vector<float>* coordV =
        static_cast<vector<float>*>(coordP->getValue());
      coord.insert(coord.end(),coordV->begin(),coordV->end());
    
      // normals per vertex
      Ply::Element::Property* normalP = vertex->getProperty("normal");
      if(normalP!=nullptr) {

        // APP->log(QString("%1  has normals per vertex").arg(indent.c_str()));

        setNormalPerVertex(true);
        normal.clear();
        normalIndex.clear();
        vector<float>* normalV =
          static_cast<vector<float>*>(normalP->getValue());
        normal.insert(normal.end(),normalV->begin(),normalV->end());

        // APP->log(QString("%1  nNormals = %2")
        //          .arg(indent.c_str()).arg(normal.size()/3));

      } else {
        // APP->log(QString("%1  does not have normal per vertex").arg(indent.c_str()));
      }
    
      // colors per vertex
      Ply::Element::Property* colorP = vertex->getProperty("color");
      if(colorP!=nullptr) {

        // APP->log(QString("%1  has colors per vertex").arg(indent.c_str()));

        setColorPerVertex(true);
        color.clear();
        colorIndex.clear();
        vector<float>* colorV =
          static_cast<vector<float>*>(colorP->getValue());
        color.insert(color.end(),colorV->begin(),colorV->end());

        // APP->log(QString("%1  nColors = %2")
        //          .arg(indent.c_str()).arg(color.size()/3));

      } else {
        // APP->log(QString("%1  does not have color per vertex").arg(indent.c_str()));
      }

      // texture coordinates per vertex
      Ply::Element::Property* texCoordP = vertex->getProperty("texCoord");
      if(texCoordP!=nullptr) {

        // APP->log(QString("%1  has texture coordinates per vertex")
        //          .arg(indent.c_str()));

        texCoord.clear();
        texCoordIndex.clear();
        vector<float>* texCoordV =
          static_cast<vector<float>*>(texCoordP->getValue());
        texCoord.insert(texCoord.end(),texCoordV->begin(),texCoordV->end());

        // APP->log(QString("%1  nTexCoord = %2")
        //          .arg(indent.c_str()).arg(texCoord.size()/2));
      } else {
        // APP->log(QString("%1  does not have texture coordinates per vertex")
        //          .arg(indent.c_str()));
      }
    
      // faces
      if(face!=nullptr) {

        // APP->log(QString("%1  has faces").arg(indent.c_str()));
      
        Ply::Element::Property* coordIndexP = face->getProperty("coordIndex");
        vector<int>* coordIndexV =
          static_cast<vector<int>*>(coordIndexP->getValue());
        coordIndex.insert(coordIndex.end(),coordIndexV->begin(),coordIndexV->end());
    
        // normals per face
        Ply::Element::Property* normalP = face->getProperty("normal");
        if(normalP!=nullptr) {

          // APP->log(QString("%1  has normals per face").arg(indent.c_str()));

          setNormalPerVertex(false);
          normal.clear();
          normalIndex.clear();
          vector<float>* normalV =
            static_cast<vector<float>*>(normalP->getValue());
          normal.insert(normal.end(),normalV->begin(),normalV->end());

          // APP->log(QString("%1  nNormals = %2")
          //          .arg(indent.c_str()).arg(normal.size()/3));

        } else {
          // APP->log(QString("%1  does not have normal per face").arg(indent.c_str()));
        }
    
        // colors per vertex
        Ply::Element::Property* colorP = face->getProperty("color");
        if(colorP!=nullptr) {

          // APP->log(QString("%1  has colors per face").arg(indent.c_str()));

          setColorPerVertex(false);
          color.clear();
          colorIndex.clear();
          vector<float>* colorV =
            static_cast<vector<float>*>(colorP->getValue());
          color.insert(color.end(),colorV->begin(),colorV->end());

          // APP->log(QString("%1  nColors = %2")
          //          .arg(indent.c_str()).arg(color.size()/3));

        } else {
          // APP->log(QString("%1  does not have color per face").arg(indent.c_str()));
        }
      
      } else {
        // APP->log(QString("%1  has no faces").arg(indent.c_str()));
      }

    } else {

      Ply::Element::Property* xP = vertex->getProperty("x");
      Ply::Element::Property* yP = vertex->getProperty("y");
      Ply::Element::Property* zP = vertex->getProperty("z");
      if(xP==nullptr || yP==nullptr || zP==nullptr)
        throw new StrException("  ply does not have vertex coordinates");

      // APP->log(QString("%1  has vertex coordinates").arg(indent.c_str()));

      vector<float>* xV = static_cast<vector<float>*>(xP->getValue());
      vector<float>* yV = static_cast<vector<float>*>(yP->getValue());
      vector<float>* zV = static_cast<vector<float>*>(zP->getValue());
      for(i=0;i<nVertices;i++) {
        coord.push_back((*xV)[UL(i)]);
        coord.push_back((*yV)[UL(i)]);
        coord.push_back((*zV)[UL(i)]);
      }
    
      // normals per vertex
      Ply::Element::Property* nxP = vertex->getProperty("nx");
      Ply::Element::Property* nyP = vertex->getProperty("ny");
      Ply::Element::Property* nzP = vertex->getProperty("nz");
      if(nxP!=nullptr && nyP!=nullptr && nzP!=nullptr) {

        // APP->log(QString("%1  has normals per vertex").arg(indent.c_str()));

        setNormalPerVertex(true);
        normal.clear();
        normalIndex.clear();
        vector<float>* nxV = static_cast<vector<float>*>(nxP->getValue());
        vector<float>* nyV = static_cast<vector<float>*>(nyP->getValue());
        vector<float>* nzV = static_cast<vector<float>*>(nzP->getValue());
        for(i=0;i<nVertices;i++) {
          normal.push_back((*nxV)[UL(i)]);
          normal.push_back((*nyV)[UL(i)]);
          normal.push_back((*nzV)[UL(i)]);
        }
        // APP->log(QString("%1  nNormals = %2")
        //          .arg(indent.c_str()).arg(normal.size()/3));

      } else {
        // APP->log(QString("%1  does not have normal per vertex").arg(indent.c_str()));
      }
    
      // colors per vertex
      Ply::Element::Property* rP = vertex->getProperty("red");
      Ply::Element::Property* gP = vertex->getProperty("green");
      Ply::Element::Property* bP = vertex->getProperty("blue");
      if(rP!=nullptr && gP!=nullptr && bP!=nullptr) {

        // APP->log(QString("%1  has color per vertex").arg(indent.c_str()));

        setColorPerVertex(true);
        color.clear();
        colorIndex.clear();
        vector<uchar>* rV = static_cast<vector<uchar>*>(rP->getValue());
        vector<uchar>* gV = static_cast<vector<uchar>*>(gP->getValue());
        vector<uchar>* bV = static_cast<vector<uchar>*>(bP->getValue());
        for(i=0;i<nVertices;i++) {      
          color.push_back(F((*rV)[UL(i)]&0xff)/255.0f);
          color.push_back(F((*gV)[UL(i)]&0xff)/255.0f);
          color.push_back(F((*bV)[UL(i)]&0xff)/255.0f);
        }
        // APP->log(QString("%1  nColors = %2")
        //          .arg(indent.c_str()).arg(color.size()/3));
      } else {
        // APP->log(QString("%1  does not have color per vertex").arg(indent.c_str()));
      }
    
      // texture coordinates per vertex
      Ply::Element::Property* uP = vertex->getProperty("u");
      Ply::Element::Property* vP = vertex->getProperty("v");
      if(uP!=nullptr && vP!=nullptr) {

        // APP->log(QString("%1  has texture coordinates per vertex")
        //          .arg(indent.c_str()));

        texCoord.clear();
        texCoordIndex.clear();
        vector<float>* u = static_cast<vector<float>*>(uP->getValue());
        vector<float>* v = static_cast<vector<float>*>(vP->getValue());
        for(i=0;i<nVertices;i++) {      
          texCoord.push_back((*u)[UL(i)]);
          texCoord.push_back((*v)[UL(i)]);
        }
        // APP->log(QString("%1  nTexCoord = %2")
        //          .arg(indent.c_str()).arg(texCoord.size()/2));
      } else {
        // APP->log(QString("%1  does not have texture coordinates per vertex")
        //          .arg(indent.c_str()));
      }
    
      // faces
      if(face!=nullptr) {

        // APP->log(QString("%1  has faces").arg(indent.c_str()));
      
        Ply::Element::Property* indxP = face->getProperty("vertex_indices");
        vector<int>*            indxV = static_cast<vector<int>*>(indxP->getValue());
        for(iF=0;iF<nFaces;iF++) {
          i0   = indxP->getListFirst(iF );
          i1   = indxP->getListFirst(iF+1);
          for(i=i0;i<i1;i++)
            coordIndex.push_back((*indxV)[UL(i)]);
          coordIndex.push_back(-1);
        }
    
        // normals per face
        Ply::Element::Property* nxP = face->getProperty("nx");
        Ply::Element::Property* nyP = face->getProperty("ny");
        Ply::Element::Property* nzP = face->getProperty("nz");
        if(nxP!=nullptr && nyP!=nullptr && nzP!=nullptr) {

          // APP->log(QString("%1  has normals per face").arg(indent.c_str()));

          setNormalPerVertex(false);
          normal.clear();
          normalIndex.clear();
          vector<float>* nxV = static_cast<vector<float>*>(nxP->getValue());
          vector<float>* nyV = static_cast<vector<float>*>(nyP->getValue());
          vector<float>* nzV = static_cast<vector<float>*>(nzP->getValue());
          for(i=0;i<nFaces;i++) {
            normal.push_back((*nxV)[UL(i)]);
            normal.push_back((*nyV)[UL(i)]);
            normal.push_back((*nzV)[UL(i)]);
          }
          // APP->log(QString("%1  nNormals = %2")
          //          .arg(indent.c_str()).arg(normal.size()/3));

        } else {
          // APP->log(QString("%1  does not have normal per face").arg(indent.c_str()));
        }
      
        // face colors
        rP = face->getProperty("red");
        gP = face->getProperty("green");
        bP = face->getProperty("blue");
        if(rP!=nullptr && gP!=nullptr && bP!=nullptr) {

          // APP->log(QString("%1  has color per face").arg(indent.c_str()));

          setColorPerVertex(false);
          color.clear();
          colorIndex.clear();
          vector<uchar>* rV = static_cast<vector<uchar>*>(rP->getValue());
          vector<uchar>* gV = static_cast<vector<uchar>*>(gP->getValue());
          vector<uchar>* bV = static_cast<vector<uchar>*>(bP->getValue());
          for(i=0;i<nFaces;i++) {      
            color.push_back(F((*rV)[UL(i)]&0xff)/255.0f);
            color.push_back(F((*gV)[UL(i)]&0xff)/255.0f);
            color.push_back(F((*bV)[UL(i)]&0xff)/255.0f);
          }
          // APP->log(QString("%1  nColor = %2")
          //          .arg(indent.c_str()).arg(color.size()/3));
        } else {
          // APP->log(QString("%1  does not have color per face").arg(indent.c_str()));
        }
      
        // // texture coordinates per corner
        // Ply::Element::Property* texCoordP = face->getProperty("texcoord");
        // if(texCoordP!=nullptr) {
        //
        //   APP->log(QString("%1  has texture coordinates per corner")
        //            .arg(indent.c_str()));
        //
        //   vector<float>* texCoordV =
        //     static_cast<vector<float>*>(texCoordP->getValue());
        //   for(j=iF=0;iF<nFaces;iF++) {
        //     i0   = texCoordP->getListFirst(iF );
        //     i1   = texCoordP->getListFirst(iF+1);
        //     n    = (i1-i0)/2;
        //     for(i=0;i<n;i++)
        //       texCoordIndex.push_back(j++);
        //     for(i=i0;i<i1;i++)
        //       texCoord.push_back((*texCoordV)[i]);
        //     texCoordIndex.push_back(-1);
        //   }
        //        
        // } else {
        //   APP->log(QString("%1  does not have texture coordinates per corner")
        //            .arg(indent.c_str()));
        // }
      
      } else {
        // APP->log(QString("%1  does not have faces").arg(indent.c_str()));
      }
    }
    
  } catch(StrException* e) {
    // APP->log(QString("%1  EXCEPTION | ").arg(indent.c_str()).arg(e->what()));
    delete e;
  }
  
  // APP->log(QString("%1}").arg(indent.c_str()));
}

IndexedFaceSetPly::~IndexedFaceSetPly() {
  if(_ply) delete _ply;
}

// void IndexedFaceSetPly::printInfo(string indent) {
//   // TODO
//   IndexedFaceSet::printInfo(indent);
// }
