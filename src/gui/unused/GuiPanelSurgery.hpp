//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2023-02-27 00:00:12 taubin>
//------------------------------------------------------------------------
//
// GuiPanelSurgery.hpp
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

#ifndef _GUI_PANEL_SURGERY_HPP_
#define _GUI_PANEL_SURGERY_HPP_

#include <QResizeEvent>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton> 
#include <QCheckBox>
#include "GuiPanel.hpp"

class GuiPanelSurgery : public GuiPanel {

  Q_OBJECT;

  static bool  _registered;
  static bool  registerPanel();

public:

               GuiPanelSurgery(QWidget *parent = 0);
  virtual     ~GuiPanelSurgery();

protected:

  virtual void mousePressEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
  virtual void resizeEvent(QResizeEvent * event)    Q_DECL_OVERRIDE;

          void _resize();

public slots:

  virtual void updateState() override; // c++11

private:

  QLabel* _labelPanelSurgery;

};

#endif // _GUI_PANEL_SURGERY_HPP_
