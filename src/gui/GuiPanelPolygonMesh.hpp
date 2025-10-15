//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-08-06 09:57:11 taubin>
//------------------------------------------------------------------------
//
// GuiPanelPolygonMesh.hpp
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

#ifndef _GUI_PANEL_POLYGON_MESH_HPP_
#define _GUI_PANEL_POLYGON_MESH_HPP_

#include "ui_GuiPanelPolygonMesh.h"

#include <QResizeEvent>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QCheckBox>
#include <wrl/Shape.hpp>
#include "GuiPanel.hpp"

class GuiMainWindow;

class GuiPanelPolygonMesh : public GuiPanel, public Ui::GuiPanelPolygonMesh {

  Q_OBJECT;

  static bool _registered;
  static bool registerPanel();

public:

  GuiPanelPolygonMesh(QWidget *parent = 0);
  virtual ~GuiPanelPolygonMesh();

public slots:

  virtual void updateState() override; // overrides virtual GuiPanel::updateState()

protected:

  virtual void mousePressEvent(QMouseEvent * event) Q_DECL_OVERRIDE;

private:

  void   _updateStateNoSceneGraph();
  void   _updateComboShape(const bool onlyIndexedfedFaceSets = false);
  int    _getCurrentShapeIndex();
  Shape* _getCurrentShape();
  void   _pushButtonCC(const bool primal); // dual = !primal

private slots:

  // Shape
  void on_comboBoxShapeSelect_currentIndexChanged(int index);
  void on_pushButtonShapeSetSurface_clicked();
  void on_pushButtonShapeSetPoints_clicked();

  // Test
  void on_checkBoxTestIsManifold_stateChanged(int state);
  void on_checkBoxTestHasBoundary_stateChanged(int state);
  void on_pushButtonTestPolygonMesh_clicked();

  // Connected Components
  void on_pushButtonCCPrimal_clicked();
  void on_pushButtonCCDual_clicked();

  // Orientation
  void on_pushButtonIsOriented_clicked();
  void on_pushButtonIsOrientable_clicked();
  void on_pushButtonOrient_clicked();

  // Manifold
  void on_pushButtonRemoveIsolatedVertices_clicked();
  void on_pushButtonCutThroughSingularVertices_clicked();
  void on_pushButtonConvertToManifold_clicked();
  
};

#endif // _GUI_PANEL_POLYGON_MESH_HPP_
