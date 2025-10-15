//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-10-14 17:22:03 taubin>
//------------------------------------------------------------------------
//
// GuiPanelPolygonMesh.cpp
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
#include "GuiPanelPolygonMesh.hpp"

// #include "wrl/SceneGraphProcessor.hpp"
#include "wrl/SceneGraphTraversal.hpp"
#include "wrl/IndexedFaceSetVariables.hpp"
#include "core/PolygonMesh.hpp"

//////////////////////////////////////////////////////////////////////

// static variable evaluated at start-up time
bool GuiPanelPolygonMesh::_registered = GuiPanelPolygonMesh::registerPanel();

// static method run once at start-up time
bool GuiPanelPolygonMesh::registerPanel() {
  return
    GuiMainWindow::registerPanelFactory
    ("Polygon Mesh",
     [](QWidget *parent)->GuiPanel*{ return new GuiPanelPolygonMesh(parent);});
}

//////////////////////////////////////////////////////////////////////
GuiPanelPolygonMesh::GuiPanelPolygonMesh(QWidget* parent):
  GuiPanel(parent) {
  setupUi(this);
}

//////////////////////////////////////////////////////////////////////
GuiPanelPolygonMesh::~GuiPanelPolygonMesh() {
}

// Panel Widgets
//
// labelShape
// panelShape
//   comboBoxShapeSelect
//   pushButtonShapeSetSurface
//   pushButtonShapeSetPoints
//
// labelTest
// panelTest
//   editTestVertices
//   editTestEdges
//   editTestFaces
//   editTestCorners
//   editTestVerticesBoundary
//   editTestVerticesInternal
//   editTestVerticesRegular
//   editTestVerticesSingular
//   editTestEdgesBoundary
//   editTestEdgesRegular
//   editTestEdgesSingular
//   editTestEdgesOther
//   editTestGenus
//   checkBoxTestIsManifold
//   checkBoxTestHasBoundary
//   pushButtonTestPolygonMesh
//
// labelConnectedComponents
// editConnectedComponents
//   pushButtonCCPrimal
//   editCCPrimal
//   pushButtonCCDual
//   editCCDual
//
// labelOrientation
// editOrientation
//   pushButtonIsOriented
//   editIsOriented
//   pushButtonIsOrientable
//   editIsOrientable
//   pushButtonIsOrient
//   editIsOrient
//
// labelManifold
// editManifold
//   pushButtonRemoveIsolatedVertices
//   editRemoveIsolatedVertices
//   pushButtonCutThroughSingularVertices
//   editCutThroughSingularVertices
//   pushButtonConvertToManifold
//   editConvertToManifold
//   pushButtonConvertToOrientedManifold
//   editConvertToOrientedManifold

//////////////////////////////////////////////////////////////////////
// private
void GuiPanelPolygonMesh::_updateStateNoSceneGraph() {

  // Shape
  comboBoxShapeSelect->clear();
  comboBoxShapeSelect->setEnabled(false);
  pushButtonShapeSetSurface->setEnabled(false);
  pushButtonShapeSetPoints->setEnabled(false);

  // Test
  editTestVertices->setText("???");
  editTestEdges->setText("???");
  editTestFaces->setText("???");
  editTestCorners->setText("???");
  editTestVerticesBoundary->setText("???");
  editTestVerticesInternal->setText("???");
  editTestVerticesRegular->setText("???");
  editTestVerticesSingular->setText("???");
  editTestEdgesBoundary->setText("???");
  editTestEdgesRegular->setText("???");
  editTestEdgesSingular->setText("???");
  editTestEdgesOther->setText("???");
  editTestGenus->setText("???");
  checkBoxTestIsManifold->setChecked(false);
  checkBoxTestHasBoundary->setChecked(false);
  pushButtonTestPolygonMesh->setEnabled(false);

  // Connected Components
  pushButtonCCPrimal->setEnabled(false);
  editCCPrimal->setText("???");
  pushButtonCCDual->setEnabled(false);
  editCCDual->setText("???");

  // Orientation
  pushButtonIsOriented->setEnabled(false);
  editIsOriented->setText("???");
  pushButtonIsOrientable->setEnabled(false);
  editIsOrientable->setText("???");
  pushButtonIsOriented->setEnabled(false);
  editIsOriented->setText("???");

  // Manifold
  pushButtonRemoveIsolatedVertices->setEnabled(false);
  editRemoveIsolatedVertices->setText("???");
  pushButtonCutThroughSingularVertices->setEnabled(false);
  editCutThroughSingularVertices->setText("???");
  pushButtonConvertToManifold->setEnabled(false);
  editConvertToManifold->setText("???");
}

//////////////////////////////////////////////////////////////////////
void GuiPanelPolygonMesh::_updateComboShape
(const bool onlyIndexedfedFaceSets) {
  comboBoxShapeSelect->clear();
  GuiViewerData& data = getApp()->getMainWindow()->getData();
  int iShape = 0;
  SceneGraph* wrl = data.getSceneGraph();
  SceneGraphTraversal sgt(*wrl);
  for(Node* node=nullptr;(node=sgt.next())!=nullptr;) {
    Shape* shape = dynamic_cast<Shape*>(node);
    if(shape==(Shape*)0) continue;
    if(onlyIndexedfedFaceSets) {
      node = shape->getGeometry();
      IndexedFaceSet* ifs = dynamic_cast<IndexedFaceSet*>(node);
      if(ifs==(IndexedFaceSet*)0) continue;
    }
    const string& name = shape->getName();
    comboBoxShapeSelect->addItem
      (QString("[%1] \"%2\"").arg(iShape).arg(name.c_str()));
    iShape++;
  }
  // disable if no Shape nodes found (iShape==0)
  comboBoxShapeSelect->setEnabled((iShape>0));
}

//////////////////////////////////////////////////////////////////////
void GuiPanelPolygonMesh::updateState() {

  // std::cout << "GuiPanelPolygonMesh::updateState() {\n";

  auto mainWindow = getApp()->getMainWindow();
  if(mainWindow) {

    GuiViewerData& data  = mainWindow->getData();
    
    SceneGraph* wrl = data.getSceneGraph();
    if(wrl==(SceneGraph*)0) {
      _updateStateNoSceneGraph();
    } else /* if(wrl!=(SceneGraph*)0) */ {
      _updateComboShape(false);
      on_pushButtonTestPolygonMesh_clicked();      

      pushButtonCCPrimal->setEnabled(true);
      pushButtonCCDual->setEnabled(true);

      pushButtonIsOriented->setEnabled(true);
      pushButtonIsOrientable->setEnabled(true);
      pushButtonOrient->setEnabled(true);

      pushButtonRemoveIsolatedVertices->setEnabled(true);
      pushButtonCutThroughSingularVertices->setEnabled(true);
    }

  }
  
  // std::cout << "}\n";
}

//////////////////////////////////////////////////////////////////////
void GuiPanelPolygonMesh::mousePressEvent(QMouseEvent * event) {

  int x = event->position().x();
  int y = event->position().y();

  bool clickedOnLabel = false;

  if(labelShape->geometry().contains(x,y,true)) {
    panelShape->setVisible(panelShape->isHidden());
    clickedOnLabel = true;
  } else if(labelTest->geometry().contains(x,y,true)) {
    panelTest->setVisible(panelTest->isHidden());
    clickedOnLabel = true;
  } else if(labelOrientation->geometry().contains(x,y,true)) {
    panelOrientation->setVisible(panelOrientation->isHidden());
    clickedOnLabel = true;
  } else if(labelConnectedComponents->geometry().contains(x,y,true)) {
    panelConnectedComponents->setVisible(panelConnectedComponents->isHidden());
    clickedOnLabel = true;
  } else if(labelManifold->geometry().contains(x,y,true)) {
    panelManifold->setVisible(panelManifold->isHidden());
    clickedOnLabel = true;
  }

  if(clickedOnLabel) updateState();
}

void GuiPanelPolygonMesh::on_checkBoxTestIsManifold_stateChanged(int state) {
  (void)state;
  on_pushButtonTestPolygonMesh_clicked();
}

void GuiPanelPolygonMesh::on_checkBoxTestHasBoundary_stateChanged(int state) {
  (void)state;
  on_pushButtonTestPolygonMesh_clicked();
}

int GuiPanelPolygonMesh::_getCurrentShapeIndex() {
  return comboBoxShapeSelect->currentIndex(); // returns -1 if empty
}

Shape* GuiPanelPolygonMesh::_getCurrentShape() {
  int currentIndex = comboBoxShapeSelect->currentIndex(); // returns -1 if empty
  if(currentIndex<0) return nullptr;
  Shape* currentShape = (Shape*)0;
  GuiViewerData& data = getApp()->getMainWindow()->getData();
  SceneGraph* pWrl = data.getSceneGraph();
  int iShape = 0;
  Node* node = (Node*)0;
  SceneGraphTraversal sgt(*pWrl);
  while((node=sgt.next())!=nullptr) {
    Shape* shape = dynamic_cast<Shape*>(node);
    if(shape==(Shape*)0) continue;
    if(iShape==currentIndex) {
      currentShape = shape;
      break;
    }
    iShape++;
  }
  return currentShape;
}

void GuiPanelPolygonMesh::on_pushButtonTestPolygonMesh_clicked() {
  if(comboBoxShapeSelect->count()==0) return;

  auto mainWindow = getApp()->getMainWindow();
  GuiViewerData& data = mainWindow->getData();

  SceneGraph* pWrl = data.getSceneGraph();
  if(pWrl==(SceneGraph*)0) return;

  Shape* currentShape = _getCurrentShape();

  PolygonMesh* pm = (PolygonMesh*)0;
  {
    if(currentShape!=(Shape*)0) {
      Node* node = currentShape->getGeometry();
      IndexedFaceSet* ifs = dynamic_cast<IndexedFaceSet*>(node);
      if(ifs!=(IndexedFaceSet*)0) {
        Variable* var = ifs->getVariable("PolygonMesh");
        if(var==nullptr) {
          var = new VariablePolygonMesh
            (ifs->getNumberOfVertices(),ifs->getCoordIndex());
          ifs->setVariable(var);
        }
        pm = (PolygonMesh*)(var->getValue());
      }
    }
  }
  
  if(pm!=(PolygonMesh*)0) {

    int nV = pm->getNumberOfVertices();
    int nE = pm->getNumberOfEdges();
    int nF = pm->getNumberOfFaces();
    int nC = pm->getNumberOfCorners();

    if(currentShape->getName()=="SURFACE")
      pushButtonShapeSetSurface->setEnabled(false);
    else if(nF>0)
      pushButtonShapeSetSurface->setEnabled(true);

    if(currentShape->getName()=="POINTS" || nF>0)
      pushButtonShapeSetPoints->setEnabled(false);
    else
      pushButtonShapeSetPoints->setEnabled(true);

    editTestVertices->setText(QString("%1").arg(nV));
    editTestEdges->setText(QString("%1").arg(nE));
    editTestFaces->setText(QString("%1").arg(nF));
    editTestCorners->setText(QString("%1").arg(nC));

    int nE_boundary  = 0;
    int nE_regular   = 0;
    int nE_singular  = 0;
    int nE_other     = 0;
    for(int iE=0;iE<nE;iE++) {
      if(pm->isBoundaryEdge(iE)) {
        nE_boundary++;
      } else if(pm->isRegularEdge(iE)) {
        nE_regular++;
      } else if(pm->isSingularEdge(iE)) {
        nE_singular++;
      } else {
        nE_other++;
      }
    }
    
    int nV_boundary  = 0;
    int nV_singular  = 0;
    for(int iV=0;iV<nV;iV++) {
      if(pm->isBoundaryVertex(iV))
        nV_boundary++;
      if(pm->isSingularVertex(iV))
        nV_singular++;
    }
    int nV_internal = nV-nV_boundary;
    int nV_regular  = nV-nV_singular;

    editTestVerticesBoundary->setText(QString("%1").arg(nV_boundary));
    editTestVerticesInternal->setText(QString("%1").arg(nV_internal));
    editTestVerticesRegular->setText(QString("%1").arg(nV_regular));
    editTestVerticesSingular->setText(QString("%1").arg(nV_singular));
    editTestEdgesBoundary->setText(QString("%1").arg(nE_boundary));
    editTestEdgesRegular->setText(QString("%1").arg(nE_regular));
    editTestEdgesSingular->setText(QString("%1").arg(nE_singular));
    editTestEdgesOther->setText(QString("%1").arg(nE_other));

    bool is_regular = pm->isRegular();
    checkBoxTestIsManifold->setChecked(is_regular);
    bool has_boundary = pm->hasBoundary();
    checkBoxTestHasBoundary->setChecked(has_boundary);

    if(nV_boundary==0 && is_regular) {
      // nV-nE+nF == 2*(1-G)
      int G = 1 -(nV-nE+nF)/2;
      editTestGenus->setText(QString("%1").arg(G));
    } else {
      editTestGenus->setText("UNDEFINED");
    }
    
  } else {

    pushButtonShapeSetSurface->setEnabled(false);
    pushButtonShapeSetPoints->setEnabled(false);
    
    editTestVertices->setText("???");
    editTestEdges->setText("???");
    editTestFaces->setText("???");
    editTestCorners->setText("???");
    editTestVerticesBoundary->setText("???");
    editTestVerticesInternal->setText("???");
    editTestVerticesRegular->setText("???");
    editTestVerticesSingular->setText("???");
    editTestEdgesBoundary->setText("???");
    editTestEdgesRegular->setText("???");
    editTestEdgesSingular->setText("???");
    editTestEdgesOther->setText("???");
    editTestGenus->setText("???");
    checkBoxTestIsManifold->setChecked(false);
    checkBoxTestHasBoundary->setChecked(false);

    // editCCPrimal
    // editCCDual
    // editIsOriented
    // editIsOrientable
    // editOrient
  }
}

void GuiPanelPolygonMesh::on_comboBoxShapeSelect_currentIndexChanged(int index) {
  (void)index;
  on_pushButtonTestPolygonMesh_clicked();
}

void GuiPanelPolygonMesh::on_pushButtonShapeSetSurface_clicked() {
  Shape* currentShape = _getCurrentShape();
  if(currentShape==nullptr || currentShape->getName()=="SURFACE") return;
  // TODO Sat Mar  4 19:06:48 2023
  // if scene graph has another Shape node named "SURFACE" ...
  currentShape->setName("SURFACE");
  updateState();
}

//////////////////////////////////////////////////////////////////////

void GuiPanelPolygonMesh::on_pushButtonShapeSetPoints_clicked() {
  Shape* currentShape = _getCurrentShape();
  if(currentShape==nullptr || currentShape->getName()=="POINTS") return;
  // TODO Sat Mar  4 19:06:48 2023
  // if scene graph has another Shape node named "POINTS" ...
  currentShape->setName("POINTS");
  updateState();
}

void GuiPanelPolygonMesh::_pushButtonCC(const bool primal) {
  Shape* shape = _getCurrentShape();
  if(shape==nullptr) return;
  Node* geometry = shape->getGeometry();
  IndexedFaceSet* ifs = dynamic_cast<IndexedFaceSet*>(geometry);
  if(ifs==nullptr) return;
  Variable* var = ifs->getVariable("PolygonMesh");
  if(var==nullptr) {
    int nV = ifs->getNumberOfVertices();
    vector<int>& coordIndex = ifs->getCoordIndex();
    var = new VariablePolygonMesh(nV,coordIndex);
    ifs->setVariable(var);
  }
  PolygonMesh* pm = (PolygonMesh*)(var->getValue());
  IndexedFaceSetVariables ifsv(*ifs);
  vector<int>& faceLabel = ifsv.getFaceSelection();
  if(primal) {
    int nCC = pm->computeConnectedComponentsPrimal(faceLabel);
    editCCPrimal->setText(QString("%1").arg(nCC));
  } else /* if(dual) */ {
    int nCC = pm->computeConnectedComponentsDual(faceLabel);
    editCCDual->setText(QString("%1").arg(nCC));
  }
  getApp()->getMainWindow()->resetSceneGraph();
  // getApp()->getMainWindow()->refresh();
}

void GuiPanelPolygonMesh::on_pushButtonCCPrimal_clicked() {
  _pushButtonCC(true);
}

void GuiPanelPolygonMesh::on_pushButtonCCDual_clicked() {
  _pushButtonCC(false);
}

//////////////////////////////////////////////////////////////////////

void GuiPanelPolygonMesh::on_pushButtonIsOriented_clicked() {
  Shape* shape = _getCurrentShape();
  if(shape==nullptr) return;
  Node* geometry = shape->getGeometry();
  IndexedFaceSet* ifs = dynamic_cast<IndexedFaceSet*>(geometry);
  if(ifs==nullptr) return;
  Variable* var = ifs->getVariable("PolygonMesh");
  if(var==nullptr) {
    int nV = ifs->getNumberOfVertices();
    vector<int>& coordIndex = ifs->getCoordIndex();
    var = new VariablePolygonMesh(nV,coordIndex);
    ifs->setVariable(var);
  }
  PolygonMesh* pm = (PolygonMesh*)(var->getValue());
  if(pm->isOriented()) {
    editIsOriented->setText("TRUE");
  } else {
    editIsOriented->setText("FALSE");
  }
}

//////////////////////////////////////////////////////////////////////

void GuiPanelPolygonMesh::on_pushButtonIsOrientable_clicked() {
  Shape* shape = _getCurrentShape();
  if(shape==nullptr) return;
  Node* geometry = shape->getGeometry();
  IndexedFaceSet* ifs = dynamic_cast<IndexedFaceSet*>(geometry);
  if(ifs==nullptr) return;
  Variable* var = ifs->getVariable("PolygonMesh");
  if(var==nullptr) {
    int nV = ifs->getNumberOfVertices();
    vector<int>& coordIndex = ifs->getCoordIndex();
    var = new VariablePolygonMesh(nV,coordIndex);
    ifs->setVariable(var);
  }
  PolygonMesh* pm = (PolygonMesh*)(var->getValue());
  if(pm->isOrientable()) {
    editIsOrientable->setText("TRUE");
  } else {
    editIsOrientable->setText("FALSE");
  }
}

//////////////////////////////////////////////////////////////////////

void GuiPanelPolygonMesh::on_pushButtonOrient_clicked() {
  Shape* shape = _getCurrentShape();
  if(shape==nullptr) return;
  Node* geometry = shape->getGeometry();
  IndexedFaceSet* ifs = dynamic_cast<IndexedFaceSet*>(geometry);
  if(ifs==nullptr) return;
  Variable* var = ifs->getVariable("PolygonMesh");
  if(var==nullptr) {
    int nV = ifs->getNumberOfVertices();
    vector<int>& coordIndex = ifs->getCoordIndex();
    var = new VariablePolygonMesh(nV,coordIndex);
    ifs->setVariable(var);
  }
  PolygonMesh* pm = (PolygonMesh*)(var->getValue());
  vector<int> ccIndex;
  vector<bool> invert_face;
  int nCC = pm->orient(ccIndex,invert_face);
  editOrient->setText(QString("%1").arg(nCC));
  if(nCC==0) return;

  int iF,iC,iC0,iC1,iCinv,iTmp,j;
  int nF = pm->getNumberOfFaces();
  
  IndexedFaceSetVariables ifsv(*ifs);
  // use the face selection array to visualize the faces which have been inverted
  vector<int>& faceSelection = ifsv.getFaceSelection();
  for(iF=0;iF<nF;iF++)
    faceSelection[iF] = -1;

  // if the mesh is already oriented we don't need to do anything
  bool oriented = true;
  for(iF=0;oriented && iF<nF;iF++) {
    if(invert_face[iF])
      oriented = false;
  }
  if(oriented) return;

  // the mesh is orientable but not oriented; we need to invert one or more faces

  // delete the PolygonMesh because it is not longer valid
  ifs->eraseVariable("PolygonMesh"); pm = nullptr;
  ifs->eraseVariable("HalfEdges"); // just in case
  
  vector<int>& coordIndex    = ifs->getCoordIndex();

  // if the mesh has properties per corner
  // they have to be fixed as well
  vector<int>& colorIndex    = ifs->getColorIndex();
  vector<int>& normalIndex   = ifs->getNormalIndex();
  vector<int>& texCoordIndex = ifs->getTexCoordIndex();

  vector<float>* normalPtr = nullptr;
  IndexedFaceSet::Binding normalBinding = ifs->getNormalBinding();
  bool invertFaceNormals =
    (normalBinding==IndexedFaceSet::Binding::PB_PER_FACE);
  // TODO Sun Mar  5 17:13:33 2023
  // what if(normalBinding==IndexedFaceSet::Binding::PB_PER_FACE_INDEXED) ???
  if(invertFaceNormals)
    normalPtr = &(ifs->getNormal());

  // if the mesh has a corner selection array
  // it has to be fixed as well
  bool hasCornerSelection = ifsv.hasCornerSelection();
  vector<int>* cornerSelectionPtr = nullptr;
  if(hasCornerSelection) cornerSelectionPtr = &(ifsv.getCornerSelection());

  // invert faces of indexedFaceSet
  int nC = coordIndex.size();
  for(iF=iC0=iC1=0;iC1<nC;iC1++) {
    if(coordIndex[iC1]>=0) continue;
    if(invert_face[iF]) {
      faceSelection[iF] = 0;
      // invert face orientation
      for(iC=iC0;iC<iC1;iC++) {
        j = iC-iC0;
        iCinv = iC1-1-j;
        if(iC>=iCinv) break;
        // swap coordIndex[iC] and coordIndex[iCinv]
        iTmp = coordIndex[iC];
        coordIndex[iC] = coordIndex[iCinv];
        coordIndex[iCinv] = iTmp;
        // also swap all per corner property values
        if(ifs->hasColorPerCorner()) {
          iTmp = colorIndex[iC];
          colorIndex[iC] = colorIndex[iCinv];
          colorIndex[iCinv] = iTmp;
        }
        if(ifs->hasNormalPerCorner()) {
          iTmp = coordIndex[iC];
          normalIndex[iC] = normalIndex[iCinv];
          normalIndex[iCinv] = iTmp;
        }
        if(ifs->hasTexCoordPerCorner()) {
          iTmp = texCoordIndex[iC];
          texCoordIndex[iC] = texCoordIndex[iCinv];
          texCoordIndex[iCinv] = iTmp;
        }
        if(hasCornerSelection) {
          iTmp = (*cornerSelectionPtr)[iC];
          (*cornerSelectionPtr)[iC] = (*cornerSelectionPtr)[iCinv];
          (*cornerSelectionPtr)[iCinv] = iTmp;
        }
      }
      // invert face normal if necessary
      if(invertFaceNormals) {
        float nx = (*normalPtr)[3*iF  ]; (*normalPtr)[3*iF  ] = -nx;
        float ny = (*normalPtr)[3*iF+1]; (*normalPtr)[3*iF+1] = -ny;
        float nz = (*normalPtr)[3*iF+2]; (*normalPtr)[3*iF+2] = -nz;
      }
    }

    iC0 = iC1+1; iF++;
  }
  // reset 3D view
  getApp()->getMainWindow()->resetSceneGraph();
}

//////////////////////////////////////////////////////////////////////

void GuiPanelPolygonMesh::on_pushButtonRemoveIsolatedVertices_clicked() {
  Shape* shape = _getCurrentShape();
  if(shape==nullptr) return;
  Node* geometry = shape->getGeometry();
  IndexedFaceSet* ifs = dynamic_cast<IndexedFaceSet*>(geometry);
  if(ifs==nullptr) return;
  Variable* var = ifs->getVariable("PolygonMesh");
  if(var==nullptr) {
    int nV = ifs->getNumberOfVertices();
    vector<int>& coordIndex = ifs->getCoordIndex();
    var = new VariablePolygonMesh(nV,coordIndex);
    ifs->setVariable(var);
  }
  PolygonMesh* pm = (PolygonMesh*)(var->getValue());

  vector<int> coordMap;
  vector<int> coordIndexOut;
  if(pm->removeIsolatedVertices(coordMap,coordIndexOut)==false) {
    editRemoveIsolatedVertices->setText("0");
    return;
  }

  int nV = ifs->getNumberOfVertices();
  int nV_isolated = nV-coordMap.size();
  editRemoveIsolatedVertices->setText(QString("%1").arg(nV_isolated));
  
  vector<float>& coord    = ifs->getCoord();
  vector<float>& normal   = ifs->getNormal();
  vector<float>& color    = ifs->getColor();
  vector<float>& texCoord = ifs->getTexCoord();

  // create the output coord array
  vector<float>  coordOut;

  bool hasNpv  = ifs->hasNormalPerVertex();
  vector<float>  normalOut; // only used if(hasNpv)

  bool hasCpv  = ifs->hasColorPerVertex();
  vector<float>  colorOut; // only used if(hasCpv)

  bool hasTpv  = ifs->hasTexCoordPerVertex();
  vector<float>  texCoordOut; // only used if(hasTpv);

  // if the mesh has a vertex selection array, we need to transfer
  // selected vertices to output as well
  IndexedFaceSetVariables ifsv(*ifs);
  bool hasVsel = ifsv.hasVertexSelection();
  vector<int>* vertexSelPtr = nullptr;
  if(hasVsel) vertexSelPtr = &(ifsv.getVertexSelection());
  vector<int>    vertexSelOut; // only used if(hasVsel)

  int   iVout,iV,iS;
  float x,y,z,nx,ny,nz,r,g,b,u,v;

  int nVout = coordMap.size();
  for(iVout=0;iVout<nVout;iVout++) {
    iV = coordMap[iVout];
    x = coord[3*iV  ]; coordOut.push_back(x);
    y = coord[3*iV+1]; coordOut.push_back(y);
    z = coord[3*iV+2]; coordOut.push_back(z);
    if(hasNpv) {
      nx = normal[3*iV  ]; normalOut.push_back(nx);
      ny = normal[3*iV+1]; normalOut.push_back(ny);
      nz = normal[3*iV+2]; normalOut.push_back(nz);
    }
    if(hasCpv) {
      r = color[3*iV  ]; colorOut.push_back(r);
      g = color[3*iV+1]; colorOut.push_back(g);
      b = color[3*iV+2]; colorOut.push_back(b);
    }
    if(hasTpv) {
      u = texCoord[2*iV  ]; texCoordOut.push_back(u);
      v = texCoord[2*iV+1]; texCoordOut.push_back(v);
    }
    if(hasVsel) {
      iS = (*vertexSelPtr)[iV];
      vertexSelOut.push_back(iS);
    }
  }
  
  // delete the PolygonMesh because it is not longer valid
  ifs->eraseVariable("PolygonMesh"); pm = nullptr;
  ifs->eraseVariable("HalfEdges"); // just in case

  // swap ifs variables
  vector<int>& coordIndex = ifs->getCoordIndex();
  coord.swap(coordOut);
  coordIndex.swap(coordIndexOut);
  if(hasNpv) normal.swap(normalOut);
  if(hasCpv) color.swap(colorOut);
  if(hasTpv)  texCoord.swap(texCoordOut);
  if(hasVsel) (*vertexSelPtr).swap(vertexSelOut);
  
  // reset 3D view
  getApp()->getMainWindow()->resetSceneGraph();
}

//////////////////////////////////////////////////////////////////////

void GuiPanelPolygonMesh::on_pushButtonCutThroughSingularVertices_clicked() {
  Shape* shape = _getCurrentShape();
  if(shape==nullptr) return;
  Node* geometry = shape->getGeometry();
  IndexedFaceSet* ifs = dynamic_cast<IndexedFaceSet*>(geometry);
  if(ifs==nullptr) return;
  Variable* var = ifs->getVariable("PolygonMesh");
  if(var==nullptr) {
    int nV = ifs->getNumberOfVertices();
    vector<int>& coordIndex = ifs->getCoordIndex();
    var = new VariablePolygonMesh(nV,coordIndex);
    ifs->setVariable(var);
  }
  PolygonMesh* pm = (PolygonMesh*)(var->getValue());

  vector<int> vIndexMap;
  vector<int> coordIndexOut;
  pm->cutThroughSingularVertices(vIndexMap, coordIndexOut);
  if(vIndexMap.size()==0 || coordIndexOut.size()==0) {
    editCutThroughSingularVertices->setText("NO CHANGE");
  } else {
    int nV     = ifs->getNumberOfVertices();
    int nVout  = static_cast<int>(vIndexMap.size());
    
    editCutThroughSingularVertices->setText
      (QString("%1 -> %2").arg(nV).arg(nVout));
  
    vector<float>& coord    = ifs->getCoord();
    vector<float>& normal   = ifs->getNormal();
    vector<float>& color    = ifs->getColor();
    vector<float>& texCoord = ifs->getTexCoord();

    // create the output coord array
    vector<float>  coordOut;

    bool hasNpv  = ifs->hasNormalPerVertex();
    vector<float>  normalOut; // only used if(hasNpv)

    bool hasCpv  = ifs->hasColorPerVertex();
    vector<float>  colorOut; // only used if(hasCpv)

    bool hasTpv  = ifs->hasTexCoordPerVertex();
    vector<float>  texCoordOut; // only used if(hasTpv);

    // if the mesh has a vertex selection array, we need to transfer
    // selected vertices to output as well
    IndexedFaceSetVariables ifsv(*ifs);
    bool hasVsel = ifsv.hasVertexSelection();
    vector<int>* vertexSelPtr = nullptr;
    if(hasVsel) vertexSelPtr = &(ifsv.getVertexSelection());
    vector<int>    vertexSelOut; // only used if(hasVsel)

    int iVout,iV,iS;
    float x,y,z,nx,ny,nz,r,g,b,u,v;
    for(iVout=0;iVout<nVout;iVout++) {
      iV = vIndexMap[iVout];
      x = coord[3*iV  ]; coordOut.push_back(x);
      y = coord[3*iV+1]; coordOut.push_back(y);
      z = coord[3*iV+2]; coordOut.push_back(z);
      if(hasNpv) {
        nx = normal[3*iV  ]; normalOut.push_back(nx);
        ny = normal[3*iV+1]; normalOut.push_back(ny);
        nz = normal[3*iV+2]; normalOut.push_back(nz);
      }
      if(hasCpv) {
        r = color[3*iV  ]; colorOut.push_back(r);
        g = color[3*iV+1]; colorOut.push_back(g);
        b = color[3*iV+2]; colorOut.push_back(b);
      }
      if(hasNpv) {
        u = texCoord[2*iV  ]; texCoordOut.push_back(u);
        v = texCoord[2*iV+1]; texCoordOut.push_back(v);
      }
      if(hasVsel) {
        iS = (*vertexSelPtr)[iV];
        vertexSelOut.push_back(iS);
      }
    }
      
    // delete the PolygonMesh because it is not longer valid
    ifs->eraseVariable("PolygonMesh"); pm = nullptr;
    ifs->eraseVariable("HalfEdges"); // just in case

    // swap ifs variables
    vector<int>& coordIndex = ifs->getCoordIndex();
    coord.swap(coordOut);
    coordIndex.swap(coordIndexOut);
    if(hasNpv) normal.swap(normalOut);
    if(hasCpv) color.swap(colorOut);
    if(hasTpv)  texCoord.swap(texCoordOut);
    if(hasVsel) (*vertexSelPtr).swap(vertexSelOut);
    
    // reset 3D view
    getApp()->getMainWindow()->resetSceneGraph();
  }
}

//////////////////////////////////////////////////////////////////////

void GuiPanelPolygonMesh::on_pushButtonConvertToManifold_clicked() {

  Shape* shape = _getCurrentShape();
  if(shape==nullptr) return;
  Node* geometry = shape->getGeometry();
  IndexedFaceSet* ifs = dynamic_cast<IndexedFaceSet*>(geometry);
  if(ifs==nullptr) return;
  Variable* var = ifs->getVariable("PolygonMesh");
  if(var==nullptr) {
    int nV = ifs->getNumberOfVertices();
    vector<int>& coordIndex = ifs->getCoordIndex();
    var = new VariablePolygonMesh(nV,coordIndex);
    ifs->setVariable(var);
  }
  PolygonMesh* pm = (PolygonMesh*)(var->getValue());

  vector<int> vIndexMap;
  vector<int> coordIndexOut;
  pm->convertToManifold(vIndexMap,coordIndexOut);
  if(vIndexMap.size()==0 || coordIndexOut.size()==0) {
    editConvertToManifold->setText("NO CHANGE");
    return;
  }

  int nV     = ifs->getNumberOfVertices();
  int nVout  = static_cast<int>(vIndexMap.size());
  editConvertToManifold->setText
    (QString("%1 -> %2").arg(nV).arg(nVout));
  
  vector<float>& coord    = ifs->getCoord();
  vector<float>& normal   = ifs->getNormal();
  vector<float>& color    = ifs->getColor();
  vector<float>& texCoord = ifs->getTexCoord();

  // create the output coord array
  vector<float>  coordOut;

  bool hasNpv  = ifs->hasNormalPerVertex();
  vector<float>  normalOut; // only used if(hasNpv)

  bool hasCpv  = ifs->hasColorPerVertex();
  vector<float>  colorOut; // only used if(hasCpv)

  bool hasTpv  = ifs->hasTexCoordPerVertex();
  vector<float>  texCoordOut; // only used if(hasTpv);

  // if the mesh has a vertex selection array, we need to transfer
  // selected vertices to output as well
  IndexedFaceSetVariables ifsv(*ifs);
  bool hasVsel = ifsv.hasVertexSelection();
  vector<int>* vertexSelPtr = nullptr;
  if(hasVsel) vertexSelPtr = &(ifsv.getVertexSelection());
  vector<int>    vertexSelOut; // only used if(hasVsel)

  int   iVout,iV,iS;
  float x,y,z,nx,ny,nz,r,g,b,u,v;

  for(iVout=0;iVout<nVout;iVout++) {
    iV = vIndexMap[iVout];
    x = coord[3*iV  ]; coordOut.push_back(x);
    y = coord[3*iV+1]; coordOut.push_back(y);
    z = coord[3*iV+2]; coordOut.push_back(z);
    if(hasNpv) {
      nx = normal[3*iV  ]; normalOut.push_back(nx);
      ny = normal[3*iV+1]; normalOut.push_back(ny);
      nz = normal[3*iV+2]; normalOut.push_back(nz);
    }
    if(hasCpv) {
      r = color[3*iV  ]; colorOut.push_back(r);
      g = color[3*iV+1]; colorOut.push_back(g);
      b = color[3*iV+2]; colorOut.push_back(b);
    }
    if(hasTpv) {
      u = texCoord[2*iV  ]; texCoordOut.push_back(u);
      v = texCoord[2*iV+1]; texCoordOut.push_back(v);
    }
    if(hasVsel) {
      iS = (*vertexSelPtr)[iV];
      vertexSelOut.push_back(iS);
    }
  }
  
  // delete the PolygonMesh because it is not longer valid
  ifs->eraseVariable("PolygonMesh"); pm = nullptr;
  ifs->eraseVariable("HalfEdges"); // just in case

  // swap ifs variables
  vector<int>& coordIndex = ifs->getCoordIndex();
  coord.swap(coordOut);
  coordIndex.swap(coordIndexOut);
  if(hasNpv) normal.swap(normalOut);
  if(hasCpv) color.swap(colorOut);
  if(hasTpv)  texCoord.swap(texCoordOut);
  if(hasVsel) (*vertexSelPtr).swap(vertexSelOut);
  
  // reset 3D view
  getApp()->getMainWindow()->resetSceneGraph();
}


