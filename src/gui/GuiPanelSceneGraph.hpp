//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin / 3D Shape Tech LLC
//  Time-stamp: <2025-10-14 17:22:03 taubin>
//------------------------------------------------------------------------
//
// GuiPanelSceneGraph.hpp
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

#ifndef GUI_PANEL_SCENE_GRAPH_HPP
#define GUI_PANEL_SCENE_GRAPH_HPP

#include "ui_GuiPanelSceneGraph.h"

// #include <QWidget>
#include "GuiPanel.hpp"

// class SceneGraphProcessor;
class Node;
class SceneGraph;
class Transform;
class Group;
class Shape;
class Appearance;
class Material;
class PixelTexture;
class ImageTexture;
class IndexedLineSet;
class IndexedFaceSet;
class IndexedFaceSetPly;
class DGPLabMainWindow;

class GuiPanelSceneGraph : public GuiPanel, public Ui_GuiPanelSceneGraph {

  Q_OBJECT

  static bool  _registered;
  static bool  registerPanel();

public:

           GuiPanelSceneGraph(QWidget* parent=nullptr);
  virtual ~GuiPanelSceneGraph();

public slots:

  virtual void updateState() override;

protected:

  virtual void mousePressEvent(QMouseEvent * event) Q_DECL_OVERRIDE;
  virtual void showEvent(QShowEvent * event) Q_DECL_OVERRIDE;

private slots:

  void on_sceneGraphTree_itemSelectionChanged(void);
  void on_sceneGraphNodeTree_itemSelectionChanged(void);
  void on_toolButtonDeleteNode_clicked();
  void on_toolButtonInsertGroupNode_clicked();
  void on_toolButtonInsertTransformNode_clicked();
  void on_editNodeFieldValue_editingFinished();

private:

  void  addSceneGraphNodeFields_SceneGraph(SceneGraph* pWrl);
  void  addSceneGraphNodeFields_Transform(Transform* transform);
  void  addSceneGraphNodeFields_Group(Group* group);
  void  addSceneGraphNodeFields_Shape(Shape* shape);
  void  addSceneGraphNodeFields_Appearance(Appearance* appearance);
  void  addSceneGraphNodeFields_Material(Material* material);
  void  addSceneGraphNodeFields_ImageTexture(ImageTexture* imageTexture);
  void  addSceneGraphNodeFields_PixelTexture(PixelTexture* pixelTexture);
  void  addSceneGraphNodeFields_IndexedLineSet(IndexedLineSet* ils);
  void  addSceneGraphNodeFields_IndexedFaceSet(IndexedFaceSet* ifs);
  void  addSceneGraphNodeFields_IndexedFaceSetPly(IndexedFaceSetPly* ifsPly);

  void clearEditNodeFields();
  void
  setEditNodeFields_SceneGraph
  (SceneGraph* pWrl, const int fieldNumber);
  void
  setEditNodeFields_Transform
  (Transform* transform, const int fieldNumber);
  void
  setEditNodeFields_Group
  (Group* group, const int fieldNumber);
  void
  setEditNodeFields_Shape
  (Shape* shape, const int fieldNumber);
  void
  setEditNodeFields_Appearance
  (Appearance* appearance, const int fieldNumber);
  void
  setEditNodeFields_Material
  (Material* material, const int fieldNumber);
  void
  setEditNodeFields_ImageTexture
  (ImageTexture* imageTexture, const int fieldNumber);
  void
  setEditNodeFields_PixelTexture
  (PixelTexture* pixelTexture, const int fieldNumber);
  void
  setEditNodeFields_IndexedLineSet
  (IndexedLineSet* ils, const int fieldNumber);
  void
  setEditNodeFields_IndexedFaceSet
  (IndexedFaceSet* ifs, const int fieldNumber);
  void
  setEditNodeFields_IndexedFaceSetPly
  (IndexedFaceSetPly* ifsPly, const int fieldNumber);

  bool
  onEditNodeFieldValueChanged_SceneGraph
  (SceneGraph* pWrl, const int fieldNumber, const QString text);
  bool
  onEditNodeFieldValueChanged_Transform
  (Transform* transform, const int fieldNumber, const QString text);
  bool
  onEditNodeFieldValueChanged_Group
  (Group* group, const int fieldNumber, const QString text);
  bool
  onEditNodeFieldValueChanged_Shape
  (Shape* shape, const int fieldNumber, const QString text);
  bool
  onEditNodeFieldValueChanged_Appearance
  (Appearance* appearance, const int fieldNumber, const QString text);
  bool
  onEditNodeFieldValueChanged_Material
  (Material* material, const int fieldNumber, const QString text);
  bool
  onEditNodeFieldValueChanged_ImageTexture
  (ImageTexture* imageTexture, const int fieldNumber, const QString text);
  bool
  onEditNodeFieldValueChanged_PixelTexture
  (PixelTexture* pixelTexture, const int fieldNumber, const QString text);
  bool
  onEditNodeFieldValueChanged_IndexedLineSet
  (IndexedLineSet* ils, const int fieldNumber, const QString text);
  bool
  onEditNodeFieldValueChanged_IndexedFaceSet
  (IndexedFaceSet* ifs, const int fieldNumber, const QString text);
  bool
  onEditNodeFieldValueChanged_IndexedFaceSetPly
  (IndexedFaceSetPly* ifsPly, const int fieldNumber, const QString text);

  Node* getSelectedNode();
  bool  enableSelectedNodeButtons(Node* node);
  bool  enableSelectedNodeButtons();
  void  log(const QString & text);
  void  logEnable(const bool value);

private:

  std::vector<Node*>               _nodeRow;
  std::map<QTreeWidgetItem*,Node*> _mapItemNode;
  bool                             _logEnable;

};

#endif // GUI_PANEL_SCENE_GRAPH_HPP
