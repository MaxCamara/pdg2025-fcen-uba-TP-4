//------------------------------------------------------------------------
//  Copyright (C) Gabriel Taubin / 3D Shape Tech LLC
//  Time-stamp: <2025-10-14 17:22:03 taubin>
//------------------------------------------------------------------------
//
// GuiPanelSceneGraph.cpp
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

// #include <iostream>
#include <utility>
#include "GuiApplication.hpp"
#include "GuiPanelSceneGraph.hpp"
#include "GuiMainWindow.hpp"
#include <wrl/SceneGraphProcessor.hpp>
#include <wrl/IndexedFaceSetPly.hpp>
#include <wrl/ImageTexture.hpp>
#include <util/CastMacros.hpp>

// static variable evaluated at start-up time
bool GuiPanelSceneGraph::_registered = GuiPanelSceneGraph::registerPanel();

// static method run once at start-up time
bool GuiPanelSceneGraph::registerPanel() {
  return
    GuiMainWindow::registerPanelFactory
    ("SceneGraph",
     [](QWidget *parent)->GuiPanel*{ return new GuiPanelSceneGraph(parent);});
}

//////////////////////////////////////////////////////////////////////
void  GuiPanelSceneGraph::log(const QString & text) {
  (void)text;
  // if(_logEnable) APP->log(text);
}

//////////////////////////////////////////////////////////////////////
GuiPanelSceneGraph::GuiPanelSceneGraph(QWidget* parent):
  GuiPanel(parent),
  _nodeRow(),
  _mapItemNode(),
  _logEnable(false) {
  setupUi(this);
  QStringList list;
  list << "Parameter" << "Type" << "Value";
  sceneGraphNodeTree->setHeaderLabels(list);
}

//////////////////////////////////////////////////////////////////////
GuiPanelSceneGraph::~GuiPanelSceneGraph() {
  QSettings & settings = getApp()->settings();
  int w0 = sceneGraphNodeTree->columnWidth(0);
  int w1 = sceneGraphNodeTree->columnWidth(1);
  int w2 = sceneGraphNodeTree->columnWidth(2);
  settings.setValue("sceneGraphWidget/nodeTree/columnWidth/0",w0);
  settings.setValue("sceneGraphWidget/nodeTree/columnWidth/1",w1);
  settings.setValue("sceneGraphWidget/nodeTree/columnWidth/2",w2);
  sceneGraphTree->selectionModel()->clearSelection();
}

//////////////////////////////////////////////////////////////////////
void GuiPanelSceneGraph::showEvent(QShowEvent * event) {
  (void) event;
  QSettings & settings = getApp()->settings();
  int w = sceneGraphNodeTree->width();
  int w0 = settings.value("sceneGraphWidget/nodeTree/columnWidth/0",w/3).toInt();
  int w1 = settings.value("sceneGraphWidget/nodeTree/columnWidth/1",w/3).toInt();
  int w2 = settings.value("sceneGraphWidget/nodeTree/columnWidth/2",w/3).toInt();
  sceneGraphNodeTree->setColumnWidth(0,w0);
  sceneGraphNodeTree->setColumnWidth(0,w1);
  sceneGraphNodeTree->setColumnWidth(0,w2);
  updateState();
}

#define TV(x) ((x)?"true":"false")

//////////////////////////////////////////////////////////////////////
void
GuiPanelSceneGraph::addSceneGraphNodeFields_SceneGraph
(SceneGraph* pWrl) {

  QTreeWidgetItem* child  = nullptr;

  // SceneGraph::Fields::SHOW
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"show");
  child->setText(1,"bool");
  child->setText(2,TV(pWrl->getShow()));

  // SceneGraph::Fields::NAME
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"name");
  child->setText(1,"string");
  child->setText(2,pWrl->getName().c_str());

  // SceneGraph::Fields::URL
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"url");
  child->setText(1,"string");
  child->setText(2,pWrl->getUrl().c_str());

  // SceneGraph::Fields::BBOX_CENTER
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"bboxCenter");
  child->setText(1,"float[3]");
  Vec3f& bbc = pWrl->getBBoxCenter();
  child->setText(2,QString("%1 %2 %3").arg(D(bbc.x)).arg(D(bbc.y)).arg(D(bbc.z)));

  // SceneGraph::Fields::BBOX_SIZE
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"bboxSize");
  child->setText(1,"float[3]");
  Vec3f& bbs = pWrl->getBBoxSize();
  child->setText(2,QString("%1 %2 %3").arg(D(bbs.x)).arg(D(bbs.y)).arg(D(bbs.z)));

  // SceneGraph::Fields::CHILDREN
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"children");
  child->setText(1,"vector<Node*>");
  child->setText(2,QString("[%1]").arg(pWrl->getChildren().size()));
}

void
GuiPanelSceneGraph::addSceneGraphNodeFields_Transform
(Transform* transform) {

  QTreeWidgetItem* child  = nullptr;

  // Transform::Fields::SHOW
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"show");
  child->setText(1,"bool");
  child->setText(2,TV(transform->getShow()));

  // Transform::Fields::NAME
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"name");
  child->setText(1,"string");
  child->setText(2,transform->getName().c_str());

  // Transform::Fields::CENTER
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"center");
  child->setText(1,"float[3]");
  Vec3f& c = transform->getCenter();
  child->setText(2,QString("%1 %2 %3").arg(D(c.x)).arg(D(c.y)).arg(D(c.z)));

  // Transform::Fields::ROTATION
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"rotation");
  child->setText(1,"float[4]");
  Rotation& r = transform->getRotation();
  Vec3f& raxis = r.getAxis();
  child->setText(2,QString("%1 %2 %3 %4")
                 .arg(D(raxis.x)).arg(D(raxis.y)).arg(D(raxis.z))
                 .arg(D(r.getAngle())));

  // Transform::Fields::SCALE
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"scale");
  child->setText(1,"float[3]");
  Vec3f& s = transform->getScale();
  child->setText(2,QString("%1 %2 %3").arg(D(s.x)).arg(D(s.y)).arg(D(s.z)));

  // Transform::Fields::SCALE_ORIENTATION
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"scaleOrientation");
  child->setText(1,"float[4]");
  Rotation& so = transform->getScaleOrientation();
  Vec3f& so_axis = so.getAxis();
  child->setText(2,QString("%1 %2 %3 %4")
                 .arg(D(so_axis.x)).arg(D(so_axis.y)).arg(D(so_axis.z))
                 .arg(D(so.getAngle())));

  // Transform::Fields::TRANSLATION
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"translation");
  child->setText(1,"float[3]");
  Vec3f& t = transform->getTranslation();
  child->setText(2,QString("%1 %2 %3").arg(D(t.x)).arg(D(t.y)).arg(D(t.z)));

  // Transform::Fields::BBOX_CENTER
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"bboxCenter");
  child->setText(1,"float[3]");
  Vec3f& bbc = transform->getBBoxCenter();
  child->setText(2,QString("%1 %2 %3").arg(D(bbc.x)).arg(D(bbc.y)).arg(D(bbc.z)));

  // Transform::Fields::BBOX_SIZE
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"bboxSize");
  child->setText(1,"float[3]");
  Vec3f& bbs = transform->getBBoxSize();
  child->setText(2,QString("%1 %2 %3").arg(D(bbs.x)).arg(D(bbs.y)).arg(D(bbs.z)));

  // Transform::Fields::CHILDREN
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"children");
  child->setText(1,"vector<Node*>");
  child->setText(2,QString("[%1]").arg(transform->getChildren().size()));
}

void
GuiPanelSceneGraph::addSceneGraphNodeFields_Group
(Group* group) {

  QTreeWidgetItem* child  = nullptr;

  // Group::Fields::SHOW
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"show");
  child->setText(1,"bool");
  child->setText(2,TV(group->getShow()));

  // Group::Fields::NAME
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"name");
  child->setText(1,"string");
  child->setText(2,group->getName().c_str());

  // Group::Fields::BBOX_CENTER
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"bboxCenter");
  child->setText(1,"float[3]");
  Vec3f& bbc = group->getBBoxCenter();
  child->setText(2,QString("%1 %2 %3").arg(D(bbc.x)).arg(D(bbc.y)).arg(D(bbc.z)));

  // Group::Fields::BBOX_SIZE
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"bboxSize");
  child->setText(1,"float[3]");
  Vec3f& bbs = group->getBBoxSize();
  child->setText(2,QString("%1 %2 %3").arg(D(bbs.x)).arg(D(bbs.y)).arg(D(bbs.z)));

  // Group::Fields::CHILDREN
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"children");
  child->setText(1,"vector<Node*>");
  child->setText(2,QString("[%1]").arg(group->getChildren().size()));
}

void
GuiPanelSceneGraph::addSceneGraphNodeFields_Shape
(Shape* shape) {

  char str[128];
  QTreeWidgetItem* child  = nullptr;

  // Shape::Fields::SHOW
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"show");
  child->setText(1,"bool");
  child->setText(2,TV(shape->getShow()));

  // Shape::Fields::NAME
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"name");
  child->setText(1,"string");
  child->setText(2,shape->getName().c_str());

  // Shape::Fields::APPEARANCE
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"appearance");
  child->setText(1,"Node*");
  void* appearance = static_cast<void*>(shape->getAppearance());
  snprintf(str,128,"0x%08lx",RUL(appearance));
  child->setText(2,str);

  // Shape::Fields::GEOMETRY
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"geometry");
  child->setText(1,"Node*");
  void* geometry = static_cast<void*>(shape->getGeometry());
  snprintf(str,128,"0x%08lx",RUL(geometry));
  child->setText(2,str);
}

void
GuiPanelSceneGraph::addSceneGraphNodeFields_Appearance
(Appearance* appearance) {

  char str[128];
  QTreeWidgetItem* child  = nullptr;

  // Appearance::Fields::NAME
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"name");
  child->setText(1,"string");
  child->setText(2,appearance->getName().c_str());

  // Appearance::Fields::MATERIAL
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"material");
  child->setText(1,"Node*");
  void* material = static_cast<void*>(appearance->getMaterial());
  snprintf(str,128,"0x%08lx",RUL(material));
  child->setText(2,str);
  
  // Appearance::Fields::TEXTURE
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"texture");
  child->setText(1,"Node*");
  void* texture = static_cast<void*>(appearance->getTexture());
  snprintf(str,128,"0x%08lx",RUL(texture));
  child->setText(2,str);
  
  // Appearance::Fields::TEXTURE_TRANSFORM
  // SFNode textureTransform NULL
}

void
GuiPanelSceneGraph::addSceneGraphNodeFields_Material
(Material* material) {

  QTreeWidgetItem* child  = nullptr;

  // Material::Fields::NAME
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"name");
  child->setText(1,"string");
  child->setText(2,material->getName().c_str());

  // Material::Fields::AMBIENT_INTENSITY
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"ambientIntensity");
  child->setText(1,"float");
  child->setText(2,QString("%1").arg(D(material->getAmbientIntensity())));

  // Material::Fields::DIFFUSE_COLOR
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"diffuseColor");
  child->setText(1,"float[3]");
  Color& dc = material->getDiffuseColor();
  child->setText(2,QString("%1 %2 %3").arg(D(dc.r)).arg(D(dc.g)).arg(D(dc.b)));

  // Material::Fields::EMISSIVE_COLOR
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"emissiveColor");
  child->setText(1,"float[3]");
  Color& ec = material->getEmissiveColor();
  child->setText(2,QString("%1 %2 %3").arg(D(ec.r)).arg(D(ec.g)).arg(D(ec.b)));

  // Material::Fields::SHININESS
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"shininess");
  child->setText(1,"float");
  child->setText(2,QString("%1").arg(D(material->getShininess())));

  // Material::Fields::SPECULAR_COLOR
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"specularColor");
  child->setText(1,"float[3]");
  Color sc = material->getSpecularColor();
  child->setText(2,QString("%1 %2 %3").arg(D(sc.r)).arg(D(sc.g)).arg(D(sc.b)));

  // Material::Fields::TRANSPARENCY
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"transparency");
  child->setText(1,"float");
  child->setText(2,QString("%1").arg(D(material->getTransparency())));
}

void
GuiPanelSceneGraph::addSceneGraphNodeFields_ImageTexture
(ImageTexture* imageTexture) {

  QTreeWidgetItem* child  = nullptr;

  // ImageTexture::Fields::SHOW
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"show");
  child->setText(1,"bool");
  child->setText(2,TV(imageTexture->getShow()));

  // ImageTexture::Fields::NAME
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"name");
  child->setText(1,"string");
  child->setText(2,imageTexture->getName().c_str());

  // ImageTexture::Fields::URL
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"url");
  child->setText(1,"vector<string>");
  vector<string>& url = imageTexture->getUrl();
  if(url.size()>0)
    child->setText(2,url[0].c_str()); // MFString url []

  // ImageTexture::Fields::REPEAT_S
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"repeatS");
  child->setText(1,"bool");
  child->setText(2,TV(imageTexture->getRepeatS()));

  // ImageTexture::Fields::REPEAT_T
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"repeatT");
  child->setText(1,"bool");
  child->setText(2,TV(imageTexture->getRepeatT()));
}

void
GuiPanelSceneGraph::addSceneGraphNodeFields_PixelTexture
(PixelTexture* pixelTexture) {

  QTreeWidgetItem* child  = nullptr;

  // PixelTexture::Fields::SHOW
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"show");
  child->setText(1,"bool");
  child->setText(2,TV(pixelTexture->getShow()));

  // PixelTexture::Fields::NAME
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"name");
  child->setText(1,"string");
  child->setText(2,pixelTexture->getName().c_str());

  // PixelTexture::Fields::REPEAT_S
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"repeatS");
  child->setText(1,"bool");
  child->setText(2,TV(pixelTexture->getRepeatS()));

  // PixelTexture::Fields::REPEAT_T
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"repeatT");
  child->setText(1,"bool");
  child->setText(2,TV(pixelTexture->getRepeatT()));

  // SFImage image 0 0 0
}

void
GuiPanelSceneGraph::addSceneGraphNodeFields_IndexedLineSet
(IndexedLineSet* ils) {

  QTreeWidgetItem* child  = nullptr;

  // IndexedLineSet::Fields::NAME
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"name");
  child->setText(1,"string");
  child->setText(2,ils->getName().c_str());

  // IndexedLineSet::Fields::COORD
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"coord");
  child->setText(1,"vector<float>");
  child->setText(2,QString("[%1]").arg(ils->getCoord().size()/3));

  // IndexedLineSet::Fields::COORD_INDEX
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"coordIndex");
  child->setText(1,"vector<int>");
  child->setText(2,QString("[%1]").arg(ils->getCoordIndex().size()));

  // IndexedLineSet::Fields::COLOR_PER_VERTEX
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"colorPerVertex");
  child->setText(1,"bool");
  child->setText(2,TV(ils->getColorPerVertex()));

  // IndexedLineSet::Fields::COLOR
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"color");
  child->setText(1,"vector<float>");
  child->setText(2,QString("[%1]").arg(ils->getColor().size()/3));

  // IndexedLineSet::Fields::COLOR_INDEX
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"colorIndex");
  child->setText(1,"vector<int>");
  child->setText(2,QString("[%1]").arg(ils->getColorIndex().size()));
}

void
GuiPanelSceneGraph::addSceneGraphNodeFields_IndexedFaceSetPly
(IndexedFaceSetPly* ifsPly) {
  addSceneGraphNodeFields_IndexedFaceSet(ifsPly);
  // TODO Sun Jan 12 12:15:09 2020
  // - add Ply fields
}

void
GuiPanelSceneGraph::addSceneGraphNodeFields_IndexedFaceSet
(IndexedFaceSet* ifs) {

  QTreeWidgetItem* child  = nullptr;

  // IndexedFaceSet::Fields::NAME
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"name");
  child->setText(1,"string");
  child->setText(2,ifs->getName().c_str());

  // IndexedFaceSet::Fields::CCW
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"ccw");
  child->setText(1,"bool");
  child->setText(2,TV(ifs->getCcw()));

  // IndexedFaceSet::Fields::CONVEX
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"convex");
  child->setText(1,"bool");
  child->setText(2,TV(ifs->getConvex()));

  // IndexedFaceSet::Fields::CREASE_ANGLE
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"creaseAngle");
  child->setText(1,"float");
  child->setText(2,QString("%1").arg(D(ifs->getCreaseAngle())));

  // IndexedFaceSet::Fields::SOLID
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"solid");
  child->setText(1,"bool");
  child->setText(2,QString("%1").arg(TV(ifs->getSolid())));

  // IndexedFaceSet::Fields::COORD
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"coord");
  child->setText(1,"vector<float>");
  child->setText(2,QString("[%1]").arg(ifs->getCoord().size()/3));

  // IndexedFaceSet::Fields::COORD_INDEX
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"coordIndex");
  child->setText(1,"vector<int>");
  child->setText(2,QString("[%1]").arg(ifs->getCoordIndex().size()));

  // IndexedFaceSet::Fields::COLOR_PER_VERTEX
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"colorPerVertex");
  child->setText(1,"bool");
  child->setText(2,TV(ifs->getColorPerVertex()));

  // IndexedFaceSet::Fields::COLOR
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"color");
  child->setText(1,"vector<float>");
  child->setText(2,QString("[%1]").arg(ifs->getColor().size()/3));

  // IndexedFaceSet::Fields::COLOR_INDEX
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"colorIndex");
  child->setText(1,"vector<int>");
  child->setText(2,QString("[%1]").arg(ifs->getColorIndex().size()));

  // IndexedFaceSet::Fields::NORMAL_PER_VERTEX
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"normalPerVertex");
  child->setText(1,"bool");
  child->setText(2,TV(ifs->getNormalPerVertex()));

  // IndexedFaceSet::Fields::NORMAL
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"normal");
  child->setText(1,"vector<float>");
  child->setText(2,QString("[%1]").arg(ifs->getNormal().size()/3));

  // IndexedFaceSet::Fields::NORMAL_INDEX
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"normalIndex");
  child->setText(1,"vector<int>");
  child->setText(2,QString("[%1]").arg(ifs->getNormalIndex().size()));

  // IndexedFaceSet::Fields::TEX_COORD
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"texCoord");
  child->setText(1,"vector<float>");
  child->setText(2,QString("[%1]").arg(ifs->getTexCoord().size()/2));

  // IndexedFaceSet::Fields::TEX_COORD_INDEX
  child = new QTreeWidgetItem(sceneGraphNodeTree);
  child->setText(0,"texCoordIndex");
  child->setText(1,"vector<int>");
  child->setText(2,QString("[%1]").arg(ifs->getTexCoordIndex().size()));
}

//////////////////////////////////////////////////////////////////////
void  GuiPanelSceneGraph::on_sceneGraphTree_itemSelectionChanged(void) {
  log("GuiPanelSceneGraph::on_sceneGraphTree_itemSelectionChanged() {");

  // clearEditNodeFields();

  // Node* node = _mapItemNode[items[0]];
  Node* node = getSelectedNode();
  if(node==nullptr) {
    log("  node==nullptr");
    log("}");
    return;
  }

  if(/*Transform* transform = */ dynamic_cast<Transform*>(node)) {

    // redraw sceneGraphTree
    updateState();

    // redrawn sceneGraphTree does not have any item selected
    // find selected node in redrawn sceneGraphTree and select item
    QTreeWidgetItem* item = nullptr;
    for(map<QTreeWidgetItem*,Node*>::iterator i=_mapItemNode.begin();
        i!=_mapItemNode.end();i++) {
      if(i->second==node) { item = i->first; break; }
    }
    if(item!=nullptr) {
      sceneGraphTree->blockSignals(true);
      QModelIndex index = sceneGraphTree->indexFromItem(item);
      sceneGraphTree->selectionModel()
        ->select(index, QItemSelectionModel::SelectCurrent);
      sceneGraphTree->blockSignals(false);
    }
  }

  enableSelectedNodeButtons(node);
  sceneGraphNodeTree->blockSignals(true);
  sceneGraphNodeTree->clear();
  if(SceneGraph* pWrl =
     dynamic_cast<SceneGraph*>(node)) {
    log("  is SceneGraph");
    addSceneGraphNodeFields_SceneGraph(pWrl);
  } else if(Transform* transform =
            dynamic_cast<Transform*>(node)) {
    log("  is Transform");
    addSceneGraphNodeFields_Transform(transform);
  } else if(Group* group =
            dynamic_cast<Group*>(node)) {
    log("  is Group");
    addSceneGraphNodeFields_Group(group);
  } else if(Shape* shape =
            dynamic_cast<Shape*>(node)) {
    log("  is Shape");
    addSceneGraphNodeFields_Shape(shape);
  } else if(Appearance* appearance =
            dynamic_cast<Appearance*>(node)) {
    log("  is Appearance");
    addSceneGraphNodeFields_Appearance(appearance);
  } else if(Material* material =
            dynamic_cast<Material*>(node)) {
    log("  is Material");
    addSceneGraphNodeFields_Material(material);
  } else if(PixelTexture* pixelTexture =
            dynamic_cast<PixelTexture*>(node)) {
    log("  is PixelTexture");
    addSceneGraphNodeFields_PixelTexture(pixelTexture);
  } else if(ImageTexture* imageTexture =
            dynamic_cast<ImageTexture*>(node)) {
    log("  is ImageTexture");
    addSceneGraphNodeFields_ImageTexture(imageTexture);
  } else if(IndexedLineSet* indexedLineSet =
            dynamic_cast<IndexedLineSet*>(node)) {
    log("  is IndexedLineSet");
    addSceneGraphNodeFields_IndexedLineSet(indexedLineSet);
  } if(IndexedFaceSet* indexedFaceSet =
       dynamic_cast<IndexedFaceSet*>(node)) {
    log("  is IndexedFaceSet");
    addSceneGraphNodeFields_IndexedFaceSet(indexedFaceSet);
  } else if(IndexedFaceSetPly* indexedFaceSetPy =
            dynamic_cast<IndexedFaceSetPly*>(node)) {
    log("  is IndexedFaceSetPly");
    addSceneGraphNodeFields_IndexedFaceSetPly(indexedFaceSetPy);
  }
  sceneGraphNodeTree->blockSignals(false);

  log("}");
}

//////////////////////////////////////////////////////////////////////
void GuiPanelSceneGraph::clearEditNodeFields() {
  editNodeFieldName->setText("");
  editNodeFieldType->setText("");
  editNodeFieldValue->setReadOnly(false);
  editNodeFieldValue->setText("");
}

void
GuiPanelSceneGraph::setEditNodeFields_SceneGraph
(SceneGraph* pWrl, const int fieldNumber) {

  switch(static_cast<SceneGraph::Fields>(fieldNumber)) {
  case SceneGraph::Fields::SHOW :
    {
      editNodeFieldName->setText("show");
      editNodeFieldType->setText("bool");
      editNodeFieldValue->setReadOnly(false);
      const bool show = pWrl->getShow();
      editNodeFieldValue->setText(((show)?"true":"false"));
    }
    break;
  case SceneGraph::Fields::NAME :
    {
      editNodeFieldName->setText("name");
      editNodeFieldType->setText("string");
      editNodeFieldValue->setReadOnly(false);
      const string & name = pWrl->getName();
      editNodeFieldValue->setText(name.c_str());
    }
    break;
  case SceneGraph::Fields::URL :
    {
      editNodeFieldName->setText("url");
      editNodeFieldType->setText("string");
      editNodeFieldValue->setReadOnly(false);
      editNodeFieldValue
        ->setText(pWrl->getUrl().c_str());
    }
    break;
  case SceneGraph::Fields::BBOX_CENTER:
    {
      editNodeFieldName->setText("bboxCenter");
      editNodeFieldType->setText("float[3]");
      editNodeFieldValue->setReadOnly(false);
      Vec3f& c = pWrl->getBBoxCenter();
      editNodeFieldValue
        ->setText(QString("%1 %2 %3").arg(D(c.x)).arg(D(c.y)).arg(D(c.z)));
    }
    break;
  case SceneGraph::Fields::BBOX_SIZE:
    {
      editNodeFieldName->setText("bboxSize");
      editNodeFieldType->setText("float[3]");
      editNodeFieldValue->setReadOnly(false);
      Vec3f& s = pWrl->getBBoxSize();
      editNodeFieldValue
        ->setText(QString("%1 %2 %3").arg(D(s.x)).arg(D(s.y)).arg(D(s.z)));
    }
    break;
  case SceneGraph::Fields::CHILDREN:
    {
      editNodeFieldName->setText("children");
      editNodeFieldType->setText("vector<Node*>");
      editNodeFieldValue->setReadOnly(true);
      vector<Node*>& children = pWrl->getChildren();
      editNodeFieldValue->setText(QString("[%1]").arg(children.size()));
    }
    break;
  default:
    clearEditNodeFields();
    break;
  }
}

void
GuiPanelSceneGraph::setEditNodeFields_Transform
(Transform* transform, const int fieldNumber) {

  switch(static_cast<Transform::Fields>(fieldNumber)) {
    case Transform::Fields::SHOW :
      {
        editNodeFieldName->setText("show");
        editNodeFieldType->setText("bool");
        editNodeFieldValue->setReadOnly(false);
        const bool show = transform->getShow();
        editNodeFieldValue->setText(((show)?"true":"false"));
      }
    break;
    case Transform::Fields::NAME :
      {
        editNodeFieldName->setText("name");
        editNodeFieldType->setText("string");
        editNodeFieldValue->setReadOnly(false);
        const string & name = transform->getName();
        editNodeFieldValue->setText(name.c_str());
      }
    break;
    case Transform::Fields::CENTER :
      {
        editNodeFieldName->setText("center");
        editNodeFieldType->setText("float[3]");
        editNodeFieldValue->setReadOnly(false);
        Vec3f& c = transform->getCenter();
        editNodeFieldValue
          ->setText(QString("%1 %2 %3").arg(D(c.x)).arg(D(c.y)).arg(D(c.z)));
      }
      break;
    case Transform::Fields::ROTATION :
      {
        editNodeFieldName->setText("rotation");
        editNodeFieldType->setText("float[4]");
        editNodeFieldValue->setReadOnly(false);
        Rotation& r = transform->getRotation();
        Vec3f& raxis = r.getAxis();
        editNodeFieldValue
          ->setText(QString("%1 %2 %3 %4")
                    .arg(D(raxis.x)).arg(D(raxis.y)).arg(D(raxis.z))
                    .arg(D(r.getAngle())));
      }
      break;
    case Transform::Fields::SCALE :
      {
        editNodeFieldName->setText("scale");
        editNodeFieldType->setText("float[3]");
        editNodeFieldValue->setReadOnly(false);
        Vec3f& s = transform->getScale();
        editNodeFieldValue
          ->setText(QString("%1 %2 %3").arg(D(s.x)).arg(D(s.y)).arg(D(s.z)));
      }
      break;
    case Transform::Fields::SCALE_ORIENTATION :
      {
        editNodeFieldName->setText("scaleOrientation");
        editNodeFieldType->setText("float[4]");
        editNodeFieldValue->setReadOnly(false);
        Rotation& so = transform->getScaleOrientation();
        Vec3f& so_axis = so.getAxis();
        editNodeFieldValue
          ->setText(QString("%1 %2 %3 %4")
                    .arg(D(so_axis.x)).arg(D(so_axis.y)).arg(D(so_axis.z))
                    .arg(D(so.getAngle())));
      }
      break;
    case Transform::Fields::TRANSLATION :
      {
        editNodeFieldName->setText("translation");
        editNodeFieldType->setText("float[3]");
        editNodeFieldValue->setReadOnly(false);
        Vec3f& t = transform->getTranslation();
        editNodeFieldValue
          ->setText(QString("%1 %2 %3").arg(D(t.x)).arg(D(t.y)).arg(D(t.z)));
      }
      break;
    case Transform::Fields::BBOX_CENTER :
      {
        editNodeFieldName->setText("bboxCenter");
        editNodeFieldType->setText("float[3]");
        editNodeFieldValue->setReadOnly(false);
        Vec3f& c = transform->getBBoxCenter();
        editNodeFieldValue
          ->setText(QString("%1 %2 %3").arg(D(c.x)).arg(D(c.y)).arg(D(c.z)));
      }
      break;
    case Transform::Fields::BBOX_SIZE :
      {
        editNodeFieldName->setText("bboxSize");
        editNodeFieldType->setText("float[3]");
        editNodeFieldValue->setReadOnly(false);
        Vec3f& s = transform->getBBoxSize();
        editNodeFieldValue
          ->setText(QString("%1 %2 %3").arg(D(s.x)).arg(D(s.y)).arg(D(s.z)));
      }
      break;
    case Transform::Fields::CHILDREN :
      {
        editNodeFieldName->setText("children");
        editNodeFieldType->setText("vector<Node*>");
        editNodeFieldValue->setReadOnly(true);
        vector<Node*>& children = transform->getChildren();
        editNodeFieldValue->setText(QString("[%1]").arg(children.size()));
      }
      break;
    default:
      clearEditNodeFields();
      break;
    }
}

void
GuiPanelSceneGraph::setEditNodeFields_Group
(Group* group, const int fieldNumber) {

  switch(static_cast<Group::Fields>(fieldNumber)) {

  case Group::Fields::SHOW :
    {
      editNodeFieldName->setText("show");
      editNodeFieldType->setText("bool");
      editNodeFieldValue->setReadOnly(false);
      const bool show = group->getShow();
      editNodeFieldValue->setText(((show)?"true":"false"));
    }
    break;
  case Group::Fields::NAME :
    {
      editNodeFieldName->setText("name");
      editNodeFieldType->setText("string");
      editNodeFieldValue->setReadOnly(false);
      const string & name = group->getName();
      editNodeFieldValue->setText(name.c_str());
    }
    break;
  case Group::Fields::BBOX_CENTER :
    {
      editNodeFieldName->setText("bboxCenter");
      editNodeFieldType->setText("float[3]");
      editNodeFieldValue->setReadOnly(false);
      Vec3f& c = group->getBBoxCenter();
      editNodeFieldValue
        ->setText(QString("%1 %2 %3").arg(D(c.x)).arg(D(c.y)).arg(D(c.z)));
    }
    break;
  case Group::Fields::BBOX_SIZE :
    {
      editNodeFieldName->setText("bboxSize");
      editNodeFieldType->setText("float[3]");
      editNodeFieldValue->setReadOnly(false);
      Vec3f& s = group->getBBoxSize();
      editNodeFieldValue
        ->setText(QString("%1 %2 %3").arg(D(s.x)).arg(D(s.y)).arg(D(s.z)));
    }
    break;
  case Group::Fields::CHILDREN :
    {
      editNodeFieldName->setText("children");
      editNodeFieldType->setText("vector<Node*>");
      editNodeFieldValue->setReadOnly(true);
      vector<Node*>& children = group->getChildren();
      editNodeFieldValue->setText(QString("[%1]").arg(children.size()));
    }
    break;
  default:
    clearEditNodeFields();
    break;
  }
}

void
GuiPanelSceneGraph::setEditNodeFields_Shape
(Shape* shape, const int fieldNumber) {

  char str[128];
  switch(static_cast<Shape::Fields>(fieldNumber)) {
  case Shape::Fields::SHOW :
    {
      editNodeFieldName->setText("show");
      editNodeFieldType->setText("bool");
      editNodeFieldValue->setReadOnly(false);
      const bool show = shape->getShow();
      editNodeFieldValue->setText(((show)?"true":"false"));
    }
    break;
  case Shape::Fields::NAME :
    {
      editNodeFieldName->setText("name");
      editNodeFieldType->setText("string");
      editNodeFieldValue->setReadOnly(false);
      const string & name = shape->getName();
      editNodeFieldValue->setText(name.c_str());
    }
    break;
  case Shape::Fields::APPEARANCE :
    {
      editNodeFieldName->setText("appearance");
      editNodeFieldType->setText("Node*");
      editNodeFieldValue->setReadOnly(true);
      void* appearance = static_cast<void*>(shape->getAppearance());
      snprintf(str,128,"0x%08lx",RUL(appearance));
      editNodeFieldValue->setText(str);
    }
    break;
  case Shape::Fields::GEOMETRY:
    {
      editNodeFieldName->setText("geometry");
      editNodeFieldType->setText("Node*");
      editNodeFieldValue->setReadOnly(true);
      void* geometry = static_cast<void*>(shape->getGeometry());
      snprintf(str,128,"0x%08lx",RUL(geometry));
      editNodeFieldValue->setText(str);
    }
    break;
  default:
    clearEditNodeFields();
    break;
  }
}

void
GuiPanelSceneGraph::setEditNodeFields_Appearance
(Appearance* appearance, const int fieldNumber) {

  char str[128];
  switch(static_cast<Appearance::Fields>(fieldNumber)) {
  case Appearance::Fields::NAME :
    {
      editNodeFieldName->setText("name");
      editNodeFieldType->setText("string");
      editNodeFieldValue->setReadOnly(false);
      const string & name = appearance->getName();
      editNodeFieldValue->setText(name.c_str());
    }
    break;
  case Appearance::Fields::MATERIAL :
    {
      editNodeFieldName->setText("material");
      editNodeFieldType->setText("Node*");
      editNodeFieldValue->setReadOnly(true);
      void* material = static_cast<void*>(appearance->getMaterial());
      snprintf(str,128,"0x%08lx",RUL(material));
      editNodeFieldValue->setText(str);
    }
    break;
  case Appearance::Fields::TEXTURE :
    {
      editNodeFieldName->setText("texture");
      editNodeFieldType->setText("Node*");
      editNodeFieldValue->setReadOnly(true);
      void* texture = static_cast<void*>(appearance->getTexture());
      snprintf(str,128,"0x%08lx",RUL(texture));
      editNodeFieldValue->setText(str);
    }
    break;
  case Appearance::Fields::TEXTURE_TRANSFORM :
    // SFNode textureTransform NULL
    break;
  default:
    clearEditNodeFields();
    break;
  }
}

void
GuiPanelSceneGraph::setEditNodeFields_Material
(Material* material, const int fieldNumber) {

  switch(static_cast<Material::Fields>(fieldNumber)) {
  case Material::Fields::NAME :
    {
      editNodeFieldName->setText("name");
      editNodeFieldType->setText("string");
      editNodeFieldValue->setReadOnly(false);
      const string & name = material->getName();
      editNodeFieldValue->setText(name.c_str());
    }
    break;
  case Material::Fields::AMBIENT_INTENSITY :
    {
      editNodeFieldName->setText("ambientIntensity");
      editNodeFieldType->setText("float");
      editNodeFieldValue->setReadOnly(false);
      editNodeFieldValue
        ->setText(QString("%1").arg(D(material->getAmbientIntensity())));
    }
    break;
  case Material::Fields::DIFFUSE_COLOR :
    {
      editNodeFieldName->setText("diffuseColor");
      editNodeFieldType->setText("float[3]");
      editNodeFieldValue->setReadOnly(false);
      Color& dc = material->getDiffuseColor();
      editNodeFieldValue
        ->setText(QString("%1 %2 %3").arg(D(dc.r)).arg(D(dc.g)).arg(D(dc.b)));
    }
    break;
  case Material::Fields::EMISSIVE_COLOR :
    {
      editNodeFieldName->setText("emissiveColor");
      editNodeFieldType->setText("float[3]");
      editNodeFieldValue->setReadOnly(false);
      Color& ec = material->getEmissiveColor();
      editNodeFieldValue
        ->setText(QString("%1 %2 %3").arg(D(ec.r)).arg(D(ec.g)).arg(D(ec.b)));
    }
    break;
  case Material::Fields::SHININESS :
    {
      editNodeFieldName->setText("shininess");
      editNodeFieldType->setText("float");
      editNodeFieldValue->setReadOnly(false);
      editNodeFieldValue
        ->setText(QString("%1").arg(D(material->getShininess())));
    }
    break;
  case Material::Fields::SPECULAR_COLOR :
    {
      editNodeFieldName->setText("specularColor");
      editNodeFieldType->setText("float[3]");
      editNodeFieldValue->setReadOnly(false);
      Color sc = material->getSpecularColor();
      editNodeFieldValue
        ->setText(QString("%1 %2 %3").arg(D(sc.r)).arg(D(sc.g)).arg(D(sc.b)));
    }
    break;
  case Material::Fields::TRANSPARENCY :
    {
      editNodeFieldName->setText("transparency");
      editNodeFieldType->setText("float");
      editNodeFieldValue->setReadOnly(false);
      editNodeFieldValue
        ->setText(QString("%1").arg(D(material->getTransparency())));
    }
    break;
  default:
    clearEditNodeFields();
    break;
  }
}

void
GuiPanelSceneGraph::setEditNodeFields_PixelTexture
(PixelTexture* pixelTexture, const int fieldNumber) {
  switch(static_cast<PixelTexture::Fields>(fieldNumber)) {
  case PixelTexture::Fields::SHOW :
    {
      editNodeFieldName->setText("show");
      editNodeFieldType->setText("bool");
      editNodeFieldValue->setReadOnly(false);
      const bool show = pixelTexture->getShow();
      editNodeFieldValue->setText(((show)?"true":"false"));
    }
    break;
  case PixelTexture::Fields::NAME :
    {
      editNodeFieldName->setText("name");
      editNodeFieldType->setText("string");
      editNodeFieldValue->setReadOnly(false);
      const string & name = pixelTexture->getName();
      editNodeFieldValue->setText(name.c_str());
    }
    break;
  case PixelTexture::Fields::REPEAT_S :
    {
      editNodeFieldName->setText("repeatS");
      editNodeFieldType->setText("bool");
      editNodeFieldValue->setReadOnly(false);
      editNodeFieldValue
        ->setText(TV(pixelTexture->getRepeatS()));
    }
    break;
  case PixelTexture::Fields::REPEAT_T :
    {
      editNodeFieldName->setText("repeatT");
      editNodeFieldType->setText("bool");
      editNodeFieldValue->setReadOnly(false);
      editNodeFieldValue
        ->setText(TV(pixelTexture->getRepeatT()));
    }
    break;
    // case 4:
    // SFImage image 0 0 0
    // break;
  default:
    clearEditNodeFields();
    break;
  }
}

void
GuiPanelSceneGraph::setEditNodeFields_ImageTexture
(ImageTexture* imageTexture, const int fieldNumber) {
  switch(static_cast<ImageTexture::Fields>(fieldNumber)) {
  case ImageTexture::Fields::SHOW :
    {
      editNodeFieldName->setText("show");
      editNodeFieldType->setText("bool");
      editNodeFieldValue->setReadOnly(false);
      const bool show = imageTexture->getShow();
      editNodeFieldValue->setText(((show)?"true":"false"));
    }
    break;
  case ImageTexture::Fields::NAME :
    {
      editNodeFieldName->setText("name");
      editNodeFieldType->setText("string");
      editNodeFieldValue->setReadOnly(false);
      const string & name = imageTexture->getName();
      editNodeFieldValue->setText(name.c_str());
    }
    break;
  case ImageTexture::Fields::URL :
    {
      editNodeFieldName->setText("url");
      editNodeFieldType->setText("vector<string>");
      editNodeFieldValue->setReadOnly(false);
      vector<string>& url = imageTexture->getUrl();
      if(url.size()>0)
        editNodeFieldValue->setText(url[0].c_str()); // MFString url []
      else
        editNodeFieldValue->setText("");
    }
    break;
  case ImageTexture::Fields::REPEAT_S :
    {
      editNodeFieldName->setText("repeatS");
      editNodeFieldType->setText("bool");
      editNodeFieldValue->setReadOnly(false);
      editNodeFieldValue
        ->setText(TV(imageTexture->getRepeatS()));
    }
    break;
  case ImageTexture::Fields::REPEAT_T :
    {
      editNodeFieldName->setText("repeatT");
      editNodeFieldType->setText("bool");
      editNodeFieldValue->setReadOnly(false);
      editNodeFieldValue
        ->setText(TV(imageTexture->getRepeatT()));
    }
    break;
  default:
    clearEditNodeFields();
    break;
  }
}

void
GuiPanelSceneGraph::setEditNodeFields_IndexedLineSet
(IndexedLineSet* ils, const int fieldNumber) {
  switch(static_cast<IndexedLineSet::Fields>(fieldNumber)) {
  case IndexedLineSet::Fields::NAME :
    {
      editNodeFieldName->setText("name");
      editNodeFieldType->setText("string");
      editNodeFieldValue->setReadOnly(false);
      const string & name = ils->getName();
      editNodeFieldValue->setText(name.c_str());
    }
    break;
  case IndexedLineSet::Fields::COORD :
    {
      editNodeFieldName->setText("coord");
      editNodeFieldType->setText("vector<float>");
      editNodeFieldValue->setReadOnly(true);
      editNodeFieldValue
        ->setText(QString("[%1]").arg(ils->getCoord().size()/3));
    }
    break;
  case IndexedLineSet::Fields::COORD_INDEX :
    {
      editNodeFieldName->setText("coordIndex");
      editNodeFieldType->setText("vector<int>");
      editNodeFieldValue->setReadOnly(true);
      editNodeFieldValue
        ->setText(QString("[%1]").arg(ils->getCoordIndex().size()));
    }
    break;
  case IndexedLineSet::Fields::COLOR_PER_VERTEX :
    {
      editNodeFieldName->setText("colorPerVertex");
      editNodeFieldType->setText("bool");
      editNodeFieldValue->setReadOnly(false);
      editNodeFieldValue
        ->setText(TV(ils->getColorPerVertex()));
    }
    break;
  case IndexedLineSet::Fields::COLOR :
    {
      editNodeFieldName->setText("color");
      editNodeFieldType->setText("vector<float>");
      editNodeFieldValue->setReadOnly(true);
      editNodeFieldValue
        ->setText(QString("[%1]").arg(ils->getColor().size()/3));
    }
    break;
  case IndexedLineSet::Fields::COLOR_INDEX :
    {
      editNodeFieldName->setText("colorIndex");
      editNodeFieldType->setText("vector<int>");
      editNodeFieldValue->setReadOnly(true);
      editNodeFieldValue
        ->setText(QString("[%1]").arg(ils->getColorIndex().size()));
    }
    break;
  default:
    clearEditNodeFields();
    break;
  }
}

void
GuiPanelSceneGraph::setEditNodeFields_IndexedFaceSetPly
(IndexedFaceSetPly* ifsPly, const int fieldNumber) {
  setEditNodeFields_IndexedFaceSet(ifsPly,fieldNumber);
}

void
GuiPanelSceneGraph::setEditNodeFields_IndexedFaceSet
(IndexedFaceSet* ifs, const int fieldNumber) {
  switch(static_cast<IndexedFaceSet::Fields>(fieldNumber)) {
  case IndexedFaceSet::Fields::NAME :
    {
      editNodeFieldName->setText("name");
      editNodeFieldType->setText("string");
      editNodeFieldValue->setReadOnly(false);
      const string & name = ifs->getName();
      editNodeFieldValue->setText(name.c_str());
    }
    break;
  case IndexedFaceSet::Fields::CCW :
    {
      editNodeFieldName->setText("ccw");
      editNodeFieldType->setText("bool");
      editNodeFieldValue->setReadOnly(false);
      editNodeFieldValue
        ->setText(TV(ifs->getCcw()));
    }
    break;
  case IndexedFaceSet::Fields::CONVEX :
    {
      editNodeFieldName->setText("convex");
      editNodeFieldType->setText("bool");
      editNodeFieldValue->setReadOnly(false);
      editNodeFieldValue
        ->setText(TV(ifs->getConvex()));
    }
    break;
  case IndexedFaceSet::Fields::CREASE_ANGLE:
    {
      editNodeFieldName->setText("creaseAngle");
      editNodeFieldType->setText("float");
      editNodeFieldValue->setReadOnly(false);
      editNodeFieldValue
        ->setText(QString("%1").arg(D(ifs->getCreaseAngle())));
    }
    break;
  case IndexedFaceSet::Fields::SOLID:
    {
      editNodeFieldName->setText("solid");
      editNodeFieldType->setText("bool");
      editNodeFieldValue->setReadOnly(false);
      editNodeFieldValue
        ->setText(QString("%1").arg(TV(ifs->getSolid())));
    }
    break;
  case IndexedFaceSet::Fields::COORD:
    {
      editNodeFieldName->setText("coord");
      editNodeFieldType->setText("vector<float>");
      editNodeFieldValue->setReadOnly(true);
      editNodeFieldValue
        ->setText(QString("[%1]").arg(ifs->getCoord().size()/3));
    }
    break;
  case IndexedFaceSet::Fields::COORD_INDEX:
    {
      editNodeFieldName->setText("coordIndex");
      editNodeFieldType->setText("vector<int>");
      editNodeFieldValue->setReadOnly(true);
      editNodeFieldValue
        ->setText(QString("[%1]").arg(ifs->getCoordIndex().size()));
    }
    break;
  case IndexedFaceSet::Fields::COLOR_PER_VERTEX :
    {
      editNodeFieldName->setText("colorPerVertex");
      editNodeFieldType->setText("bool");
      editNodeFieldValue->setReadOnly(false);
      editNodeFieldValue
        ->setText(TV(ifs->getColorPerVertex()));
    }
    break;
  case IndexedFaceSet::Fields::COLOR :
    {
      editNodeFieldName->setText("color");
      editNodeFieldType->setText("vector<float>");
      editNodeFieldValue->setReadOnly(true);
      editNodeFieldValue
        ->setText(QString("[%1]").arg(ifs->getColor().size()/3));
    }
    break;
  case IndexedFaceSet::Fields::COLOR_INDEX :
    {
      editNodeFieldName->setText("colorIndex");
      editNodeFieldType->setText("vector<int>");
      editNodeFieldValue->setReadOnly(true);
      editNodeFieldValue
        ->setText(QString("[%1]").arg(ifs->getColorIndex().size()));
    }
    break;
  case IndexedFaceSet::Fields::NORMAL_PER_VERTEX :
    {
      editNodeFieldName->setText("normalPerVertex");
      editNodeFieldType->setText("bool");
      editNodeFieldValue->setReadOnly(false);
      editNodeFieldValue
        ->setText(TV(ifs->getNormalPerVertex()));
    }
    break;
  case IndexedFaceSet::Fields::NORMAL :
    {
      editNodeFieldName->setText("normal");
      editNodeFieldType->setText("vector<float>");
      editNodeFieldValue->setReadOnly(true);
      editNodeFieldValue
        ->setText(QString("[%1]").arg(ifs->getNormal().size()/3));
    }
    break;

  case IndexedFaceSet::Fields::NORMAL_INDEX :
    {
      editNodeFieldName->setText("normalIndex");
      editNodeFieldType->setText("vector<int>");
      editNodeFieldValue->setReadOnly(true);
      editNodeFieldValue
        ->setText(QString("[%1]").arg(ifs->getNormalIndex().size()));
    }
    break;
  case IndexedFaceSet::Fields::TEX_COORD :
    {
      editNodeFieldName->setText("texCoord");
      editNodeFieldType->setText("vector<float>");
      editNodeFieldValue->setReadOnly(true);
      editNodeFieldValue
        ->setText(QString("[%1]").arg(ifs->getTexCoord().size()/2));
    }
    break;
  case IndexedFaceSet::Fields::TEX_COORD_INDEX :
    {
      editNodeFieldName->setText("texCoordIndex");
      editNodeFieldType->setText("vector<int>");
      editNodeFieldValue->setReadOnly(true);
      editNodeFieldValue
        ->setText(QString("[%1]").arg(ifs->getTexCoordIndex().size()));
    }
    break;
  default:
    clearEditNodeFields();
    break;
  }
}

//////////////////////////////////////////////////////////////////////
void  GuiPanelSceneGraph::on_sceneGraphNodeTree_itemSelectionChanged(void) {
  Node* node = getSelectedNode();
  if(node==nullptr) return;
  QList<QTreeWidgetItem*> items = sceneGraphNodeTree->selectedItems();
  if(items.size()!=1) return;
  QModelIndex index = sceneGraphNodeTree->indexFromItem(items[0]);
  int fieldNumber = index.row();
  if(SceneGraph* pWrl = dynamic_cast<SceneGraph*>(node)) {
    setEditNodeFields_SceneGraph(pWrl,fieldNumber);
  } else if(Transform* transform = dynamic_cast<Transform*>(node)) {
    setEditNodeFields_Transform(transform,fieldNumber);
  } else if(Group* group = dynamic_cast<Group*>(node)) {
    setEditNodeFields_Group(group,fieldNumber);
  } else if(Shape* shape = dynamic_cast<Shape*>(node)) {
    setEditNodeFields_Shape(shape,fieldNumber);
  } else if(Appearance* appearance = dynamic_cast<Appearance*>(node)) {
    setEditNodeFields_Appearance(appearance,fieldNumber);
  } else if(Material* material = dynamic_cast<Material*>(node)) {
    setEditNodeFields_Material(material,fieldNumber);
  } else if(ImageTexture* imageTexture = dynamic_cast<ImageTexture*>(node)) {
    setEditNodeFields_ImageTexture(imageTexture,fieldNumber);
  } else if(PixelTexture* pixelTexture = dynamic_cast<PixelTexture*>(node)) {
    setEditNodeFields_PixelTexture(pixelTexture,fieldNumber);
  } else if(IndexedLineSet* indexedLineSet = dynamic_cast<IndexedLineSet*>(node)) {
    setEditNodeFields_IndexedLineSet(indexedLineSet,fieldNumber);
  } else if(IndexedFaceSet* indexedFaceSet = dynamic_cast<IndexedFaceSet*>(node)) {
    setEditNodeFields_IndexedFaceSet(indexedFaceSet,fieldNumber);
  } else if(IndexedFaceSetPly* indexedFaceSetPy =
            dynamic_cast<IndexedFaceSetPly*>(node)) {
    setEditNodeFields_IndexedFaceSetPly(indexedFaceSetPy,fieldNumber);
  } else {
    clearEditNodeFields();
  }
}

//////////////////////////////////////////////////////////////////////
bool
GuiPanelSceneGraph::onEditNodeFieldValueChanged_SceneGraph
(SceneGraph* pWrl, const int fieldNumber, const QString text) {
  bool repaint = false;
  if(text!="")
  switch(static_cast<SceneGraph::Fields>(fieldNumber)) {
  case SceneGraph::Fields::SHOW :
    {
      pWrl->setShow(text=="true" || text=="TRUE");
      repaint = true;
    }
    break;
  case SceneGraph::Fields::NAME :
    {
      pWrl->setName(text.toStdString());
    }
    break;
  case SceneGraph::Fields::URL :
    {
      pWrl->setUrl(text.toStdString());
    }
    break;
  case SceneGraph::Fields::BBOX_CENTER :
    {
      float x,y,z;
      if(sscanf(text.toStdString().c_str(),"%f %f %f",&x,&y,&z)==3) {
        Vec3f& bbc = pWrl->getBBoxCenter();
        bbc.x=x;bbc.y=y;bbc.z=z;
        repaint = true;
      }
    }
    break;
  case SceneGraph::Fields::BBOX_SIZE:
    {
      float x,y,z;
      if(sscanf(text.toStdString().c_str(),"%f %f %f",&x,&y,&z)==3) {
        Vec3f& bbs = pWrl->getBBoxSize();
        bbs.x=x;bbs.y=y;bbs.z=z;
        repaint = true;
      }
    }
    break;
  case SceneGraph::Fields::CHILDREN :
    break;
  default:
    break;
  }
  return repaint;
}

bool
GuiPanelSceneGraph::onEditNodeFieldValueChanged_Transform
(Transform* transform, const int fieldNumber, const QString text) {
  bool repaint = false;
  switch(static_cast<Transform::Fields>(fieldNumber)) {
  case Transform::Fields::SHOW :
    {
      transform->setShow(text=="true" || text=="TRUE");
      repaint = true;
    }
    break;
  case Transform::Fields::NAME :
    {
      transform->setName(text.toStdString());
    }
    break;
  case Transform::Fields::CENTER:
    {
      float x,y,z;
      if(sscanf(text.toStdString().c_str(),"%f %f %f",&x,&y,&z)==3) {
        Vec3f& c = transform->getCenter();
        c.x=x;c.y=y;c.z=z;
        repaint = true;
      }
    }
    break;
  case Transform::Fields::ROTATION :
    {
      float x,y,z,angle;
      if(sscanf(text.toStdString().c_str(),"%f %f %f %f",&x,&y,&z,&angle)==4) {
        Rotation& r = transform->getRotation();
        r.set(x,y,z,angle);
        repaint = true;
      }
    }
    break;
  case Transform::Fields::SCALE :
    {
      float x,y,z;
      if(sscanf(text.toStdString().c_str(),"%f %f %f",&x,&y,&z)==3) {
        Vec3f& s = transform->getScale();
        s.x=x;s.y=y;s.z=z;
        repaint = true;
      }
    }
    break;
  case Transform::Fields::SCALE_ORIENTATION :
    {
      float x,y,z,angle;
      if(sscanf(text.toStdString().c_str(),"%f %f %f %f",&x,&y,&z,&angle)==4) {
        Rotation& so = transform->getScaleOrientation();
        so.set(x,y,z,angle);
        repaint = true;
      }
    }
    break;
  case Transform::Fields::TRANSLATION :
    {
      Vec3f& t = transform->getTranslation();
      sscanf(text.toStdString().c_str(),"%f %f %f",&t.x,&t.y,&t.z);
      repaint = true;
    }
    break;
  case Transform::Fields::BBOX_CENTER :
    {
      float x,y,z;
      if(sscanf(text.toStdString().c_str(),"%f %f %f",&x,&y,&z)==3) {
        Vec3f& c = transform->getBBoxCenter();
        c.x=x;c.y=y;c.z=z;
        repaint = true;
      }
    }
    break;
  case Transform::Fields::BBOX_SIZE :
    {
      float x,y,z;
      if(sscanf(text.toStdString().c_str(),"%f %f %f",&x,&y,&z)==3) {
        Vec3f& s = transform->getBBoxSize();
        s.x=x;s.y=y;s.z=z;
        repaint = true;
      }
    }
    break;
  case Transform::Fields::CHILDREN :
    break;
  default:
    break;
  }
  return repaint;
}

bool
GuiPanelSceneGraph::onEditNodeFieldValueChanged_Group
(Group* group, const int fieldNumber, const QString text) {
  bool repaint = false;
  switch(static_cast<Group::Fields>(fieldNumber)) {
  case Group::Fields::SHOW:
    {
      group->setShow(text=="true" || text=="TRUE");
      repaint = true;
    }
    break;
  case Group::Fields::NAME :
    {
      group->setName(text.toStdString());
    }
    break;
  case Group::Fields::BBOX_CENTER :
    {
      float x,y,z;
      if(sscanf(text.toStdString().c_str(),"%f %f %f",&x,&y,&z)==3) {
        Vec3f& c = group->getBBoxCenter();
        c.x=x;c.y=y;c.z=z;
        repaint = true;
      }
    }
    break;
  case Group::Fields::BBOX_SIZE :
    {
      float x,y,z;
      if(sscanf(text.toStdString().c_str(),"%f %f %f",&x,&y,&z)==3) {
        Vec3f& s = group->getBBoxSize();
        s.x=x;s.y=y;s.z=z;
        repaint = true;
      }
    }
    break;
  case Group::Fields::CHILDREN:
    break;
  default:
    break;
  }
  return repaint;
}

bool
GuiPanelSceneGraph::onEditNodeFieldValueChanged_Shape
(Shape* shape, const int fieldNumber, const QString text) {
  bool repaint = false;
  switch(static_cast<Shape::Fields>(fieldNumber)) {
  case Shape::Fields::SHOW :
    shape->setShow(text=="true" || text=="TRUE");
    repaint = true;
    break;
  case Shape::Fields::NAME :
    shape->setName(text.toStdString());
    break;
  case Shape::Fields::APPEARANCE:
    break;
  case Shape::Fields::GEOMETRY:
    break;
  }
  return repaint;
}

bool
GuiPanelSceneGraph::onEditNodeFieldValueChanged_Appearance
(Appearance* appearance, const int fieldNumber, const QString text) {
  bool repaint = false;
  switch(static_cast<Appearance::Fields>(fieldNumber)) {
  case Appearance::Fields::NAME :
    appearance->setName(text.toStdString());
    break;
  case Appearance::Fields::MATERIAL :
    break;
  case Appearance::Fields::TEXTURE :
    break;
  case Appearance::Fields::TEXTURE_TRANSFORM :
    break;
  }
  return repaint;
}

bool
GuiPanelSceneGraph::onEditNodeFieldValueChanged_Material
(Material* material, const int fieldNumber, const QString text) {
  bool repaint = false;
  switch(static_cast<Material::Fields>(fieldNumber)) {
  case Material::Fields::NAME :
    {
      material->setName(text.toStdString());
    }
    break;
  case Material::Fields::AMBIENT_INTENSITY :
    {
      float ai;
      if(sscanf(text.toStdString().c_str(),"%f",&ai)==1) {
        material->setAmbientIntensity(ai);
        repaint = true;
      }
    }
    break;
  case Material::Fields::DIFFUSE_COLOR :
    {
      float r,g,b;
      if(sscanf(text.toStdString().c_str(),"%f %f %f",&r,&g,&b)==3) {
        Color& dc = material->getDiffuseColor();
        dc.r=r;dc.g=g;dc.b=b;
        repaint = true;
      }
    }
    break;
  case Material::Fields::EMISSIVE_COLOR:
    {
      float r,g,b;
      if(sscanf(text.toStdString().c_str(),"%f %f %f",&r,&g,&b)==3) {
        Color& ec = material->getEmissiveColor();
        ec.r=r;ec.g=g;ec.b=b;
        repaint = true;
      }
    }
    break;
  case Material::Fields::SHININESS :
    {
      float sh;
      if(sscanf(text.toStdString().c_str(),"%f",&sh)==1) {
        material->setShininess(sh);
        repaint = true;
      }
    }
    break;
  case Material::Fields::SPECULAR_COLOR :
    {
      float r,g,b;
      if(sscanf(text.toStdString().c_str(),"%f %f %f",&r,&g,&b)==3) {
        Color sc = material->getSpecularColor();
        sc.r=r;sc.g=g;sc.b=b;
        repaint = true;
      }
    }
    break;
  case Material::Fields::TRANSPARENCY :
    {
      float tr;
      if(sscanf(text.toStdString().c_str(),"%f",&tr)==1) {
        material->setTransparency(tr);
        repaint = true;
      }
    }
    break;
  }
  return repaint;
}

bool
GuiPanelSceneGraph::onEditNodeFieldValueChanged_ImageTexture
(ImageTexture* imageTexture, const int fieldNumber, const QString text) {
  bool repaint = false;
  switch(static_cast<ImageTexture::Fields>(fieldNumber)) {
  case ImageTexture::Fields::SHOW :
    {
      imageTexture->setShow(text=="true" || text=="TRUE");
      repaint = true;
    }
    break;
  case ImageTexture::Fields::NAME :
    {
      imageTexture->setName(text.toStdString());
    }
    break;
  case ImageTexture::Fields::URL :
    {
      imageTexture->setUrl(text.toStdString());
    }
    break;
  case ImageTexture::Fields::REPEAT_S :
    {
      bool rs = (text=="true" || text=="TRUE");
      imageTexture->setRepeatS(rs);
      repaint = true;
    }
    break;
  case ImageTexture::Fields::REPEAT_T :
    {
      bool rt = (text=="true" || text=="TRUE");
      imageTexture->setRepeatT(rt);
      repaint = true;
    }
  }
  return repaint;
}

bool
GuiPanelSceneGraph::onEditNodeFieldValueChanged_PixelTexture
(PixelTexture* pixelTexture, const int fieldNumber, const QString text) {
  bool repaint = false;
  switch(static_cast<PixelTexture::Fields>(fieldNumber)) {
  case PixelTexture::Fields::SHOW :
    {
      pixelTexture->setShow(text=="true" || text=="TRUE");
      repaint = true;
    }
    break;
  case PixelTexture::Fields::NAME :
    {
      pixelTexture->setName(text.toStdString());
    }
    break;
  case PixelTexture::Fields::REPEAT_S :
    {
      bool rs = (text=="true" || text=="TRUE");
      pixelTexture->setRepeatS(rs);
      repaint = true;
    }
    break;
  case PixelTexture::Fields::REPEAT_T :
    {
      bool rt = (text=="true" || text=="TRUE");
      pixelTexture->setRepeatT(rt);
      repaint = true;
    }
    break;
    // case 4:
    // SFImage image 0 0 0
    // break;
  }
  return repaint;
}

bool
GuiPanelSceneGraph::onEditNodeFieldValueChanged_IndexedLineSet
(IndexedLineSet* ils, const int fieldNumber, const QString text) {
  bool repaint = false;
  switch(static_cast<IndexedLineSet::Fields>(fieldNumber)) {
  case IndexedLineSet::Fields::NAME :
    {
      ils->setName(text.toStdString());
    }
    break;
  case IndexedLineSet::Fields::COORD :
    break;
  case IndexedLineSet::Fields::COORD_INDEX :
    break;
  case IndexedLineSet::Fields::COLOR_PER_VERTEX :
    {
      bool cpv = (text=="true" || text=="TRUE");
      ils->setColorPerVertex(cpv);
      repaint = true;
    }
    break;
  case IndexedLineSet::Fields::COLOR :
    break;
  case IndexedLineSet::Fields::COLOR_INDEX :
    break;
  }
  return repaint;
}

bool
GuiPanelSceneGraph::onEditNodeFieldValueChanged_IndexedFaceSet
(IndexedFaceSet* ifs, const int fieldNumber, const QString text) {
  bool repaint = false;
  switch(static_cast<IndexedFaceSet::Fields>(fieldNumber)) {
  case IndexedFaceSet::Fields::NAME :
    {
      ifs->setName(text.toStdString());
    }
    break;
  case IndexedFaceSet::Fields::CCW :
    {
      bool& ccw = ifs->getCcw();
      ccw = (text=="true" || text=="TRUE");
      repaint = true;
    }
    break;
  case IndexedFaceSet::Fields::CONVEX :
    {
      bool& cvx = ifs->getConvex();
      cvx = (text=="true" || text=="TRUE");
      repaint = true;
    }
    break;
  case IndexedFaceSet::Fields::CREASE_ANGLE :
    {
      float a;
      if(sscanf(text.toStdString().c_str(),"%f",&a)==1) {
        float& ca = ifs->getCreaseAngle(); ca=a;
        repaint = true;
      }
    }
    break;
  case IndexedFaceSet::Fields::SOLID :
    {
      bool& so = ifs->getSolid();
      so = (text=="true" || text=="TRUE");
      repaint = true;
    }
    break;
  case IndexedFaceSet::Fields::COORD :
    break;
  case IndexedFaceSet::Fields::COORD_INDEX :
    break;
  case IndexedFaceSet::Fields::COLOR_PER_VERTEX :
    {
      bool cpv = (text=="true" || text=="TRUE");
      ifs->setColorPerVertex(cpv);
      repaint = true;
    }
    break;
  case IndexedFaceSet::Fields::COLOR :
    break;
  case IndexedFaceSet::Fields::COLOR_INDEX :
    break;
  case IndexedFaceSet::Fields::NORMAL_PER_VERTEX :
    {
      bool npv = (text=="true" || text=="TRUE");
      ifs->setNormalPerVertex(npv);
      repaint = true;
    }
    break;
  case IndexedFaceSet::Fields::NORMAL :
    break;
  case IndexedFaceSet::Fields::NORMAL_INDEX :
    break;
  case IndexedFaceSet::Fields::TEX_COORD :
    break;
  case IndexedFaceSet::Fields::TEX_COORD_INDEX :
    break;
  }
  return repaint;
}

bool
GuiPanelSceneGraph::onEditNodeFieldValueChanged_IndexedFaceSetPly
(IndexedFaceSetPly* ifsPly, const int fieldNumber, const QString text) {
  return
  onEditNodeFieldValueChanged_IndexedFaceSet(ifsPly,fieldNumber,text);
  // TODO Sun Jan 12 13:48:53 2020
  // handle Ply fields
}

//////////////////////////////////////////////////////////////////////
void GuiPanelSceneGraph::on_editNodeFieldValue_editingFinished() {
  log("GuiPanelSceneGraph::on_editNodeFieldValue_editingFinished() {");

  Node* node = getSelectedNode();
  if(node==nullptr) {
    log("  node==nullptr");
    log("}");
    return;
  }

  QList<QTreeWidgetItem*> itemFields = sceneGraphNodeTree->selectedItems();
  if(itemFields.size()==0) {
    log("  nItemFields==0");
    log("}");
    return;
  }
  QModelIndex indexField =
    sceneGraphNodeTree->indexFromItem(itemFields[0]);
  int rowField = indexField.row();

  int fieldNumber = indexField.row();
  log(QString("  fieldNumber = %1").arg(fieldNumber));
    
  QString text = editNodeFieldValue->text();

  // bool repaint = false;
  if(SceneGraph* pWrl =
     dynamic_cast<SceneGraph*>(node)) {
    // repaint =
      onEditNodeFieldValueChanged_SceneGraph
      (pWrl,fieldNumber,text);
  } else if(Transform* transform =
            dynamic_cast<Transform*>(node)) {
    // repaint =
      onEditNodeFieldValueChanged_Transform
      (transform,fieldNumber,text);
  } else if(Group* group =
            dynamic_cast<Group*>(node)) {
    // repaint =
      onEditNodeFieldValueChanged_Group
      (group,fieldNumber,text);
  } else if(Shape* shape =
            dynamic_cast<Shape*>(node)) {
    // repaint =
      onEditNodeFieldValueChanged_Shape
      (shape,fieldNumber,text);
  } else if(Appearance* appearance =
            dynamic_cast<Appearance*>(node)) {
    // repaint =
      onEditNodeFieldValueChanged_Appearance
      (appearance,fieldNumber,text);
  } else if(Material* material =
            dynamic_cast<Material*>(node)) {
    // repaint =
      onEditNodeFieldValueChanged_Material
      (material,fieldNumber,text);
  } else if(PixelTexture* pixelTexture =
            dynamic_cast<PixelTexture*>(node)) {
    // repaint =
      onEditNodeFieldValueChanged_PixelTexture
      (pixelTexture,fieldNumber,text);
  } else if(ImageTexture* imageTexture =
            dynamic_cast<ImageTexture*>(node)) {
    // repaint =
      onEditNodeFieldValueChanged_ImageTexture
      (imageTexture,fieldNumber,text);
  } else if(IndexedLineSet* indexedLineSet =
            dynamic_cast<IndexedLineSet*>(node)) {
    // repaint =
      onEditNodeFieldValueChanged_IndexedLineSet
      (indexedLineSet,fieldNumber,text);
  } else if(IndexedFaceSetPly* indexedFaceSetPy =
            dynamic_cast<IndexedFaceSetPly*>(node)) {
    // repaint =
      onEditNodeFieldValueChanged_IndexedFaceSetPly
      (indexedFaceSetPy,fieldNumber,text);
  } else if(IndexedFaceSet* indexedFaceSet =
            dynamic_cast<IndexedFaceSet*>(node)) {
    // repaint =
      onEditNodeFieldValueChanged_IndexedFaceSet
      (indexedFaceSet,fieldNumber,text);
  }

  updateState();

  QTreeWidgetItem* itemNode = nullptr;
  for(map<QTreeWidgetItem*,Node*>::iterator i=_mapItemNode.begin();
      i!=_mapItemNode.end();i++) {
    if(i->second==node) { itemNode = i->first; break; }
  }
  if(itemNode!=nullptr) {
    editNodeFieldValue->blockSignals(true);
    QModelIndex indexNode = sceneGraphTree->indexFromItem(itemNode);
    sceneGraphTree->selectionModel()
      ->select(indexNode, QItemSelectionModel::SelectCurrent);
    editNodeFieldValue->blockSignals(false);
  }

  sceneGraphNodeTree->blockSignals(true);
  for(int i=0;i<3;i++) {
    indexField = sceneGraphNodeTree->model()->index(rowField,i);
    sceneGraphNodeTree->selectionModel()
      ->select(indexField, QItemSelectionModel::Select);
  }
  sceneGraphNodeTree->blockSignals(false);

  SceneGraph* pWrl = getApp()->getMainWindow()->getSceneGraph();
  getApp()->getMainWindow()->setSceneGraph(pWrl,false);
  getApp()->getMainWindow()->refresh();

  log(QString("  \"%1\"").arg(text));
  log("}");
}

//////////////////////////////////////////////////////////////////////
bool GuiPanelSceneGraph::enableSelectedNodeButtons(Node *node) {
  log("GuiPanelSceneGraph::enableSelectedNodeButtons() {");
  bool value = false;
  if(node!=nullptr) {
    if(dynamic_cast<Shape*>(node)) {
      log("  selected node is Shape");
      value = true;
    } else if(dynamic_cast<SceneGraph*>(node)) {
      log("  selected node is SceneGraph");
      value = false;
    } else if(dynamic_cast<Transform*>(node)) {
      log("  selected node is Transform");
      value = true;
    } else if(dynamic_cast<Group*>(node)) {
      log("  selected node is Group");
      value = true;
    }
  }
#ifdef TODO
  toolButtonDeleteNode->setEnabled(value);
  toolButtonInsertGroupNode->setEnabled(value);
  toolButtonInsertTransformNode->setEnabled(value);
#endif
  log("}");
  return value;
}

//////////////////////////////////////////////////////////////////////
bool GuiPanelSceneGraph::enableSelectedNodeButtons() {
  return enableSelectedNodeButtons(getSelectedNode());
}

//////////////////////////////////////////////////////////////////////
void GuiPanelSceneGraph::on_toolButtonDeleteNode_clicked() {
#ifdef TODO
  Node* node = getSelectedNode();
  if(enableSelectedNodeButtons(node)==false) return;
  SceneGraph* pWrl = getApp()->getMainWindow()->getSceneGraph();
  SceneGraphProcessor sgp(*pWrl);
  sgp.deleteNode(node);
  getApp()->getMainWindow()->setSceneGraph(pWrl,false);
  getApp()->getMainWindow()->refresh();
  updateState();
#endif
}

//////////////////////////////////////////////////////////////////////
void GuiPanelSceneGraph::on_toolButtonInsertGroupNode_clicked() {
#ifdef TODO
  Node* node = getSelectedNode();
  if(enableSelectedNodeButtons(node)==false) return;
  SceneGraph* pWrl = getApp()->getMainWindow()->getSceneGraph();
  SceneGraphProcessor sgp(*pWrl);
  sgp.insertGroup(node);
  getApp()->getMainWindow()->setMainSceneGraph(pWrl,false);
  getApp()->getMainWindow()->refresh();
  updateState();
#endif
}

//////////////////////////////////////////////////////////////////////
void GuiPanelSceneGraph::on_toolButtonInsertTransformNode_clicked() {
#ifdef TODO
  Node* node = getSelectedNode();
  if(enableSelectedNodeButtons(node)==false) return;
  SceneGraph* pWrl = getApp()->getMainWindow()->getSceneGraph();
  SceneGraphProcessor sgp(*pWrl);
  sgp.insertTransform(node);
  getApp()->getMainWindow()->setMainSceneGraph(pWrl,false);
  getApp()->getMainWindow()->refresh();
  updateState();
#endif
}

//////////////////////////////////////////////////////////////////////
Node* GuiPanelSceneGraph::getSelectedNode() {
  log("GuiPanelSceneGraph::getSelectedNode() {");

  QList<QTreeWidgetItem*> items = sceneGraphTree->selectedItems();
  if(items.size()==0) {
    log("  nItems==0");
    log("}");
    return nullptr;
  }

  Node* node = _mapItemNode[items[0]];
  if(node==nullptr) {
    log("  node==nullptr");
    log("}");
    return nullptr;
  }

  log("}");
  return node;
}

//////////////////////////////////////////////////////////////////////
void GuiPanelSceneGraph::updateState() {

  _nodeRow.clear();
  _mapItemNode.clear();

  sceneGraphNodeTree->clear();

  sceneGraphTree->clear();
  SceneGraph* pWrl = getApp()->getMainWindow()->getSceneGraph();
  if(pWrl==nullptr) return;

  sceneGraphTree->blockSignals(true);

  vector<pair<Node*,QTreeWidgetItem*>> stack;
  pair<Node*,QTreeWidgetItem*> pNodeItem;
  pNodeItem.first = pWrl;
  pNodeItem.second= nullptr;
  stack.push_back(pNodeItem);

  while(stack.size()>0) {
    pNodeItem = stack.back(); stack.pop_back();

    Node*            node   = pNodeItem.first;
    QTreeWidgetItem* parent = pNodeItem.second;
    QTreeWidgetItem* child  = nullptr;
    QString          text   = "";

    // cout << "name = \"" << node->getName().c_str() << "\"" << endl;

    if(parent==nullptr) { // only for the root node

      _nodeRow.push_back(pWrl);

      child = new QTreeWidgetItem(sceneGraphTree);
      _mapItemNode[child] = pWrl;

      const string & url = pWrl->getUrl();
      size_t i    = url.find_last_of("/\\");
      // string dir  = url.substr(0,i);
      string file = url.substr(i+1);
      text = (file=="")?"SceneGraph":
        QString("SceneGraph [%1]").arg(file.c_str());
      child->setText(0,text);
      child->setExpanded(true);

    } else {

      _nodeRow.push_back(node);
      child = new QTreeWidgetItem(parent);
      _mapItemNode[child] = node;
      child->setExpanded(true);
      if(Transform * t = dynamic_cast<Transform*>(node)) {

        text = (t->getName()=="")?"Transform":
          QString("DEF %1 Transform").arg(t->getName().c_str());
        child->setText(0,text);

      } else if(Group* g = dynamic_cast<Group*>(node)) {

        text = (g->getName()=="")?"Group":
          QString("DEF %1 Group").arg(g->getName().c_str());
        child->setText(0,text);

      } else if(Shape* s = dynamic_cast<Shape*>(node)) {

        text = (s->getName()=="")?"Shape":
          QString("DEF %1 Shape").arg(s->getName().c_str());
        child->setText(0,text);

        Node* sa = s->getAppearance();

        if(Appearance* a = dynamic_cast<Appearance*>(sa)) {
          _nodeRow.push_back(a);
          QTreeWidgetItem* childA  = new QTreeWidgetItem(child);
          _mapItemNode[childA] = a;
          text = (a->getName()=="")?"Appearance":
            QString("DEF %1 Appearance").arg(a->getName().c_str());
          childA->setText(0,text);
          childA->setExpanded(true);

          if(Material* m =
             dynamic_cast<Material*>(a->getMaterial())) {
            _nodeRow.push_back(m);
            QTreeWidgetItem* childM  = new QTreeWidgetItem(childA);
            _mapItemNode[childM] = m;
            text = (m->getName()=="")?"Material":
              QString("DEF %1 Material").arg(m->getName().c_str());
            childM->setText(0,text);
          }

          if(ImageTexture* it =
             dynamic_cast<ImageTexture*>(a->getTexture())) {
            _nodeRow.push_back(it);
            QTreeWidgetItem* childIT  = new QTreeWidgetItem(childA);
            _mapItemNode[childIT] = it;
            text = (it->getName()=="")?"ImageTexture":
              QString("DEF %1 ImageTexture").arg(it->getName().c_str());
            childIT->setText(0,text);
          } else if(PixelTexture* pt =
                    dynamic_cast<PixelTexture*>(a->getTexture())) {
            _nodeRow.push_back(pt);
            QTreeWidgetItem* childPT  = new QTreeWidgetItem(childA);
            _mapItemNode[childPT] = pt;
            text = (it->getName()=="")?"PixelTexture":
              QString("DEF %1 PixelTexture").arg(it->getName().c_str());
            childPT->setText(0,text);
          }
        }

        Node* sg = s->getGeometry();

        if(IndexedFaceSetPly* ifsp =
           dynamic_cast<IndexedFaceSetPly*>(sg)) {
          _nodeRow.push_back(ifsp);
          QTreeWidgetItem* childG  = new QTreeWidgetItem(child);
          _mapItemNode[childG] = ifsp;
          text = (sg->getName()=="")?"IndexedFaceSetPly":
            QString("DEF %1 IndexedFaceSetPly").arg(sg->getName().c_str());
          childG->setText(0,text);
        } else if(IndexedFaceSet* ifs =
                  dynamic_cast<IndexedFaceSet*>(sg)) {
          _nodeRow.push_back(ifs);
          QTreeWidgetItem* childG  = new QTreeWidgetItem(child);
          _mapItemNode[childG] = ifs;
          text = (sg->getName()=="")?"IndexedFaceSet":
            QString("DEF %1 IndexedFaceSet").arg(sg->getName().c_str());
          childG->setText(0,text);
        } else if(IndexedLineSet* ils = 
                  dynamic_cast<IndexedLineSet*>(sg)) {
          _nodeRow.push_back(ils);
          QTreeWidgetItem* childG  = new QTreeWidgetItem(child);
          _mapItemNode[childG] = ils;
          text = (sg->getName()=="")?"IndexedLineSet":
            QString("DEF %1 IndexedLineSet").arg(sg->getName().c_str());
          childG->setText(0,text);
        }

      } // else { ERROR }

    }

    // append children of group
    if(Group* g = dynamic_cast<Group*>(node)) {
      int n = g->getNumberOfChildren();
      while((--n)>=0) {
        pNodeItem.first  = (*g)[n];
        pNodeItem.second = child;
        stack.push_back(pNodeItem);
      }
    }

  } // while(stack.size()>0)

  enableSelectedNodeButtons();
  sceneGraphTree->blockSignals(false);
}

//////////////////////////////////////////////////////////////////////
void GuiPanelSceneGraph::mousePressEvent(QMouseEvent * event) {
  (void) event;
  bool clickedOnLabel = false;

  // int x = event->x();
  // int y = event->y();

  // TBD

  if(clickedOnLabel) updateState();
}
