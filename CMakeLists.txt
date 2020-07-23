# SPDX-License-Identifier: MIT
# 
# Copyright (c) 2020 Lukas Sommer somerluk@gmail.com
#  
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation
# files (the "Software"), to deal in the Software without
# restriction, including without limitation the rights to use,
# copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following
# conditions:
#  
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#  
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
# OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE. 

cmake_minimum_required(VERSION 3.9)
project(perceptualcolor DESCRIPTION "Perceptual color tools")

# Provide project version
set(PROJECT_MAJOR_VERSION "0")
set(PROJECT_MINOR_VERSION "0")
set(PROJECT_PATCH_VERSION "1")
set(PROJECT_VERSION "${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_PATCH_VERSION}")
# Substitutes all occurences of @PROJECT_VERSION@ in the source code file by the actual value in CMake
# configure_file( version_config.h.in ${CMAKE_BINARY_DIR}/generated/version_config.h )
# include_directories( ${CMAKE_BINARY_DIR}/generated/ ) # Make sure it can be included...

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
# Instruct CMake to create code from Qt designer ui files
# set(CMAKE_AUTOUIC ON)
# Use also our local FIND modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
# Enable compiler warings
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR
    "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(warnings "-Wall -Wextra -Werror -Wconversion -Wsign-conversion -pedantic-errors")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(warnings "/W4 /WX /EHsc")
endif()
message("-- Using compiler ${CMAKE_CXX_COMPILER_ID} with these warnings: ${warnings}")
# Get directories for installation process
include(GNUInstallDirs)

# Setup external library dependencies
find_package(Qt5 COMPONENTS Core Gui Widgets Test REQUIRED) # TODO require Test only for unit tests, not for normal building
find_package(LCMS2 REQUIRED)
include_directories(${LCMS2_INCLUDE_DIRS})
set(LIBS ${LIBS} Qt5::Core Qt5::Gui Qt5::Widgets ${LCMS2_LIBRARIES}) # Define external library dependencies


# TODO Do this only during development, not for release

# set(CMAKE_CXX_COMPILER "clazy")
# add_compile_options("-Xclang -plugin-arg-clazy -Xclang level0,no-qproperty-without-notify")
# set(ENV{CLAZY_CHECKS} "level2,no-qproperty-without-notify") # Why doesn't this work?

# # Find iwyu (include-what-you-use), a tool to check for unnecessary header includes
# find_program(iwyu_path NAMES include-what-you-use iwyu)
# if(NOT iwyu_path)
#   message(FATAL_ERROR "Could not find the program include-what-you-use")
# endif()


# Set the sources for our library
set(perceptualcolor_SRC
  src/alphaselector.cpp
  src/chromahuediagram.cpp
  src/chromalightnessdiagram.cpp
  src/colordialog.cpp
  src/colorpatch.cpp
  src/fullcolordescription.cpp
  src/gradientselector.cpp
  src/helper.cpp
  src/polarpointf.cpp
  src/rgbcolorspace.cpp
  src/simplecolorwheel.cpp
  src/wheelcolorpicker.cpp
)

# Set the headers for our library
set(perceptualcolor_HEADERS
  include/PerceptualColor/alphaselector.h
  include/PerceptualColor/chromahuediagram.h
  include/PerceptualColor/chromalightnessdiagram.h
  include/PerceptualColor/colordialog.h
  include/PerceptualColor/colorpatch.h
  include/PerceptualColor/fullcolordescription.h
  include/PerceptualColor/gradientselector.h
  include/PerceptualColor/helper.h
  include/PerceptualColor/polarpointf.h
  include/PerceptualColor/rgbcolorspace.h
  include/PerceptualColor/simplecolorwheel.h
  include/PerceptualColor/wheelcolorpicker.h
)

# Include directories
include_directories("${CMAKE_SOURCE_DIR}/src/")
include_directories("${CMAKE_SOURCE_DIR}/include/")

# Create our library
add_library(perceptualcolor SHARED ${perceptualcolor_SRC} ${perceptualcolor_HEADERS}) # The headers have to be included here because otherwise Qt's MOC would not find them because they are not in the same directory as the correponding .cpp files
set_target_properties(perceptualcolor PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(perceptualcolor PROPERTIES SOVERSION ${PROJECT_MAJOR_VERSION})
# set_target_properties(perceptualcolor PROPERTIES CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})
target_link_libraries(perceptualcolor ${LIBS})
install(TARGETS perceptualcolor LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Create our application
add_executable(perceptualcolorpicker src/main.cpp)
target_link_libraries(perceptualcolorpicker ${LIBS} perceptualcolor)
install(TARGETS perceptualcolorpicker DESTINATION ${CMAKE_INSTALL_BINDIR})

# Provide unit tests
enable_testing ()
add_executable (testhelper test/testhelper.cpp)
target_link_libraries (testhelper ${LIBS} Qt5::Test perceptualcolor)
add_test (NAME testhelper COMMAND testhelper)

add_executable (testpolarpointf test/testpolarpointf.cpp)
target_link_libraries (testpolarpointf ${LIBS} Qt5::Test perceptualcolor)
add_test (NAME testpolarpointf COMMAND testpolarpointf)

add_executable (testcolordialog test/testcolordialog.cpp)
target_link_libraries (testcolordialog ${LIBS} Qt5::Test perceptualcolor)
add_test (NAME testcolordialog COMMAND testcolordialog)

add_executable (testcolorpatch test/testcolorpatch.cpp)
target_link_libraries (testcolorpatch ${LIBS} Qt5::Test perceptualcolor)
add_test (NAME testcolorpatch COMMAND testcolorpatch)
