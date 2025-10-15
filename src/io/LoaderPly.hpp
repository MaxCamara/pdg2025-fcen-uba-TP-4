//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin / 3D Shape Tech LLC
//  Time-stamp: <2025-08-06 09:57:26 taubin>
//------------------------------------------------------------------------
//
// LoaderPly.hpp
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

#ifndef _LOADER_PLY_HPP_
#define _LOADER_PLY_HPP_

#include "Loader.hpp"
#include <util/Endian.hpp>
#include <wrl/Ply.hpp>
#include <wrl/SceneGraph.hpp>

class LoaderPly : public Loader {

private:

  const static char* _ext;

public:

  LoaderPly()  {};
  ~LoaderPly() {};

  bool  load(const char* filename, SceneGraph & wrl);
  const char* ext() const { return _ext; }

  static bool load(const char* filename, Ply & ply, const string indent="");

private:

  static Ply::DataType systemEndian();
  static bool          sameAsSystemEndian(Ply::DataType fileEndian);

  static void addBinaryValue
  (Endian::SingleValueBuffer& buff,
   const Ply::Element::Property::Type propertyType,
   const bool swapBytes,
   void* value);
  
  static void addAsciiValue
  (const string& token,
   const Ply::Element::Property::Type propertyType,
   void* value);
  
  static size_t readHeader(FILE* fp, Ply& ply, const string indent="");
  static size_t readBinaryData(FILE* fp, Ply& ply, const string indent="");
  static size_t readAsciiData(FILE* fp, Ply& ply, const string indent="");

};

#endif // _LOADER_PLY_HPP_
