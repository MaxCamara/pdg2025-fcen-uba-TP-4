# list of source files
set(dgpTest4_files dgpTest4.cpp dgpPrt.cpp)

# define the executable
if(WIN32)
  add_executable(dgpTest4 WIN32 ${dgpTest4_files})
else()
  add_executable(dgpTest4 ${dgpTest4_files})
endif()

# in Windows + Visual Studio we need this to make it a console application
if(WIN32)
  if(MSVC)
    set_target_properties(dgpTest4 PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE")
  endif(MSVC)
endif(WIN32)

# install application
set(BIN_DIR ${CMAKE_INSTALL_PREFIX}/bin)

# target_link_libraries(dgpTest2 core io util wrl)
target_link_libraries(dgpTest4 ${LIB_LIST})

install(TARGETS dgpTest4 DESTINATION ${BIN_DIR})

