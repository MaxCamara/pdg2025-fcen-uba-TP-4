//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-08-06 09:57:13 taubin>
//------------------------------------------------------------------------
//
// GuiPanelSelection.hpp
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

#ifndef _GUI_PANEL_SELECTION_HPP_
#define _GUI_PANEL_SELECTION_HPP_

#include "ui_GuiPanelSelection.h"

#include <QResizeEvent>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QCheckBox>
#include <wrl/IndexedFaceSet.hpp>
#include <wrl/IndexedLineSet.hpp>
#include "GuiPanel.hpp"

class GuiPanelSelection : public GuiPanel, public Ui::GuiPanelSelection {

  Q_OBJECT;

  static bool  _registered;
  static bool  registerPanel();

public:

               GuiPanelSelection(QWidget *parent = 0);
  virtual     ~GuiPanelSelection();

protected:

  virtual void mousePressEvent(QMouseEvent * event) Q_DECL_OVERRIDE;

public slots:

  virtual void updateState() override; // c++11

private: // Operators

  static void clearIfsVertices(IndexedFaceSet& ifs);
  static void clearIlsVertices(IndexedLineSet& ifs);

private:

  static int _selectedVertexIndex;
  static int _selectedEdgeIndex;
  static int _selectedFaceIndex;

  enum Element {
    VERTICES, EDGES, FACES
  };
  enum Mode {
    CLEAR, INVERT, BOUNDARY, REGULAR, SINGULAR
  };

  static void _selectVertices(IndexedFaceSet& ifs, const Mode mode);
  static void _selectEdges(IndexedFaceSet& ifs, const Mode mode);
  static void _selectFaces(IndexedFaceSet& ifs, const Mode mode);

  static void _select(const Element elem, const Mode mode);
 
  static void _fromTo(const Element elemFrom, const Element elemTo);

private slots:

  void on_spinSelectedVertexIndex_valueChanged(int value);
  void on_spinSelectedEdgeIndex_valueChanged(int value);
  void on_spinSelectedFaceIndex_valueChanged(int value);

  void on_buttonClearVertices_clicked();
  void on_buttonClearEdges_clicked();
  void on_buttonClearFaces_clicked();

  void on_buttonInvertVertices_clicked();
  void on_buttonInvertEdges_clicked();
  void on_buttonInvertFaces_clicked();

  void on_buttonBoundaryVertices_clicked();
  void on_buttonBoundaryEdges_clicked();
  void on_buttonBoundaryFaces_clicked();

  void on_buttonRegularVertices_clicked();
  void on_buttonRegularEdges_clicked();
  void on_buttonRegularFaces_clicked();

  void on_buttonSingularVertices_clicked();
  void on_buttonSingularEdges_clicked();
  void on_buttonSingularFaces_clicked();

  void on_buttonFromVerticesToEdges_clicked();
  void on_buttonFromVerticesToFaces_clicked();

  void on_buttonFromEdgesToVertices_clicked();
  void on_buttonFromEdgesToFaces_clicked();

  void on_buttonFromFacesToVertices_clicked();
  void on_buttonFromFacesToEdges_clicked();

  void on_buttonShapeVertices_clicked();
  void on_buttonShapeEdges_clicked();
  void on_buttonShapeFaces_clicked();

  void on_buttonConnectedVertices_clicked();
  void on_buttonConnectedEdges_clicked();
  void on_buttonConnectedFaces_clicked();

  void on_buttonDeleteVertices_clicked();
  void on_buttonDeleteEdges_clicked();
  void on_buttonDeleteFaces_clicked();

  void on_buttonCutEdges_clicked();
  void on_buttonCutFaces_clicked();

};

#endif // _GUI_PANEL_SELECTION_HPP_
