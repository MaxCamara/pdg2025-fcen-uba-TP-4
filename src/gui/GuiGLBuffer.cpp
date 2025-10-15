//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-08-06 09:56:58 taubin>
//------------------------------------------------------------------------
//
// GuiGLBuffer.cpp
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

#include <iostream>
#include <math.h>
#include "GuiApplication.hpp"
#include "GuiViewerData.hpp"
#include "GuiGLBuffer.hpp"
#include <core/PolygonMesh.hpp>
#include <wrl/IndexedFaceSetVariables.hpp>

//////////////////////////////////////////////////////////////////////
GuiGLBuffer::GuiGLBuffer():
  QOpenGLBuffer(),
  _type(MATERIAL),
  _nVertices(0),
  _nNormals(0),
  _nColors(0),
  _hasFaces(false),
  _hasPolylines(false),
  _hasColor(false),
  _hasNormal(false),
  _paintMode(POINTS) {
}

//////////////////////////////////////////////////////////////////////
GuiGLBuffer::GuiGLBuffer
(IndexedFaceSet* pIfs,
 const Render    render,
 const bool      paintOnlySelected):
  QOpenGLBuffer(),
  _nVertices(0),
  _nNormals(0),
  _nColors(0),
  _hasFaces(false),
  _hasPolylines(false),
  _hasColor(false),
  _hasNormal(false),
  _paintMode(POINTS) {

  // std::cout << "GuiGLBuffer::GuiGLBuffer(IndexedFaceSet) {\n";

  GuiViewerData& data  = getApp()->getMainWindow()->getData();

  if(pIfs==(IndexedFaceSet*)0) return;
  if(render==Render::POLYLINES) return;

  int            nV                = pIfs->getNumberOfVertices();
  vector<float>& coord             = pIfs->getCoord();
  vector<int>&   coordIndex        = pIfs->getCoordIndex();

  IndexedFaceSetVariables ifsv(*pIfs);
  PolygonMesh *pMesh = ifsv.getPolygonMesh(true); // build if not found

  Color&         materialColor     = ifsv.getMaterialColor();
  bool           colorPerVertex    = pIfs->getColorPerVertex();
  vector<float>& color             = pIfs->getColor();
  vector<int>&   colorIndex        = pIfs->getColorIndex();
  // IndexedFaceSet::Binding cBinding = pIfs->getColorBinding();
  bool           normalPerVertex   = pIfs->getNormalPerVertex();
  vector<float>& normal            = pIfs->getNormal();
  vector<int>&   normalIndex       = pIfs->getNormalIndex();
  IndexedFaceSet::Binding nBinding = pIfs->getNormalBinding();
  int            nF                = pIfs->getNumberOfFaces();
  int            nE                = pMesh->getNumberOfEdges();

  _hasFaces  = (nF>0);

  if(render==Render::FACES) { //////////////////////////////////////////
    if(_hasFaces==false) return;

    // std::cout << "  FACES\n";
    // std::cout << "  nF    = " << nF << "\n";

    vector<int>& faceSelection = ifsv.getFaceSelection();

    // int nFsel = 0;
    // for(int iF=0;iF<nF;iF++)
    //   if(faceSelection[iF]>=0) nFsel++;
    // std::cout << "  nFsel = " << nFsel << "\n";

    float  selFaceRgb[3]={0.0f,0.0f,0.0f},defFaceRgb[3]={0.0f,0.0f,0.0f};
    QColor selFaceColor;

    defFaceRgb[0] = materialColor.r;
    defFaceRgb[1] = materialColor.g;
    defFaceRgb[2] = materialColor.b;

    int    nSelFaceColors = data.getNumberOfSelectedFaceColors();
    QRgb*  selFaceColors  = data.getSelectedFaceColors();

    bool ifsHasNormal = (normal.size()>0);
    bool ifsHasColor  = (color.size()>0);

    _hasNormal = ifsHasNormal;
    _hasColor  = true;

    _type      =
      (_hasColor)?
      ((_hasNormal)?COLOR_NORMAL:COLOR):
      ((_hasNormal)?MATERIAL_NORMAL:MATERIAL);

    // count number of vertices, normals, and colors
    int iF,i0,i1;
    for(iF=i0=i1=0;i1<(int)coordIndex.size();i1++)
      if(coordIndex[i1]<0) {
        if(!paintOnlySelected || faceSelection[iF]>=0)
          _nVertices += 3*(i1-i0-2);
        i0 = i1+1; iF++;
      }
    if(_nVertices==0) return;
    if(_hasNormal) _nNormals = _nVertices;
    if(_hasColor)  _nColors  = _nVertices;

    // allocate vertex buffer object.
    this->create();
    this->bind();
    QVector<GLfloat> buf;
    int bufSize = 3*_nVertices+3*_nNormals+3*_nColors;
    buf.resize(bufSize);
    GLfloat *p = buf.data();

    // std::cout << "  bSize = " << bufSize << "\n";

    float x[3][3],n[3][3],c[3][3],rgb[3];
    int   j[3],iN,iC,iV,k,h,iFsel;
    for(iF=i0=i1=0;i1<(int)coordIndex.size();i1++) {
      if(coordIndex[i1]<0) { // number of triangles in this face : i1-i0-2;
        iFsel = faceSelection[iF];
        if(!paintOnlySelected || iFsel>=0) {

          if(iFsel>=0) {
            selFaceColor  = selFaceColors[iFsel%nSelFaceColors];
            selFaceRgb[0] = selFaceColor.redF();
            selFaceRgb[1] = selFaceColor.greenF();
            selFaceRgb[2] = selFaceColor.blueF();
          }

          if(ifsHasNormal && normalPerVertex==false) {
            // NORMAL_PER_FACE_INDEXED or NORMAL_PER_FACE
            iN = (normalIndex.size()>0)?normalIndex[iF]:iF;
            for(h=0;h<3;h++) n[0][h] = n[1][h] = n[2][h] = normal[3*iN+h];
          }

          if(ifsHasColor==false) {
            if(iFsel>=0) { // use selected face color
              for(h=0;h<3;h++)
                c[0][h] = c[1][h] = c[2][h] = selFaceRgb[h];
            } else { // use material color
              for(h=0;h<3;h++)
                c[0][h] = c[1][h] = c[2][h] = defFaceRgb[h];
            }
          } else if(colorPerVertex==false) {
            // COLOR_PER_FACE_INDEXED or COLOR_PER_FACE
            iC = (colorIndex.size()>0)?colorIndex[iF]:iF;
            for(h=0;h<3;h++)
              c[0][h] = c[1][h] = c[2][h] =
                (!paintOnlySelected)?color[3*iC+h]:rgb[h];
          }

          // triangulate face [i0:i1) on the fly and add triangles to current mesh
          for(j[0]=i0,j[1]=i0+1,j[2]=i0+2;j[2]<i1;j[1]=j[2]++) {
            // triangle [j0,j1,j2]
            for(k=0;k<3;k++) {
              // get vertex coordinates
              iV = coordIndex[j[k]];
              for(h=0;h<3;h++) x[k][h] = coord[3*iV+h];

              if(ifsHasNormal && normalPerVertex==true) {
                // NORMAL_PER_CORNER or NORNAL_PER_VERTEX
                iN = (normalIndex.size()>0)?normalIndex[j[k]]:iV;
                for(h=0;h<3;h++) n[k][h] = normal[3*iN+h];
              }
              if(ifsHasColor && colorPerVertex==true) {
                // COLOR_PER_CORNER or COLOR_PER_VERTEX
                iC = (colorIndex.size()>0)?colorIndex[j[k]]:iV;
                for(h=0;h<3;h++)
                  c[k][h] =
                    (!paintOnlySelected)?color[3*iC+h]:rgb[h];
              }
            }

            // push values into buffers
            for(k=2;k>=0;k--) {
                               *p++ = x[k][0]; *p++ = x[k][1]; *p++ = x[k][2];
              if(_hasNormal) { *p++ = n[k][0]; *p++ = n[k][1]; *p++ = n[k][2]; }
              if(_hasColor)  { *p++ = c[k][0]; *p++ = c[k][1]; *p++ = c[k][2]; }
            }
          }

        }
        // advance to next face
        i0 = i1+1; iF++;
      }
    }

    this->allocate(buf.constData(), buf.count() * sizeof(GLfloat));
    this->release();

    _paintMode = TRIANGLES;

  } else if(render==Render::EDGES) { /////////////////////////////////////
    if(_hasFaces==false) return;

    // std::cout << "  EDGES\n";
    // std::cout << "  nE    = " << nE << "\n";

    vector<int>& edgeSelection = ifsv.getEdgeSelection();

    int iE,nEsel = 0;
    for(iE=0;iE<nE;iE++)
      if(edgeSelection[iE]>=0) nEsel++;

    // std::cout << "  nEsel = " << nEsel << "\n";

    float  selEdgeRgb[3]={0.0f,0.0f,0.0f},defEdgeRgb[3]={0.0f,0.0f,0.0f};
    QColor selEdgeColor,defEdgeColor;

    defEdgeColor  = QColor(data.getEdgeColor());
    defEdgeRgb[0] = defEdgeColor.redF();
    defEdgeRgb[1] = defEdgeColor.greenF();
    defEdgeRgb[2] = defEdgeColor.blueF();

    int    nSelEdgeColors = data.getNumberOfSelectedEdgeColors();
    QRgb*  selEdgeColors  = data.getSelectedEdgeColors();

    // only color per vertex is supported
    bool hasColorPerVertex  =
      colorPerVertex  && color.size()>0  && colorIndex.size()==0;
    // only normal per vertex is supported
    bool hasNormalPerVertex =
      normalPerVertex && normal.size()>0 && normalIndex.size()==0;

    _hasColor = true;
    _hasNormal = hasNormalPerVertex;

    _type = (_hasNormal)?COLOR_NORMAL:COLOR;

    // _nVertices = 2*nE;
    _nVertices = (paintOnlySelected)?2*nEsel:2*nE;
    _nColors   = _nVertices;
    if(_hasNormal) _nNormals = _nVertices;

    // allocate vertex buffer object.
    this->create();
    this->bind();
    QVector<GLfloat> buf;
    int bufSize = 3*_nVertices+3*_nNormals+3*_nColors;
    buf.resize(bufSize);
    GLfloat *p = buf.data();

    // std::cout << "  bSize = " << bufSize << "\n";

    int iV0,iV1,iEsel;
    for(iE=0;iE<nE;iE++) {
      
      iEsel = edgeSelection[iE];
      if(paintOnlySelected && iEsel<0) continue;
      
      if(iEsel>=0) {
        selEdgeColor  = selEdgeColors[iEsel%nSelEdgeColors];
        selEdgeRgb[0] = selEdgeColor.redF();
        selEdgeRgb[1] = selEdgeColor.greenF();
        selEdgeRgb[2] = selEdgeColor.blueF();
      }

      iV0 = pMesh->getVertex0(iE);
        *p++ =  coord[3*iV0+0]; *p++ =  coord[3*iV0+1]; *p++ =  coord[3*iV0+2];
      if(hasNormalPerVertex) { // use normal per vertex
        *p++ = normal[3*iV0+0]; *p++ = normal[3*iV0+1]; *p++ = normal[3*iV0+2];
      }
      if(iEsel>=0) {
        *p++ =   selEdgeRgb[0]; *p++ =   selEdgeRgb[1]; *p++ =   selEdgeRgb[2];
      } else if(hasColorPerVertex) { // use color per vertex
        *p++ =  color[3*iV0+0]; *p++ =  color[3*iV0+1]; *p++ =  color[3*iV0+2];
      } else { // use default edge color
        *p++ =   defEdgeRgb[0]; *p++ =   defEdgeRgb[1]; *p++ =   defEdgeRgb[2];
      }
      
      iV1 = pMesh->getVertex1(iE);
        *p++ =  coord[3*iV1+0]; *p++ =  coord[3*iV1+1]; *p++ =  coord[3*iV1+2];
      if(hasNormalPerVertex) { // use normal per vertex
        *p++ = normal[3*iV1+0]; *p++ = normal[3*iV1+1]; *p++ = normal[3*iV1+2];
      }
      if(iEsel>=0) {
        *p++ =   selEdgeRgb[0]; *p++ =   selEdgeRgb[1]; *p++ =   selEdgeRgb[2];
      } else if(hasColorPerVertex) { // use color per vertex
        *p++ =  color[3*iV1+0]; *p++ =  color[3*iV1+1]; *p++ =  color[3*iV1+2];
      } else { // use default edge color
        *p++ =   defEdgeRgb[0]; *p++ =   defEdgeRgb[1]; *p++ =   defEdgeRgb[2];
      }
    }
    
    this->allocate(buf.constData(), buf.count() * sizeof(GLfloat));
    this->release();

    // std::cout << "  buf = [\n";
    // p = buf.data();
    // for(int i=0;i<buf.count()/3;i++) {
    //   std::cout << "  " << (*p++) << " , " << (*p++) << " , " << (*p++) << "\n";
    // }
    // std::cout << "  ];\n";
    
    _paintMode = LINES;

    } else if(render==Render::VERTICES) { //////////////////////////////////

    // std::cout << "  VERTICES\n";
    // std::cout << "  nV    = " << nV << "\n";

    vector<int>& vertexSelection = ifsv.getVertexSelection();

    int iV,nVsel = 0;
    for(iV=0;iV<nV;iV++)
      if(vertexSelection[iV]>=0) nVsel++;

    // std::cout << "  nVsel = " << nVsel << "\n";

    float  selVertexRgb[3]={0.0f,0.0f,0.0f},defVertexRgb[3]={0.0f,0.0f,0.0f};
    QColor selVertexColor,defVertexColor;

    defVertexColor  = QColor(data.getVertexColor());
    defVertexRgb[0] = defVertexColor.redF();
    defVertexRgb[1] = defVertexColor.greenF();
    defVertexRgb[2] = defVertexColor.blueF();

    int    nSelVertexColors = data.getNumberOfSelectedVertexColors();
    QRgb*  selVertexColors  = data.getSelectedVertexColors();

    // only color per vertex is supported
    bool hasColorPerVertex  =
      colorPerVertex  && color.size()>0  && colorIndex.size()==0;
    // only normal per vertex is supported
    bool hasNormalPerVertex =
      normalPerVertex && normal.size()>0 && normalIndex.size()==0;

    _hasColor = true;
    _hasNormal = hasNormalPerVertex;

    _type = (_hasNormal)?COLOR_NORMAL:COLOR;

    _nVertices = (paintOnlySelected)?nVsel:nV;
    _nColors   = _nVertices;
    if(_hasNormal) _nNormals = _nVertices;

    // allocate vertex buffer object.
    this->create();
    this->bind();
    QVector<GLfloat> buf;
    int bufSize = 3*_nVertices+3*_nNormals+3*_nColors;
    buf.resize(bufSize);
    GLfloat *p = buf.data();

    // std::cout << "  bSize = " << bufSize << "\n";

    int iVsel;
    for(iV=0;iV<nV;iV++) {

      iVsel = vertexSelection[iV];
      if(paintOnlySelected && iVsel<0) continue;

      if(iVsel>=0) {
        selVertexColor  = selVertexColors[iVsel%nSelVertexColors];
        selVertexRgb[0] = selVertexColor.redF();
        selVertexRgb[1] = selVertexColor.greenF();
        selVertexRgb[2] = selVertexColor.blueF();
      }

      // coord
        *p++ =   coord[3*iV+0]; *p++ =   coord[3*iV+1]; *p++ =  coord[3*iV+2];
      // normal
      if(hasNormalPerVertex) { // use normal per vertex
        *p++ =  normal[3*iV+0]; *p++ =  normal[3*iV+1]; *p++ = normal[3*iV+2];
      }
      // color
      if(iVsel>=0) { // use selected vertex color
        *p++ = selVertexRgb[0]; *p++ = selVertexRgb[1]; *p++ = selVertexRgb[2];
      } else if(hasColorPerVertex) { // use color per vertex
        *p++ =   color[3*iV+0]; *p++ =   color[3*iV+1]; *p++ =   color[3*iV+2];
      } else { // use default vertex color
        *p++ = defVertexRgb[0]; *p++ = defVertexRgb[1]; *p++ = defVertexRgb[2];
      }

    }  
    
    this->allocate(buf.constData(), buf.count() * sizeof(GLfloat));
    this->release();

    _paintMode = POINTS;

  } else if(render==Render::NORMALS) { /////////////////////////////////////

    _hasNormal = (nBinding!=IndexedFaceSet::Binding::PB_NONE);
    if(_hasNormal==false) return;
    _hasColor = true;
    _type = COLOR;

    QColor defNormalColor;
    defNormalColor  = QColor(data.getNormalColor());
    float  defNormalRgb[3];    
    defNormalRgb[0] = defNormalColor.redF();
    defNormalRgb[1] = defNormalColor.greenF();
    defNormalRgb[2] = defNormalColor.blueF();

    _nNormals = 0;
    _nVertices = 0;
    switch(nBinding) {
    case IndexedFaceSet::Binding::PB_PER_VERTEX:
      _nVertices = 2*nV;
      break;
    case IndexedFaceSet::Binding::PB_PER_FACE:
    case IndexedFaceSet::Binding::PB_PER_FACE_INDEXED:
      _nVertices = 2*nF;
      break;
    case IndexedFaceSet::Binding::PB_PER_CORNER:
      _nVertices = 2*(coordIndex.size()-nF);
      break;
    default:
      break;
    }
    _nColors   = _nVertices; // ???

    // allocate vertex buffer object.
    this->create();
    this->bind();
    QVector<GLfloat> buf;
    int bufSize = 3*_nVertices+3*_nColors; // ???
    buf.resize(bufSize);
    GLfloat *p = buf.data();

     // TODO Sat Mar 11 18:00:27 2023

    float nLen = data.getNormalLength()*data.getNormalFactor()/10.0f;

    switch(nBinding) {
    case IndexedFaceSet::Binding::PB_PER_VERTEX:
      // for each vertex v with normal n
      // - draw segment from v to v+nLen*n
      {
        for(int iV=0;iV<nV;iV++) {
          // get vertex coordinates
          float x0 = coord[3*iV+0];
          float x1 = coord[3*iV+1];
          float x2 = coord[3*iV+2];
          *p++ = x0; *p++ = x1; *p++ = x2;
          *p++ = defNormalRgb[0]; *p++ = defNormalRgb[1]; *p++ = defNormalRgb[2];
          // get vertex normal
          float n0 = normal[3*iV+0];
          float n1 = normal[3*iV+1];
          float n2 = normal[3*iV+2];
          // compute displaced coordinates
          x0 += nLen * n0;
          x1 += nLen * n1;
          x2 += nLen * n2;
          *p++ = x0; *p++ = x1; *p++ = x2;
          *p++ = defNormalRgb[0]; *p++ = defNormalRgb[1]; *p++ = defNormalRgb[2];
        }
      }
      break;
    case IndexedFaceSet::Binding::PB_PER_FACE:
    case IndexedFaceSet::Binding::PB_PER_FACE_INDEXED:
      // for each face f with normal n
      // - draw segment from face centroid v to v+nLen*n
      {
        bool indexed = (nBinding==IndexedFaceSet::Binding::PB_PER_FACE_INDEXED);
        int iF,niF,iC0,iC1,iC,iV,iN,nC=static_cast<int>(coordIndex.size());
        float x0,x1,x2,w,n0,n1,n2;
        for(iF=iC0=iC1=0;iC1<nC;iC1++) {
          if(coordIndex[iC1]>=0) continue;
          niF = iC1-iC0;
          w=static_cast<float>(niF);
        
          // compute face centroid
          x0=x1=x2=0;
          for(iC=iC0;iC<iC1;iC++) {
            iV = coordIndex[iC];
            x0 += coord[3*iV  ];
            x1 += coord[3*iV+1];
            x2 += coord[3*iV+2];
          }
          x0 /= w; x1 /= w; x2 /= w;
          *p++ = x0; *p++ = x1; *p++ = x2;
          *p++ = defNormalRgb[0]; *p++ = defNormalRgb[1]; *p++ = defNormalRgb[2];
        
          // get face normal
          iN = (indexed)?normalIndex[iF]:iF;
          n0 = normal[3*iN  ];
          n1 = normal[3*iN+1];
          n2 = normal[3*iN+2];
          // compute displaced coordinates
          x0 += nLen * n0;
          x1 += nLen * n1;
          x2 += nLen * n2;
          *p++ = x0; *p++ = x1; *p++ = x2;
          *p++ = defNormalRgb[0]; *p++ = defNormalRgb[1]; *p++ = defNormalRgb[2];
        
          // next face
          iC0=iC1+1; iF++;
        }
      }
      break;
    case IndexedFaceSet::Binding::PB_PER_CORNER:
      // for corner v of each face, with corner normal n
      // - draw segment from v to v+nLen*n
      {
        int /*iF,*/niF,iC0,iC1,iC,iV,iN,nC=static_cast<int>(coordIndex.size());
        float x0,x1,x2,x0c,x1c,x2c,w,n0,n1,n2;
        for(/*iF=*/iC0=iC1=0;iC1<nC;iC1++) {
          if(coordIndex[iC1]>=0) continue;
          niF = iC1-iC0;
          w=static_cast<float>(niF);
        
          // compute face centroid coordinates
          x0c=x1c=x2c=0;
          for(iC=iC0;iC<iC1;iC++) {
            iV = coordIndex[iC];
            x0c += coord[3*iV  ];
            x1c += coord[3*iV+1];
            x2c += coord[3*iV+2];
          }
          x0c /= w; x1c /= w; x2c /= w;
        
          for(iC=iC0;iC<iC1;iC++) {
            // get corner coordinates
            iV = coordIndex[iC];
            x0 = coord[3*iV  ];
            x1 = coord[3*iV+1];
            x2 = coord[3*iV+2];
            // translate it a bit towards the centroid
            x0 += 0.1f*(x0c-x0);
            x1 += 0.1f*(x1c-x1);
            x2 += 0.1f*(x2c-x2);
            *p++ = x0; *p++ = x1; *p++ = x2;
            *p++ = defNormalRgb[0]; *p++ = defNormalRgb[1]; *p++ = defNormalRgb[2];
            // get face normal
            iN = normalIndex[iC];
            n0 = normal[3*iN  ];
            n1 = normal[3*iN+1];
            n2 = normal[3*iN+2];
            // compute displaced coordinates
            x0 += nLen * n0;
            x1 += nLen * n1;
            x2 += nLen * n2;
            *p++ = x0; *p++ = x1; *p++ = x2;
            *p++ = defNormalRgb[0]; *p++ = defNormalRgb[1]; *p++ = defNormalRgb[2];
          }
        
          // next face
          iC0=iC1+1; /*iF++;*/
        }
      }

      break;
    default:
      break;
    }

    this->allocate(buf.constData(), buf.count() * sizeof(GLfloat));
    this->release();

    _paintMode = LINES;      
  }

  // std::cout << "}\n";
}

//////////////////////////////////////////////////////////////////////
GuiGLBuffer::GuiGLBuffer
(IndexedLineSet* pIls,
 const Render    render,
 const bool      paintOnlySelected):
  QOpenGLBuffer(),
  _nVertices(0),
  _nNormals(0),
  _nColors(0),
  _hasFaces(false),
  _hasPolylines(false),
  _hasColor(false),
  _hasNormal(false),
  _paintMode(POINTS) {
  (void)paintOnlySelected;

  // std::cout << "GuiGLBuffer::GuiGLBuffer(IndexedLineSet) {\n";

  // GuiViewerData& data  = getApp()->getMainWindow()->getData();

  if(pIls==(IndexedLineSet*)0) return;
  if(render==Render::FACES) return;

  // vector<int>& polylineSelection = pIls->getVertexSelection();

  // QColor&     materialColor  = pIls->getMaterialColor();
  vector<float>& coord          = pIls->getCoord();
  vector<int>&   coordIndex     = pIls->getCoordIndex();
  vector<float>& color          = pIls->getColor();
  vector<int>&   colorIndex     = pIls->getColorIndex();
  bool           colorPerVertex = pIls->getColorPerVertex();
  // int         nV             = pIls->getNumberOfCoord();
  int            nP             = pIls->getNumberOfPolylines();

  // material color values in [0.0:1.0] range
  // qreal matR,matG,matB,matA;
  // materialColor.getRgbF(&matR,&matG,&matB,&matA);
  // QVector3D matColor(matR,matG,matB);

  _hasPolylines = (nP>0);
  _hasColor     = (color.size()>0);
  _type         = (_hasColor)?COLOR:MATERIAL;

  // count _nVertices
  if(_hasPolylines) {
    for(int i0=0,i1=0;i1<(int)coordIndex.size();i1++) {
      if(coordIndex[i1]<0) {
        _nVertices += 2*(i1-i0-1);
        i0 = i1+1;
      }
    }
  } else {
    _nVertices = pIls->getNumberOfCoord();
  }
  if( _hasColor) _nColors  = _nVertices;

  // allocate vertex buffer object.
  this->create();
  this->bind();
  QVector<GLfloat> buf;
  int bufSize = 3*_nVertices+3*_nColors;
  buf.resize(bufSize);
  GLfloat *p = buf.data();

  // std::cout << "  " << bufSize << "\n";

  if(_hasPolylines) {

    // vector<int>& polylineSelection = pIls->getPolylineSelection();

    float x[2][3], c[2][3];
    int   j[2];

    int iC,iV,k,h,i0,i1,iP; // ,nPolylineEdges;
    for(iP=i0=i1=0;i1<(int)coordIndex.size();i1++) {
      if(coordIndex[i1]<0) {
        // nPolylineEdges = i1-i0-1;
        if(_hasColor && colorPerVertex==false) {
          // get color index for this polyline
          iC = (colorIndex.size()>0)?colorIndex[iP]:iP;
          // assign the same color to the two vertices
          for(h=0;h<3;h++)
            c[0][h] = c[1][h] = color[3*iC+h];
        }

        // for each edge in the polyline
        for(j[0]=i0,j[1]=i0+1;j[1]<i1;j[0]=j[1]++) {
          // edge [j0,j1]
          for(k=0;k<2;k++) {
            // get vertex coordinates
            iV = coordIndex[j[k]];
            for(h=0;h<3;h++)
              x[k][h] = coord[3*iV+h];
            // get color per vertex or per corner
            if(_hasColor && colorPerVertex==true) {
              iC = (colorIndex.size()>0)?colorIndex[j[k]]:iV;
              for(h=0;h<3;h++)
                c[k][h] = color[3*iC+h];
            }
          }

          // push values onto buffers
          for(k=1;k>=0;k--) {
                            *p++ = x[k][0]; *p++ = x[k][1]; *p++ = x[k][2];
            if(_hasColor) { *p++ = c[k][0]; *p++ = c[k][1]; *p++ = c[k][2]; }
          }
        }

        // advance to next polyline
        i0 = i1+1; iP++;
      }
    }

    _paintMode = LINES;
  } else

  /*if(!_hasPolylines)*/ { // treat as point cloud

    // vector<int>& vertexSelection = pIls->getVertexSelection();

    unsigned iV,iC;
    for(iV=0;iV<_nVertices;iV++) {
      *p++ = coord[3*iV  ]; *p++ = coord[3*iV+1]; *p++ = coord[3*iV+2];
      if(color.size()>0) {
        iC = (colorIndex.size()>0)?colorIndex[iV]:iV;
        *p++ = color[3*iC  ]; *p++ = color[3*iC+1]; *p++ = color[3*iC+2];
      }
    }

    _paintMode = POINTS;
  }

  this->allocate(buf.constData(), buf.count() * sizeof(GLfloat));
  this->release();

  // std::cout << "  buf = [\n";
  // p = buf.data();
  // for(int i=0;i<buf.count()/3;i++) {
  //   std::cout << "  " << (*p++) << " , " << (*p++) << " , " << (*p++) << "\n";
  // }
  // std::cout << "  ];\n";

  // std::cout << "}\n";
}
