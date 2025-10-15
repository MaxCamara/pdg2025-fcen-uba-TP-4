//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-10-14 17:22:03 taubin>
//------------------------------------------------------------------------
//
// GuiViewerData.cpp
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

#include "GuiViewerData.hpp"

//////////////////////////////////////////////////////////////////////

QRgb GuiViewerData::_vSelColor[] = {
  0xFFFF0000, 0xFF1FD200, 0xFFA7FB00, 0xFFFFE9E6,
  0xFFBC0C1A, 0xFF494614, 0xFF60A808, 0xFFA7C930,
  0xFFE2CECD, 0xFF965055, 0xFF605555, 0xFF5B5500,
  0xFFE50000, 0xFFFF0061, 0xFFBCDE36, 0xFF4D4848,
  0xFFDB265D, 0xFFB7233B, 0xFFEB4F9C, 0xFFA4B16E,
  0xFFFF0800, 0xFFD60000, 0xFFFF6B00, 0xFFFFA200,
  0xFF624300, 0xFFCE6A05, 0xFFAB5004, 0xFFE59D16,
  0xFFDFB02F, 0xFF4E4209, 0xFF081107, 0xFF1C2F00,
  0xFF5D5A00, 0xFFFFE0CD, 0xFFD22200, 0xFF0A0D09,
  0xFF1E2612, 0xFF4A491C, 0xFFECCFC8, 0xFFA86142,
  0xFF0E102E, 0xFF751229, 0xFFBD0000, 0xFFFBFED7,
  0xFFA0BC98, 0xFF181925, 0xFF5E363F, 0xFF97404A,
  0xFFCACBBB, 0xFF8B9688, 0xFF161711, 0xFF75541A,
  0xFFDEA42D, 0xFFFFE687, 0xFFB86400, 0xFF121310,
  0xFF5E5039, 0xFFB19A6B, 0xFFDDCAA4, 0xFF937248,
  0xFF0C0E14, 0xFF506952, 0xFFFFFFFF, 0xFFB5FFA7
}; // 64 colors

QRgb GuiViewerData::_eSelColor[] = {
  0xFF9D16E5, 0xFFD2001F, 0xFFFB00A7, 0xFFE9E6FF,
  0xFF0C1ABC, 0xFF461449, 0xFFA80860, 0xFFC930A7,
  0xFFCECDE2, 0xFF505596, 0xFF555560, 0xFF0000FF,
  0xFF0000E5, 0xFF0061FF, 0xFFDE36BC, 0xFF48484D,
  0xFF265DDB, 0xFF233BB7, 0xFF4F9CEB, 0xFFB16EA4,
  0xFF0800FF, 0xFF0000D6, 0xFF6B00FF, 0xFFA200FF,
  0xFF430062, 0xFF6A05CE, 0xFF5004AB, 0xFF55005B,
  0xFFB02FDF, 0xFF42094E, 0xFF110708, 0xFF2F001C,
  0xFF5A005D, 0xFFE0CDFF, 0xFF2200D2, 0xFF0D090A,
  0xFF26121E, 0xFF491C4A, 0xFFCFC8EC, 0xFF6142A8,
  0xFF102E0E, 0xFF122975, 0xFF0000BD, 0xFFFED7FB,
  0xFFBC98A0, 0xFF192518, 0xFF363F5E, 0xFF404A97,
  0xFFCBBBCA, 0xFF96888B, 0xFF171116, 0xFF541A75,
  0xFFA42DDE, 0xFFE687FF, 0xFF6400B8, 0xFF131012,
  0xFF50395E, 0xFF9A6BB1, 0xFFCAA4DD, 0xFF724893,
  0xFF0E140C, 0xFF695250, 0xFFFFFFFF, 0xFFFFA7B5
};

QRgb GuiViewerData::_fSelColor[] = {
  0xFF00A7FB, 0xFFE63F29, 0xFF001FD2, 0xFF005B55,
  0xFF1ABC0C, 0xFF144946, 0xFF0860A8, 0xFF30A7C9,
  0xFFCDE2CE, 0xFF559650, 0xFF556055, 0xFF00FF00,
  0xFF00E500, 0xFF61FF00, 0xFF36BCDE, 0xFF484D48,
  0xFF5DDB26, 0xFF3BB723, 0xFF9CEB4F, 0xFF6EA4B1,
  0xFF00FF08, 0xFF00D600, 0xFF00FF6B, 0xFF00FFA2,
  0xFF006243, 0xFF05CE6A, 0xFF04AB50, 0xFF16E59D,
  0xFF2FDFB0, 0xFF094E42, 0xFF070811, 0xFF001C2F,
  0xFF005D5A, 0xFFCDFFE0, 0xFF00D222, 0xFF090A0D,
  0xFF121E26, 0xFF1C4A49, 0xFFC8ECCF, 0xFF42A861,
  0xFF2E0E10, 0xFF297512, 0xFF00BD00, 0xFFD7FBFE,
  0xFF98A0BC, 0xFF251819, 0xFF3F5E36, 0xFF4A9740,
  0xFFBBCACB, 0xFF888B96, 0xFF111617, 0xFF1A7554,
  0xFF2DDEA4, 0xFF87FFE6, 0xFF00B864, 0xFF101213,
  0xFF395E50, 0xFF6BB19A, 0xFFA4DDCA, 0xFF489372,
  0xFF140C0E, 0xFF525069, 0xFFFFFFFF, 0xFFA7B5FF
};

int GuiViewerData::getNumberOfSelectedVertexColors() { return 64; }

QRgb GuiViewerData::getSelectedVertexColor(int iColor) {
  if(iColor<0) iColor = 0;
    if(iColor>=64/*_vSelColor.length*/) iColor = 63; //_vSelColor.length-1;
  return _vSelColor[iColor];
}

int GuiViewerData::getNumberOfSelectedEdgeColors() { return 64; }

QRgb GuiViewerData::getSelectedEdgeColor(int iColor) {
  if(iColor<0) iColor = 0;
    if(iColor>=64/*_eSelColor.length*/) iColor = 63; // _eSelColor.length-1;
  return _eSelColor[iColor];
}

int GuiViewerData::getNumberOfSelectedFaceColors() { return 64; }

QRgb GuiViewerData::getSelectedFaceColor(int iColor) {
  if(iColor<0) iColor = 0;
    if(iColor>=64/*_fSelColor.length*/) iColor = 63; // _fSelColor.length-1;
  return _fSelColor[iColor];
}


void GuiViewerData::setSelectedVertexIndex(int iColor) {
  _vSelIndex = (iColor<0)?0:(iColor%64);
}

void GuiViewerData::setSelectedEdgeIndex(int iColor) {
  _eSelIndex = (iColor<0)?0:(iColor%64);
}

void GuiViewerData::setSelectedFaceIndex(int iColor) {
  _fSelIndex = (iColor<0)?0:(iColor%64);
}

int GuiViewerData::getSelectedVertexIndex() {
  return _vSelIndex;
}

int GuiViewerData::getSelectedEdgeIndex() {
  return _eSelIndex;
}

int GuiViewerData::getSelectedFaceIndex() {
  return _fSelIndex;
}

QRgb GuiViewerData::getSelectedVertexColor() {
  return getSelectedVertexColor(_vSelIndex);
}

QRgb GuiViewerData::getSelectedEdgeColor() {
  return getSelectedEdgeColor(_eSelIndex);
}

QRgb GuiViewerData::getSelectedFaceColor() {
  return getSelectedFaceColor(_fSelIndex);
}

//////////////////////////////////////////////////////////////////////

GuiViewerData::GuiViewerData():
  // _initialized(false),
  _pWrl((SceneGraph*)0),
  _bboxDepth(0),
  _bboxCube(true),
  _bboxOccupied(false),
  _bboxScale(1.05f),
  _bboxScaleMin(1.01f),
  _vColor(0xff004000),
  // _eColor(0xffff7f00),
  _eColor(0xff805000),
  // _edColor(0xff000000),
  // _fcColor(0xff7f7f00),
  // _backFaceColor(0xff000000),
  _nColor(0xff0000c0),
  _plColor(0xff00c000),
  _fgColor(0xff000000),
  _bgColor(0xffffffff),
  _vSelIndex(0),
  _eSelIndex(0),
  _fSelIndex(0),
  _paintAllVertices(false),
  _paintSelectedVertices(true),
  _paintAllEdges(false),
  _paintSelectedEdges(true),
  _paintAllFaces(true),
  _paintSelectedFaces(true),
  _paintAllPolylines(true),
  _paintSelectedPolylines(true),
  _paintNormals(false),
  // _lineWidth(2.0f),
  _lineWidth(1.5f),
  _pointSize(4.0f),
  _normalLength(1.0f),
  _normalFactor(10.0f) {
}

GuiViewerData::~GuiViewerData() {
  if(_pWrl!=(SceneGraph*)0) delete _pWrl;
}

void GuiViewerData::setSceneGraph(SceneGraph* pWrl) {
  if(pWrl!=_pWrl) {
    if(_pWrl!=(SceneGraph*)0) delete _pWrl;
    _pWrl = pWrl;
  }
}
