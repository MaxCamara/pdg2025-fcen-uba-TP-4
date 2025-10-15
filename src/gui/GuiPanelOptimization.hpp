//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-08-06 09:57:10 taubin>
//------------------------------------------------------------------------
//
// GuiPanelOptimization.hpp
//
// Software developed for the course
// Digital Geometry Processing
// Copyright (c) 2025, Gabriel Taubin
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//     * Redistributions of source code must retain the above
//       copyright notice, this list of conditions and the following
//       disclaimer.
//     * Redistributions in binary form must reproduce the above
//       copyright notice, this list of conditions and the following
//       disclaimer in the documentation and/or other materials
//       provided with the distribution.
//     * Neither the name of the Brown University nor the names of its
//       contributors may be used to endorse or promote products
//       derived from this software without specific prior written
//       permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL GABRIEL
// TAUBIN BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
// USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.

#ifndef _GUI_PANEL_OPTIMIZATION_HPP_
#define _GUI_PANEL_OPTIMIZATION_HPP_

#include "ui_GuiPanelOptimization.h"

#include <QResizeEvent>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QCheckBox>
#include "GuiPanel.hpp"
#include "core/Optimization.hpp"

class GuiPanelOptimization : public GuiPanel, public Ui::GuiPanelOptimization {

  Q_OBJECT;

  static bool  _registered;
  static bool  registerPanel();

public:

               GuiPanelOptimization(QWidget *parent = 0);
  virtual     ~GuiPanelOptimization();

protected:

  virtual void mousePressEvent(QMouseEvent * event) Q_DECL_OVERRIDE;

public slots:

  virtual void updateState() override; // c++11

private slots:

  // panelShow
  void on_pushButtonReset_clicked();
  void on_pushButtonSave_clicked();
  void on_pushButtonRemove_clicked();
  void on_checkBoxShowInput_stateChanged(int state);
  void on_checkBoxShowOutput_stateChanged(int state);
  void on_pushButtonShowToggle_clicked();
  void on_checkBoxShowNormals_stateChanged(int state);  
  void on_pushButtonNormalsRecompute_clicked();
  
  // panelSmoothingParameters
  void on_spinBoxSmoothingSteps_valueChanged(int steps);
  void on_editLambda_returnPressed();
  void on_editMu_returnPressed();
  void on_editKappa_returnPressed();
  void on_pushButtonMuToggleSign_clicked();
  
  // panelLaplacianSmoothing
  void on_pushButtonLaplacianSmoothingRun_clicked();

  // panelJacobi
  void on_editJacobiWeightData_returnPressed();
  void on_editJacobiWeightSmoothing_returnPressed();
  void on_pushButtonJacobiRun_clicked();
  
  // panelIntegrateNormals
  void on_editIntegrateNormalsWeightData_returnPressed();
  void on_editIntegrateNormalsWeightLink_returnPressed();
  void on_editIntegrateNormalsWeightSmoothing_returnPressed();
  void on_pushButtonIntegrateNormalsRun_clicked();
  
  // panelEdgeLengths
  void on_editTargetEdgeLength_returnPressed();
  
  // panelCollapseEdges
  void on_comboCollapseEdgesErrorMetric_currentIndexChanged(int index);
  void on_comboCollapseEdgesIndependentSet_currentIndexChanged(int index);
  void on_pushButtonCollapseEdgesShow_clicked();
  void on_pushButtonCollapseEdgesApply_clicked();
  
  // panelSplitEdges
  void on_comboSplitEdgesMode_currentIndexChanged(int index);
  void on_pushButtonSplitEdgesShow_clicked();
  void on_pushButtonSplitEdgesApply_clicked();

  // panelEqualizeValences
  void on_pushButtonEqualizeValencesShow_clicked();
  void on_pushButtonEqualizeValencesApply_clicked();

private:

  Optimization::EdgeCollapseErrorMetric    _getErrorMetric();
  Optimization::EdgeCollapseIndependentSet _getIndependentSet();
  Optimization::SplitEdgesMode             _getSplitEdgesMode();

private:

  Optimization    _optimization;
  static Color    _diffuseColor;
};

#endif // _GUI_PANEL_OPTIMIZATION_HPP_
