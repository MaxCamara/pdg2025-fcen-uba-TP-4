//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-10-14 17:15:44 taubin>
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

#include <iostream>
// #include <iomanip>
// #include <cstring>
#include <cmath>

#include <QPainter>
#include <QPaintEngine>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QCoreApplication>

// #include "GuiApplication.hpp"
#include "GuiMainWindow.hpp"
#include "GuiQtLogo.hpp"
#include "GuiGLBuffer.hpp"

#include "wrl/IndexedLineSetVariables.hpp"
#include "wrl/IndexedFaceSetVariables.hpp"
#include "wrl/SceneGraphTraversal.hpp"

#include "core/Geometry.hpp"
#include "core/PolygonMesh.hpp"

#ifdef near
# undef near
#endif
#ifdef far
# undef far
#endif

int   GuiGLWidget::_borderUp         =     20;
int   GuiGLWidget::_borderDown       =     20;
int   GuiGLWidget::_borderLeft       =     20;
int   GuiGLWidget::_borderRight      =     20;
float GuiGLWidget::_angleHomeX       =  10.0f; // 0.0f;
float GuiGLWidget::_angleHomeY       =  10.0f; // 0.0f;
float GuiGLWidget::_angleHomeZ       =   0.00f;

// void printQMatrix4x4(const string& name, const QMatrix4x4& M) {
//   string str;
//   static char cstr[128];
//   std::cout << name << " = [\n";
//   sprintf(cstr,"%10.4f %10.4f %10.4f %10.4f", M(0,0),M(0,1),M(0,2),M(0,3));
//   str = cstr; cout << cstr << "\n";
//   sprintf(cstr,"%10.4f %10.4f %10.4f %10.4f", M(1,0),M(1,1),M(1,2),M(1,3));
//   str = cstr; cout << cstr << "\n";
//   sprintf(cstr,"%10.4f %10.4f %10.4f %10.4f", M(2,0),M(2,1),M(2,2),M(2,3));
//   str = cstr; cout << cstr << "\n";
//   sprintf(cstr,"%10.4f %10.4f %10.4f %10.4f", M(3,0),M(3,1),M(3,2),M(3,3));
//   str = cstr; cout << cstr << "\n";
//   std::cout << "]\n";
// }

//////////////////////////////////////////////////////////////////////
GuiGLWidget::GuiGLWidget
(GuiMainWindow *mw, const QColor &background, const QColor &material):
  _mainWindow(mw),
  _data(),
  _bboxDiameter(2.0),
  _eye(0,0,0),
  _center(0,0,1),
  _up(0,1,0),
  _mouseZone(-1),
  _mouseInside(false),
  _mousePressed(true),
  _pressedMouseX(0),
  _pressedMouseY(0),
  _prevMouseX(0),
  _prevMouseY(0),
  _zone4enabled(true),
  _translateStep(0.010f),
  _cameraTranslation(0,0,0),
  _animationOn(true),
  _fAngle(0),
  _vAngle(10.0f),
  _background(background),
  _material(material),
  // _lightSource(0.0, 0.3, -1.0)
  _lightSource(0.0,-0.3, 1.0)
{

  setMinimumSize(400,400);
  // _data.setInitialized(false);
  _data.setSceneGraph((SceneGraph*)0);
  _viewRotation.setToIdentity();
  _projectionMatrix.setToIdentity();
  setMouseTracking(true);
}

//////////////////////////////////////////////////////////////////////
GuiGLWidget::~GuiGLWidget() {
  makeCurrent();
  map<Shape*,VectorGuiGLShader*>::iterator i;
  for(i=_shaderMap.begin();i!=_shaderMap.end();i++) {
    VectorGuiGLShader* vecShader = i->second;
    if(vecShader) {
      while(vecShader->size()>0) {
        GuiGLShader* shader = vecShader->back();
        delete shader;
        vecShader->pop_back();
      }
      i->second = (VectorGuiGLShader*)0;
      delete vecShader;
    }
  }
  _shaderMap.clear();
  delete _handles;
  doneCurrent();
}

//////////////////////////////////////////////////////////////////////
GuiViewerData& GuiGLWidget::getData() {
  return _data;
}

//////////////////////////////////////////////////////////////////////
SceneGraph* GuiGLWidget::getSceneGraph() {
  return _data.getSceneGraph();
}

//////////////////////////////////////////////////////////////////////
void GuiGLWidget::setSceneGraph(SceneGraph* pWrl, bool resetHomeView) {
  // cout << "void GuiGLWidget::setSceneGraph() {\n";

  // clear _shaderMap
  map<Shape*,VectorGuiGLShader*>::iterator i;
  for(i=_shaderMap.begin();i!=_shaderMap.end();i++) {
    VectorGuiGLShader* vecShader = i->second;
    if(vecShader) {
      while(vecShader->size()>0) {
        GuiGLShader* shader = vecShader->back();
        delete shader;
        vecShader->pop_back();
      }
      i->second = (VectorGuiGLShader*)0;
      delete vecShader;
    }
  }
  _shaderMap.clear();

  _data.setSceneGraph(pWrl);
  if(pWrl!=(SceneGraph*)0) {

    SceneGraphTraversal sgt(*pWrl);
    sgt.start();
    Node* node=(Node*)0;
    while((node=sgt.next())!=(Node*)0) {
      if(Shape* shape = dynamic_cast<Shape*>(node)) {
        
        QColor materialColor(255,150,90);

        node = shape->getAppearance();
        if(Appearance* appearance = dynamic_cast<Appearance*>(node)) {

          node = appearance->getMaterial();
          if(Material* material = dynamic_cast<Material*>(node)) {

            Color& diffuseColor = material->getDiffuseColor();
            materialColor.setRedF(diffuseColor.r);
            materialColor.setGreenF(diffuseColor.g);
            materialColor.setBlueF(diffuseColor.b);
          }
        }

        node = shape->getGeometry();

        bool paintAllVertices       = _data.getPaintAllVertices();
        bool paintSelectedVertices  = _data.getPaintSelectedVertices();
        bool paintAllEdges          = _data.getPaintAllEdges();
        bool paintSelectedEdges     = _data.getPaintSelectedEdges();
        bool paintAllFaces          = _data.getPaintAllFaces();
        bool paintSelectedFaces     = _data.getPaintSelectedFaces();
        bool paintAllPolylines      = _data.getPaintAllPolylines();
        bool paintSelectedPolylines = _data.getPaintSelectedPolylines();
        bool paintNormals           = _data.getPaintNormals();

        if(IndexedFaceSet* pIfs = dynamic_cast<IndexedFaceSet*>(node)) {
          IndexedFaceSetVariables ifsv(*pIfs);
          PolygonMesh* pm = ifsv.getPolygonMesh(true);
          vector<float>& coord = pIfs->getCoord();
          int nV = pm->getNumberOfVertices();
          int nE = pm->getNumberOfEdges();
          int nF = pm->getNumberOfFaces();

          if(nF==0) {
            paintAllVertices = true;
            float coordDiameter =
            Geometry::computeDiameter(coord);
            _data.setNormalLength(0.05f*coordDiameter);
          } else {
            // avrgEdgeLength==NaN if nF==0 !!!
            float avrgEdgeLength =
            Geometry::computeAverageEdgeLength(coord,*pm);
            _data.setNormalLength(0.5f*avrgEdgeLength);
          }

          QColor ifsMaterialColor;
          Color mc = ifsv.getMaterialColor();
          ifsMaterialColor.setRgbF(mc.r,mc.g,mc.g);

          VectorGuiGLShader* vecShader = new VectorGuiGLShader;
          _shaderMap[shape] = vecShader;

          int nPaintFaces =
            (paintAllFaces     )?nF:
            (paintSelectedFaces)?ifsv.getNumberOfSelectedFaces():0;

          if(nPaintFaces>0) {

            GuiGLBuffer* ifsb   =
              new GuiGLBuffer(pIfs,GuiGLBuffer::Render::FACES,
                              paintAllFaces==false);

            GuiGLShader* shader = new GuiGLShader(materialColor,&_lightSource);
            shader->setVertexBuffer(ifsb);
            shader->setPointSize(_data.getPointSize());
            vecShader->push_back(shader);
            
          }

          int nPaintEdges =
            (paintAllEdges     )?nE:
            (paintSelectedEdges)?ifsv.getNumberOfSelectedEdges():0;

          if(nPaintEdges>0) {

            GuiGLBuffer* ifsb   =
              new GuiGLBuffer(pIfs,GuiGLBuffer::Render::EDGES,
                              paintAllEdges==false);

            GuiGLShader* shader = new GuiGLShader(materialColor,&_lightSource);
            shader->setVertexBuffer(ifsb);
            shader->setPointSize(_data.getPointSize());
            vecShader->push_back(shader);

          }

          int nPaintVertices =
            (paintAllVertices     )?nV:
            (paintSelectedVertices)?ifsv.getNumberOfSelectedVertices():0;

          if(nPaintVertices>0) {

            bool paintOnlySelected = !paintAllVertices;
            GuiGLBuffer* ifsb   =
              new GuiGLBuffer(pIfs,GuiGLBuffer::Render::VERTICES,
                              paintOnlySelected);

            GuiGLShader* shader = new GuiGLShader(materialColor,&_lightSource);
            shader->setVertexBuffer(ifsb);
            shader->setPointSize(_data.getPointSize());
            vecShader->push_back(shader);

          }

          if(paintNormals) {
          
            GuiGLBuffer* ifsb   =
              new GuiGLBuffer(pIfs,GuiGLBuffer::Render::NORMALS,false);
            GuiGLShader* shader = new GuiGLShader(materialColor,&_lightSource);
            shader->setVertexBuffer(ifsb);
            shader->setPointSize(_data.getPointSize());
            vecShader->push_back(shader);
          
          }

        } else

     /* } else */ if(IndexedLineSet* pIls = dynamic_cast<IndexedLineSet*>(node)) {

            IndexedLineSetVariables ilsv(*pIls);

            QColor ilsMaterialColor;
            Color mc = ilsv.getMaterialColor();
            ilsMaterialColor.setRgbF(mc.r,mc.g,mc.b);

            VectorGuiGLShader* vecShader = new VectorGuiGLShader;
            _shaderMap[shape] = vecShader;

            if(paintAllPolylines || paintSelectedPolylines) {

              GuiGLBuffer* ifsb   =
                new GuiGLBuffer(pIls,GuiGLBuffer::Render::POLYLINES,
                                paintAllPolylines==false);

              GuiGLShader* shader = new GuiGLShader(materialColor);
              shader->setVertexBuffer(ifsb);
              shader->setPointSize(_data.getPointSize());
              vecShader->push_back(shader);

            }

            if(paintAllVertices || paintSelectedVertices) {

              GuiGLBuffer* ifsb   =
                new GuiGLBuffer(pIls,GuiGLBuffer::Render::VERTICES,
                                paintAllVertices);

              GuiGLShader* shader = new GuiGLShader(materialColor);
              shader->setVertexBuffer(ifsb);
              shader->setPointSize(_data.getPointSize());
              vecShader->push_back(shader);
            
            }
          
          } // else if(pIfs==0 && pIls==0) ...
        
      } // if(Shape* shape ...
    } // while((node ...

    if(resetHomeView) {
      _vAngle = 10.0f;
      _bboxDiameter = 2.0f;
      if(pWrl->hasEmptyBBox()) {
        // cout << "  hasEmptyBBox\n";
        _center.setX(0); _center.setY(0); _center.setZ(0);
      } else {
        Vec3f& bbCenter = pWrl->getBBoxCenter();
        _center.setX(bbCenter.x);
        _center.setY(bbCenter.y);
        _center.setZ(bbCenter.z);
        _bboxDiameter = pWrl->getBBoxDiameter();
      }

      _translateStep = 0.001f*_bboxDiameter;
      _setHomeView(false);
      setProjectionMatrix(_vAngle);
    }

  }

  // cout << "}\n";
}

//////////////////////////////////////////////////////////////////////
void GuiGLWidget::resetSceneGraph() {
  setSceneGraph(getSceneGraph(),false);
}

//////////////////////////////////////////////////////////////////////
void GuiGLWidget::setQtLogo() {
  SceneGraph* wrl = new GuiQtLogo();
  _data.setSceneGraph(wrl);
  // _mainWindow->updateState();
}

//////////////////////////////////////////////////////////////////////
void GuiGLWidget::invertNormal() {

  // TODO Thu Oct 20 18:47:14 2016

  // map<Shape*,VectorGuiGLShader*>::iterator i;
  // for(i=_shaderMap.begin();i!=_shaderMap.end();i++) {
  //   Shape*         shape    = i->first;
  //   VectorGuiGLShader* vecShader = i->second;
  //   // GuiGLShader*   shader   = i->second;
  //   GuiGLShader*   shader   = (*vecShader)[0];
  //   GuiGLBuffer*   vbo      = shader->getVertexBuffer();
  //
  //   Node* geometry = shape->getGeometry();
  //   if(IndexedFaceSet* ifs=dynamic_cast<IndexedFaceSet*>(geometry)) {
  //
  //     vector<float> &normal = ifs->getNormal();    
  //     float n0,n1,n2;
  //     for(unsigned i=0;i<normal.size();i+=3) {
  //       n0 = normal[i+0]; n1 = normal[i+1]; n2 = normal[i+2];
  //       normal[i+0] = -n0; normal[i+1] = -n1; normal[i+2] = -n2;
  //     }
  //
  //     QColor materialColor(255,150,90);
  //     if(Appearance* appearance =
  //        dynamic_cast<Appearance*>(shape->getAppearance())) {
  //       if(Material* material =
  //          dynamic_cast<Material*>(appearance->getMaterial())) {
  //         Color& diffuseColor = material->getDiffuseColor();
  //         materialColor.setRedF(diffuseColor.r);
  //         materialColor.setGreenF(diffuseColor.g);
  //         materialColor.setBlueF(diffuseColor.b);
  //       }
  //     }
  //     QColor& ifsMaterialColor = ifs->getMaterialColor();
  //     ifsMaterialColor = materialColor;
  //     GuiGLBuffer* ifsb = new GuiGLBuffer(ifs);
  //     shader->setVertexBuffer(ifsb);
  //     if(vbo) { vbo->destroy(); delete vbo; }
  //   }
  // }

}

//////////////////////////////////////////////////////////////////////
void GuiGLWidget::initializeGL() {

  // cout << "void GuiGLWidget::initializeGL() {\n";
  
  initializeOpenGLFunctions();
  
  union {
    const unsigned char* u;
    const char*          s;
  } a,b,c;

  a.u = glGetString(GL_VENDOR);
  std::string vendor(a.s);
  b.u = glGetString(GL_RENDERER);
  std::string renderer(b.s);
  c.u = glGetString(GL_VERSION);
  std::string version(c.s);
  
  // cout << "  [OpenGL] vendor  : " << vendor   << "\n";
  // cout << "  [OpenGL] renderer: " << renderer << "\n";
  // cout << "  [OpenGL] version : " << version  << "\n";
    
  _mainWindow->timerStart();
  _animationOn = false;
  _mousePressed = true;
  _mainWindow->timerStop();

  _handles = new GuiGLHandles(); // TODO !!!

  SceneGraph* wrl = new GuiQtLogo();
  setSceneGraph(wrl,true);

  float lineWidth = _data.getLineWidth();
  GLfloat lineWidthRange[2] = {0.0f, 0.0f};
  glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, lineWidthRange);
  if(lineWidth<lineWidthRange[0])
    lineWidth = lineWidthRange[0];
  else if(lineWidth>lineWidthRange[1])
    lineWidth = lineWidthRange[1];
  glLineWidth(lineWidth);
  _data.setLineWidth(lineWidth);

  // cout << "}\n";
}

//////////////////////////////////////////////////////////////////////
void GuiGLWidget::paintShape(QMatrix4x4& mvp, Shape* shape) {
  if(shape==(Shape*)0 || shape->getShow()==false) return;
  if(dynamic_cast<IndexedFaceSet*>(shape->getGeometry()) ||
     dynamic_cast<IndexedLineSet*>(shape->getGeometry())) {
    if(VectorGuiGLShader* vecShader = _shaderMap[shape]) {
      for(int j=0;j<static_cast<int>(vecShader->size());j++) {
        GuiGLShader* shader = (*vecShader)[j];
        shader->setMVPMatrix(mvp);
        shader->paint(*this);
      }
    }
  }
}

//////////////////////////////////////////////////////////////////////
void GuiGLWidget::paintGroup(QMatrix4x4& mvp, Group* group) {
  if(group==(Group*)0 || group->getShow()==false) return;
  unsigned nChildren = group->getNumberOfChildren();
  for(unsigned i=0;i<nChildren;i++) {
    Node* node = (*group)[i];
    if(Shape* s = dynamic_cast<Shape*>(node)) {
      paintShape(mvp, s);
    } else if(Transform* t = dynamic_cast<Transform*>(node)) {
      paintTransform(mvp, t);
    } else if(Group* g = dynamic_cast<Group*>(node)) {
      paintGroup(mvp, g);
    }
  }
}

//////////////////////////////////////////////////////////////////////
void GuiGLWidget::paintTransform(QMatrix4x4& mvp, Transform* transform) {
  if(transform==(Transform*)0 || transform->getShow()==false) return;

  float T[16] = {
    1.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
  };

  transform->getMatrix(T);

  // mvpt = mvp * T
  QMatrix4x4 mvpt =
    mvp *
    QMatrix4x4(T[ 0],T[ 1],T[ 2],T[ 3],
               T[ 4],T[ 5],T[ 6],T[ 7],
               T[ 8],T[ 9],T[10],T[11],
               T[12],T[13],T[14],T[15]);

  paintGroup(mvpt, transform);
}

//////////////////////////////////////////////////////////////////////
void GuiGLWidget::paintSceneGraph(QMatrix4x4& mvp, SceneGraph* wrl) {
  if(wrl==(SceneGraph*)0 || wrl->getShow()==false) return;
  paintGroup(mvp,wrl);
}

//////////////////////////////////////////////////////////////////////
void GuiGLWidget::paintData(QMatrix4x4& mvp) {
  SceneGraph* wrl = _data.getSceneGraph();
  if(wrl!=(SceneGraph*)0) paintSceneGraph(mvp,wrl);
}

//////////////////////////////////////////////////////////////////////
void GuiGLWidget::paintGL() {

  QPainter painter;
  painter.begin(this);
  painter.beginNativePainting();

  glClearColor(_background.redF(), _background.greenF(), _background.blueF(),1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  float lineWidth = _data.getLineWidth();
  glLineWidth(lineWidth);

  // paint
  glFrontFace(GL_CW);
  glCullFace(GL_FRONT);
  // glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL); // does it do anything ???
  glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

  // to render the points as flat shaded circles rather than squares
  if(/*_roundPoints*/ true) {
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT,GL_NICEST);
    if(/*_blendSpheres*/ true) {
      glEnable(GL_BLEND);
      glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    }
  }
  
  // QMatrix4x4& mvp = _shader->getMVPMatrix();
  QMatrix4x4 mvp;
  mvp.setToIdentity();

  QMatrix4x4 cameraTranslationMatrix;
  cameraTranslationMatrix.setToIdentity();
  cameraTranslationMatrix.translate(_cameraTranslation);

  // void QMatrix4x4::lookAt
  //   (const QVector3D & eye, const QVector3D & center, const QVector3D & up);
  //
  // Multiplies this matrix by a viewing matrix derived from an eye
  // point. The center value indicates the center of the view that the
  // eye is looking at. The up value indicates which direction should
  // be considered up with respect to the eye.

  QMatrix4x4 viewMatrix;
  viewMatrix = cameraTranslationMatrix;
  viewMatrix.lookAt(_eye,_center,_up);
  mvp = _projectionMatrix * viewMatrix;
  mvp.translate(_center.x(),_center.y(),_center.z());
  mvp.rotate(    _fAngle, 0.0f, 1.0f, 0.0f);
  mvp *= _viewRotation;
  mvp.translate(-_center.x(),-_center.y(),-_center.z());

  paintData(mvp);

  glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
  glDisable(GL_DEPTH_TEST);
  // glDisable(GL_CULL_FACE);

  if(_animationOn) _fAngle += 0.25f;

  // paint mouse handles

  float w    =                    (float)width();
  float h    =                   (float)height();
  float x0   =                              0.0f;
  float x1   =            ((float)_borderLeft)/w;
  float x2   = ((float)(width()-_borderRight))/w;
  float x3   =                              1.0f;
  float y0   =                              1.0f;
  float y1   =   ((float)(height()-_borderUp))/h;
  float y2   =            ((float)_borderDown)/h;
  float y3   =                              0.0f;
  
  float hx0=0.0f,hx1=0.0f,hy0=0.0f,hy1=0.0f;
  if(_mouseZone==0) {
    hx0 = x0; hy0 = y0; hx1 = x1; hy1 = y1;
  } else if(_mouseZone==1) {
    hx0 = x1; hy0 = y0; hx1 = x2; hy1 = y1;
  } else if(_mouseZone==2) {
    hx0 = x2; hy0 = y0; hx1 = x3; hy1 = y1;
  } else if(_mouseZone==3) {
    hx0 = x0; hy0 = y1; hx1 = x1; hy1 = y2;
  } else if(_mouseZone==4) {
    // don't paint anything for the center region
    // hx0 = x1; hy0 = y1; hx1 = x2; hy1 = y2;
  } else if(_mouseZone==5) {
    hx0 = x2; hy0 = y1; hx1 = x3; hy1 = y2;
  } else if(_mouseZone==6) {
    hx0 = x0; hy0 = y2; hx1 = x1; hy1 = y3;
  } else if(_mouseZone==7) {
    hx0 = x1; hy0 = y2; hx1 = x2; hy1 = y3;
  } else if(_mouseZone==8) {
    hx0 = x2; hy0 = y2; hx1 = x3; hy1 = y3;
  }
  
  if(hx0!=hx1 && hy0!=hy1) {
    QColor colorHandles;
    colorHandles.setRgbF(0.6f,0.6f,0.9f);
    _handles->setColor(colorHandles);
    QMatrix4x4& handlesMatrix = _handles->getMatrix();
    handlesMatrix.setToIdentity();
    handlesMatrix.ortho(0.0f,1.0f,0.0f,1.0f,-1.0f,1.0f);
    _handles->setGeometry(hx0, hy0, hx1, hy1);
    _handles->paint(*this);
  }

  painter.endNativePainting();
  painter.end();

}

//////////////////////////////////////////////////////////////////////
void GuiGLWidget::resizeGL(int /*w*/, int /*h*/ ) {
  setProjectionMatrix(_vAngle);
}

//////////////////////////////////////////////////////////////////////
void GuiGLWidget::enterEvent(QEnterEvent* /*event*/) {
  _mouseInside = true;
}

//////////////////////////////////////////////////////////////////////
void GuiGLWidget::leaveEvent(QEvent* /*event*/) {
  _mouseInside = false;
  _mouseZone = -1;
  _mainWindow->showStatusBarMessage("");
}

//////////////////////////////////////////////////////////////////////
void GuiGLWidget::mousePressEvent(QMouseEvent * event) {
  _mousePressed = true;
  _mainWindow->timerStop();
  _buttons = event->buttons();
  int x       = event->position().x(); _pressedMouseX = _prevMouseX = x;
  int y       = event->position().y(); _pressedMouseY = _prevMouseY = y;
  int codeX   = (x<_borderLeft)?0:(x>=width() -_borderRight)?2:1;
  int codeY   = (y<_borderUp  )?0:(y>=height()-_borderDown )?2:1;
  _mouseZone = codeX+3*codeY;
  Qt::MouseButtons buttons = event->buttons();
  if(buttons & Qt::LeftButton) {
    switch(_mouseZone) {
    case 1:
      if(_zone4enabled)
        _mainWindow->showStatusBarMessage("Rotating Light Source");
      break;
    case 3:
      _mainWindow->showStatusBarMessage("Rotating Object");
      break;
    case 4:
      if(_zone4enabled)
        _mainWindow->showStatusBarMessage("Rotating Object");
      break;
    case 5:
      _mainWindow->showStatusBarMessage("Zooming");
      break;
    case 7:
      _mainWindow->showStatusBarMessage("Rotating Object");
      break;
    default:
      _mainWindow->showStatusBarMessage("");
      break;
    }
  // } else if(buttons & Qt::MidButton) {
  } else if(buttons & Qt::RightButton) {
    switch(_mouseZone) {
    case 3:
      _mainWindow->showStatusBarMessage("Translating Object");
      break;
    case 4:
      if(_zone4enabled) {
        _mainWindow->showStatusBarMessage("Rotating Light Source");
      }
      break;
    case 7:
      _mainWindow->showStatusBarMessage("Translating Object");
      break;
    default:
        _mainWindow->showStatusBarMessage("");
      break;
    }
  }
}

//////////////////////////////////////////////////////////////////////
void GuiGLWidget::mouseReleaseEvent(QMouseEvent* event) {
  (void)event;

  // int x = event->x();
  // int y = event->y();

  // TODO : implement picking
  // Mon Oct 24 01:20:56 2016

  // if(x==_pressedMouseX && y==_pressedMouseY) {
  //   cout << "void GuiGLWidget::mouseClicked() {\n";
  //
  //   glFlush();
  //   glFinish();
  //   glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  //
  //   union {
  //     unsigned int  value;
  //     unsigned char bgra[4]; /* 0=b 1=g 2=r 3=a */
  //   } p;
  //
  //   int yy = y; // width()-1-y;
  //
  //   glReadPixels(x,yy,1,1, GL_RGBA, GL_UNSIGNED_BYTE, p.bgra);
  //
  //   cout << "  argb(" << x << "," << yy << ") = 0x"
  //        << setbase(16) << p.value << "\n";
  //
  //   float depth;
  //   glReadPixels(x,yy,1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&depth);
  //    
  //   cout << "  depth(" << x << "," << yy << ") = "<< depth <<"\n";
  //   cout << "}\n";
  // }

  switch(_mouseZone) {
  case 0:
    _setHomeView(false);
    break;
  case 2:
    invertNormal();
    break;
  case 6:
    _mainWindow->showStatusBarMessage("Stoping Animation");
    _mainWindow->timerStop();
    _animationOn = false;
    break;
  case 8:
    _mainWindow->showStatusBarMessage("Restarting Animation");
    _mainWindow->timerStart();
    _animationOn = true;
    break;
  default:
    break;
  }
  _mainWindow->showStatusBarMessage("");
  _mousePressed = false;
  _mainWindow->timerStart();
  update();
}

//////////////////////////////////////////////////////////////////////
void GuiGLWidget::mouseMoveEvent(QMouseEvent * event) {

  if(_mouseInside==false) return;

  int x      = event->position().x();
  int y      = event->position().y();
  int codeX  = (x<_borderLeft)?0:(x>=width() -_borderRight)?2:1;
  int codeY  = (y<_borderUp  )?0:(y>=height()-_borderDown )?2:1;
  _mouseZone = codeX+3*codeY;

  if(_mousePressed) {

    int   dx     = (x-_prevMouseX); _prevMouseX = x;
    int   dy     = (_prevMouseY-y); _prevMouseY = y;

    float fdx    = ((float)dx/(float)width() );
    float thetaY =  180.0f*fdx;
    float fdy    = ((float)dy/(float)height());
    float thetaX = -180.0f*fdy;

    float ux = -fdy;
    float uy =  fdx;
    float uz = 0.0f;
    float uu = fdx*fdx+fdy*fdy;
    if(uu>0.0f) { uu = sqrt(uu); }
    ux /= uu; uy /= uu;
    float thetaXY = 180.0f*uu;

    float dxT = -0.5f * dx * _translateStep;
    float dyT = -0.5f * dy * _translateStep;
    float dzT = -8.0f * dy * _translateStep;

    Qt::MouseButtons buttons = event->buttons();

    QVector3D  translation;
    QMatrix4x4 sceneRotation;
    QMatrix4x4 lightRotation;

    if(buttons & Qt::LeftButton) {
      switch(_mouseZone) {
      case 1:
        if(_zone4enabled)
          lightRotation.rotate(thetaXY,ux,uy,uz);
        break;
      case 3:
        sceneRotation.rotate(thetaX,1.0f,0.0f,0.0f);
        break;
      case 4:
        if(_zone4enabled)
          sceneRotation.rotate(thetaXY,ux,uy,uz);
        break;
      case 5:
        _zoom(dzT);
        break;
      case 7:
        sceneRotation.rotate(thetaY,0.0f,1.0f,0.0f);
        break;
      default:
        break;
      }
    // } else if(buttons & Qt::MidButton) {
    } else if(buttons & Qt::RightButton) {
      switch(_mouseZone) {
      case 3:
        translation += QVector3D(0,-dyT,0);
        break;
      case 4:
        if(_zone4enabled)
          lightRotation.rotate(thetaXY,ux,uy,uz);
        break;
      case 7:
        translation += QVector3D(-dxT,0,0);
        break;
      default:
        break;
      }
    }

    _viewRotation = sceneRotation * _viewRotation;
    _lightSource = lightRotation.map(_lightSource);
    _cameraTranslation += translation;

    update();

  } else {

    switch(_mouseZone) {
    case 0:
      _mainWindow->showStatusBarMessage
        ("HOME");
      break;
    case 1:
      _mainWindow->showStatusBarMessage
        ("");
      break;
    case 2:
      _mainWindow->showStatusBarMessage
        ("Invert Normals");
      break;
    case 3:
      _mainWindow->showStatusBarMessage
        ("Rotate Around Horizontal Axis | Translate Left-Right");
      break;
    case 4:
      _mainWindow->showStatusBarMessage
        ("Rotate Object With Respect To Center");
      break;
    case 5:
      _mainWindow->showStatusBarMessage
        ("Translate Object Along Viewing Direction");
      break;
    case 6:
      _mainWindow->showStatusBarMessage
        ("Stop Animation");
      break;
    case 7:
      _mainWindow->showStatusBarMessage
        ("Rotate Around Vertical Axis | Translate Up-Down");
      break;
    case 8:
      _mainWindow->showStatusBarMessage
        ("Resume Animation");
      break;
    default:
      _mainWindow->showStatusBarMessage
        ("");
      break;
    }

  }
}

//////////////////////////////////////////////////////////////////////
void GuiGLWidget::wheelEvent(QWheelEvent *event) {

  QPoint numPixels  = event->pixelDelta();
  QPoint numDegrees = event->angleDelta() / 8;

  bool up = false;
  bool dn = false;

  if (!numPixels.isNull()) {
    up = (numPixels.y()>0);
    dn = (numPixels.y()<0);
  } else if (!numDegrees.isNull()) {
    QPoint numSteps = numDegrees / 15;
    up = (numSteps.y()>0);
    dn = (numSteps.y()<0);
  }
  if(up) {
    // float dzT = 80.0f * _translateStep;
    // _zoom(dzT);
    _vAngle *= 1.05f;
    setProjectionMatrix(_vAngle);
    update();
  } else if(dn) {
    // float dzT = -80.0f * _translateStep;
    // _zoom(dzT);
    _vAngle /= 1.05f;
    setProjectionMatrix(_vAngle);
    update();
  }

  event->accept();
}

//////////////////////////////////////////////////////////////////////
void GuiGLWidget::_zoom(const float value) {
  QVector3D v(0.0f,0.0f,value);
  _eye += v;
  float eyeToCenterDistance = _eye.z()-_center.z();
  if(eyeToCenterDistance<0.25*_bboxDiameter) {
    eyeToCenterDistance = 0.25*_bboxDiameter;
    _eye.setZ(_center.z()+eyeToCenterDistance);
  }
}

//////////////////////////////////////////////////////////////////////
void GuiGLWidget::_setHomeView(const bool identity) {

  if(identity) {
    _viewRotation.setToIdentity();
  } else {
    QMatrix4x4 Rx;
    Rx.rotate(_angleHomeX, 1.0f, 0.0f, 0.0f);
    QMatrix4x4 Ry;
    Ry.rotate(_angleHomeY, 0.0f, 1.0f, 0.0f);
    QMatrix4x4 Rz;
    Rz.rotate(_angleHomeZ, 0.0f, 0.0f, 1.0f);
    _viewRotation = Rx*Ry*Rz;
  }

  _fAngle = 0.0f;
  float eyeToCenterDistance = 5.0f*_bboxDiameter;
  _eye = _center+QVector3D(0,0,eyeToCenterDistance);
}

//////////////////////////////////////////////////////////////////////
void GuiGLWidget::setProjectionMatrix(const float verticalAngle) {

  float w             = (float)width();
  float h             = (float)height();
  float aspectRatio   = w/h;
  float near          =   1.0f*_bboxDiameter;
  float far           =  20.0f*_bboxDiameter; // 100.0f*_bboxDiameter;

  // void QMatrix4x4::perspective
  //   (float verticalAngle, float aspectRatio,
  //     float nearPlane, float farPlane);
  //
  // Multiplies this matrix by another that applies a perspective
  // projection. The vertical field of view will be verticalAngle
  // degrees within a window with a given aspectRatio that determines
  // the horizontal field of view. The projection will have the
  // specified nearPlane and farPlane clipping planes which are the
  // distances from the viewer to the corresponding planes.

  QMatrix4x4 perspectiveMatrix;
  perspectiveMatrix.setToIdentity();
  perspectiveMatrix.perspective(verticalAngle, aspectRatio, near, far);

  _projectionMatrix = perspectiveMatrix;
}
