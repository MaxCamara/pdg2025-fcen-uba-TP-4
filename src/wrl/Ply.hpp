//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin / 3D Shape Tech LLC
//  Time-stamp: <2025-08-06 09:59:25 taubin>
//------------------------------------------------------------------------
//
// Ply.hpp
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

#ifndef  PLY_HPP
#define  PLY_HPP

#include <string>
#include <vector>

using namespace std;

class Ply {
  
public:

  enum DataType {
    NONE, ASCII, BINARY_LITTLE_ENDIAN, BINARY_BIG_ENDIAN
  };

  static void         setDebug(const bool value);
  static void         setFloatFormat(const string fmt);
  static void         setIntFormat(const string fmt);
  static void         setSkipComments(const bool value);
  static bool         getSkipComments();
  static DataType     getDefaultDataType();
  static const string getDataTypeName(const DataType dataType);
  static void         setDefaultDataType(const DataType dataType);

  static void         setDefaultWrlMode(const bool value);
  static bool         getDefaultWrlMode();
  // if(_wrlMode) {
  //   // automatically convert on load and save :
  //   vertex:(x,y,z) to coord
  //   vertex:(nx,ny,nz) to normal       + normalPerVertex=true
  //   vertex:(red,green.blue) to color  + colorPerVertex=true
  //   face:vertex_indices to coordIndex
  //   face:(nx,ny,nz) to normal         + normalPerVertex=false
  //   face:(red,green.blue) to color    + colorPerVertex=false
  // }
  // - per corner bindings not supported

  class Element {

  public:

    class Property {

    public:

      enum Type {
        NONE, CHAR, UCHAR, SHORT, USHORT, INT, UINT, FLOAT, DOUBLE,
        INT8, UINT8, INT16, UINT16, INT32, UINT32, FLOAT32, FLOAT64,
        // new types to support wrlMode
        FLOAT32_2, FLOAT32_3
      };

      static Type         parseType(const string& token);
      static const string getTypeName(const Type type);
      static int          getTypeSize(const Type type);

      Property(const string& name, const bool list,
               const Type listType, const Type type, Element& element);
      ~Property();
      
      void             swap(Property& p);
      string&          getName();
      void*            getValue();
      bool             isList();
      Type             getListType();
      const string     getListTypeName();
      int              getListTypeSize();
      Type             getPropertyType();
      const string     getPropertyTypeName();
      int              getPropertyTypeSize();
      void             pushBackList(const int nList);
      int              getListFirst(const int i);
      Element&         element();

    private:

      string          _name;
      void*           _value;
      vector<int>     _first;
      Type            _type;
      Type            _listType;
      Element&        _element;

    };

    Element(const string& name, const int nRecords, Ply& ply);
    ~Element();

    string            getName();
    int               getNumberOfRecords();
    int               getNumberOfProperties();
    Property*         addProperty
                      (const string& name,
                       const Property::Type type);
    Property*         addProperty
                      (const string& name,
                       const bool list,
                       const Property::Type listType,
                       const Property::Type type);
    Property*         getProperty(const int i);
    bool              hasProperty(const string& name);
    int               getPropertyIndex(const string& name);
    Property*         getProperty(const string& name);
    string            getPropertyName(const int i);
    void              deleteProperty(const int i);
    void              deleteProperty(const string& name);
    Ply&              ply();

  private:

    string            _name;
    int               _nRecords;
    vector<Property*> _property;
    Ply&              _ply;

  };

  Ply();
  ~Ply();

  void                  clear();
  void                  setTextureFile(const string path);
  string                getTextureFile();
  void                  setDataType(const DataType& dataType);
  DataType              getDataType();
  const string          getDataTypeName();
  const vector<string>& getComments();
  void                  addComment(const string& comment);
  void                  addObjInfo(const string& objInfo);
  int                   getNumberOfElements();
  Element*              addElement(const string& name, const int nRecords);
  Element*              getElement(const int i);
  Element*              getElement(const string& name);
  int                   getNumberOfElementRecords(const string& name);

  // wrlMode variables

  bool                  getWrlMode();

  bool                  hasMaterial();
  int                   getNumberOfVertices();
  int                   getNumberOfFaces();
  bool                  hasColorPerVertex();
  bool                  hasColorPerFace();
  bool                  hasColorPerCorner();
  bool                  hasColor();
  bool                  hasNormalPerVertex();
  bool                  hasNormalPerFace();
  bool                  hasNormalPerCorner();
  bool                  hasNormal();
  bool                  hasTexCoordPerVertex();
  bool                  hasTexCoordPerCorner();
  bool                  hasTexCoord();
  bool                  isTextured();

  vector<float>*        getCoord()           { return           _coord; }
  vector<int>*          getCoordIndex()      { return      _coordIndex; }
  bool                  getNormalPerVertex() { return _normalPerVertex; }
  vector<float>*        getNormal()          { return          _normal; }
  bool                  getColorPerVertex()  { return  _colorPerVertex; }
  vector<float>*        getColor()           { return           _color; }
  vector<float>*        getTexCoord()        { return        _texCoord; }

  void                  logInfo(ostream & ostr, const string indent="");

private:

  friend class LoaderPly;
  friend class IndexedFaceSetPly;

  static bool      _debug;
  static bool      _skipComments;
  static string    _floatFormat;
  static string    _intFormat;
  static DataType  _defaultDataType;
  static bool      _defaultWrlMode;

  DataType         _dataType;
  string           _textureFile;
  vector<Element*> _element;
  vector<string>   _objInfo;
  vector<string>   _comment;

  // - per corner bindings not supported
  bool             _wrlMode;
  vector<float>*   _coord; // vertex(x,y,z)
  vector<int>*     _coordIndex; // face(vertex_indices)
  bool             _normalPerVertex;
  vector<float>*   _normal; // vertex(nx,ny,nz) or face(nx,ny,nz)
  bool             _colorPerVertex;
  vector<float>*   _color; // vertex(red,green,blue) or face(red,green,blue)
  vector<float>*   _texCoord; // vertex(u,v)

};

#endif // PLY_HPP
