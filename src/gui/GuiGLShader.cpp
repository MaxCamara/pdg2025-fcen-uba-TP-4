//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-08-06 09:57:02 taubin>
//------------------------------------------------------------------------
//
// GuiGLShader.cpp
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
#include "GuiGLShader.hpp"

const char *GuiGLShader::s_vsMaterial =
  "attribute highp vec4 vertex;\n"
  "uniform  mediump float pointsize;\n"
  "uniform  mediump float linewidth;\n"
  "uniform mediump mat4 mvpmatrix;\n"
  "uniform mediump vec4 matcolor;\n"
  "varying mediump vec4 color;\n"
  "void main(void) {\n"
  "  color = matcolor;\n"
  "  gl_Position = mvpmatrix * vertex;\n"
  "  gl_PointSize = pointsize;\n"
  "}\n";

const char *GuiGLShader::s_vsMaterialNormal =
  "attribute highp vec4 vertex;\n"
  "attribute mediump vec3 vnormal;\n"
  "uniform mediump float pointsize;\n"
  "uniform mediump float linewidth;\n"
  "uniform mediump mat4 mvpmatrix;\n"
  "uniform mediump vec4 matcolor;\n"
  "uniform mediump vec3 lightsource;\n"
  "varying mediump vec4 color;\n"
  "void main(void) {\n"
  "  vec3 toLight = normalize(lightsource);\n"
  "  float angle = max(dot(vnormal, toLight), 0.0);\n"
  "  vec3 col = vec3(matcolor);\n"
  "  color = vec4(col * 0.2 + col * 0.8 * angle, 1.0);\n"
  "  color = clamp(color, 0.0, 1.0);\n"
  "  gl_Position = mvpmatrix * vertex;\n"
  "  gl_PointSize = pointsize;\n"
  "}\n";

const char *GuiGLShader::s_vsColor =
  "attribute highp vec4 vertex;\n"
  "attribute mediump vec4 vcolor;\n"
  "uniform mediump float pointsize;\n"
  "uniform mediump float linewidth;\n"
  "uniform mediump mat4 mvpmatrix;\n"
  "varying mediump vec4 color;\n"
  "void main(void) {\n"
  "  color = vcolor;\n"
  "  gl_Position = mvpmatrix * vertex;\n"
  "  gl_PointSize = pointsize;\n"
  "}\n";

const char *GuiGLShader::s_vsColorNormal =
  "attribute highp vec4 vertex;\n"
  "attribute mediump vec3 vnormal;\n"
  "attribute mediump vec4 vcolor;\n"
  "uniform mediump float pointsize;\n"
  "uniform mediump float linewidth;\n"
  "uniform mediump mat4 mvpmatrix;\n"
  "uniform mediump vec3 lightsource;\n"
  "varying mediump vec4 color;\n"
  "void main(void) {\n"
  "  vec3 toLight = normalize(lightsource);\n"
  "  float angle = max(dot(vnormal, toLight), 0.0);\n"
  "  vec3 col = vec3(vcolor);\n"
  "  color = vec4(col * 0.2 + col * 0.8 * angle, 1.0);\n"
  "  color = clamp(color, 0.0, 1.0);\n"
  "  gl_Position = mvpmatrix * vertex;\n"
  "  gl_PointSize = pointsize;\n"
  "}\n";

//////////////////////////////////////////////////////////////////////
const char *GuiGLShader::s_fsColor =
  "varying mediump vec4 color;\n"
  "void main(void) {\n"
  "  gl_FragColor = color;\n"
  "}\n";

//////////////////////////////////////////////////////////////////////
GuiGLShader::GuiGLShader(QColor& materialColor, QVector3D* lightSource):
  _vshader((QOpenGLShader*)0),
  _fshader((QOpenGLShader*)0),
  _program((QOpenGLShaderProgram*)0),
  _pointSizeAttr(-1),
  _vertexAttr(-1),
  _normalAttr(-1),
  _colorAttr(-1),
  _mvpMatrixAttr(-1),
  _materialAttr(-1),
  _lightSourceAttr(-1),
  _vertexBuffer((GuiGLBuffer*)0),
  _materialColor(materialColor),
  _lightSource(lightSource),
  _pointSize(4.0f) {
  _mvpMatrix.setToIdentity();
}

//////////////////////////////////////////////////////////////////////
GuiGLShader::~GuiGLShader() {
  delete _program;
  delete _vshader;
  delete _fshader;
  if(_vertexBuffer==(GuiGLBuffer*)0) return;
  _vertexBuffer->destroy();
  delete _vertexBuffer;
  _vertexBuffer = (GuiGLBuffer*)0;
}

//////////////////////////////////////////////////////////////////////
void GuiGLShader::setPointSize(float pointSize) {
  _pointSize = (pointSize<0.0f)?0.0f:pointSize;
}

//////////////////////////////////////////////////////////////////////
int GuiGLShader::getNumberOfVertices() {
  return (_vertexBuffer)?_vertexBuffer->getNumberOfVertices():0;
}

//////////////////////////////////////////////////////////////////////
QMatrix4x4& GuiGLShader::getMVPMatrix() {
  return _mvpMatrix;
}

//////////////////////////////////////////////////////////////////////
void GuiGLShader::setMVPMatrix(const QMatrix4x4& mvp) {
  _mvpMatrix = mvp;
}

//////////////////////////////////////////////////////////////////////
GuiGLBuffer* GuiGLShader::getVertexBuffer() const {
  return _vertexBuffer;
}

//////////////////////////////////////////////////////////////////////
void GuiGLShader::setVertexBuffer(GuiGLBuffer* vb) {

  _vertexBuffer = vb;
  if(_vertexBuffer==(GuiGLBuffer*)0) return;

  GuiGLBuffer::Type type = _vertexBuffer->getType();

  // create the vertex shader
  _vshader = new QOpenGLShader(QOpenGLShader::Vertex);
  switch(type) {
  case GuiGLBuffer::Type::MATERIAL:
    _vshader->compileSourceCode(s_vsMaterial);
    break;
  case GuiGLBuffer::Type::MATERIAL_NORMAL:
    _vshader->compileSourceCode(s_vsMaterialNormal);
    break;
  case GuiGLBuffer::Type::COLOR:
    _vshader->compileSourceCode(s_vsColor);
    break;
  case GuiGLBuffer::Type::COLOR_NORMAL:
    _vshader->compileSourceCode(s_vsColorNormal);
    break;
  }

  // create the fragment shader
  _fshader = new QOpenGLShader(QOpenGLShader::Fragment);
  _fshader->compileSourceCode(s_fsColor);

  // create the shader program
  _program = new QOpenGLShaderProgram;
  _program->addShader(_vshader);
  _program->addShader(_fshader);
  _program->link();

  _pointSizeAttr       = -1;
  _vertexAttr          = -1;
  _normalAttr          = -1;
  _colorAttr           = -1;
  _mvpMatrixAttr       = -1;
  _materialAttr        = -1;
  _lightSourceAttr     = -1;

  _pointSizeAttr         = _program->uniformLocation("pointsize");
  _vertexAttr            = _program->attributeLocation("vertex");
  switch(type) {
  case GuiGLBuffer::Type::MATERIAL:
    _materialAttr        = _program->uniformLocation("matcolor");
    break;
  case GuiGLBuffer::Type::MATERIAL_NORMAL:
    _materialAttr        = _program->uniformLocation("matcolor");
    _normalAttr          = _program->attributeLocation("vnormal");
    break;
  case GuiGLBuffer::Type::COLOR:
    _colorAttr           = _program->attributeLocation("vcolor");
    break;
  case GuiGLBuffer::Type::COLOR_NORMAL:
    _colorAttr           = _program->attributeLocation("vcolor");
    _normalAttr          = _program->attributeLocation("vnormal");
    break;
  }
  _mvpMatrixAttr         = _program->uniformLocation("mvpmatrix");
  _lightSourceAttr       = _program->uniformLocation("lightsource");
}

//////////////////////////////////////////////////////////////////////
void GuiGLShader::paint(QOpenGLFunctions& f) {

  if(_vertexBuffer==(GuiGLBuffer*)0) return;

  GuiGLBuffer::Type type = _vertexBuffer->getType();

  _program->bind();

  _program->setUniformValue(_mvpMatrixAttr, _mvpMatrix);
  _program->setUniformValue(_lightSourceAttr, *_lightSource);

  _program->setUniformValue(_pointSizeAttr, _pointSize);
  _program->enableAttributeArray(_vertexAttr);
  switch(type) {
  case GuiGLBuffer::Type::MATERIAL:
    _program->setUniformValue(_materialAttr, _materialColor);
    break;
  case GuiGLBuffer::Type::MATERIAL_NORMAL:
    _program->setUniformValue(_materialAttr, _materialColor);
    _program->enableAttributeArray(_normalAttr);
    break;
  case GuiGLBuffer::Type::COLOR:
    _program->enableAttributeArray(_colorAttr);
    break;
  case GuiGLBuffer::Type::COLOR_NORMAL:
    _program->enableAttributeArray(_colorAttr);
    _program->enableAttributeArray(_normalAttr);
    break;
  }
  
  _vertexBuffer->bind();

  switch(type) {
  case GuiGLBuffer::Type::MATERIAL:
    _program->setAttributeBuffer
      (_vertexAttr, GL_FLOAT,                 0, 3, 3*sizeof(GLfloat));
    break;
  case GuiGLBuffer::Type::MATERIAL_NORMAL:
    _program->setAttributeBuffer
      (_vertexAttr, GL_FLOAT,                 0, 3, 6*sizeof(GLfloat));
    _program->setAttributeBuffer
      (_normalAttr, GL_FLOAT, 3*sizeof(GLfloat), 3, 6*sizeof(GLfloat));
    break;
  case GuiGLBuffer::Type::COLOR:
    _program->setAttributeBuffer
      (_vertexAttr, GL_FLOAT,                 0, 3, 6*sizeof(GLfloat));
    _program->setAttributeBuffer
      ( _colorAttr, GL_FLOAT, 3*sizeof(GLfloat), 3, 6*sizeof(GLfloat));
    break;
  case GuiGLBuffer::Type::COLOR_NORMAL:
    _program->setAttributeBuffer
      (_vertexAttr, GL_FLOAT,                 0, 3, 9*sizeof(GLfloat));
    _program->setAttributeBuffer
      (_normalAttr, GL_FLOAT, 3*sizeof(GLfloat), 3, 9*sizeof(GLfloat));
    _program->setAttributeBuffer
      ( _colorAttr, GL_FLOAT, 6*sizeof(GLfloat), 3, 9*sizeof(GLfloat));
    break;
  }

  _vertexBuffer->release();

  int nVertices =  getNumberOfVertices();
  GuiGLBuffer::PaintMode paintMode = _vertexBuffer->getPaintMode();
  switch(paintMode) {
  case GuiGLBuffer::PaintMode::TRIANGLES:
    f.glDrawArrays(GL_TRIANGLES, 0, nVertices);
    break;
  case GuiGLBuffer::PaintMode::LINES:
    f.glDrawArrays(GL_LINES, 0, nVertices);
    break;
  case GuiGLBuffer::PaintMode::POINTS:
    f.glDrawArrays(GL_POINTS, 0, nVertices);
    break;
  }

  _program->disableAttributeArray(_vertexAttr);
  switch(type) {
  case GuiGLBuffer::Type::MATERIAL:
    break;
  case GuiGLBuffer::Type::MATERIAL_NORMAL:
    _program->disableAttributeArray(_normalAttr);
    break;
  case GuiGLBuffer::Type::COLOR:
    _program->disableAttributeArray(_colorAttr);
    break;
  case GuiGLBuffer::Type::COLOR_NORMAL:
    _program->disableAttributeArray(_colorAttr);
    _program->disableAttributeArray(_normalAttr);
    break;
  }

  _program->release();
}
