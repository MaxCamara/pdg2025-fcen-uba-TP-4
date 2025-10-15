//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-08-06 09:57:09 taubin>
//------------------------------------------------------------------------
//
// GuiPanelGeneral.hpp
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

#ifndef _GUI_PANEL_GENERAL_HPP_
#define _GUI_PANEL_GENERAL_HPP_

#include "ui_GuiPanelGeneral.h"

#include <QResizeEvent>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QCheckBox>
#include "GuiPanel.hpp"

class GuiMainWindow;

class GuiPanelGeneral : public GuiPanel, public Ui::GuiPanelGeneral {

  Q_OBJECT;

  static bool _registered;
  static bool registerPanel();

public:

  GuiPanelGeneral(QWidget *parent = 0);
  virtual ~GuiPanelGeneral();

public slots:

  virtual void updateState() override; // overrides virtual GuiPanel::updateState()

protected:

  virtual void mousePressEvent(QMouseEvent * event) Q_DECL_OVERRIDE;


private slots:

  // Hexahedral Grid
  void hexGridDepthUp();
  void hexGridDepthDown();
  void on_spinBoxHexGridDepth_valueChanged(int depth);
  void on_pushButtonHexGridAdd_clicked();
  void on_pushButtonHexGridRemove_clicked();
  void on_editHexGridScale_returnPressed();
  void on_checkBoxHexGridCube_stateChanged(int state);

  // Scene Graph
  void on_pushButtonSceneGraphIndexedFaceSetsShow_clicked();
  void on_pushButtonSceneGraphIndexedFaceSetsHide_clicked();
  void on_pushButtonSceneGraphIndexedLineSetsShow_clicked();
  void on_pushButtonSceneGraphIndexedLineSetsHide_clicked();
  
  // Normals
  void on_pushButtonNormalsNone_clicked();
  void on_pushButtonNormalsPerVertex_clicked();
  void on_pushButtonNormalsPerFace_clicked();
  void on_pushButtonNormalsPerCorner_clicked();
  void on_pushButtonNormalsInvert_clicked();

  // Points
  void on_pushButtonPointsRemove_clicked();
  void on_pushButtonPointsShow_clicked();
  void on_pushButtonPointsHide_clicked();

  // Edges
  void on_pushButtonSceneGraphEdgesAdd_clicked();
  void on_pushButtonSceneGraphEdgesRemove_clicked();
  void on_pushButtonSceneGraphEdgesShow_clicked();
  void on_pushButtonSceneGraphEdgesHide_clicked();

  // Surface
  void on_pushButtonSurfaceRemove_clicked();
  void on_pushButtonSurfaceShow_clicked();
  void on_pushButtonSurfaceHide_clicked();
};

#endif // _GUI_PANEL_GENERAL_HPP_
