//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-10-14 17:22:03 taubin>
//------------------------------------------------------------------------
//
// GuiPanelRendering.cpp
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
#include <QColorDialog>

#include "GuiApplication.hpp"
#include "GuiMainWindow.hpp"
#include "GuiPanelRendering.hpp"

// #include "wrl/SceneGraphProcessor.hpp"
// #include "core/PolygonMeshTest.hpp"

const char* GuiPanelRendering::bg255 =
  "QLabel { background-color : rgb(255,255,255); color : black; }";
const char* GuiPanelRendering::bg100 =
  "QLabel { background-color : rgb(100,100,100); color : black; }";
const char* GuiPanelRendering::bg200 =
  "QLabel { background-color : rgb(200,200,200); color : black; }";

//////////////////////////////////////////////////////////////////////

// static variable evaluated at start-up time
bool GuiPanelRendering::_registered = GuiPanelRendering::registerPanel();

// static method run once at start-up time
bool GuiPanelRendering::registerPanel() {
  return
    GuiMainWindow::registerPanelFactory
    ("Rendering",
     [](QWidget *parent)->GuiPanel*{ return new GuiPanelRendering(parent);});
}

//////////////////////////////////////////////////////////////////////
GuiPanelRendering::GuiPanelRendering(QWidget* parent):
  GuiPanel(parent) {
  setupUi(this);
}

//////////////////////////////////////////////////////////////////////
void GuiPanelRendering::_setCheckBoxRow
(QString title, QFont& font, QCheckBox*& cbName ) {
  cbName = new QCheckBox(title,this);
  cbName->setFont(font);
  cbName->setChecked(false);
  // cbName->setStyleSheet(bg200);
}

//////////////////////////////////////////////////////////////////////
void GuiPanelRendering::_setCheckBoxRowColor
(QString title, QFont& font, QLabel*& lName ,QLabel*& lHex, QLabel*& lColor) {
  lName = new QLabel(title,this);
  lName->setFont(font);
  lName->setIndent(20);

  lHex = new QLabel("",this);
  lHex->setFont(font);
  lHex->setAlignment(Qt::AlignCenter);
  lHex->setStyleSheet(bg255);

  lColor = new QLabel("",this);
  lColor->setStyleSheet(bg100);
}

//////////////////////////////////////////////////////////////////////
void GuiPanelRendering::_setCheckBoxRowColor
(QString title, QFont& font, QCheckBox*& cbName ,QLabel*& lHex, QLabel*& lColor) {
  cbName = new QCheckBox(title,this);
  cbName->setFont(font);
  cbName->setChecked(false);

  lHex = new QLabel("",this);
  lHex->setFont(font);
  lHex->setAlignment(Qt::AlignCenter);
  lHex->setStyleSheet(bg255);

  lColor = new QLabel("",this);
  lColor->setStyleSheet(bg100);
}

//////////////////////////////////////////////////////////////////////
GuiPanelRendering::~GuiPanelRendering() {
  static int indx = 0;
  // std::cout << "GuiPanelRendering::~GuiPanelRendering("<<(indx++)<<") {\n";
  // std::cout << "}\n";
}

//////////////////////////////////////////////////////////////////////
void GuiPanelRendering::_updateColor
(QRgb newRGB, QLabel* labelHex, QLabel* labelColor) {
  QColor   color   = QColor(newRGB);
  QVariant variant = color;
  QString  colcode = variant.toString();
  QString  hexstr  = QString("%1").arg(newRGB,8,16);
  labelHex->setText(hexstr);
  labelColor->setStyleSheet
    ("QLabel { background-color :"+colcode+" ; color : black; }");
}

//////////////////////////////////////////////////////////////////////
QRgb GuiPanelRendering::_selectColor
(QString title, QRgb oldRGB, QLabel* labelHex, QLabel* labelColor) {
  GuiMainWindow* mainWin = getApp()->getMainWindow();
  QColor   color   = QColorDialog::getColor(QColor(oldRGB),mainWin,title);
  QRgb     newRGB  = color.rgb();
  _updateColor(newRGB,labelHex,labelColor);
  return newRGB;
}

//////////////////////////////////////////////////////////////////////
// overrides pure virtual GuiPanel::updateState()

void GuiPanelRendering::updateState() {
  // std::cout << "GuiPanelRendering::updateState() {\n";

  GuiMainWindow* mainWin   = getApp()->getMainWindow();
  GuiViewerData& data      = mainWin->getData();

  float          pointSize = data.getPointSize();
  spinPointSize->setValue(static_cast<int>(pointSize));
  
  _updateColor
    (data.getVertexColor(),labelVertexHex,labelVertexColor);
  _updateColor
    (data.getEdgeColor(),labelEdgeHex,labelEdgeColor);

  _updateColor
    (data.getPolylineColor(),labelPolylinesHex,labelPolylinesColor);

  _updateColor
    (data.getNormalColor(),labelNormalHex,labelNormalColor);

  bool stateHasChanged = false;
  
  if(cbAllVertices->isChecked()       != data.getPaintAllVertices()) {
    // _cbAllVertices->toggle();
    cbAllVertices->setChecked(data.getPaintAllVertices());
    stateHasChanged = true;
  }
  if(cbSelectedVertices->isChecked()  != data.getPaintSelectedVertices()) {
    // _cbSelectedVertices->toggle();
    cbSelectedVertices->setChecked(data.getPaintSelectedVertices());
    stateHasChanged = true;
  }
  if(cbAllEdges->isChecked()          != data.getPaintAllEdges()) {
    // _cbAllEdges->toggle();
    cbAllEdges->setChecked(data.getPaintAllEdges());
    stateHasChanged = true;
  }
  if(cbSelectedEdges->isChecked()     != data.getPaintSelectedEdges()) {
    // _cbSelectedEdges->toggle();
    cbSelectedEdges->setChecked(data.getPaintSelectedEdges());
    stateHasChanged = true;
  }
  if(cbAllFaces->isChecked()          != data.getPaintAllFaces()) {
    // _cbAllFaces->toggle();
    cbAllFaces->setChecked(data.getPaintAllFaces());
    stateHasChanged = true;
  }
  if(cbSelectedFaces->isChecked()     != data.getPaintSelectedFaces()) {
    // _cbSelectedFaces->toggle();
    cbSelectedFaces->setChecked(data.getPaintSelectedFaces());
    stateHasChanged = true;
  }
  if(cbAllPolylines->isChecked()      != data.getPaintAllPolylines()) {
    // _cbAllPolylines->toggle();
    cbAllPolylines->setChecked(data.getPaintAllPolylines());
    stateHasChanged = true;
  }
  if(cbSelectedPolylines->isChecked() != data.getPaintSelectedPolylines()) {
    // _cbSelectedPolylines->toggle();
    cbSelectedPolylines->setChecked(data.getPaintSelectedPolylines());
    stateHasChanged = true;
  }
  if(cbNormals->isChecked()           != data.getPaintNormals()) {
    // _cbNormals->toggle();
    cbNormals->setChecked(data.getPaintNormals());
    stateHasChanged = true;
  }

  cbSelectedVertices->setEnabled(!data.getPaintAllVertices());
  cbSelectedEdges->setEnabled(!data.getPaintAllEdges());
  cbSelectedFaces->setEnabled(!data.getPaintAllFaces());
  cbSelectedPolylines->setEnabled(!data.getPaintAllPolylines());

  if(stateHasChanged) mainWin->resetSceneGraph();

  // std::cout << "}\n";
}

//////////////////////////////////////////////////////////////////////
void GuiPanelRendering::mousePressEvent(QMouseEvent * event) {

  int x = event->position().x();
  int y = event->position().y();

  GuiMainWindow* mainWin = getApp()->getMainWindow();
  GuiViewerData& data    = mainWin->getData();

  bool update = false;

  QRect pVGeo = panelVariables->geometry();
  x -= pVGeo.x();
  y -= pVGeo.y();

  // geometry with respect to panelVariables
  QRect vGeo = labelVertexColor->geometry();
  QRect eGeo = labelEdgeColor->geometry();
  QRect pGeo = labelPolylinesColor->geometry();
  QRect nGeo = labelNormalColor->geometry();
  
  if(vGeo.contains(x,y,true)) {
    data.setVertexColor
      (_selectColor("VERTEX COLOR", data.getVertexColor(),
                    labelVertexHex, labelVertexColor));
    update = true;
  } else if(eGeo.contains(x,y,true)) {
    data.setEdgeColor
      (_selectColor("EDGE COLOR", data.getEdgeColor(),
                    labelEdgeHex, labelEdgeColor));
    update = true;
  } else if(nGeo.contains(x,y,true)) {
    data.setNormalColor
      (_selectColor("NORMAL COLOR", data.getNormalColor(),
                    labelNormalHex, labelNormalColor));
    update = true;
  } else if(pGeo.contains(x,y,true)) {
    data.setPolylineColor
      (_selectColor("POLYLINE COLOR",data.getPolylineColor(),
                    labelPolylinesHex, labelPolylinesColor));
    update = true;
  }

  if(update) mainWin->resetSceneGraph();
}

//////////////////////////////////////////////////////////////////////

void GuiPanelRendering::on_cbAllVertices_stateChanged(int state) {
  GuiMainWindow* mainWin = getApp()->getMainWindow();
  GuiViewerData& data = mainWin->getData();
  data.setPaintAllVertices(state!=0);
  mainWin->resetSceneGraph();
}

void GuiPanelRendering::on_cbSelectedVertices_stateChanged(int state) {
  GuiMainWindow* mainWin = getApp()->getMainWindow();
  GuiViewerData& data = mainWin->getData();
  data.setPaintSelectedVertices(state!=0);
  mainWin->resetSceneGraph();
}

void GuiPanelRendering::on_cbAllEdges_stateChanged(int state) {
  GuiMainWindow* mainWin = getApp()->getMainWindow();
  GuiViewerData& data = mainWin->getData();
  data.setPaintAllEdges(state!=0);
  mainWin->resetSceneGraph();
}

void GuiPanelRendering::on_cbSelectedEdges_stateChanged(int state) {
  GuiMainWindow* mainWin = getApp()->getMainWindow();
  GuiViewerData& data = mainWin->getData();
  data.setPaintSelectedEdges(state!=0);
  mainWin->resetSceneGraph();
}

void GuiPanelRendering::on_cbAllFaces_stateChanged(int state) {
  GuiMainWindow* mainWin = getApp()->getMainWindow();
  GuiViewerData& data = mainWin->getData();
  data.setPaintAllFaces(state!=0);
  mainWin->resetSceneGraph();
}

void GuiPanelRendering::on_cbSelectedFaces_stateChanged(int state) {
  GuiMainWindow* mainWin = getApp()->getMainWindow();
  GuiViewerData& data = mainWin->getData();
  data.setPaintSelectedFaces(state!=0);
  mainWin->resetSceneGraph();
}

void GuiPanelRendering::on_cbNormals_stateChanged(int state) {
  GuiMainWindow* mainWin = getApp()->getMainWindow();
  GuiViewerData& data = mainWin->getData();
  data.setPaintNormals(state!=0);
  mainWin->resetSceneGraph();
}

void GuiPanelRendering::on_cbAllPolylines_stateChanged(int state) {
  GuiMainWindow* mainWin = getApp()->getMainWindow();
  GuiViewerData& data = mainWin->getData();
  data.setPaintAllPolylines(state!=0);
  mainWin->resetSceneGraph();
}

void GuiPanelRendering::on_cbSelectedPolylines_stateChanged(int state) {
  GuiMainWindow* mainWin = getApp()->getMainWindow();
  GuiViewerData& data = mainWin->getData();
  data.setPaintSelectedPolylines(state!=0);
  mainWin->resetSceneGraph();
}

//////////////////////////////////////////////////////////////////////

void GuiPanelRendering::on_spinPointSize_valueChanged(int value) {
    GuiMainWindow* mainWin = getApp()->getMainWindow();
    GuiViewerData& data = mainWin->getData();
    data.setPointSize((float)value);
    mainWin->resetSceneGraph();
}

void GuiPanelRendering::on_spinLineWidth_valueChanged(int value) {
    GuiMainWindow* mainWin = getApp()->getMainWindow();
    GuiViewerData& data = mainWin->getData();
    data.setLineWidth((float)value);
    mainWin->resetSceneGraph();
}

void GuiPanelRendering::on_spinNormalFactor_valueChanged(int value) {
  GuiMainWindow* mainWin = getApp()->getMainWindow();
  GuiViewerData& data = mainWin->getData();
  data.setNormalFactor((float)value);
  mainWin->resetSceneGraph();
}

