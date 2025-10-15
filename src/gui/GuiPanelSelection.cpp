//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-10-14 17:22:03 taubin>
//------------------------------------------------------------------------
//
// GuiPanelSelection.cpp
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

#include "GuiApplication.hpp"
#include "GuiMainWindow.hpp"
#include "GuiPanelSelection.hpp"
#include "GuiViewerData.hpp"

#include "wrl/SceneGraphTraversal.hpp"
// #include "wrl/SceneGraphProcessor.hpp"
#include "wrl/Shape.hpp"
#include "wrl/IndexedFaceSetVariables.hpp"

int GuiPanelSelection::_selectedVertexIndex = -1;
int GuiPanelSelection::_selectedEdgeIndex = -1;
int GuiPanelSelection::_selectedFaceIndex = -1;

//////////////////////////////////////////////////////////////////////

// static variable evaluated at start-up time
bool GuiPanelSelection::_registered = GuiPanelSelection::registerPanel();

// static method run once at start-up time
bool GuiPanelSelection::registerPanel() {
  return
    GuiMainWindow::registerPanelFactory
    ("Selection",
     [](QWidget *parent)->GuiPanel*{ return new GuiPanelSelection(parent);});
}

//////////////////////////////////////////////////////////////////////
// static
void GuiPanelSelection::_selectVertices(IndexedFaceSet& ifs, const Mode mode) {
  IndexedFaceSetVariables ifsv(ifs);
  vector<int>& vSel = ifsv.getVertexSelection();
  int nV = ifs.getNumberOfVertices();
  PolygonMesh* pm = ifsv.getPolygonMesh();
  int iV;
  switch(mode) {
  case CLEAR:
    for(iV=0;iV<nV;iV++)
      vSel[iV] = -1;
    break;
  case INVERT:
    for(iV=0;iV<nV;iV++)
      vSel[iV] = (vSel[iV]<0)?_selectedVertexIndex:-1;
    break;
  case BOUNDARY:
    for(iV=0;iV<nV;iV++)
      if(pm->isBoundaryVertex(iV))
        vSel[iV] = _selectedVertexIndex;
    break;
  case REGULAR:
    for(iV=0;iV<nV;iV++)
      if(pm->isSingularVertex(iV)==false)
        vSel[iV] = _selectedVertexIndex;
    break;
  case SINGULAR:
    for(iV=0;iV<nV;iV++)
      if(pm->isSingularVertex(iV))
        vSel[iV] = _selectedVertexIndex;
    break;
  }
}

//////////////////////////////////////////////////////////////////////
// static
void GuiPanelSelection::_selectEdges(IndexedFaceSet& ifs, const Mode mode) {
  IndexedFaceSetVariables ifsv(ifs);
  vector<int>& eSel = ifsv.getEdgeSelection();
  int nE = ifsv.getNumberOfEdges();
  PolygonMesh* pm = ifsv.getPolygonMesh();
  int iE;
  switch(mode) {
  case CLEAR:
    for(iE=0;iE<nE;iE++)
      eSel[iE] = -1;
    break;
  case INVERT:
    for(iE=0;iE<nE;iE++)
      eSel[iE] = (eSel[iE]<0)?_selectedEdgeIndex:-1;
    break;
  case BOUNDARY:
    for(iE=0;iE<nE;iE++)
      if(pm->isBoundaryEdge(iE))
        eSel[iE] = _selectedEdgeIndex;
    break;
  case REGULAR:
    for(iE=0;iE<nE;iE++)
      if(pm->isRegularEdge(iE))
        eSel[iE] = _selectedEdgeIndex;
    break;
  case SINGULAR:
    for(iE=0;iE<nE;iE++)
      if(pm->isSingularEdge(iE))
        eSel[iE] = _selectedEdgeIndex;
    break;
  }
}

//////////////////////////////////////////////////////////////////////
// static
void GuiPanelSelection::_selectFaces(IndexedFaceSet& ifs, const Mode mode) {
  IndexedFaceSetVariables ifsv(ifs);
  vector<int>& fSel = ifsv.getFaceSelection();
  int nF = ifs.getNumberOfFaces();

  int iF;

  if(mode==CLEAR) {
    for(iF=0;iF<nF;iF++)
      fSel[iF] = -1;
    return;
  }
  
  if(mode==INVERT) {
    for(iF=0;iF<nF;iF++)
      fSel[iF] = (fSel[iF]<0)?_selectedFaceIndex:-1;
    return;
  }

  PolygonMesh* pm = ifsv.getPolygonMesh();
   vector<int>& coordIndex = ifs.getCoordIndex();
  int nC = coordIndex.size(); // == pm->getNumberOfCorners();

  int iC,iC0,iC1,iV0,iV1,iE;
  for(iF=iC0=iC1=0;iC1<nC;iC1++) {
    if(coordIndex[iC1]>=0) continue;
    for(iC=iC0;iC<iC1;iC++) {
      iV0 = coordIndex[iC]; // == pm->getSrc(iC);
      iV1 = pm->getDst(iC);
      iE  = pm->getEdge(iV0,iV1);

      switch(mode) {
      case BOUNDARY:
        if(pm->isBoundaryVertex(iV0) || pm->isBoundaryEdge(iE))
          fSel[iF] = _selectedFaceIndex;
        break;
      case REGULAR:
        if(pm->isSingularVertex(iV0)==false || pm->isRegularEdge(iE))
          fSel[iF] = _selectedFaceIndex;
        break;
      case SINGULAR:
        if(pm->isSingularVertex(iV0) || pm->isSingularEdge(iE))
          fSel[iF] = _selectedFaceIndex;
        break;
      default:
          break;
      }
    }

    iC0 = iC1+1; iF++;
  }
}

//////////////////////////////////////////////////////////////////////
// static
void GuiPanelSelection::_select(const Element elem, const Mode mode) {
  GuiViewerData& data = getApp()->getMainWindow()->getData();
  SceneGraph* pWrl = data.getSceneGraph();

  Node* node;
  SceneGraphTraversal sgt(*pWrl);
  while((node=sgt.next())!=nullptr) {
    Shape* shape = dynamic_cast<Shape*>(node);
    if(shape==(Shape*)0) continue;
    node = shape->getGeometry();
    if(IndexedFaceSet* ifs=dynamic_cast<IndexedFaceSet*>(node)) {
      switch(elem) {
      case VERTICES: _selectVertices(*ifs,mode); break;
      case EDGES:    _selectEdges(*ifs,mode);    break;
      case FACES:    _selectFaces(*ifs,mode);    break;
      }
    }
  }

  getApp()->getMainWindow()->resetSceneGraph();
}

//////////////////////////////////////////////////////////////////////
// static
void GuiPanelSelection::_fromTo(const Element elemFrom, const Element elemTo) {
  GuiViewerData& data = getApp()->getMainWindow()->getData();
  SceneGraph* pWrl = data.getSceneGraph();

  Node* node;
  SceneGraphTraversal sgt(*pWrl);
  while((node=sgt.next())!=nullptr) {
    Shape* shape = dynamic_cast<Shape*>(node);
    if(shape==(Shape*)0) continue;

    node = shape->getGeometry();
    IndexedFaceSet* ifs=dynamic_cast<IndexedFaceSet*>(node);
    if(ifs==nullptr) continue;

    IndexedFaceSetVariables ifsv(*ifs);
    vector<int>& vSel = ifsv.getVertexSelection();
    vector<int>& eSel = ifsv.getEdgeSelection();
    vector<int>& fSel = ifsv.getFaceSelection();

    PolygonMesh* pm = ifsv.getPolygonMesh();
    vector<int>& coordIndex = ifs->getCoordIndex();
    int nC= coordIndex.size();

    int iF,iC,iC0,iC1,iV0,iV1,iE;
    for(iF=iC0=iC1=0;iC1<nC;iC1++) {
      if(coordIndex[iC1]>=0) continue;
      for(iC=iC0;iC<iC1;iC++) {
        iV0 = coordIndex[iC]; // == pm->getSrc(iC);
        iV1 = pm->getDst(iC);
        iE  = pm->getEdge(iV0,iV1);
      
        switch(elemFrom) {
        case VERTICES:
          switch(elemTo) {
          case EDGES:
            if(vSel[iV0]==_selectedVertexIndex || vSel[iV1]==_selectedVertexIndex)
              eSel[iE] = _selectedEdgeIndex;
            break;
          case FACES:
            if(vSel[iV0]==_selectedVertexIndex || vSel[iV1]==_selectedVertexIndex)
              fSel[iF] = _selectedFaceIndex;
            break;
          default:
              break;
          }
          break;
        case EDGES:
          switch(elemTo) {
          case VERTICES:
            if(eSel[iE]==_selectedEdgeIndex)
              vSel[iV0] = vSel[iV1] = _selectedVertexIndex;
            break;
          case FACES:
            if(eSel[iE]==_selectedEdgeIndex)
              fSel[iF] = _selectedFaceIndex;
            break;
          default:
              break;
          }
          break;
        case FACES:
          switch(elemTo) {
          case VERTICES:
            if(fSel[iF]==_selectedFaceIndex)
              vSel[iV0] = vSel[iV1] = _selectedVertexIndex;
            break;
          case EDGES:
            if(fSel[iF]==_selectedFaceIndex)
              eSel[iE] = _selectedEdgeIndex;
            break;
          default:
              break;
          }
          break;
        }

      } // for(iC= ...
      // next face  
      iC0 = iC1+1; iF++;
    } // for(iF=iC0=iC1=0;iC1<nC;iC1++)
  } // end of scene traversal
  
  getApp()->getMainWindow()->resetSceneGraph();
}

//////////////////////////////////////////////////////////////////////
GuiPanelSelection::GuiPanelSelection(QWidget* parent):
  GuiPanel(parent)
{
  setupUi(this);
  
  GuiViewerData& data = getApp()->getMainWindow()->getData();

  _selectedVertexIndex = data.getSelectedVertexIndex();
  _selectedEdgeIndex   = data.getSelectedEdgeIndex();
  _selectedFaceIndex   = data.getSelectedFaceIndex();
  
  spinSelectedVertexIndex->setValue(_selectedVertexIndex);
  spinSelectedEdgeIndex->setValue(_selectedEdgeIndex);
  spinSelectedFaceIndex->setValue(_selectedFaceIndex);

  // TODO Sun Mar  5 21:19:40 2023
  // temporarily ...

  buttonShapeVertices->setEnabled(false);
  buttonShapeEdges->setEnabled(false);
  buttonShapeFaces->setEnabled(false);

  buttonConnectedVertices->setEnabled(false);
  buttonConnectedEdges->setEnabled(false);
  buttonConnectedFaces->setEnabled(false);

  buttonDeleteVertices->setEnabled(false);
  buttonDeleteEdges->setEnabled(false);
  buttonDeleteFaces->setEnabled(false);

  buttonCutEdges->setEnabled(false);
  buttonCutFaces->setEnabled(false);
}

//////////////////////////////////////////////////////////////////////
GuiPanelSelection::~GuiPanelSelection() {
  // ...
}

//////////////////////////////////////////////////////////////////////
// overrides pure virtual GuiPanel::updateState()

void GuiPanelSelection::updateState() {
  // std::cout << "GuiPanelSelection::updateState() {\n";
  // auto _mainWindow = getApp()->getMainWindow();
  // GuiViewerData& data  = _mainWindow->getData();
  int sVIndex = spinSelectedVertexIndex->value();
  on_spinSelectedVertexIndex_valueChanged(sVIndex);
  int sEIndex = spinSelectedEdgeIndex->value();
  on_spinSelectedEdgeIndex_valueChanged(sEIndex);
  int sFIndex = spinSelectedFaceIndex->value();
  on_spinSelectedFaceIndex_valueChanged(sFIndex);
  // std::cout << "}\n";
}

//////////////////////////////////////////////////////////////////////
void GuiPanelSelection::mousePressEvent(QMouseEvent * event) {

  int x = event->position().x();
  int y = event->position().y();

  bool clickedOnLabel = false;

  if(labelIndices->geometry().contains(x,y,true)) {
    panelIndices->setVisible(panelIndices->isHidden());
    clickedOnLabel = true;
  } else if(labelSelect->geometry().contains(x,y,true)) {
    panelSelect->setVisible(panelSelect->isHidden());
    clickedOnLabel = true;
  } else if(labelSegment->geometry().contains(x,y,true)) {
    panelSegment->setVisible(panelSegment->isHidden());
    clickedOnLabel = true;
  } else if(labelEdit->geometry().contains(x,y,true)) {
    panelEdit->setVisible(panelEdit->isHidden());
    clickedOnLabel = true;
  }

  if(clickedOnLabel) updateState();

}

//////////////////////////////////////////////////////////////////////

void GuiPanelSelection::on_spinSelectedVertexIndex_valueChanged(int value) {
  GuiViewerData& data = getApp()->getMainWindow()->getData();
  data.setSelectedVertexIndex(value);
  QVariant variant = QColor(data.getSelectedVertexColor());
  QString  colcode = variant.toString();
  labelSelectedVertexColor->setStyleSheet
    ("QLabel { background-color :"+colcode+" ; color : black; }");
}

void GuiPanelSelection::on_spinSelectedEdgeIndex_valueChanged(int value) {
  GuiViewerData& data = getApp()->getMainWindow()->getData();
  data.setSelectedEdgeIndex(value);
  QVariant variant = QColor(data.getSelectedEdgeColor());
  QString  colcode = variant.toString();
  labelSelectedEdgeColor->setStyleSheet
    ("QLabel { background-color :"+colcode+" ; color : black; }");
}

void GuiPanelSelection::on_spinSelectedFaceIndex_valueChanged(int value) {
  GuiViewerData& data = getApp()->getMainWindow()->getData();
  data.setSelectedFaceIndex(value);
  QVariant variant = QColor(data.getSelectedFaceColor());
  QString  colcode = variant.toString();
  labelSelectedFaceColor->setStyleSheet
    ("QLabel { background-color :"+colcode+" ; color : black; }");
}

//////////////////////////////////////////////////////////////////////

void GuiPanelSelection::on_buttonClearVertices_clicked() {
  _select(VERTICES,CLEAR);
}

void GuiPanelSelection::on_buttonClearEdges_clicked() {
  _select(EDGES,CLEAR);
}

void GuiPanelSelection::on_buttonClearFaces_clicked() {
  _select(FACES,CLEAR);
}

//////////////////////////////////////////////////////////////////////

void GuiPanelSelection::on_buttonInvertVertices_clicked() {
  _select(VERTICES,INVERT);
}

void GuiPanelSelection::on_buttonInvertEdges_clicked() {
  _select(EDGES,INVERT);
}

void GuiPanelSelection::on_buttonInvertFaces_clicked() {
  _select(FACES,INVERT);
}

//////////////////////////////////////////////////////////////////////

void GuiPanelSelection::on_buttonBoundaryVertices_clicked() {
  _select(VERTICES,BOUNDARY);
}

void GuiPanelSelection::on_buttonBoundaryEdges_clicked() {
  _select(EDGES,BOUNDARY);
}

void GuiPanelSelection::on_buttonBoundaryFaces_clicked() {
  _select(FACES,BOUNDARY);
}

//////////////////////////////////////////////////////////////////////

void GuiPanelSelection::on_buttonRegularVertices_clicked() {
  _select(VERTICES,REGULAR);
}

void GuiPanelSelection::on_buttonRegularEdges_clicked() {
  _select(EDGES,REGULAR);
}

void GuiPanelSelection::on_buttonRegularFaces_clicked() {
  _select(FACES,REGULAR);
}

//////////////////////////////////////////////////////////////////////

void GuiPanelSelection::on_buttonSingularVertices_clicked() {
  _select(VERTICES,SINGULAR);
}

void GuiPanelSelection::on_buttonSingularEdges_clicked() {
  _select(EDGES,SINGULAR);
}

void GuiPanelSelection::on_buttonSingularFaces_clicked() {
  _select(FACES,SINGULAR);
}

//////////////////////////////////////////////////////////////////////

void GuiPanelSelection::on_buttonFromVerticesToEdges_clicked() {
  _fromTo(VERTICES,EDGES);
}

void GuiPanelSelection::on_buttonFromVerticesToFaces_clicked() {
  _fromTo(VERTICES,FACES);
}

void GuiPanelSelection::on_buttonFromEdgesToVertices_clicked() {
  _fromTo(EDGES,VERTICES);
}

void GuiPanelSelection::on_buttonFromEdgesToFaces_clicked() {
  _fromTo(EDGES,FACES);
}

void GuiPanelSelection::on_buttonFromFacesToVertices_clicked() {
  _fromTo(FACES,VERTICES);
}

void GuiPanelSelection::on_buttonFromFacesToEdges_clicked() {
  _fromTo(FACES,EDGES);
}

//////////////////////////////////////////////////////////////////////

void GuiPanelSelection::on_buttonConnectedVertices_clicked() {
  // TODO Sun Mar  5 23:04:47 2023
}

void GuiPanelSelection::on_buttonConnectedEdges_clicked() {
  // TODO Sun Mar  5 23:04:47 2023
}

void GuiPanelSelection::on_buttonConnectedFaces_clicked() {
  // TODO Sun Mar  5 23:04:47 2023
}

//////////////////////////////////////////////////////////////////////

void GuiPanelSelection::on_buttonShapeVertices_clicked() {
  // TODO Sun Mar  5 23:04:47 2023
}

void GuiPanelSelection::on_buttonShapeEdges_clicked() {
  // TODO Sun Mar  5 23:04:47 2023
}

void GuiPanelSelection::on_buttonShapeFaces_clicked() {
  // TODO Sun Mar  5 23:04:47 2023
}

//////////////////////////////////////////////////////////////////////

void GuiPanelSelection::on_buttonDeleteVertices_clicked() {
  // TODO Sun Mar  5 23:04:47 2023
}

void GuiPanelSelection::on_buttonDeleteEdges_clicked() {
  // TODO Sun Mar  5 23:04:47 2023
}

void GuiPanelSelection::on_buttonDeleteFaces_clicked() {
  // TODO Sun Mar  5 23:04:47 2023
}

//////////////////////////////////////////////////////////////////////

void GuiPanelSelection::on_buttonCutEdges_clicked() {
  // TODO Sun Mar  5 23:04:47 2023
}

void GuiPanelSelection::on_buttonCutFaces_clicked() {
  // TODO Sun Mar  5 23:04:47 2023
}
