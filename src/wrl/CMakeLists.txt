set(NAME wrl)

set(HEADERS
  Appearance.hpp
  Group.hpp
  ImageTexture.hpp
  IndexedFaceSet.hpp
  IndexedFaceSetPly.hpp
  IndexedLineSet.hpp
  IndexedFaceSetVariables.hpp
  IndexedLineSetVariables.hpp
  Material.hpp
  Node.hpp
  PixelTexture.hpp
  Ply.hpp
  Rotation.hpp
  SceneGraph.hpp
  SceneGraphProcessor.hpp
  SceneGraphTraversal.hpp
  Shape.hpp
  Transform.hpp
  Types.hpp
) # HEADERS    

set(SOURCES
  Appearance.cpp
  Group.cpp
  ImageTexture.cpp
  IndexedFaceSet.cpp
  IndexedFaceSetPly.cpp
  IndexedLineSet.cpp
  IndexedFaceSetVariables.cpp
  IndexedLineSetVariables.cpp
  Material.cpp
  Node.cpp
  PixelTexture.cpp
  Ply.cpp
  Rotation.cpp
  SceneGraph.cpp
  SceneGraphProcessor.cpp
  SceneGraphTraversal.cpp
  Shape.cpp
  Transform.cpp
  Types.cpp
) # SOURCES

add_library(${NAME}
    ${HEADERS}
    ${SOURCES}
) # add_library

target_compile_features(${NAME} PRIVATE cxx_lambdas)

target_link_libraries(${NAME} ${LIB_LIST})

