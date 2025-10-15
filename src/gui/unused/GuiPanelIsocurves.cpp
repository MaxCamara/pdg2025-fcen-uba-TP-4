//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2023-02-27 00:00:12 taubin>
//------------------------------------------------------------------------
//
// GuiPanelIsocurves.cpp
//
// Software developed for the Spring 2023 Brown University course
// ENGN 2501 Digital Geometry Processing
// Copyright (c) 2023, Gabriel Taubin
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
#include "GuiPanelIsocurves.hpp"

#include "wrl/SceneGraphProcessor.hpp"
#include "core/PolygonMeshTest.hpp"

//////////////////////////////////////////////////////////////////////

// static variable evaluated at start-up time
bool GuiPanelIsocurves::_registered = GuiPanelIsocurves::registerPanel();

// static method run once at start-up time
bool GuiPanelIsocurves::registerPanel() {
  return
    GuiMainWindow::registerPanelFactory
    ("Isocurves",
     [](QWidget *parent)->GuiPanel*{ return new GuiPanelIsocurves(parent);});
}

//////////////////////////////////////////////////////////////////////
GuiPanelIsocurves::GuiPanelIsocurves(QWidget* parent):
  GuiPanel(parent) {

  static int indx = 0;
  std::cout << "GuiPanelIsocurves::GuiPanelIsocurves("<<(indx++)<<") {\n";

  static const char* bg0 =
    "QLabel { background-color : rgb(200,200,200); color : black; }";

  QFont font;
  font.setPointSize(10);
      
  // QWidget* p;

  _labelPanelIsocurves = new QLabel("ISOCURVES",this);
  _labelPanelIsocurves->setFont(font);
  _labelPanelIsocurves->setAlignment(Qt::AlignCenter);
  _labelPanelIsocurves->setStyleSheet(bg0);

  // ...

  // updateState();
  std::cout << "}\n";
}

//////////////////////////////////////////////////////////////////////
GuiPanelIsocurves::~GuiPanelIsocurves() {
  static int indx = 0;
  std::cout << "GuiPanelIsocurves::~GuiPanelIsocurves("<<(indx++)<<") {\n";
  std::cout << "}\n";
}

//////////////////////////////////////////////////////////////////////
void GuiPanelIsocurves::resizeEvent(QResizeEvent * event) {
  _resize();
}

//////////////////////////////////////////////////////////////////////
void GuiPanelIsocurves::_resize() {

  int width  = this->width();

  int rowH = 20;
  int rowBorder = 0;
  int colBorder = 0;
  int rowSpace = 3;
  int colSpace = 3;

    int x,y,w,x0,x1,x2,x3,w0,w1,w2,w3; // ,panelH;

  x = colBorder;
  w = width-2*colBorder;
  y = rowBorder;

  w0 = (width-3*colSpace-2*colBorder)/4;
  w1 = w0;
  w2 = w1;
  w3 = width-3*colSpace-2*colBorder-w0-w1-w2;
  x0 = colBorder;
  x1 = x0+w0+colSpace;
  x2 = x1+w1+colSpace;
  x3 = x2+w2+colSpace;

  _labelPanelIsocurves->setFixedHeight(rowH);
  _labelPanelIsocurves->setGeometry(x,y,w,rowH);

  y += rowH+rowSpace;

  // ...
}

//////////////////////////////////////////////////////////////////////
// overrides pure virtual GuiPanel::updateState()

void GuiPanelIsocurves::updateState() {
  std::cout << "GuiPanelIsocurves::updateState() {\n";

  // auto _mainWindow = getApp()->getMainWindow();
  // GuiViewerData& data  = _mainWindow->getData();

  std::cout << "}\n";
}


//////////////////////////////////////////////////////////////////////
void GuiPanelIsocurves::mousePressEvent(QMouseEvent * event) {

  int x = event->x();
  int y = event->y();

  bool clickedOnLabel = false;

  if(_labelPanelIsocurves->geometry().contains(x,y,true)) {
    // _panelPanelIsocurves->setVisible(_panelCanvas->isHidden());
    clickedOnLabel = true;
  }

  if(clickedOnLabel) _resize();

}

