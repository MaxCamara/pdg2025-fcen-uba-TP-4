#
# vim:filetype=qmake sw=4 ts=4 expandtab nospell
#

FORMS = \
        $$FORMSDIR/GuiPanelGeneral.ui \
        $$FORMSDIR/GuiPanelSelection.ui \
        $$FORMSDIR/GuiPanelOptimization.ui \
        $$FORMSDIR/GuiPanelPolygonMesh.ui \
        $$FORMSDIR/GuiPanelRendering.ui \
        $$FORMSDIR/GuiPanelSceneGraph.ui \
        $$(NULL)

RESOURCES = \
	$$ASSETSDIR/assets.qrc

CORE_DIR = $$SOURCEDIR/core
GUI_DIR  = $$SOURCEDIR/gui
IO_DIR   = $$SOURCEDIR/io
UTIL_DIR = $$SOURCEDIR/util
WRL_DIR  = $$SOURCEDIR/wrl

SOURCES += \
	$$SOURCEDIR/core/Edges.cpp \
	$$SOURCEDIR/core/Faces.cpp \
	$$SOURCEDIR/core/Geometry.cpp \
	$$SOURCEDIR/core/Graph.cpp \
	$$SOURCEDIR/core/Heap.cpp \
	$$SOURCEDIR/core/HalfEdges.cpp \
	$$SOURCEDIR/core/HexGridPartition.cpp \
	$$SOURCEDIR/core/Partition.cpp \
	$$SOURCEDIR/core/PolygonMesh.cpp \
	$$SOURCEDIR/core/PolygonMeshTest.cpp \
	$$SOURCEDIR/core/Optimization.cpp \
	$$SOURCEDIR/core/Variable.cpp \
#
	$$SOURCEDIR/gui/GuiAboutDialog.cpp \
	$$SOURCEDIR/gui/GuiApplication.cpp \
	$$SOURCEDIR/gui/GuiGLBuffer.cpp \
	$$SOURCEDIR/gui/GuiGLHandles.cpp \
	$$SOURCEDIR/gui/GuiGLShader.cpp \
	$$SOURCEDIR/gui/GuiGLWidget.cpp \
	$$SOURCEDIR/gui/GuiMainWindow.cpp \
	$$SOURCEDIR/gui/GuiPanel.cpp \
	$$SOURCEDIR/gui/GuiPanelGeneral.cpp \
	$$SOURCEDIR/gui/GuiPanelOptimization.cpp \
	$$SOURCEDIR/gui/GuiPanelPolygonMesh.cpp \
	$$SOURCEDIR/gui/GuiPanelRendering.cpp \
	$$SOURCEDIR/gui/GuiPanelSelection.cpp \
	$$SOURCEDIR/gui/GuiPanelSceneGraph.cpp \
#	$$SOURCEDIR/gui/GuiPanelTemplate.cpp \
	$$SOURCEDIR/gui/GuiQtLogo.cpp \
	$$SOURCEDIR/gui/GuiViewerApp.cpp \
	$$SOURCEDIR/gui/GuiViewerData.cpp \
#
        $$SOURCEDIR/io/AppLoader.cpp \
	$$SOURCEDIR/io/AppSaver.cpp \
	$$SOURCEDIR/io/LoaderObj.cpp \
	$$SOURCEDIR/io/LoaderOff.cpp \
	$$SOURCEDIR/io/LoaderPly.cpp \
	$$SOURCEDIR/io/LoaderStl.cpp \
	$$SOURCEDIR/io/LoaderWrl.cpp \
	$$SOURCEDIR/io/SaverObj.cpp \
	$$SOURCEDIR/io/SaverOff.cpp \
	$$SOURCEDIR/io/SaverPly.cpp \
	$$SOURCEDIR/io/SaverStl.cpp \
	$$SOURCEDIR/io/SaverWrl.cpp \
	$$SOURCEDIR/io/Tokenizer.cpp \
	$$SOURCEDIR/io/TokenizerFile.cpp \
	$$SOURCEDIR/io/TokenizerString.cpp \
#
	$$SOURCEDIR/util/BBox.cpp \
	$$SOURCEDIR/util/Endian.cpp \
	$$SOURCEDIR/util/StaticRotation.cpp \
#
	$$SOURCEDIR/wrl/Ply.cpp \
	$$SOURCEDIR/wrl/Appearance.cpp \
	$$SOURCEDIR/wrl/Group.cpp \
	$$SOURCEDIR/wrl/ImageTexture.cpp \
	$$SOURCEDIR/wrl/IndexedLineSet.cpp \
	$$SOURCEDIR/wrl/IndexedLineSetVariables.cpp \
	$$SOURCEDIR/wrl/IndexedFaceSet.cpp \
	$$SOURCEDIR/wrl/IndexedFaceSetPly.cpp \
	$$SOURCEDIR/wrl/IndexedFaceSetVariables.cpp \
	$$SOURCEDIR/wrl/Material.cpp \
	$$SOURCEDIR/wrl/Node.cpp \
	$$SOURCEDIR/wrl/Types.cpp \
	$$SOURCEDIR/wrl/PixelTexture.cpp \
	$$SOURCEDIR/wrl/Rotation.cpp \
	$$SOURCEDIR/wrl/SceneGraph.cpp \
	$$SOURCEDIR/wrl/SceneGraphProcessor.cpp \
	$$SOURCEDIR/wrl/SceneGraphTraversal.cpp \
	$$SOURCEDIR/wrl/Shape.cpp \
	$$SOURCEDIR/wrl/Transform.cpp \
#
        $$(NULL)

HEADERS += \
	$$SOURCEDIR/core/Edges.hpp \
	$$SOURCEDIR/core/Faces.hpp \
	$$SOURCEDIR/core/Geometry.hpp \
	$$SOURCEDIR/core/Graph.hpp \
	$$SOURCEDIR/core/Heap.hpp \
	$$SOURCEDIR/core/HalfEdges.hpp \
	$$SOURCEDIR/core/HexGridPartition.hpp \
	$$SOURCEDIR/core/Partition.hpp \
	$$SOURCEDIR/core/PolygonMesh.hpp \
	$$SOURCEDIR/core/PolygonMeshTest.hpp \
	$$SOURCEDIR/core/Optimization.hpp \
	$$SOURCEDIR/core/Variable.hpp \
#
	$$SOURCEDIR/gui/GuiAboutDialog.hpp \
	$$SOURCEDIR/gui/GuiApplication.hpp \
	$$SOURCEDIR/gui/GuiGLBuffer.hpp \
	$$SOURCEDIR/gui/GuiGLHandles.hpp \
	$$SOURCEDIR/gui/GuiGLShader.hpp \
	$$SOURCEDIR/gui/GuiGLWidget.hpp \
	$$SOURCEDIR/gui/GuiMainWindow.hpp \
	$$SOURCEDIR/gui/GuiPanel.hpp \
	$$SOURCEDIR/gui/GuiPanelGeneral.hpp \
	$$SOURCEDIR/gui/GuiPanelOptimization.hpp \
	$$SOURCEDIR/gui/GuiPanelPolygonMesh.hpp \
	$$SOURCEDIR/gui/GuiPanelRendering.hpp \
	$$SOURCEDIR/gui/GuiPanelSceneGraph.hpp \
	$$SOURCEDIR/gui/GuiPanelSelection.hpp \
#	$$SOURCEDIR/gui/GuiPanelTemplate.hpp \
	$$SOURCEDIR/gui/GuiQtLogo.hpp \
	$$SOURCEDIR/gui/GuiSceneGraphTree.hpp \
	$$SOURCEDIR/gui/GuiStrings.hpp \
	$$SOURCEDIR/gui/GuiViewerApp.cpp \
	$$SOURCEDIR/gui/GuiViewerData.cpp \
	$$SOURCEDIR/gui/GuiViewerData.hpp \
#
	$$SOURCEDIR/io/AppLoader.hpp \
	$$SOURCEDIR/io/AppSaver.hpp \
	$$SOURCEDIR/io/Loader.hpp \
	$$SOURCEDIR/io/LoaderObj.hpp \
	$$SOURCEDIR/io/LoaderOff.hpp \
	$$SOURCEDIR/io/LoaderPly.hpp \
	$$SOURCEDIR/io/LoaderStl.hpp \
	$$SOURCEDIR/io/LoaderWrl.hpp \
	$$SOURCEDIR/io/Saver.hpp \
	$$SOURCEDIR/io/SaverObj.hpp \
	$$SOURCEDIR/io/SaverOff.hpp \
	$$SOURCEDIR/io/SaverPly.hpp \
	$$SOURCEDIR/io/SaverStl.hpp \
	$$SOURCEDIR/io/SaverWrl.hpp \
	$$SOURCEDIR/io/StrException.hpp \
	$$SOURCEDIR/io/Tokenizer.hpp \
	$$SOURCEDIR/io/TokenizerFile.hpp \
	$$SOURCEDIR/io/TokenizerString.hpp \
#
	$$SOURCEDIR/util/CastMacros.hpp \
	$$SOURCEDIR/util/BBox.hpp \
	$$SOURCEDIR/util/Endian.hpp \
	$$SOURCEDIR/util/StaticRotation.hpp \
#
	$$SOURCEDIR/wrl/Ply.hpp \
	$$SOURCEDIR/wrl/Appearance.hpp \
	$$SOURCEDIR/wrl/Group.hpp \
	$$SOURCEDIR/wrl/ImageTexture.hpp \
	$$SOURCEDIR/wrl/IndexedLineSet.hpp \
	$$SOURCEDIR/wrl/IndexedLineSetVariables.hpp \
	$$SOURCEDIR/wrl/IndexedFaceSet.hpp \
	$$SOURCEDIR/wrl/IndexedFaceSetPly.hpp \
	$$SOURCEDIR/wrl/IndexedFaceSetVariables.hpp \
	$$SOURCEDIR/wrl/Material.hpp \
	$$SOURCEDIR/wrl/Node.hpp \
	$$SOURCEDIR/wrl/Types.cpp \
	$$SOURCEDIR/wrl/PixelTexture.hpp \
	$$SOURCEDIR/wrl/Rotation.hpp \
	$$SOURCEDIR/wrl/SceneGraph.hpp \
	$$SOURCEDIR/wrl/SceneGraphProcessor.hpp \
	$$SOURCEDIR/wrl/SceneGraphTraversal.hpp \
	$$SOURCEDIR/wrl/Shape.hpp \
	$$SOURCEDIR/wrl/Transform.hpp \
#
	$$(NULL)

# $$EIGEN_DIR
INCLUDEPATH += $$SOURCEDIR $$CORE_DIR $$GUI_DIR $$IO_DIR $$UTIL_DIR $$WRL_DIR 

message(INCLUDEPATH = $$INCLUDEPATH)

