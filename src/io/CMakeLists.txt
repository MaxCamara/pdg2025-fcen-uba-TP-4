set(NAME io)

set(HEADERS
  AppLoader.hpp
  AppSaver.hpp
  Loader.hpp
  LoaderObj.hpp
  LoaderOff.hpp
  LoaderPly.hpp
  LoaderStl.hpp
  LoaderWrl.hpp
  Saver.hpp
  SaverObj.hpp
  SaverOff.hpp
  SaverPly.hpp
  SaverStl.hpp
  SaverWrl.hpp
  StrException.hpp
  Tokenizer.hpp
  TokenizerFile.hpp
  TokenizerString.hpp
) # HEADERS    

set(SOURCES
  AppLoader.cpp
  AppSaver.cpp
  LoaderObj.cpp
  LoaderOff.cpp
  LoaderPly.cpp
  LoaderStl.cpp
  LoaderWrl.cpp
  SaverObj.cpp
  SaverOff.cpp
  SaverPly.cpp
  SaverStl.cpp
  SaverWrl.cpp
  Tokenizer.cpp
  TokenizerFile.cpp
  TokenizerString.cpp
) # SOURCES

add_library(${NAME}
    ${HEADERS}
    ${SOURCES}
) # add_library

target_compile_features(${NAME} PRIVATE cxx_lambdas)

target_link_libraries(${NAME} ${LIB_LIST})

