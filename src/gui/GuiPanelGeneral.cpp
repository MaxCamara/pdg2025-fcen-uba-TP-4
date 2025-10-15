//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-10-14 17:22:03 taubin>
//------------------------------------------------------------------------
//
// GuiPanelGeneral.cpp
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
#include "GuiPanelGeneral.hpp"

#include "wrl/SceneGraphProcessor.hpp"
// #include "wrl/SceneGraphTraversal.hpp"
// #include "core/PolygonMesh.hpp"

//////////////////////////////////////////////////////////////////////

// static variable evaluated at start-up time
bool GuiPanelGeneral::_registered = GuiPanelGeneral::registerPanel();

// static method run once at start-up time
bool GuiPanelGeneral::registerPanel() {
  return
    GuiMainWindow::registerPanelFactory
    ("General",
     [](QWidget *parent)->GuiPanel*{ return new GuiPanelGeneral(parent);});
}

//////////////////////////////////////////////////////////////////////
GuiPanelGeneral::GuiPanelGeneral(QWidget* parent):
  GuiPanel(parent) {
  setupUi(this);
}

//////////////////////////////////////////////////////////////////////
GuiPanelGeneral::~GuiPanelGeneral() {
}

//////////////////////////////////////////////////////////////////////
void GuiPanelGeneral::updateState() {

  // std::cout << "GuiPanelGeneral::updateState() {\n";

  auto mainWindow = getApp()->getMainWindow();
  if(mainWindow) {
    
    edit3DCanvasWidth->setText
      ("  "+QString::number(mainWindow->getGLWidgetWidth()));
    edit3DCanvasHeight->setText
      ("  "+QString::number(mainWindow->getGLWidgetHeight()));

    GuiViewerData& data  = mainWindow->getData();

    int   bboxDepth = data.getBBoxDepth();
    bool  bboxCube  = data.getBBoxCube();
    float bboxScale = data.getBBoxScale();

    SceneGraph* pWrl = data.getSceneGraph();
    if(pWrl!=(SceneGraph*)0) {
      Vec3f& bboxCenter = pWrl->getBBoxCenter();
      editBBoxCenterX->setText("  "+QString::number(bboxCenter.x,'f',4));
      editBBoxCenterY->setText("  "+QString::number(bboxCenter.y,'f',4));
      editBBoxCenterZ->setText("  "+QString::number(bboxCenter.z,'f',4));
      Vec3f& bboxSide   = pWrl->getBBoxSize();
      editBBoxSideX->setText("  "+QString::number(bboxSide.x,'f',4));
      editBBoxSideY->setText("  "+QString::number(bboxSide.y,'f',4));
      editBBoxSideZ->setText("  "+QString::number(bboxSide.z,'f',4));
    } else {
      editBBoxCenterX->setText("  "+QString::number(0.0f,'f',2));
      editBBoxCenterY->setText("  "+QString::number(0.0f,'f',2));
      editBBoxCenterZ->setText("  "+QString::number(0.0f,'f',2));
      editBBoxSideX->setText("  "+QString::number(0.0f,'f',2));
      editBBoxSideY->setText("  "+QString::number(0.0f,'f',2));
      editBBoxSideZ->setText("  "+QString::number(0.0f,'f',2));
    }

    spinBoxHexGridDepth->setValue(bboxDepth);
    checkBoxHexGridCube->setChecked(bboxCube);
    editHexGridScale->setText("  "+QString::number(bboxScale,'f',2));

    SceneGraph* wrl = data.getSceneGraph();
    if(wrl==(SceneGraph*)0) {

      pushButtonHexGridAdd->setEnabled(false);
      pushButtonHexGridRemove->setEnabled(false);
      pushButtonNormalsNone->setEnabled(false);
      pushButtonNormalsPerVertex->setEnabled(false);
      pushButtonNormalsPerFace->setEnabled(false);
      pushButtonNormalsPerCorner->setEnabled(false);
      pushButtonNormalsInvert->setEnabled(false);
      pushButtonSceneGraphEdgesAdd->setEnabled(false);
      pushButtonSceneGraphEdgesRemove->setEnabled(false);
      pushButtonSceneGraphEdgesShow->setEnabled(false);
      pushButtonSceneGraphEdgesHide->setEnabled(false);
      pushButtonSceneGraphIndexedFaceSetsShow->setEnabled(false);
      pushButtonSceneGraphIndexedFaceSetsHide->setEnabled(false);
      pushButtonSceneGraphIndexedLineSetsShow->setEnabled(false);
      pushButtonSceneGraphIndexedLineSetsHide->setEnabled(false);
      editSceneShapeNodes->setText("0");
      editPointsStatus->setText("SCENE GRAPH IS EMPTY");
      pushButtonPointsRemove->setEnabled(false);
      pushButtonPointsShow->setEnabled(false);
      pushButtonPointsHide->setEnabled(false);
      editPointsStatus->setText("SCENE GRAPH IS EMPTY");
      pushButtonSurfaceRemove->setEnabled(false);
      pushButtonSurfaceShow->setEnabled(false);
      pushButtonSurfaceHide->setEnabled(false);

    } else /* if(wrl!=(SceneGraph*)0) */ {

      SceneGraphProcessor processor(*wrl);

      bool hasGrid   = processor.hasGrid();
      pushButtonHexGridAdd->setEnabled(!hasGrid);
      pushButtonHexGridRemove->setEnabled(hasGrid);

      bool hasNormal = false;
      bool hasFaces  = processor.hasIndexedFaceSetFaces();
      bool value     = processor.hasIndexedFaceSetNormalNone();
      pushButtonNormalsNone->setEnabled(!value);
      value = processor.hasIndexedFaceSetNormalPerVertex();
      hasNormal |= value;
      pushButtonNormalsPerVertex->setEnabled(hasFaces && !value);
      value = processor.hasIndexedFaceSetNormalPerFace();
      hasNormal |= value;
      pushButtonNormalsPerFace->setEnabled(hasFaces && !value);
      value = processor.hasIndexedFaceSetNormalPerCorner();
      hasNormal |= value;
      pushButtonNormalsPerCorner->setEnabled(hasFaces && !value);
      pushButtonNormalsInvert->setEnabled(hasNormal);

      value = processor.hasIndexedFaceSetShown();
      pushButtonSceneGraphIndexedFaceSetsHide->setEnabled(value);
      value = processor.hasIndexedFaceSetHidden();
      pushButtonSceneGraphIndexedFaceSetsShow->setEnabled(value);

      value = processor.hasIndexedLineSetShown();
      pushButtonSceneGraphIndexedLineSetsHide->setEnabled(value);
      value = processor.hasIndexedLineSetHidden();
      pushButtonSceneGraphIndexedLineSetsShow->setEnabled(value);

      editSceneShapeNodes->setText
        (QString("%1").arg(processor.numberOfShapeNodes()));

      Node* points = wrl->find("POINTS"); // should be a Shape node
      bool  hasPoints = (points!=(Node*)0 && points->isShape());
           
      // std::cout << "  hasPoints = "<< ((hasPoints)?"true":"false") <<"\n";

      if(hasPoints) {
        editPointsStatus->setText("SCENE GRAPH HAS A \"POINTS\" NODE");

        pushButtonPointsRemove->setEnabled(true);
        bool show = points->getShow();
        pushButtonPointsShow->setEnabled(!show);
        pushButtonPointsHide->setEnabled(show);

        Shape* shape = (Shape*)points;
        bool hasPointNormals = false;
        bool hasPointColors  = false;
        Node* node = shape->getGeometry();
        if(node!=(Node*)0 && node->isIndexedFaceSet()) {
          IndexedFaceSet* ifs = (IndexedFaceSet*)node;
          hasPointNormals = (ifs->getNormalBinding()==IndexedFaceSet::PB_PER_VERTEX);
          hasPointColors  = (ifs->getColorBinding() ==IndexedFaceSet::PB_PER_VERTEX);

          // std::cout << "  hasPointsNormals = "
          //           << ((hasPointNormals)?"true":"false") <<"\n";
          // std::cout << "  hasPointsColors  = "
          //           << ((hasPointColors)?"true":"false") <<"\n";
          //
          // ifs->printInfo("  ");
        }

        checkBoxPointsHasNormals->setChecked(hasPointNormals);
        checkBoxPointsHasColors->setChecked(hasPointColors);
      } else {
        editPointsStatus->setText("NO \"POINTS\" NODE FOUND IN SCENE GRAPH");
        pushButtonPointsRemove->setEnabled(false);
        pushButtonPointsShow->setEnabled(false);
        pushButtonPointsHide->setEnabled(false);
      }

      Node* edges = wrl->find("EDGES"); // should be a Shape node
      bool  hasEdges = (edges!=(Node*)0 && edges->isShape());
      if(hasEdges) {
        pushButtonSceneGraphEdgesAdd->setEnabled(false);
        pushButtonSceneGraphEdgesRemove->setEnabled(true);
        bool show = edges->getShow();
        pushButtonSceneGraphEdgesShow->setEnabled(!show);
        pushButtonSceneGraphEdgesHide->setEnabled(show);

      } else {
        pushButtonSceneGraphEdgesAdd->setEnabled(true);
        pushButtonSceneGraphEdgesRemove->setEnabled(false);
        pushButtonSceneGraphEdgesShow->setEnabled(false);
        pushButtonSceneGraphEdgesHide->setEnabled(false);
      }

      Node* surface    = wrl->find("SURFACE"); // should be a Shape node
      bool  hasSurface = (surface!=(Node*)0 && surface->isShape());
      if(hasSurface) {
        editSurfaceStatus->setText("SCENE GRAPH HAS A \"SURFACE\" NODE");
        pushButtonSurfaceRemove->setEnabled(true);
        bool show = surface->getShow();
        pushButtonSurfaceShow->setEnabled(!show);;
        pushButtonSurfaceHide->setEnabled(show);;
      } else {
        editSurfaceStatus->setText("NO \"SURFACE\" NODE FOUND IN SCENE GRAPH");
        pushButtonSurfaceRemove->setEnabled(false);
        pushButtonSurfaceShow->setEnabled(false);;
        pushButtonSurfaceHide->setEnabled(false);;
      }
    
      // get number of points
      int nPts = 0;
      if(hasPoints) {
        Shape* shape = (Shape*)points;
        Node* node = shape->getGeometry();
        if(node!=0 && node->isIndexedFaceSet()) {
          IndexedFaceSet* ifs = (IndexedFaceSet*)node;
          nPts = ifs->getNumberOfCoord();
        }
      }
      editPointsNumber->setText("  "+QString::number(nPts));

      // get number of surface vertices and faces
      int nVertices = 0;
      int nFaces = 0;
      if(hasSurface) {
        Shape* shape = (Shape*)surface;
        Node* node = shape->getGeometry();
        if(node!=0 && node->isIndexedFaceSet()) {
          IndexedFaceSet* ifs = (IndexedFaceSet*)node;
          nVertices = ifs->getNumberOfCoord();
          nFaces = ifs->getNumberOfFaces();
        }
      }
      editSurfaceVertices->setText("  "+QString::number(nVertices));
      editSurfaceFaces->setText("  "+QString::number(nFaces));

    }
  }
  // std::cout << "}\n";
}

//////////////////////////////////////////////////////////////////////
void GuiPanelGeneral::mousePressEvent(QMouseEvent * event) {

  int x = event->position().x();
  int y = event->position().y();

  bool clickedOnLabel = false;

  if(label3DCanvasSize->geometry().contains(x,y,true)) {
    panel3DCanvasSize->setVisible(panel3DCanvasSize->isHidden());
    clickedOnLabel = true;
  } else if(labelBBox->geometry().contains(x,y,true)) {
    panelBBox->setVisible(panelBBox->isHidden());
    clickedOnLabel = true;
  } else if(labelHexGrid->geometry().contains(x,y,true)) {
    panelHexGrid->setVisible(panelHexGrid->isHidden());
    clickedOnLabel = true;
  } else if(labelSceneGraph->geometry().contains(x,y,true)) {
    panelSceneGraph->setVisible(panelSceneGraph->isHidden());
    clickedOnLabel = true;
  } else if(labelNormals->geometry().contains(x,y,true)) {
    panelNormals->setVisible(panelNormals->isHidden());
    clickedOnLabel = true;
  } else if(labelPoints->geometry().contains(x,y,true)) {
    panelPoints->setVisible(panelPoints->isHidden());
    clickedOnLabel = true;
  } else if(labelEdges->geometry().contains(x,y,true)) {
    panelEdges->setVisible(panelEdges->isHidden());
    clickedOnLabel = true;
  } else if(labelSurface->geometry().contains(x,y,true)) {
    panelSurface->setVisible(panelSurface->isHidden());
    clickedOnLabel = true;
  }

  if(clickedOnLabel) updateState();
}

void GuiPanelGeneral::on_spinBoxHexGridDepth_valueChanged(int depth) {
  auto mainWindow = getApp()->getMainWindow();
  GuiViewerData& data = mainWindow->getData();
  int newDepth = (depth<0)?0:(depth>10)?10:depth;
  int oldDepth = data.getBBoxDepth();
  if(newDepth!=oldDepth) {
    data.setBBoxDepth(depth);
    // is scene graph has a BOUNDING-BOX node => rebuild it
    SceneGraph* pWrl = data.getSceneGraph();
    SceneGraphProcessor processor(*pWrl);
    if(processor.hasGrid()) {
      float scale = data.getBBoxScale();
      bool  cube  = data.getBBoxCube();
      processor.gridAdd(newDepth,scale,cube);
      mainWindow->setSceneGraph(pWrl,false);
      mainWindow->refresh();
      updateState();
    }
  }
}

void GuiPanelGeneral::hexGridDepthUp() {
  auto mainWindow = getApp()->getMainWindow();
  GuiViewerData& data = mainWindow->getData();
  on_spinBoxHexGridDepth_valueChanged(data.getBBoxDepth()+1);
}

void GuiPanelGeneral::hexGridDepthDown() {
  auto mainWindow = getApp()->getMainWindow();
  GuiViewerData& data = mainWindow->getData();
  on_spinBoxHexGridDepth_valueChanged(data.getBBoxDepth()-1);
}

void GuiPanelGeneral::on_pushButtonHexGridAdd_clicked() {
  auto mainWindow = getApp()->getMainWindow();
  GuiViewerData& data = mainWindow->getData();
  int depth = data.getBBoxDepth();
  SceneGraph* pWrl = data.getSceneGraph();
  SceneGraphProcessor processor(*pWrl);
  data.setBBoxDepth(depth);
  float scale = data.getBBoxScale();
  bool  cube  = data.getBBoxCube();
  processor.gridAdd(depth,scale,cube);
  mainWindow->setSceneGraph(pWrl,false);
  mainWindow->refresh();
  updateState();
}

void GuiPanelGeneral::on_pushButtonHexGridRemove_clicked() {
  auto mainWindow = getApp()->getMainWindow();
  GuiViewerData& data = mainWindow->getData();
  SceneGraph* pWrl = data.getSceneGraph();
  if(pWrl!=(SceneGraph*)0) {
    SceneGraphProcessor processor(*pWrl);
    processor.gridRemove();
    mainWindow->setSceneGraph(pWrl,false);
    mainWindow->refresh();
    updateState();
  }
}

void GuiPanelGeneral::on_editHexGridScale_returnPressed() {
  auto mainWindow = getApp()->getMainWindow();
  GuiViewerData& data = mainWindow->getData();
  float scale = data.getBBoxScale();
  float scaleMin = data.getBBoxScaleMin();
  QString str = editHexGridScale->text(); // "1.05"; 
  float value = str.toFloat();
  if(value<scaleMin) value=scaleMin;
  if(value!=scale) {
    data.setBBoxScale(value);
    SceneGraphProcessor processor(*(data.getSceneGraph()));
    if(processor.hasGrid()) {
      int   depth = data.getBBoxDepth();
      float scale = data.getBBoxScale();
      bool  cube  = data.getBBoxCube();
      processor.gridAdd(depth,scale,cube);
      mainWindow->setSceneGraph(data.getSceneGraph(),false);
      mainWindow->refresh();
      updateState();
    }
  }
}

void GuiPanelGeneral::on_checkBoxHexGridCube_stateChanged(int state) {
  auto mainWindow = getApp()->getMainWindow();
  GuiViewerData& data = mainWindow->getData();
  data.setBBoxCube((state!=0));
  SceneGraphProcessor processor(*(data.getSceneGraph()));
  if(processor.hasGrid()) {
    int   depth = data.getBBoxDepth();
    float scale = data.getBBoxScale();
    bool  cube  = data.getBBoxCube();
    processor.gridAdd(depth,scale,cube);
    mainWindow->setSceneGraph(data.getSceneGraph(),false);
    mainWindow->refresh();
    updateState();
  }
}

void GuiPanelGeneral::on_pushButtonNormalsInvert_clicked() {
  auto mainWindow = getApp()->getMainWindow();
  GuiViewerData& data = mainWindow->getData();
  SceneGraph*    pWrl = data.getSceneGraph();
  if(pWrl!=(SceneGraph*)0) {
    SceneGraphProcessor processor(*pWrl);
    processor.normalInvert();
    mainWindow->setSceneGraph(pWrl,false);
    mainWindow->refresh();
    updateState();
  }
}

void GuiPanelGeneral::on_pushButtonNormalsNone_clicked() {
  auto mainWindow = getApp()->getMainWindow();
  GuiViewerData& data = mainWindow->getData();
  SceneGraph*    pWrl = data.getSceneGraph();
  if(pWrl!=(SceneGraph*)0) {
    SceneGraphProcessor processor(*pWrl);
    processor.normalClear();
    mainWindow->setSceneGraph(pWrl,false);
    mainWindow->refresh();
    updateState();
  }
}

void GuiPanelGeneral::on_pushButtonNormalsPerVertex_clicked() {
  auto mainWindow = getApp()->getMainWindow();
  GuiViewerData& data = mainWindow->getData();
  SceneGraph*    pWrl = data.getSceneGraph();
  if(pWrl!=(SceneGraph*)0) {
    SceneGraphProcessor processor(*pWrl);
    processor.computeNormalPerVertex();
    mainWindow->setSceneGraph(pWrl,false);
    mainWindow->refresh();
    updateState();
  }
}

void GuiPanelGeneral::on_pushButtonNormalsPerFace_clicked() {
  auto mainWindow = getApp()->getMainWindow();
  GuiViewerData& data = mainWindow->getData();
  SceneGraph*    pWrl = data.getSceneGraph();
  if(pWrl!=(SceneGraph*)0) {
    SceneGraphProcessor processor(*pWrl);
    processor.computeNormalPerFace();
    mainWindow->setSceneGraph(pWrl,false);
    mainWindow->refresh();
    updateState();
  }
}

void GuiPanelGeneral::on_pushButtonNormalsPerCorner_clicked() {
  auto mainWindow = getApp()->getMainWindow();
  GuiViewerData& data = mainWindow->getData();
  SceneGraph*    pWrl = data.getSceneGraph();
  if(pWrl!=(SceneGraph*)0) {
    SceneGraphProcessor processor(*pWrl);
    processor.computeNormalPerCorner();
    mainWindow->setSceneGraph(pWrl,false);
    mainWindow->refresh();
    updateState();
  }
}

void GuiPanelGeneral::on_pushButtonPointsRemove_clicked() {
  auto mainWindow = getApp()->getMainWindow();
  GuiViewerData& data = mainWindow->getData();
  SceneGraph*    pWrl = data.getSceneGraph();
  if(pWrl!=(SceneGraph*)0) {
    SceneGraphProcessor processor(*pWrl);
    processor.pointsRemove();
    mainWindow->setSceneGraph(pWrl,false);
    mainWindow->refresh();
    updateState();
  }
}

void GuiPanelGeneral::on_pushButtonPointsShow_clicked() {
  auto mainWindow = getApp()->getMainWindow();
  GuiViewerData& data = mainWindow->getData();
  SceneGraph*    pWrl = data.getSceneGraph();
  if(pWrl==(SceneGraph*)0) return;
  Node* node = pWrl->find("POINTS");
  if(node==(Node*)0) return;
  node->setShow(true);
  mainWindow->setSceneGraph(pWrl,false);
  mainWindow->refresh();
  updateState();
}

void GuiPanelGeneral::on_pushButtonPointsHide_clicked() {
  auto mainWindow = getApp()->getMainWindow();
  GuiViewerData& data = mainWindow->getData();
  SceneGraph*    pWrl = data.getSceneGraph();
  if(pWrl==(SceneGraph*)0) return;
  Node* node = pWrl->find("POINTS");
  if(node==(Node*)0) return;
  node->setShow(false);
  mainWindow->setSceneGraph(pWrl,false);
  mainWindow->refresh();
  updateState();
}

void GuiPanelGeneral::on_pushButtonSceneGraphEdgesAdd_clicked() {
  auto mainWindow = getApp()->getMainWindow();
  GuiViewerData& data = mainWindow->getData();
  SceneGraph*    pWrl = data.getSceneGraph();
  if(pWrl!=(SceneGraph*)0) {
    SceneGraphProcessor processor(*pWrl);
    processor.edgesAdd();
    mainWindow->setSceneGraph(pWrl,false);
    mainWindow->refresh();
    updateState();
  }
}

void GuiPanelGeneral::on_pushButtonSceneGraphEdgesRemove_clicked() {
  auto mainWindow = getApp()->getMainWindow();
  GuiViewerData& data = mainWindow->getData();
  SceneGraph*    pWrl = data.getSceneGraph();
  if(pWrl!=(SceneGraph*)0) {
    SceneGraphProcessor processor(*pWrl);
    processor.edgesRemove();
    mainWindow->setSceneGraph(pWrl,false);
    mainWindow->refresh();
    updateState();
  }
}

void GuiPanelGeneral::on_pushButtonSceneGraphEdgesShow_clicked() {
  auto mainWindow = getApp()->getMainWindow();
  GuiViewerData& data = mainWindow->getData();
  SceneGraph*    pWrl = data.getSceneGraph();
  if(pWrl==(SceneGraph*)0) return;
  Node* node = pWrl->find("EDGES");
  if(node==(Node*)0) return;
  node->setShow(true);
  mainWindow->setSceneGraph(pWrl,false);
  mainWindow->refresh();
  updateState();
}

void GuiPanelGeneral::on_pushButtonSceneGraphEdgesHide_clicked() {
  auto mainWindow = getApp()->getMainWindow();
  GuiViewerData& data = mainWindow->getData();
  SceneGraph*    pWrl = data.getSceneGraph();
  if(pWrl==(SceneGraph*)0) return;
  Node* node = pWrl->find("EDGES");
  if(node==(Node*)0) return;
  node->setShow(false);
  mainWindow->setSceneGraph(pWrl,false);
  mainWindow->refresh();
  updateState();
}

void GuiPanelGeneral::on_pushButtonSceneGraphIndexedFaceSetsShow_clicked() {
  auto mainWindow = getApp()->getMainWindow();
  GuiViewerData& data = mainWindow->getData();
  SceneGraph*    pWrl = data.getSceneGraph();
  if(pWrl!=(SceneGraph*)0) {
    SceneGraphProcessor processor(*pWrl);
    processor.shapeIndexedFaceSetShow();
    mainWindow->setSceneGraph(pWrl,false);
    mainWindow->refresh();
    updateState();
  }
}

void GuiPanelGeneral::on_pushButtonSceneGraphIndexedFaceSetsHide_clicked() {
  auto mainWindow = getApp()->getMainWindow();
  GuiViewerData& data = mainWindow->getData();
  SceneGraph*    pWrl = data.getSceneGraph();
  if(pWrl!=(SceneGraph*)0) {
    SceneGraphProcessor processor(*pWrl);
    processor.shapeIndexedFaceSetHide();
    mainWindow->setSceneGraph(pWrl,false);
    mainWindow->refresh();
    updateState();
  }
}

void GuiPanelGeneral::on_pushButtonSceneGraphIndexedLineSetsShow_clicked() {
  auto mainWindow = getApp()->getMainWindow();
  GuiViewerData& data = mainWindow->getData();
  SceneGraph*    pWrl = data.getSceneGraph();
  if(pWrl!=(SceneGraph*)0) {
    SceneGraphProcessor processor(*pWrl);
    processor.shapeIndexedLineSetShow();
    mainWindow->setSceneGraph(pWrl,false);
    mainWindow->refresh();
    updateState();
  }
}

void GuiPanelGeneral::on_pushButtonSceneGraphIndexedLineSetsHide_clicked() {
  auto mainWindow = getApp()->getMainWindow();
  GuiViewerData& data = mainWindow->getData();
  SceneGraph*    pWrl = data.getSceneGraph();
  if(pWrl!=(SceneGraph*)0) {
    SceneGraphProcessor processor(*pWrl);
    processor.shapeIndexedLineSetHide();
    mainWindow->setSceneGraph(pWrl,false);
    mainWindow->refresh();
    updateState();
  }
}

void GuiPanelGeneral::on_pushButtonSurfaceRemove_clicked() {
  auto mainWindow = getApp()->getMainWindow();
  GuiViewerData& data = mainWindow->getData();
  SceneGraph*    pWrl = data.getSceneGraph();
  if(pWrl!=(SceneGraph*)0) {
    SceneGraphProcessor processor(*pWrl);
    processor.surfaceRemove();
    mainWindow->setSceneGraph(pWrl,false);
    mainWindow->refresh();
    updateState();
  }
}

void GuiPanelGeneral::on_pushButtonSurfaceShow_clicked() {
  auto mainWindow = getApp()->getMainWindow();
  GuiViewerData& data = mainWindow->getData();
  SceneGraph*    pWrl = data.getSceneGraph();
  if(pWrl==(SceneGraph*)0) return;
  Node* node = pWrl->find("SURFACE");
  if(node==(Node*)0) return;
  node->setShow(true);
  mainWindow->setSceneGraph(pWrl,false);
  mainWindow->refresh();
  updateState();
}

void GuiPanelGeneral::on_pushButtonSurfaceHide_clicked() {
  auto mainWindow = getApp()->getMainWindow();
  GuiViewerData& data = mainWindow->getData();
  SceneGraph*    pWrl = data.getSceneGraph();
  if(pWrl==(SceneGraph*)0) return;
  Node* node = pWrl->find("SURFACE");
  if(node==(Node*)0) return;
  node->setShow(false);
  mainWindow->setSceneGraph(pWrl,false);
  mainWindow->refresh();
  updateState();
}
