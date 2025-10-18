//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-08-06 09:57:19 taubin>
//------------------------------------------------------------------------
//
// AppLoader
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

#include "AppLoader.hpp"

bool AppLoader::load(const char* filename, SceneGraph& wrl) {
  setlocale(LC_NUMERIC, "C");
  bool success = false;
  if(filename!=(const char*)0) {
    // int n = (int)strlen(filename);
    string f(filename);
    int n = static_cast<int>(f.size());
    int i;
    for(i=n-1;i>=0;i--)
      if(filename[i]=='.')
        break;
    if(i>=0) {
      string ext(filename+i+1);
      Loader* loader = _registry[ext];
      if(loader!=(Loader*)0)
        success = loader->load(filename,wrl);
    }
  }
  return success;
}

void AppLoader::registerLoader(Loader* loader) {
  if(loader!=(Loader*)0) {
    string ext(loader->ext()); // constructed from const char*
    pair<string,Loader*> ext_loader(ext,loader);
    _registry.insert(ext_loader);
  }
}
