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

# TODO Only force this in debug, not in production
set(CMAKE_C_COMPILER   "/usr/bin/clang"  )
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

cmake_minimum_required(VERSION 3.9)
project(perceptualcolor DESCRIPTION "Perceptual color tools")

# Provide project version
set(PROJECT_MAJOR_VERSION "0")
set(PROJECT_MINOR_VERSION "0")
set(PROJECT_PATCH_VERSION "1")
set(PROJECT_VERSION
    "${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_PATCH_VERSION}"
)
# # Substitutes all occurences of @PROJECT_VERSION@ in the source code file
# # by the actual value in CMake
# configure_file( version_config.h.in ${CMAKE_BINARY_DIR}/generated/version_config.h )
# # Make sure the generated file can be included:
# include_directories( ${CMAKE_BINARY_DIR}/generated/ )

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
# Instruct CMake to create code from Qt designer ui files
set(CMAKE_AUTOUIC ON)
# Use also our local FIND modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
# TODO Make the following code support more compilers
# Enable compiler warings and optimization
if (("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") OR
    ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))

    # Qt requieres C++11 since Qt 5.7. Source:
    # https://doc.qt.io/qt-5.9/cmake-manual.html#using-qt-5-with-cmake-older-than-3-1-0
    # Also our library requieres C++11. Therefore, we set -std=c++11 and
    # -Wpedantic and -pedantic-errors

    # The flag -Werror (treat all warnings as errors) is intentionally not
    # enabled: In new compiler versions, new warnings might be introduced.
    # These new warnings could now cause previously working build to fail.

    # Set common flags for both, GCC and Clang.
    set(CMAKE_CXX_FLAGS "\
        -O3\
        -std=c++11\
        -Wpedantic\
        -pedantic-errors\
        -Wall\
        -Wextra\
        -Wvla\
        -Wconversion\
        -Wsign-conversion\
    ")
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        # Add additional flags for GCC
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsuggest-override")
    endif()
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        # TODO Do not enable -Weverything (and the excluded checks)
        #      on non-debug builds
        # TODO Get rid of -Wno-float-equal and -Wno-c++2a-compat
        # Add additional flags for Clang
        set(CMAKE_CXX_FLAGS "\
            ${CMAKE_CXX_FLAGS}\
            -Wunreachable-code\
            -Weverything\
            -Wno-c++98-compat\
            -Wno-documentation\
            -Wno-redundant-parens\
            -Wno-padded\
            -Wno-extra-semi-stmt\
            -Wno-documentation-unknown-command\
            -Wno-covered-switch-default\
            -Wno-float-equal\
            -Wno-c++2a-compat\
        ")
    endif()
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(warnings "/W4 /WX /EHsc")
endif()
# Get directories for installation process
include(GNUInstallDirs)

# Setup external library dependencies
# TODO require Test only for unit tests, not for normal building
find_package(Qt5 COMPONENTS Core Gui Widgets Test REQUIRED)
find_package(LCMS2 REQUIRED)
include_directories(${LCMS2_INCLUDE_DIRS})
# Define external library dependencies
set(LIBS ${LIBS} Qt5::Core Qt5::Gui Qt5::Widgets ${LCMS2_LIBRARIES})


# TODO Do this only during development, not for release
# # Find iwyu (include-what-you-use), a tool to check for unnecessary
# # header includes:
# find_program(iwyu_path NAMES include-what-you-use iwyu)
# if(NOT iwyu_path)
#   message(FATAL_ERROR "Could not find the program include-what-you-use")
# endif()


# Set the sources for our library
set(perceptualcolor_SRC
  src/abstractdiagram.cpp
  src/alphaselector.cpp
  src/chromahuediagram.cpp
  src/chromalightnessdiagram.cpp
  src/circulardiagram.cpp
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
  include/PerceptualColor/abstractdiagram.h
  include/PerceptualColor/alphaselector.h
  include/PerceptualColor/chromahuediagram.h
  include/PerceptualColor/chromalightnessdiagram.h
  include/PerceptualColor/circulardiagram.h
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
# The headers have to be added here explicitly because otherwise Qt's MOC
# would not find them because they are not in the same directory as the
# correponding .cpp files
add_library(perceptualcolor SHARED
    ${perceptualcolor_SRC}
    ${perceptualcolor_HEADERS}
)
set_target_properties(perceptualcolor PROPERTIES VERSION
    ${PROJECT_VERSION}
)
set_target_properties(perceptualcolor PROPERTIES SOVERSION
    ${PROJECT_MAJOR_VERSION}
)
# set_target_properties(perceptualcolor PROPERTIES CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})
target_link_libraries(perceptualcolor ${LIBS})
install(
    TARGETS perceptualcolor
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Create our application
add_executable(perceptualcolorpicker src/main.cpp)
# Get warnings when using features that have been deprecated during
# the development cicle of Qt 5. So we can be sure to be well-prepared
# for Qt 6. TODO It should be on debug only, and not on release build!
target_compile_definitions(perceptualcolorpicker PRIVATE QT_DEPRECATED_WARNINGS)
target_link_libraries(perceptualcolorpicker ${LIBS} perceptualcolor)
install(TARGETS perceptualcolorpicker DESTINATION ${CMAKE_INSTALL_BINDIR})

# Provide unit tests

enable_testing ()

add_executable (testabstractdiagram test/testabstractdiagram.cpp)
target_link_libraries (testabstractdiagram ${LIBS} Qt5::Test perceptualcolor)
add_test (NAME testabstractdiagram COMMAND testabstractdiagram)

add_executable (testchromahuediagram test/testchromahuediagram.cpp)
target_link_libraries (testchromahuediagram ${LIBS} Qt5::Test perceptualcolor)
add_test (NAME testchromahuediagram COMMAND testchromahuediagram)

add_executable (testcirculardiagram test/testcirculardiagram.cpp)
target_link_libraries (testcirculardiagram ${LIBS} Qt5::Test perceptualcolor)
add_test (NAME testcirculardiagram COMMAND testcirculardiagram)

add_executable (testcolordialog test/testcolordialog.cpp)
target_link_libraries (testcolordialog ${LIBS} Qt5::Test perceptualcolor)
add_test (NAME testcolordialog COMMAND testcolordialog)

add_executable (testcolorpatch test/testcolorpatch.cpp)
target_link_libraries (testcolorpatch ${LIBS} Qt5::Test perceptualcolor)
add_test (NAME testcolorpatch COMMAND testcolorpatch)

add_executable (testhelper test/testhelper.cpp)
target_link_libraries (testhelper ${LIBS} Qt5::Test perceptualcolor)
add_test (NAME testhelper COMMAND testhelper)

add_executable (testpolarpointf test/testpolarpointf.cpp)
target_link_libraries (testpolarpointf ${LIBS} Qt5::Test perceptualcolor)
add_test (NAME testpolarpointf COMMAND testpolarpointf)
