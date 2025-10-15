//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin
//  Time-stamp: <2025-10-14 17:22:03 taubin>
//------------------------------------------------------------------------
//
// GuiViewerData.hpp
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

#ifndef _GUI_VIEWER_DATA_HPP_
#define _GUI_VIEWER_DATA_HPP_

#include <QColor>
#include "wrl/SceneGraph.hpp"

class GuiViewerData {

public:
               GuiViewerData();
              ~GuiViewerData();

  void         setSceneGraph(SceneGraph* pWrl);
  SceneGraph*  getSceneGraph()             { return _pWrl;                         }

  int          getBBoxDepth()              { return _bboxDepth;                    }
  void         setBBoxDepth(int d)         { _bboxDepth = (d<0)?0:(d>10)?10:d;     }
  bool         getBBoxCube()               { return _bboxCube;                     }
  void         setBBoxCube(bool value)     { _bboxCube = value;                    }
  bool         getBBoxOccupied()           { return _bboxOccupied;                 }
  void         setBBoxOccupied(bool value) { _bboxOccupied = value;                }
  float        getBBoxScale()              { return   _bboxScale;                  }
  void         setBBoxScale(float scale)   { _bboxScale = (scale<0.0f)?0.0f:scale; }
  float        getBBoxScaleMin()
  { return   _bboxScaleMin;                  }
  void         setBBoxScaleMin(float scaleMin)
  { _bboxScaleMin = (scaleMin<1.01f)?1.01f:scaleMin; }

  // colors
  
 static QRgb*  getSelectedVertexColors()   { return _vSelColor;                    }
 static QRgb*  getSelectedEdgeColors()     { return _eSelColor;                    }
 static QRgb*  getSelectedFaceColors()     { return _fSelColor;                    }
         
 static int    getNumberOfSelectedVertexColors();
 static int    getNumberOfSelectedEdgeColors();
 static int    getNumberOfSelectedFaceColors();
         
 static QRgb   getSelectedVertexColor(int iColor);
 static QRgb   getSelectedEdgeColor(int iColor);
 static QRgb   getSelectedFaceColor(int iColor);

  void         setVertexColor(const QRgb c)      { _vColor = c;                    }
  void         setEdgeColor(const QRgb c)        { _eColor = c;                    }
  void         setNormalColor(const QRgb c)      { _nColor = c;                    }
  void         setPolylineColor(const QRgb c)    { _plColor = c;                   }
  void         setForegroundColor(const QRgb c)  { _fgColor = c;                   }
  void         setBackgroundColor(const QRgb c)  { _bgColor = c;                   }
  void         setSelectedVertexIndex(int iColor);
  void         setSelectedEdgeIndex(int iColor);
  void         setSelectedFaceIndex(int iColor);

  QRgb         getVertexColor() const            { return _vColor;                 }
  QRgb         getEdgeColor() const              { return _eColor;                 }
  QRgb         getNormalColor() const            { return _nColor;                 }
  QRgb         getPolylineColor() const          { return _plColor;                }
  QRgb         getForegroundColor() const        { return _fgColor;                }
  QRgb         getBackgroundColor() const        { return _bgColor;                }
  int          getSelectedVertexIndex();
  int          getSelectedEdgeIndex();
  int          getSelectedFaceIndex();
  QRgb         getSelectedVertexColor();
  QRgb         getSelectedEdgeColor();
  QRgb         getSelectedFaceColor();

  bool         getPaintAllVertices() const       { return _paintAllVertices;       }
  bool         getPaintSelectedVertices() const  { return _paintSelectedVertices;  }
  bool         getPaintAllEdges() const          { return _paintAllEdges;          }
  bool         getPaintSelectedEdges() const     { return _paintSelectedEdges;     }
  bool         getPaintAllFaces() const          { return _paintAllFaces;          }
  bool         getPaintSelectedFaces() const     { return _paintSelectedFaces;     }
  bool         getPaintAllPolylines() const      { return _paintAllPolylines;      }
  bool         getPaintSelectedPolylines() const { return _paintSelectedPolylines; }
  bool         getPaintNormals() const           { return _paintNormals;           }
  float        getLineWidth() const              { return _lineWidth;              }
  float        getPointSize() const              { return _pointSize;              }
  float        getNormalLength() const           { return _normalLength;           }
  float        getNormalFactor() const           { return _normalFactor;           }

  bool         getPaintVertices() const
  { return _paintAllVertices  || _paintSelectedVertices;  }
  bool         getPaintEdges() const
  { return _paintAllEdges     || _paintSelectedEdges;     }
  bool         getPaintFaces() const
  { return _paintAllFaces     || _paintSelectedFaces;     }
  bool         getPaintPolylines() const
  { return _paintAllPolylines || _paintSelectedPolylines; }

  void         setPaintAllVertices(const bool value)
  { _paintAllVertices = value; }
  void         setPaintSelectedVertices(const bool value)
  { _paintSelectedVertices = value; }
  void         setPaintAllEdges(const bool value)
  { _paintAllEdges = value; }
  void         setPaintSelectedEdges(const bool value)
  { _paintSelectedEdges = value; }
  void         setPaintAllFaces(const bool value)
  { _paintAllFaces = value; }
  void         setPaintSelectedFaces(const bool value)
  { _paintSelectedFaces = value; }
  void         setPaintAllPolylines(const bool value)
  { _paintAllPolylines = value; }
  void         setPaintSelectedPolylines(const bool value)
  { _paintSelectedPolylines = value; }
  void         setPaintNormals(const bool value)
  { _paintNormals = value; }
  void         setLineWidth(const float value)
  { _lineWidth = value; }
  void         setPointSize(const float value)
  { _pointSize = value; }
  void         setNormalLength(const float value)
  { _normalLength = value; }
  void         setNormalFactor(const float value)
  { _normalFactor = value; }

private:
  
  static QRgb   _vSelColor[];
  static QRgb   _eSelColor[];
  static QRgb   _fSelColor[];  

  SceneGraph*   _pWrl;

  int           _bboxDepth;
  bool          _bboxCube;
  bool          _bboxOccupied;
  float         _bboxScale;
  float         _bboxScaleMin;

  QRgb          _vColor;
  QRgb          _eColor;
  QRgb          _nColor;
  QRgb          _plColor;
  QRgb          _fgColor;
  QRgb          _bgColor;
  int           _vSelIndex;
  int           _eSelIndex;
  int           _fSelIndex;

  bool          _paintAllVertices;
  bool          _paintSelectedVertices;
  bool          _paintAllEdges;
  bool          _paintSelectedEdges;
  bool          _paintAllFaces;
  bool          _paintSelectedFaces;
  bool          _paintAllPolylines;
  bool          _paintSelectedPolylines;

  bool          _paintNormals;

  float         _lineWidth;
  float         _pointSize;
  float         _normalLength;
  float         _normalFactor;

};

#endif /* _GUI_VIEWER_DATA_H_ */
