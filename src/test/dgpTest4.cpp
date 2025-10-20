//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-10-14 17:23:34 taubin>
//------------------------------------------------------------------------
//
// dgpTest4.cpp
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

#include <string>
#include <iostream>

using namespace std;

#include <wrl/SceneGraphTraversal.hpp>

#include <io/AppLoader.hpp>
#include <io/AppSaver.hpp>
#include <io/LoaderObj.hpp>
#include <io/LoaderOff.hpp>
#include <io/LoaderPly.hpp>
#include <io/LoaderStl.hpp>
#include <io/LoaderWrl.hpp>
#include <io/SaverOff.hpp>
#include <io/SaverPly.hpp>
#include <io/SaverStl.hpp>
#include <io/SaverWrl.hpp>

// #include <core/PolygonMesh.hpp>
#include <core/PolygonMeshTest.hpp>
#include <core/Optimization.hpp>
#include "dgpPrt.hpp"

// Build your own command line program to test your code

enum Operation {
  NONE,
  SMOOTHING_LAPLACIAN_COORD,
  SMOOTHING_LAPLACIAN_NORM,
  SMOOTHING_JACOBI
  // ...
};

class Data {
public:
  bool      _debug;
  bool      _binaryOutput;
  bool      _removeProperties;
  Operation _operation;
  string    _inFile;
  string    _outFile;
public:
  Data():
    _debug(false),
    _binaryOutput(false),
    _removeProperties(false),
    _operation(NONE),
    _inFile(""),
    _outFile("")
  { }
};

void options(Data& D) {
  cout << "   -d|-debug               [" << tv(D._debug)            << "]" << endl;
  cout << "   -b|-binaryOutput        [" << tv(D._binaryOutput)     << "]" << endl;
  cout << "   -r|-removeProperties    [" << tv(D._removeProperties) << "]" << endl;

  // - add lines to explain how to specify the operation to be performed
  cout << "  -lc|-laplacianCoord      ["
       << tv(D._operation==Operation::SMOOTHING_LAPLACIAN_COORD) << "]" << endl;
  cout << "  -ln|-laplacianNorm       ["
       << tv(D._operation==Operation::SMOOTHING_LAPLACIAN_NORM) << "]" << endl;
  cout << "   -j|-jacobi              ["
       << tv(D._operation==Operation::SMOOTHING_JACOBI) << "]" << endl;

}

void usage(Data& D) {
  cout << "USAGE: dgpTest4 [options] inFile outFile" << endl;
  cout << "   -h|-help" << endl;
  options(D);
  cout << endl;
  exit(0);
}

void error(const char *msg) {
  cout << "ERROR: dgpTest4 | " << ((msg)?msg:"") << endl;
  exit(0);
}

//////////////////////////////////////////////////////////////////////
int main(int argc, char **argv) {

  Data D;

  if(argc==1) usage(D);

  for(int i=1;i<argc;i++) {
    if(string(argv[i])=="-h" || string(argv[i])=="-help") {
      usage(D);
    } else if(string(argv[i])=="-d" || string(argv[i])=="-debug") {
      D._debug = !D._debug;
    } else if(string(argv[i])=="-b" || string(argv[i])=="-binaryOutput") {
      D._binaryOutput = !D._binaryOutput;
    } else if(string(argv[i])=="-r" || string(argv[i])=="-removeProperties") {
      D._removeProperties = !D._removeProperties;

      // - add code to parse the desired operation to be performed
      // - from the command line
    } else if(string(argv[i])=="-lc" || string(argv[i])=="-laplacianCoord") {
      D._operation = Operation::SMOOTHING_LAPLACIAN_COORD;
    } else if(string(argv[i])=="-ln" || string(argv[i])=="-laplacianNorm") {
      D._operation = Operation::SMOOTHING_LAPLACIAN_NORM;
    } else if(string(argv[i])=="-j" || string(argv[i])=="-jacobi") {
      D._operation = Operation::SMOOTHING_JACOBI;

    } else if(string(argv[i])[0]=='-') {
      error("unknown option");
    } else if(D._inFile=="") {
      D._inFile = string(argv[i]);
    } else if(D._outFile=="") {
      D._outFile = string(argv[i]);
    }
  }

  if(D._inFile =="") error("no inFile");

  // if D._outFile is not specified then no output file will be written
  // if(D._outFile=="") error("no outFile");

  if(D._debug) {
    cout << "dgpTest4 {" << endl;
    cout << endl;
    options(D);
    cout << endl;
    cout << "  inFile  = " << D._inFile << endl;
    cout << "  outFile = " << D._outFile << endl;
    cout << endl;
    fflush(stderr);
  }

  bool success;

  //////////////////////////////////////////////////////////////////////
  // create loader and saver factories
  AppLoader loaderFactory;
  AppSaver  saverFactory;

  // register input file loaders
  LoaderObj* objLoader = new LoaderObj();
  loaderFactory.registerLoader(objLoader);
  LoaderOff* offLoader = new LoaderOff();
  loaderFactory.registerLoader(offLoader);
  LoaderPly* plyLoader = new LoaderPly();
  loaderFactory.registerLoader(plyLoader);
  LoaderStl* stlLoader = new LoaderStl();
  loaderFactory.registerLoader(stlLoader);
  LoaderWrl* wrlLoader = new LoaderWrl();
  loaderFactory.registerLoader(wrlLoader);

  //  If SaverPly::setDefaultDataType is used, it must be called
  //  before the Saver constructor; otherwise SaverPly::setDataType
  //  should be called after to set the proper value for the private
  //  variable SaverPly::_dataType before this instance of SaverPly is
  //  used

  // register output file savers  
  SaverOff* offSaver = new SaverOff();
  saverFactory.registerSaver(offSaver);
  SaverPly* plySaver = new SaverPly();
  saverFactory.registerSaver(plySaver);
  SaverStl* stlSaver = new SaverStl();
  saverFactory.registerSaver(stlSaver);
  SaverWrl* wrlSaver = new SaverWrl();
  saverFactory.registerSaver(wrlSaver);

  SaverStl::FileType stlFt =
    (D._binaryOutput)?SaverStl::FileType::BINARY:SaverStl::FileType::ASCII;
  stlSaver->setFileType(stlFt);

  Ply::DataType plyDt =
    (D._binaryOutput)?Ply::DataType::BINARY_LITTLE_ENDIAN:Ply::DataType::ASCII;
  plySaver->setDataType(plyDt);

  if(D._debug) {
    SaverPly::setOstream(&cout);
    SaverPly::setIndent("    ");
  }

  //////////////////////////////////////////////////////////////////////
  // read SceneGraph

  SceneGraph wrl; // create empty scene graph

  if(D._debug) {
    cout << "  loading inFile {" << endl;
  }

  success = loaderFactory.load(D._inFile.c_str(),wrl);

  if(D._debug) {
    cout << "    success        = " << tv(success)          << endl;
    cout << "  } loading inFile" << endl;
    cout << endl;
  }

  if(success==false) return -1;

  if(D._removeProperties) {
    if(D._debug) cout << "  removing properties {" << endl;
    Node* node;
    SceneGraphTraversal sgt(wrl);
    for(int iIfs=0;(node=sgt.next())!=(Node*)0;iIfs++) {
      Shape* shape = dynamic_cast<Shape*>(node);
      if(shape==(Shape*)0) continue;
      IndexedFaceSet* ifs = dynamic_cast<IndexedFaceSet*>(shape->getGeometry());
      if(ifs==(IndexedFaceSet*)0) continue;
      ifs->setNormalPerVertex(true);
      ifs->getNormal().clear();
      ifs->getNormalIndex().clear();
      ifs->setColorPerVertex(true);
      ifs->getColor().clear();
      ifs->getColorIndex().clear();
      ifs->getTexCoord().clear();
      ifs->getTexCoordIndex().clear();
    }
    if(D._debug) cout << "  } removing properties" << endl;
    if(D._debug) cout << endl;
  }

  // print PolygonMesh info before processing
  if(D._debug) {
    cout << "  before processing" << endl;
    PolygonMeshTest(wrl,"  ");
    cout << endl;
  }
  
  // process
  
  if(D._debug) cout << "  processing {" << endl;

  // TODO
  Node* node;
  SceneGraphTraversal sgt(wrl);

  switch(D._operation) {
    case Operation::SMOOTHING_LAPLACIAN_COORD:
      // perform the operation here
      cout << "    Suavizando la malla usando método laplaciano sobre las coordenadas de vértices" << endl;
      for(int iIfs=0;(node=sgt.next())!=(Node*)0;iIfs++) {
        Shape* shape = dynamic_cast<Shape*>(node);
        if(shape==(Shape*)0) continue;
        IndexedFaceSet* ifs = dynamic_cast<IndexedFaceSet*>(shape->getGeometry());
        if(ifs==(IndexedFaceSet*)0) continue;

        cout << "      IndexedFaceSet iIfs[" << iIfs <<"]:" << endl;

        //Ejecutar operación sobre el IndexedFaceSet

        Optimization* optimization = new Optimization();
        optimization->setInput(ifs);

        IndexedFaceSet* ifsOpt = new IndexedFaceSet();
        optimization->setOptimized(ifsOpt);

        //optimization->setMu(-0.525);
        optimization->setSteps(10);
        optimization->laplacianSmoothingVertexCoordinatesRun();

        optimization->saveOptimized();

        cout << endl;
      }
      break;
    case Operation::SMOOTHING_LAPLACIAN_NORM:
      break;
    case Operation::SMOOTHING_JACOBI:
      break;
    case Operation::NONE:
    default:
      break;
  }

  if(D._debug) cout << "  } processing" << endl;
  
  // print PolygonMesh info after processing
  if(D._debug) {
    cout << "  after processing" << endl;
    PolygonMeshTest(wrl,"  ");
    cout << endl;
  }
  
  // write output file
  if(D._outFile!="") {

    if(D._debug) {
      cout << "  saving outFile {" << endl;
    }
        
    success = saverFactory.save(D._outFile.c_str(),wrl);
        
    if(D._debug) {
      cout << "    success        = " << tv(success)          << endl;
      cout << "  }" << endl;
      cout << endl;
    }

  } else {
    if(D._debug) {
      cout << "  no outFile written" << endl;
    }
  }
    
  //////////////////////////////////////////////////////////////////////

  if(D._debug) {
    cout << "} dgpTest4" << endl;
    fflush(stderr);
  }

  return 0;
}
