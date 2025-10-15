//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-08-06 09:56:48 taubin>
//------------------------------------------------------------------------
//
// Variable.cpp
//
// Software developed for the course
// Digital Geometry Processing
// Copyright (c) 2015, Gabriel Taubin
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

#include "Variable.hpp"

VariablePointer::VariablePointer(const string& name, void* value):
  Variable(name), _value(value) {
}

void* VariablePointer::getValue() {
  return (void*)(&_value);
}

VariableBool::VariableBool(const string& name, bool initValue):
  Variable(name),_value(initValue) {
}
void* VariableBool::getValue() {
  return (void*)(&_value);
}

VariableInt::VariableInt(const string& name, int initValue):
  Variable(name),_value(initValue) {
}
void* VariableInt::getValue() {
  return (void*)(&_value);
}

VariableFloat::VariableFloat(const string& name, float initValue):
  Variable(name),_value(initValue) {
}
void* VariableFloat::getValue() {
  return (void*)(&_value);
}

VariableColor::VariableColor(const string& name, float r, float g, float b):
  Variable(name), _value(r,g,b) {
}
VariableColor::VariableColor(const string& name, const Color& color):
  Variable(name), _value(color) {
}
void* VariableColor::getValue() {
  return (void*)(&_value);
}

VariableVecInt::VariableVecInt(const string& name, size_t length, int initValue):
  Variable(name),_value() {
  if(length>0) _value.reserve(length);
  while((length--)>0) {
    _value.push_back(initValue);
  }
}
void* VariableVecInt::getValue() {
  return (void*)(&_value);
}

VariableVecFloat::VariableVecFloat
(const string& name, size_t length, float initValue):
  Variable(name),_value() {
  if(length>0) _value.reserve(length);
  while((length--)>0) {
    _value.push_back(initValue);
  }
}
void* VariableVecFloat::getValue() {
  return (void*)(&_value);
}

VariableFaces::VariableFaces
(const string& name, const int nV, const vector<int>& coordIndex):
  Variable(name),_value(nV,coordIndex) {
}
void* VariableFaces::getValue() {
  return (void*)(&_value);
}

VariableHalfEdges::VariableHalfEdges
(const int nV, const vector<int>& coordIndex):
  Variable("HalfEdges"),_value(nV,coordIndex) {
}
void* VariableHalfEdges::getValue() {
  return (void*)(&_value);
}

VariablePolygonMesh::VariablePolygonMesh
(const int nV, const vector<int>& coordIndex):
  Variable("PolygonMesh"),_value(nV,coordIndex) {
}
void* VariablePolygonMesh::getValue() {
  return (void*)(&_value);
}
