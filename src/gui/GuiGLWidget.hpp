//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-10-14 17:16:52 taubin>
//------------------------------------------------------------------------
//
// GuiGLWidget.cpp
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

#ifndef _GUI_GL_WIDGET_HPP_
#define _GUI_GL_WIDGET_HPP_

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QVector3D>
#include <QMatrix4x4>
#include <QTime>
#include <QVector>
#include <QPushButton>
#include <QMouseEvent>
#include <QDragMoveEvent>

// #include "util/BBox.hpp"
#include "wrl/SceneGraph.hpp"
#include "wrl/Transform.hpp"
#include "wrl/Shape.hpp"
// #include "wrl/IndexedFaceSet.hpp"
// #include "wrl/Appearance.hpp"
// #include "wrl/Material.hpp"

#include "GuiViewerData.hpp"
#include "GuiGLShader.hpp"
#include "GuiGLHandles.hpp"

class GuiMainWindow;

QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)
QT_FORWARD_DECLARE_CLASS(QOpenGLShader)
QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram)

class GuiGLWidget : public QOpenGLWidget, protected QOpenGLFunctions {

  Q_OBJECT

public:

  GuiGLWidget(GuiMainWindow *mw,
              const QColor& backgroundColor,
              const QColor& materialColor);
  ~GuiGLWidget();

  GuiViewerData& getData();

  SceneGraph* getSceneGraph();
  void        setSceneGraph(SceneGraph* pWrl, bool resetHomeView);
  void        resetSceneGraph();

  void invertNormal(); // TODO

  void setProjectionMatrix(const float vAngle);

public slots:

  void setQtLogo();

protected:

  void initializeGL()         Q_DECL_OVERRIDE;
  void paintGL()              Q_DECL_OVERRIDE;
  void resizeGL(int w, int h) Q_DECL_OVERRIDE;

  // virtual void resizeEvent(QResizeEvent * event) Q_DECL_OVERRIDE;

  void paintData(QMatrix4x4& mvp);
  void paintGroup(QMatrix4x4& mvp, Group* group);
  void paintTransform(QMatrix4x4& mvp, Transform* transform);
  void paintSceneGraph(QMatrix4x4& mvp, SceneGraph* wrl);
  void paintShape(QMatrix4x4& mvp, Shape* shape);

  virtual void	enterEvent(QEnterEvent * event)            Q_DECL_OVERRIDE;
  virtual void	leaveEvent(QEvent * event)                 Q_DECL_OVERRIDE;
  virtual void	mousePressEvent(QMouseEvent * event)       Q_DECL_OVERRIDE;
  virtual void	mouseReleaseEvent(QMouseEvent * event)     Q_DECL_OVERRIDE;
  virtual void	mouseMoveEvent(QMouseEvent * event)        Q_DECL_OVERRIDE;
  virtual void  wheelEvent(QWheelEvent *event)             Q_DECL_OVERRIDE;

private:

  void _setHomeView(const bool identity);
  void _zoom(const float value);

private:

  GuiMainWindow*        _mainWindow;
  GuiViewerData         _data;
  float                 _bboxDiameter;

  QVector3D             _eye;
  QVector3D             _center;
  QVector3D             _up;

  int                   _mouseZone;
  bool                  _mouseInside;
  bool                  _mousePressed;
  Qt::MouseButtons      _buttons;
  int                   _pressedMouseX;
  int                   _pressedMouseY;
  int                   _prevMouseX;
  int                   _prevMouseY;
  bool                  _zone4enabled;
  float                 _translateStep;

  QVector3D             _cameraTranslation;
  QMatrix4x4            _viewRotation;
  QMatrix4x4            _projectionMatrix;

  bool                  _animationOn;
  qreal                 _fAngle;
  float                 _vAngle;

  // map<Shape*,GuiGLShader*> _shaderMap;
  map<Shape*,VectorGuiGLShader*> _shaderMap;

  GuiGLHandles*         _handles;

  QColor                _background;
  QColor                _material;
  QVector3D             _lightSource;

  static int            _borderUp;
  static int            _borderDown;
  static int            _borderLeft;
  static int            _borderRight;

  static float          _angleHomeX;
  static float          _angleHomeY;
  static float          _angleHomeZ;

};

#endif // _GUI_GL_WIDGET_HPP_
