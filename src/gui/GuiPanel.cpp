//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-08-06 09:57:08 taubin>
//------------------------------------------------------------------------
//
// GuiPanel.cpp
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

#include "GuiApplication.hpp"
#include "GuiPanel.hpp"

GuiPanel::GuiPanel(QWidget *parent):
  QWidget(parent) {
}

GuiPanel::~GuiPanel() { }

// void GuiPanel::updateState() { }

void GuiPanel::showEvent(QShowEvent * event) {
  (void)event;
  getApp()->getMainWindow()->refresh();
  updateState();
}


void GuiPanel::_showNamed(const string& name, int value) {
  auto mainWindow = getApp()->getMainWindow();
  GuiViewerData& data = mainWindow->getData();
  SceneGraph*    pWrl = data.getSceneGraph();
  if(pWrl==(SceneGraph*)0) return;
  Node* node = pWrl->find(name);
  if(node==(Node*)0) return;
  node->setShow((value!=0));
  mainWindow->setSceneGraph(pWrl,false);
  mainWindow->refresh();
  updateState();
}

void GuiPanel::_refresh3DView() {
  auto mainWindow = getApp()->getMainWindow();
  GuiViewerData& data  = mainWindow->getData();
  SceneGraph* wrl = data.getSceneGraph();
  mainWindow->setSceneGraph(wrl,false);
  mainWindow->refresh();
  updateState();
}

IndexedFaceSet* GuiPanel::_getNamedIndexedFaceSet
(const string& name, const bool create, Color diffuseColor) {
  IndexedFaceSet* ifs = (IndexedFaceSet*)0;

  auto mainWindow = getApp()->getMainWindow();
  GuiViewerData& data  = mainWindow->getData();
  SceneGraph* wrl = data.getSceneGraph();
  Node* node = wrl->find(name); // should be a Shape node
  if(node==(Node*)0) {
    if(create) {
      Shape* s = new Shape();
      wrl->addChild(s);
      s->setName(name);
      Appearance* a = new Appearance();
      s->setAppearance(a);
      Material* m = new Material();
      m->setDiffuseColor(diffuseColor);
      a->setMaterial(m);
      ifs = new IndexedFaceSet();
      s->setGeometry(ifs);
    }
  } else if(node->isShape()) {
    Shape* shape    = (Shape*)node;
    Node*  geometry = shape->getGeometry();
    if(geometry!=(Node*)0 && geometry->isIndexedFaceSet())
      ifs = (IndexedFaceSet*)geometry;
  }

  return ifs;
}
