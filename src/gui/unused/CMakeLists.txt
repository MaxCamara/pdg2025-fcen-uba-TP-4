set(NAME dgp2023a3)

set(HEADERS
        GuiAboutDialog.hpp
        GuiApplication.hpp
        GuiGLBuffer.hpp
        GuiGLHandles.hpp
        GuiGLShader.hpp
        GuiGLWidget.hpp
        GuiMainWindow.hpp
        GuiPanel.hpp
        GuiPanelGeneral.hpp
#       GuiPanelIsocurves.hpp
        GuiPanelMesh.hpp
#       GuiPanelRemeshing.hpp
#       GuiPanelRemeshingICP.hpp
        GuiPanelRendering.hpp
        GuiPanelSelection.hpp
#       GuiPanelSmoothing.hpp
#       GuiPanelSurgery.hpp
#       GuiPanelTemplate.hpp
        GuiQtLogo.hpp
        GuiStrings.hpp
        GuiViewerData.hpp
) # HEADERS    

set(SOURCES
        GuiAboutDialog.cpp
        GuiApplication.cpp
        GuiGLBuffer.cpp
        GuiGLHandles.cpp
        GuiGLShader.cpp
        GuiGLWidget.cpp
        GuiMainWindow.cpp
        GuiPanel.cpp
        GuiPanelGeneral.cpp
#       GuiPanelIsocurves.cpp
        GuiPanelMesh.cpp
#       GuiPanelRemeshing.cpp
#       GuiPanelRemeshingICP.cpp
        GuiPanelRendering.cpp
        GuiPanelSelection.cpp
#       GuiPanelSmoothing.cpp
#       GuiPanelSurgery.cpp
#       GuiPanelTemplate.cpp
        GuiQtLogo.cpp
        GuiViewerApp.cpp
        GuiViewerData.cpp
) # SOURCES

qt5_wrap_ui(FORMS
) # FORMS

qt5_add_resources(ASSETS
) # ASSETS

if (APPLE)
  set(APP_ICON ${ASSETS_DIR}/bunny.icns)
  set_source_files_properties(${APP_ICON} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
  add_executable(${NAME} MACOSX_BUNDLE ${APP_ICON}
      ${HEADERS}
      ${SOURCES}
      ${FORMS}
      ${ASSETS}
  ) # add_executable
  set(MACOSX_BUNDLE_ICON_FILE "bunny.icns" )
else(APPLE)
  add_executable(${NAME}
      ${HEADERS}
      ${SOURCES}
      ${FORMS}
      ${ASSETS}
  ) # add_executable
endif(APPLE)

target_compile_features(${NAME} PRIVATE cxx_right_angle_brackets cxx_lambdas)

target_link_libraries(${NAME} ${LIB_LIST})
