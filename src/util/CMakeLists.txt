set(NAME util)

set(HEADERS
  CastMacros.hpp
  BBox.hpp
  Endian.hpp
  StaticRotation.hpp
) # HEADERS    

set(SOURCES
  BBox.cpp
  Endian.cpp
  StaticRotation.cpp
) # SOURCES

add_library(${NAME}
    ${HEADERS}
    ${SOURCES}
) # add_library

target_compile_features(${NAME} PRIVATE cxx_lambdas)

target_link_libraries(${NAME} ${LIB_LIST})

