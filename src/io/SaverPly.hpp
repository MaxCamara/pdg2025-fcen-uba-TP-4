//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin / 3D Shape Tech LLC
//  Time-stamp: <2025-08-06 09:57:33 taubin>
//------------------------------------------------------------------------
//
// SaverPly.hpp
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

#ifndef SAVER_PLY_HPP
#define SAVER_PLY_HPP

#include <iostream>
#include <util/Endian.hpp>
#include <wrl/SceneGraph.hpp>
#include <wrl/IndexedFaceSet.hpp>
#include <wrl/IndexedFaceSetPly.hpp>
#include "Saver.hpp"

class SaverPly : public Saver {

public:

  SaverPly();
  virtual ~SaverPly() {}

  virtual const char* ext() const;

  virtual bool
  save(const char* filename, SceneGraph& wrl) const;

  static  bool
  save(const char* filename, Ply & ply, const string indent="",
       Ply::DataType dataType=Ply::DataType::ASCII);
  static  bool
  save(const char* filename, IndexedFaceSet & ifs, const string indent="",
       Ply::DataType dataType=Ply::DataType::ASCII);

         void setDataType(const Ply::DataType dataType);
  static void setDefaultDataType(const Ply::DataType dataType);

  static void setSkipAlpha(const bool value);

  static void setOstream(ostream* ostrm);
  static void setIndent(const string s="");

  private:

  static Ply::DataType systemEndian();
  static bool          sameAsSystemEndian(Ply::DataType fileEndian);

  static bool writeBinaryValue
  (FILE * fp, const Ply::Element::Property::Type listType,
   const bool swapBytes, int nList);

  static bool writeBinaryValue
  (FILE * fp, const Ply::Element::Property::Type propertyType,
   const bool swapBytes, void* value, int i);
  
  static bool writeBinaryColorValue
  (FILE * fp, const bool swapBytes, void* value, int i);

  static bool writeAsciiValue
  (FILE * fp, const Ply::Element::Property::Type propertyType,
   void* value, int i);
  
  static bool writeAsciiColorValue
  (FILE * fp, void* value, int i);
  
  static bool
  writeHeader(FILE * fp, Ply& ply, const string indent="",
              Ply::DataType dataType=Ply::DataType::ASCII);
  static bool
  writeBinaryData(FILE * fp, Ply& ply, const string indent="",
                  Ply::DataType dataType=Ply::DataType::ASCII);
  static bool
  writeAsciiData(FILE * fp, Ply& ply, const string indent="",
                 Ply::DataType dataType=Ply::DataType::ASCII);
  
  static bool
  writeHeader(FILE * fp, IndexedFaceSet& ifs, const string indent="",
              Ply::DataType dataType=Ply::DataType::ASCII);
  static bool
  writeBinaryData(FILE * fp, IndexedFaceSet& ifs, const string indent="",
                  Ply::DataType dataType=Ply::DataType::ASCII);
  static bool
  writeAsciiData(FILE * fp, IndexedFaceSet& ifs, const string indent="",
                 Ply::DataType dataType=Ply::DataType::ASCII);

private:

  const static char* _ext;

  static ostream* _ostrm;
  static string   _indent;

  static Ply::DataType _defaultDataType;
  static bool _skipAlpha;

  Ply::DataType _dataType;
};

#endif // SAVER_PLY_HPP
