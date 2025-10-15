//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-08-06 09:56:49 taubin>
//------------------------------------------------------------------------
//
// Variable.hpp
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

#ifndef _VARIABLE_HPP_
#define _VARIABLE_HPP_

#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <wrl/Types.hpp>
#include "Faces.hpp"
#include "PolygonMesh.hpp"

using namespace std;

// abstract class
class Variable {
public:
  Variable(const string& name):_name(name) {}
  virtual ~Variable() {}
  const string& getName() { return _name; }
  virtual void* getValue() = 0;
private:
  string _name;
};

class VariablePointer : public Variable {
public:
  VariablePointer(const string& name, void* value=(void*)0);
  virtual ~VariablePointer() {}
  virtual void* getValue(); //
private:
  void* _value;
};

class VariableBool : public Variable {
public:
  VariableBool(const string& name, bool initValue=false);
  virtual ~VariableBool() {}
  virtual void* getValue(); //
private:
  bool _value;
};

class VariableInt : public Variable {
public:
  VariableInt(const string& name, int initValue=0);
  virtual ~VariableInt() {}
  virtual void* getValue();
private:
  int _value;
};

class VariableFloat : public Variable {
public:
  VariableFloat(const string& name, float initValue=0);
  virtual ~VariableFloat() {}
  virtual void* getValue();
private:
  float _value;
};

// class VariableQColor : public Variable {
// public:
//   VariableQColor(const string& name, int r=0, int g=0, int b=0, int a=255);
//   VariableQColor(const string& name, const QColor& color);
//   virtual ~VariableQColor() {}
//   virtual void* getValue();
// private:
//   QColor _value;
// };

class VariableColor : public Variable {
public:
  VariableColor(const string& name, float r=0, float g=0, float b=0);
  VariableColor(const string& name, const Color& color);
  virtual ~VariableColor() {}
  virtual void* getValue();
private:
  Color _value;
};

class VariableVecInt : public Variable {
public:
  VariableVecInt(const string& name,
                 size_t length=0, int initValue=0);
  virtual ~VariableVecInt() {}
  virtual void* getValue();
private:
  vector<int> _value;
};

class VariableVecFloat : public Variable {
public:
  VariableVecFloat(const string& name,
                   size_t length=0, float initValue=0);
  virtual ~VariableVecFloat() {}
  virtual void* getValue();
private:
  vector<float> _value;
};

class VariableFaces : public Variable {
public:
  VariableFaces(const string& name, const int nV, const vector<int>& coordIndex);
  virtual ~VariableFaces() {}
  virtual void* getValue();
private:
  Faces _value;
};
  
class VariableHalfEdges : public Variable {
public:
  VariableHalfEdges(const int nV, const vector<int>& coordIndex);
  virtual ~VariableHalfEdges() {}
  virtual void* getValue();
private:
  HalfEdges _value;
};
  
class VariablePolygonMesh : public Variable {
public:
  VariablePolygonMesh(const int nV, const vector<int>& coordIndex);
  virtual ~VariablePolygonMesh() {}
  virtual void* getValue();
private:
  PolygonMesh _value;
};

#endif // _VARIABLE_HPP_
