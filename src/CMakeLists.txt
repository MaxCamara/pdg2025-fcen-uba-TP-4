cmake_minimum_required(VERSION 4.0)

set(CMAKE_OSX_DEPLOYMENT_TARGET "15")

project(dgp2025)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# This variable is used to locate the bin directory where the dgpTest
# application will be installed
set(CMAKE_INSTALL_PREFIX
    ${PROJECT_SOURCE_DIR}/..
    CACHE PATH "Project bin directory" FORCE)
# you can comment the following line
# message("CMAKE_INSTALL_PREFIX = ${CMAKE_INSTALL_PREFIX}")

# you can comment the following line
# message ("CMAKE_PREFIX_PATH = ${CMAKE_PREFIX_PATH}") 

add_definitions(-DNOMINMAX -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -D_USE_MATH_DEFINES)

#add current dir to include search path
include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(io)
set(LIB_LIST ${LIB_LIST} io)

add_subdirectory(util)
set(LIB_LIST ${LIB_LIST} util)

add_subdirectory(core)
set(LIB_LIST ${LIB_LIST} core)

add_subdirectory(wrl)
set(LIB_LIST ${LIB_LIST} wrl)

# build command line executable ifsTest
add_subdirectory(test)
