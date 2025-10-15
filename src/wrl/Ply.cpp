//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin / 3D Shape Tech LLC
//  Time-stamp: <2025-08-06 09:59:25 taubin>
//------------------------------------------------------------------------
//
// Plc.hpp
//
// Software developed for the course
// Digital Geometry Processing
// Copyright (c) 2025, Gabriel Taubin
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in
//   the documentation and/or other materials provided with the
//   distribution.
// * Neither the name of the Brown University nor the names of its
//   contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
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

#include <iostream>
#include "Ply.hpp"
#include <io/StrException.hpp>
#include <util/Endian.hpp>
#include <util/CastMacros.hpp>

using namespace std;

// static
bool          Ply::_debug           = true;
bool          Ply::_skipComments    = false;
string        Ply::_floatFormat     = "%f";
string        Ply::_intFormat       = "%d";
Ply::DataType Ply::_defaultDataType = Ply::DataType::NONE;
bool          Ply::_defaultWrlMode  = true; // false;

// static
void Ply::setDebug(const bool value) {
  _debug = value;
}

// static
void Ply::setFloatFormat(const string fmt) {
  _floatFormat = fmt;
}

// static
void Ply::setIntFormat(const string fmt) {
  _intFormat = fmt;
}

// static
void Ply::setSkipComments(const bool value) {
  _skipComments = value;
}

// static
bool Ply::getSkipComments() {
  return _skipComments;
}

// static
Ply::DataType Ply::getDefaultDataType() {
  return _defaultDataType;
}

// static
const string Ply::getDataTypeName(const DataType dataType) {
  switch (dataType) {
    case NONE:                 return "NONE";
    case ASCII:                return "ASCII";
    case BINARY_LITTLE_ENDIAN: return "BINARY_LITTLE_ENDIAN";
    case BINARY_BIG_ENDIAN:    return "BINARY_BIG_ENDIAN";
  }
}

// static
void Ply::setDefaultDataType(const DataType dataType) {
  _defaultDataType = dataType;
}

// static
void Ply::setDefaultWrlMode(const bool value) {
  _defaultWrlMode = value;
}

// static
bool Ply::getDefaultWrlMode() {
    return _defaultWrlMode;
}

Ply::Ply():
  _dataType(Ply::DataType::NONE),
  _textureFile(""),
  _wrlMode(_defaultWrlMode), // cannot be changed after construction
  _coord(nullptr),
  _coordIndex(nullptr),
  _normalPerVertex(true),
  _normal(nullptr),
  _colorPerVertex(true),
  _color(nullptr),
  _texCoord(nullptr) {
}
  
Ply::~Ply() {
  clear();
}

void Ply::clear() {
  while(_element.size()>0) {
    delete _element.back();
    _element.pop_back();
  }
  _element.clear();
  _comment.clear();
  _dataType = Ply::DataType::NONE;
}

void Ply::setTextureFile(const string path) {
  _textureFile = path;
}

string Ply::getTextureFile() {
  return _textureFile;
}

void Ply::setDataType(const DataType& dataType) {
  _dataType = dataType;
}

Ply::DataType Ply::getDataType() {
  return _dataType;
}

const string Ply::getDataTypeName() {
  return getDataTypeName(_dataType);
}

const vector<string>& Ply::getComments() {
  return _comment;
}

void Ply::addComment(const string& comment) {
  if(comment=="") return;
  _comment.push_back(comment); 
}

void Ply::addObjInfo(const string& objInfo) {
  if(objInfo=="") return;
  _objInfo.push_back(objInfo);
}

int Ply::getNumberOfElements() {
  return static_cast<int>(_element.size());
}

Ply::Element* Ply::addElement(const string& name, const int nRecords) {
  Ply::Element* e = new Ply::Element(name,nRecords,*this);
  _element.push_back(e);
  return e;
}

Ply::Element* Ply::getElement(const int i) {
  Ply::Element* element_i = nullptr;
  int nElements = getNumberOfElements();
  if(0<=i && i<nElements)
    element_i = _element[UL(i)];
  return element_i;
}

Ply::Element* Ply::getElement(const string& name) {
  Ply::Element* element_name = nullptr;
  int nElements = getNumberOfElements();
  for(int i=0;i<nElements;i++) {
    Ply::Element* element_i = _element[UL(i)];
    if(name == element_i->getName()) {
      element_name = element_i;
      break;
    }
  }
  return element_name;
}

int Ply::getNumberOfElementRecords(const string& name) {
  Ply::Element* element = getElement(name);
  return (element!=nullptr)?element->getNumberOfRecords():0;
}

bool Ply::getWrlMode() {
  return _wrlMode;
}

int Ply::getNumberOfVertices() {
  return getNumberOfElementRecords("vertex");
}

int Ply::getNumberOfFaces() {
  return getNumberOfElementRecords("face");
}

bool Ply::hasMaterial() {
  Ply::Element* material = getElement("material");
  return (material!=nullptr);
}

bool Ply::hasColorPerVertex() {
  Ply::Element* vertex = getElement("vertex");
  if(vertex==nullptr) return false;
  int nVertices = vertex->getNumberOfRecords();
  if(_wrlMode) {
    return
      (_colorPerVertex==true &&
       _color!=nullptr &&
       _color->size()==UL(3*nVertices));
  } else {
    return
      (vertex->getProperty("red")  !=nullptr &&
       vertex->getProperty("green")!=nullptr &&
       vertex->getProperty("blue") !=nullptr);
  }
}

bool Ply::hasColorPerFace() {
  Ply::Element* face = getElement("face");
  if(face==nullptr) return false;
  int nFaces = face->getNumberOfRecords();
  if(_wrlMode) {
    return
      (_colorPerVertex==false &&
       _color!=nullptr &&
       _color->size()==UL(3*nFaces));
  } else {
    return
      (face->getProperty("red")  !=nullptr &&
       face->getProperty("green")!=nullptr &&
       face->getProperty("blue") !=nullptr);
  }
}

bool Ply::hasColorPerCorner() {
  // Ply::Element* face = getElement("face");
  // return (face->getProperty("color")!=nullptr);
  return false;
}

bool Ply::hasColor() {
  return
    (hasColorPerVertex() || hasColorPerFace() || hasColorPerCorner());
}

bool Ply::hasNormalPerVertex() {
  Ply::Element* vertex = getElement("vertex");
  if(vertex==nullptr) return false;
  int nVertices = vertex->getNumberOfRecords();
  if(_wrlMode) {
    return
      (_normalPerVertex==true &&
       _normal!=nullptr &&
       _normal->size()==UL(3*nVertices));
  } else {
    return
      (vertex->getProperty("nx")!=nullptr &&
       vertex->getProperty("ny")!=nullptr &&
       vertex->getProperty("nz")!=nullptr);
  }
}

bool Ply::hasNormalPerFace() {
  Ply::Element* face = getElement("face");
  if(face==nullptr) return false;
  int nFaces = face->getNumberOfRecords();
  if(_wrlMode) {
    return
      (_normalPerVertex==false &&
       _normal!=nullptr &&
       _normal->size()==UL(3*nFaces));
  } else {
    return
      (face->getProperty("nx")!=nullptr &&
       face->getProperty("ny")!=nullptr &&
       face->getProperty("nz")!=nullptr);
  }
}

bool Ply::hasNormalPerCorner() {
  // Ply::Element* face = getElement("face");
  // return (face->getProperty("normal")!=nullptr);
  return false;
}

bool Ply::hasNormal() {
  return
    (hasNormalPerVertex() || hasNormalPerFace() || hasNormalPerCorner());
}

bool Ply::hasTexCoordPerVertex() {
  Ply::Element* vertex = getElement("vertex");
  if(vertex==nullptr) return false;
  int nVertices = vertex->getNumberOfRecords();
  if(_wrlMode) {
    return
      (_texCoord!=nullptr && _texCoord->size() == UL(2*nVertices));
  } else {
    return
      (vertex->getProperty("u")!=nullptr &&
       vertex->getProperty("v")!=nullptr);
  }
}

bool Ply::hasTexCoordPerCorner() {
  // Ply::Element* face = getElement("face");
  // return (face->getProperty("texcoord")!=nullptr);
  return false;
}

bool Ply::hasTexCoord() {
  return (hasTexCoordPerVertex() || hasTexCoordPerCorner());
}

bool Ply::isTextured() {
  return (hasTexCoord() && _textureFile!="");
}

//////////////////////////////////////////////////////////////////////
void Ply::logInfo(ostream & ostr, const string indent) {

  ostr << indent << "Ply::logInfo() {" << endl;

  try {

    Ply::Element* element;
    Ply::Element::Property* property;
    Ply::Element::Property::Type propertyType;
    Ply::Element::Property::Type listType;
    void* propertyValue;
    int iElement,i0,i1;
    int iProperty,iRecord,nElements,nProperties,nRecords,propertySize;
    string elementName,propertyName;

    nElements = getNumberOfElements();

    ostr << indent << "  wrlMode   = " << ((_wrlMode)?"true":"false") << endl;
    if(_wrlMode) {

      if(hasMaterial()) {
        ostr << indent << "    has material" << endl;
      } else {
        ostr << indent << "    does not have material" << endl;
      }

      int nVertices = getNumberOfVertices();
      ostr << indent << "    has " << nVertices << " vertices" << endl;
      int nFaces = getNumberOfFaces();

      if(nFaces>0) {
        ostr << indent << "    has " << nFaces << " faces" << endl;
      } else {
        ostr << indent << "    does not have faces" << endl;
      }

      if(hasColor()) {
        if(hasColorPerVertex()) {
          ostr << indent << "    has color per vertex"<< endl;
        } else if(hasColorPerFace()) {
          ostr << indent << "    has color per face"<< endl;
        } else /* if(hasColorPerCorner()) */ {
          ostr << indent << "    has color per corner"<< endl;
        }
      } else {
        ostr << indent << "    does not have color"<< endl;
      }

      if(hasNormal()) {
        if(hasNormalPerVertex()) {
          ostr << indent << "    has normal per vertex"<< endl;
        } else if(hasNormalPerFace()) {
          ostr << indent << "    has normal per face"<< endl;
        } else /* if(hasNormalPerCorner()) */ {
          ostr << indent << "    has normal per corner"<< endl;
        }
      } else {
        ostr << indent << "    does not have normal"<< endl;
      }

      if(hasTexCoord()) {
        if(hasTexCoordPerVertex()) {
        ostr << indent << "    has texCoord per vertex"<< endl;
        } else /* hasTexCoordPerCorner() */ {
        ostr << indent << "    has texCoord per corner"<< endl;
        }
      } else {
        ostr << indent << "    does not have texCoord"<< endl;
      }

      if(isTextured()) {
        ostr << indent << "    is textured"<< endl;
      } else {
        ostr << indent << "    is not textured"<< endl;
      }

    } // } if(_wrlMode)

    ostr << indent << "  nElements = " << nElements << endl;

    for(iElement=0;iElement<nElements;iElement++) {

      element     = getElement(iElement);
      elementName = element->getName();
      ostr << indent << "    element " << iElement
           << " name =\"" << elementName << "\"" << endl;
      nRecords = element->getNumberOfRecords();
      ostr << indent << "      nRecords = " << nRecords << endl;
      nProperties = element->getNumberOfProperties();
      ostr << indent << "      nProperties = " << nProperties << endl;

      for(iProperty=0;iProperty<nProperties;iProperty++) {

        property      = element->getProperty(iProperty);
        propertyName  = property->getName();
        propertyType  = property->getPropertyType();
        propertyValue = property->getValue(); 

        if(property->isList()==true) {

          listType  = property->getListType();

          propertySize = 0;
          for(iRecord=0;iRecord<nRecords;iRecord++) {
            i0   = property->getListFirst(iRecord );
            i1   = property->getListFirst(iRecord+1);
            propertySize += (i1-i0+1);
          }

          ostr << indent
               << "      property[" << iProperty << "] = "
               << propertyName << ":list("
               << Ply::Element::Property::getTypeName(listType) << ","
               << Ply::Element::Property::getTypeName(propertyType)
               << ")[" << propertySize << "]"
               << endl;

        } else /* if(property.isList()==false) */ {

          propertySize = 0;
          switch(propertyType) {
          case Ply::Element::Property::Type::CHAR:
          case Ply::Element::Property::Type::INT8:
            propertySize =
              I(static_cast<vector<char>*>(propertyValue)->size());
            break;
          case Ply::Element::Property::Type::UCHAR:
          case Ply::Element::Property::Type::UINT8:
            propertySize =
              I(static_cast<vector<unsigned char>*>(propertyValue)->size());
            break;
          case Ply::Element::Property::Type::SHORT:
          case Ply::Element::Property::Type::INT16:
            propertySize =
              I(static_cast<vector<short>*>(propertyValue)->size());
            break;
          case Ply::Element::Property::Type::USHORT:
          case Ply::Element::Property::Type::UINT16:
            propertySize =
              I(static_cast<vector<unsigned short>*>(propertyValue)->size());
            break;
          case Ply::Element::Property::Type::INT:
          case Ply::Element::Property::Type::INT32:
            propertySize =
              I(static_cast<vector<int>*>(propertyValue)->size());
            break;
          case Ply::Element::Property::Type::UINT:
          case Ply::Element::Property::Type::UINT32:
            propertySize =
              I(static_cast<vector<unsigned int>*>(propertyValue)->size());
            break;
          case Ply::Element::Property::Type::FLOAT:
          case Ply::Element::Property::Type::FLOAT32:
          case Ply::Element::Property::Type::FLOAT32_2:
          case Ply::Element::Property::Type::FLOAT32_3:
            propertySize =
              I(static_cast<vector<float>*>(propertyValue)->size());
            break;
          case Ply::Element::Property::Type::DOUBLE:
          case Ply::Element::Property::Type::FLOAT64:
            propertySize =
              I(static_cast<vector<double>*>(propertyValue)->size());
            break;

          case Ply::Element::Property::Type::NONE:
              propertySize = 0;
          }

          ostr << indent
               << "      property[" << iProperty << "] = "
               << propertyName << ":"
               << Ply::Element::Property::getTypeName(propertyType)
               << "[" << propertySize << "]"
               << endl;

        } // if(property.isList()=={true|false})
      } // } for(iProperty=0;iProperty<nProperties;iProperty++)
    } // } for(iElement=0;iElement<nElements;iElement++)

  } catch (StrException* e) {
    ostr << indent << "ERROR _writeBinaryData() | " << e->what() << endl;
    delete e;
  }

  ostr << indent << "} Ply::logInfo()" << endl;
}

// class Ply::Element ////////////////////////////////////////////////

Ply::Element::Element(const string& name, const int nRecords, Ply& ply):
  _name(name),
  _nRecords(nRecords),
  _property(),
  _ply(ply) {
}

Ply::Element::~Element() {
  while(_property.size()>0) {
    delete _property.back();
    _property.pop_back();
  }
}

string Ply::Element::getName() {
  return _name;
}
  
int Ply::Element::getNumberOfRecords() {
  return _nRecords;
}

int Ply::Element::getNumberOfProperties() {
  return static_cast<int>(_property.size());
}

Ply::Element::Property*
Ply::Element::addProperty
(const string& name, const Property::Type type) {
  return addProperty(name,false,Property::Type::NONE,type);
}

Ply::Element::Property*
Ply::Element::addProperty
(const string&        name,
 const bool           list,
 const Property::Type listType,
 const Property::Type type) {

  const bool wrlMode = ply()._wrlMode;
  Property* p = nullptr;
  Property::Type typeWrl = Property::Type::NONE;

  if(wrlMode && _name=="vertex" && list==false &&
     (name=="x" || name=="y" || name=="z") &&
     (type==Property::Type::FLOAT || type==Property::Type::DOUBLE ||
      type==Property::Type::FLOAT32 || type==Property::Type::FLOAT64)) {
    typeWrl = Property::Type::FLOAT32_3;
    if((p=getProperty("coord"))==nullptr) {
      p = new Property("coord",false,Property::Type::NONE,typeWrl,*this);
      _ply._coord = static_cast<vector<float>*>(p->getValue());
      _property.push_back(p);
    }
  } else if(wrlMode && _name=="vertex" && list==false &&
            (name=="nx" || name=="ny" || name=="nz") &&
            (type==Property::Type::FLOAT || type==Property::Type::DOUBLE ||
             type==Property::Type::FLOAT32 || type==Property::Type::FLOAT64)) {
    typeWrl = Property::Type::FLOAT32_3;
    if((p=getProperty("normal"))==nullptr) {
      p = new Property("normal",false,Property::Type::NONE,typeWrl,*this);
      _ply._normalPerVertex = true;
      _ply._normal = static_cast<vector<float>*>(p->getValue());
      _property.push_back(p);
    }
  } else if(wrlMode && _name=="vertex" && list==false &&
            (name=="red" || name=="green" || name=="blue") &&
            (type==Property::Type::UCHAR || type==Property::Type::UINT8)) {
    typeWrl = Property::Type::FLOAT32_3;
    if((p=getProperty("color"))==nullptr) {
      p = new Property("color",false,Property::Type::NONE,typeWrl,*this);
      _ply._colorPerVertex = true;
      _ply._color = static_cast<vector<float>*>(p->getValue());
      _property.push_back(p);
    }
  } else if(wrlMode && _name=="vertex" && list==false &&
            (name=="u" || name=="v") &&
            (type==Property::Type::FLOAT || type==Property::Type::DOUBLE ||
             type==Property::Type::FLOAT32 || type==Property::Type::FLOAT64)) {
    typeWrl = Property::Type::FLOAT32_2;
    if((p=getProperty("texCoord"))==nullptr) {
      p = new Property("texCoord",false,Property::Type::NONE,typeWrl,*this);
      _ply._coord = static_cast<vector<float>*>(p->getValue());
      _property.push_back(p);
    }
  } else if(wrlMode && _name=="face" && list==false &&
            (name=="nx" || name=="ny" || name=="nz") &&
            (type==Property::Type::FLOAT || type==Property::Type::DOUBLE ||
             type==Property::Type::FLOAT32 || type==Property::Type::FLOAT64)) {
    typeWrl = Property::Type::FLOAT32_3;
    if((p=getProperty("normal"))==nullptr) {
      p = new Property("normal",false,Property::Type::NONE,typeWrl,*this);
      _ply._normalPerVertex = false;
      _ply._normal = static_cast<vector<float>*>(p->getValue());
      _property.push_back(p);
    }
  } else if(wrlMode && _name=="face" && list==false &&
            (name=="red" || name=="green" || name=="blue") &&
            (type==Property::Type::UCHAR || type==Property::Type::UINT8)) {
    typeWrl = Property::Type::FLOAT32_3;
    if((p=getProperty("color"))==nullptr) {
      p = new Property("color",false,Property::Type::NONE,typeWrl,*this);
      _ply._colorPerVertex = false;
      _ply._color = static_cast<vector<float>*>(p->getValue());
      _property.push_back(p);
    }
  } else if(wrlMode && _name=="face" && list==true && name=="vertex_indices") {
    typeWrl = Property::Type::INT32;
    if((p=getProperty("coordIndex"))==nullptr) {
      p = new Property("coordIndex",true,listType/*Property::Type::INT*/,typeWrl,*this);
      _ply._coordIndex = static_cast<vector<int>*>(p->getValue());
      _property.push_back(p);
    }
  } else {
    if((p=getProperty(name))==nullptr) {
      p = new Property(name,list,listType,type,*this);
      _property.push_back(p);
    }
  }

  return p;
}

Ply::Element::Property*
Ply::Element::getProperty(const int i) {
  Property* p = nullptr;
  if(0<=i) {
    uint ui = static_cast<uint>(i);
    if(ui<_property.size()) {
      p = _property[ui];
    }
  }
  return p;
}

bool Ply::Element::hasProperty(const string& name) {
  bool hasProperty = false;
  for(uint i=0;i<_property.size();i++) {
    if(_property[i]->getName()!=name) continue;
    hasProperty = true;
    break;
  }
  return hasProperty;
}

int Ply::Element::getPropertyIndex(const string& name) {
  int index = -1;
  for(uint i=0;i<_property.size();i++) {
    if(_property[i]->getName()!=name) continue;
    index = static_cast<int>(i);
    break;
  }
  return index;
}

Ply::Element::Property*
Ply::Element::getProperty(const string& name) {
  Property* p = nullptr;
  for(uint i=0;i<_property.size();i++) {
    if(_property[i]->getName()!=name) continue;
    p = _property[i];
    break;
  }
  return p;
}

string Ply::Element::getPropertyName(const int i) {
  string name;
  if(0<=i) {
    uint ui = static_cast<uint>(i);
    if(ui<_property.size()) {
      name = _property[ui]->getName();
    }
  }
  return name;
}

void Ply::Element::deleteProperty(const int i) {
  if(0<=i) {
    uint ui = static_cast<uint>(i);
    if(ui<_property.size()) {
      _property.erase(_property.begin()+ui);
    }
  }
}

Ply& Ply::Element::ply() {
  return _ply;
}

void Ply::Element::deleteProperty(const string& name) {
  for(uint i=0;i<_property.size();i++) {
    if(_property[i]->getName()!=name) continue;
    _property.erase(_property.begin()+i);
  }
}

// class Ply::Element::Property //////////////////////////////////////

// static
Ply::Element::Property::Type
Ply::Element::Property::parseType(const string& token) {
  Type type = Type::NONE;
  if(token=="char"         || token=="CHAR")
    type = Type::CHAR;
  else if(token=="uchar"   || token=="UCHAR")
    type = Type::UCHAR;
  else if(token=="int8"    || token=="INT8")
    type = Type::INT8;
  else if(token=="uint8"   || token=="UINT8")
    type = Type::UINT8;
  else if(token=="short"   || token=="SHORT")
    type = Type::SHORT;
  else if(token=="ushort"  || token=="USHORT")
    type = Type::USHORT;
  else if(token=="int16"   || token=="INT16")
    type = Type::INT16;
  else if(token=="uint16"  || token=="UINT16")
    type = Type::UINT16;
  else if(token=="int"     || token=="INT")
    type = Type::INT;
  else if(token=="uint"    || token=="UINT")
    type = Type::UINT;
  else if(token=="int32"   || token=="INT32")
    type = Type::INT32;
  else if(token=="uint32"  || token=="UINT32")
    type = Type::UINT32;
  else if(token=="float"   || token=="FLOAT")
    type = Type::FLOAT;
  else if(token=="float32" || token=="FLOAT32")
    type = Type::FLOAT32;
  else if(token=="double"  || token=="DOUBLE")
    type = Type::DOUBLE;
  else if(token=="float64" || token=="FLOAT64")
    type = Type::FLOAT64;
  else if(token=="float32_2" || token=="FLOAT32_2")
    type = Type::FLOAT32_2;
  else if(token=="float32_3" || token=="FLOAT32_3")
    type = Type::FLOAT32_3;

  return type;
}

// static
const string Ply::Element::Property::getTypeName(const Type type) {
  switch(type) {
  case CHAR:      return      "char";
  case UCHAR:     return     "uchar";
  case INT8:      return      "int8";
  case UINT8:     return     "uint8";
  case SHORT:     return     "short";
  case USHORT:    return    "ushort";
  case INT16:     return     "int16";
  case UINT16:    return    "uint16";
  case INT:       return       "int";
  case UINT:      return      "uint";
  case INT32:     return     "int32";
  case UINT32:    return    "uint32";
  case FLOAT:     return     "float";
  case FLOAT32:   return   "float32";
  case DOUBLE:    return    "double";
  case FLOAT64:   return   "float64";
  case FLOAT32_2: return "float32_2";
  case FLOAT32_3: return "float32_3";
  default:        return      "none";
  }
}

// static
int Ply::Element::Property::getTypeSize(const Type type) {
  int dimension = 0;
  switch(type) {
  case CHAR:      dimension =  1; break;
  case UCHAR:     dimension =  1; break;
  case INT8:      dimension =  1; break;
  case UINT8:     dimension =  1; break;
  case SHORT:     dimension =  2; break;
  case USHORT:    dimension =  2; break;
  case INT16:     dimension =  2; break;
  case UINT16:    dimension =  2; break;
  case INT:       dimension =  4; break;
  case UINT:      dimension =  4; break;
  case INT32:     dimension =  4; break;
  case UINT32:    dimension =  4; break;
  case FLOAT:     dimension =  4; break;
  case FLOAT32:   dimension =  4; break;
  case DOUBLE:    dimension =  8; break;
  case FLOAT64:   dimension =  8; break;
  case FLOAT32_2: dimension =  8; break;
  case FLOAT32_3: dimension = 12; break;
  case NONE:                   break;
  }
  return dimension;
}

Ply::Element::Property::Property
(const string& name, const bool list,
 const Type listType, const Type type, Element& element):
  _name(name),
  _value(nullptr),
  _first(),
  _type(type),
  _listType(Ply::Element::Property::Type::NONE),
  _element(element) {

  switch(type) {
  case CHAR:
  case INT8:
    _value = static_cast<void*>(new vector<char>());
    break;
  case UCHAR:
  case UINT8:
    _value = static_cast<void*>(new vector<unsigned char>());
    break;
  case SHORT:
  case INT16:
    _value = static_cast<void*>(new vector<short>());
    break;
  case USHORT:
  case UINT16:
    _value = static_cast<void*>(new vector<unsigned short>());
    break;
  case INT:
  case INT32:
    _value = static_cast<void*>(new vector<int>());
    break;
  case UINT:
  case UINT32:
    _value = static_cast<void*>(new vector<unsigned int>());
    break;
  case FLOAT:
  case FLOAT32:
  case FLOAT32_2:
  case FLOAT32_3:
    _value = static_cast<void*>(new vector<float>());
    break;
  case DOUBLE:
  case FLOAT64:
    _value = static_cast<void*>(new vector<double>());
    break;
  case NONE:
    throw new StrException("unexpected Property type");
  }

  if(list) {
    _listType = listType;
    _first.push_back(0);
  }
}

Ply::Element::Property::~Property() {
  switch(_type) {
  case CHAR:
  case INT8:
    delete static_cast<vector<char>*>(_value);
    break;
  case UCHAR:
  case UINT8:
    delete static_cast<vector<unsigned char>*>(_value);
    break;
  case SHORT:
  case INT16:
    delete static_cast<vector<short>*>(_value);
    break;
  case USHORT:
  case UINT16:
    delete static_cast<vector<unsigned short>*>(_value);
    break;
  case INT:
  case INT32:
    delete static_cast<vector<int>*>(_value);
    break;
  case UINT:
  case UINT32:
    delete static_cast<vector<unsigned int>*>(_value);
    break;
  case FLOAT:
  case FLOAT32:
  case FLOAT32_2:
  case FLOAT32_3:
    delete static_cast<vector<float>*>(_value);
    break;
  case DOUBLE:
  case FLOAT64:
    delete static_cast<vector<double>*>(_value);
    break;
  case NONE:
    break;
  }
}

void Ply::Element::Property::swap(Property& p) {
  string name     =     _name; _name     =     p._name; p._name     =      name;
  void*  value    =    _value; _value    =    p._value; p._value    =     value;
  Type   type     =     _type; _type     =     p._type; p._type     =      type;
  Type   listType = _listType; _listType = p._listType; p._listType =  listType;
  _first.swap(p._first);
}

string& Ply::Element::Property::getName() {
  return _name;
}
void* Ply::Element::Property::getValue() {
  return _value;
}
bool Ply::Element::Property::isList() {
  return (_first.size()>0);
}
Ply::Element::Property::Type Ply::Element::Property::getListType() {
  return _listType;
}
const string Ply::Element::Property::getListTypeName() {
  return getTypeName(getListType());
}
int Ply::Element::Property::getListTypeSize() {
  return getTypeSize(_listType);
}
Ply::Element::Property::Type Ply::Element::Property::getPropertyType() {
  return _type;
}
const string Ply::Element::Property::getPropertyTypeName() {
  return getTypeName(getPropertyType());
}
int Ply::Element::Property::getPropertyTypeSize() {
  return getTypeSize(_type);
}

void Ply::Element::Property::pushBackList(const int nList) {
  int index = _first.back()+nList;
  _first.push_back(index);
}
int Ply::Element::Property::getListFirst(const int i) {
  if(i<0) return -1;
  uint ui = static_cast<uint>(i);
  if(ui>=_first.size()) return -1;
  return _first[ui];
}

Ply::Element & Ply::Element::Property::element() {
  return _element;
}

