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





################# General setup #################
cmake_minimum_required(VERSION 3.15)
# Project setup. We need only a C++ (CXX) compiler, and no C compiler.
# Therefore, we set LANGUAGES explicitly.
project(
    perceptualcolor
    DESCRIPTION "Perceptual color tools"
    LANGUAGES CXX
)
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Get predefined directories for installation process
include(GNUInstallDirs)





################# Version number #################
# Provide project version
# We are following “Semantic Versioning 2.0.0”.
# See https://semver.org/ for details.
set(PROJECT_MAJOR_VERSION "0")
set(PROJECT_MINOR_VERSION "0")
set(PROJECT_PATCH_VERSION "1")
set(PROJECT_VERSION
    "${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_PATCH_VERSION}"
)
# # Substitutes all occurrences of @PROJECT_VERSION@ in the source code file
# # by the actual value in CMake
# configure_file(
#     version_config.h.in
#     ${CMAKE_BINARY_DIR}/generated/version_config.h
# )
# # Make sure the generated file can be included:
# include_directories( ${CMAKE_BINARY_DIR}/generated/ )





################# Compiler settings #################
# Our choice for the required C++ standard is documented via Doxygen (main
# page of the documentation).
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# TODO Available compiler warnings change incompatible from version to version.
#      For a release build, no seldom extra options should be used!
# Enable compiler warings and optimization
if (("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU") OR
    ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))

    # TODO It might be better not to enable warnings in production builds,
    # as the available parameters might change or go away in later
    # compiler releases.

    # The flag -Werror (treat all warnings as errors) is intentionally not
    # enabled: In new compiler versions, new warnings might be introduced.
    # These new warnings could now cause previously working build to fail.

    # Set common flags for both, GCC and Clang.
    set(CMAKE_CXX_FLAGS "\
        -O3\
        -Wpedantic\
        -pedantic-errors\
        -Wall\
        -Wextra\
        -Wcast-align\
        -Wvla\
        -Wconversion\
        -Wsign-conversion\
        -finput-charset=UTF-8\
    ")
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        # TODO Do not enable -Weverything (and the following excluded checks)
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
    set(CMAKE_CXX_FLAGS "/W4 /WX /EHsc /utf-8")
endif()
add_definitions(
    -DQT_NO_CAST_FROM_ASCII
    -DQT_NO_CAST_FROM_BYTEARRAY
    -DQT_NO_CAST_TO_ASCII
    -DQT_NO_NARROWING_CONVERSIONS_IN_CONNECT
    -DQT_NO_PROCESS_COMBINED_ARGUMENT_START
    -DQT_NO_URL_CAST_FROM_STRING
    -DQT_USE_QSTRINGBUILDER
    -DQT_NO_KEYWORDS
    -DQT_DISABLE_DEPRECATED_BEFORE=0x060000
    #-DQT_STRICT_ITERATORS # This might crash on older Qt versions on Windows,
    # see https://bugreports.qt.io/browse/AUTOSUITE-946
)





################# Setup external library dependencies #################
# Use also our local FIND modules
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package(LCMS2 REQUIRED)
# TODO require Test only for unit tests, not for normal building
find_package(Qt5 COMPONENTS Core Gui Widgets Test REQUIRED)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
# Instruct CMake to create code from Qt designer ui files
set(CMAKE_AUTOUIC ON)
include_directories(${LCMS2_INCLUDE_DIRS})
# Define external library dependencies
set(LIBS ${LIBS} Qt5::Core Qt5::Gui Qt5::Widgets ${LCMS2_LIBRARIES})
# TODO xxx Is SVG support really necessary? It seems to work nevertheless
# even without linking it! Why?  It seems that SVG support is
# enabled automatically for icons via plugins like
# /usr/lib/x86_64-linux-gnu/qt5/plugins/iconengines/libqsvgicon.so
# or /usr/lib/x86_64-linux-gnu/qt5/plugins/imageformats/libqsvg.so which
# both are dynamically linking to /lib/x86_64-linux-gnu/libQt5Svg.so.5
# The plugins are loaded automatically when present. If not present,
# they are not available, but there is no linker error either. It seems
# possible to dynamically link to plugins, which would probably force
# the package manager to include a corresponding dependency. Not sure
# if in this case they are finally loaded as plugins or as dynamic
# libraries.





################# Setup source code #################
# Set the sources for our library
set(perceptualcolor_SRC
  src/abstractdiagram.cpp
  src/alphaselector.cpp
  src/chromahuediagram.cpp
  src/chromahueimage.cpp
  src/chromalightnessdiagram.cpp
  src/colordialog.cpp
  src/colorpatch.cpp
  src/colorwheelimage.cpp
  src/extendeddoublevalidator.cpp
  src/fallbackiconengine.cpp
  src/fullcolordescription.cpp
  src/gradientselector.cpp
  src/helper.cpp
  src/labdouble.cpp
  src/lchdouble.cpp
  src/multispinbox.cpp
  src/polarpointf.cpp
  src/rgbcolorspace.cpp
  src/rgbdouble.cpp
  src/simplecolorwheel.cpp
  src/wheelcolorpicker.cpp
)
# Set the headers for our library. This list only contains the headers
# that are _not_ within the source directory.
set(perceptualcolor_HEADERS
  include/PerceptualColor/abstractdiagram.h
  include/PerceptualColor/alphaselector.h
  include/PerceptualColor/chromahuediagram.h
  include/PerceptualColor/chromalightnessdiagram.h
  include/PerceptualColor/colordialog.h
  include/PerceptualColor/colorpatch.h
  include/PerceptualColor/constpropagatinguniquepointer.h
  include/PerceptualColor/extendeddoublevalidator.h
  include/PerceptualColor/fullcolordescription.h
  include/PerceptualColor/gradientselector.h
  include/PerceptualColor/labdouble.h
  include/PerceptualColor/lchdouble.h
  include/PerceptualColor/multispinbox.h
  include/PerceptualColor/perceptualcolorlib_global.h
  include/PerceptualColor/polarpointf.h
  include/PerceptualColor/rgbcolorspace.h
  include/PerceptualColor/rgbdouble.h
  include/PerceptualColor/simplecolorwheel.h
  include/PerceptualColor/wheelcolorpicker.h
)
# Include directories
include_directories("${CMAKE_SOURCE_DIR}/src/")
include_directories("${CMAKE_SOURCE_DIR}/include/")





################# Build library #################
# TODO enable this
# The headers have to be added here explicitly because otherwise Qt's MOC
# would not find them because they are not in the same directory as the
# corresponding .cpp files
# add_library(perceptualcolor SHARED
#     ${perceptualcolor_SRC}
#     ${perceptualcolor_HEADERS}
# )
# set_target_properties(perceptualcolor PROPERTIES CXX_VISIBILITY_PRESET
#     hidden
# )
# target_compile_definitions(perceptualcolor PUBLIC
#     PERCEPTUALCOLORLIB_BUILD_DYNAMIC_LIBRARY
# )
# set_target_properties(perceptualcolor PROPERTIES VERSION
#     ${PROJECT_VERSION}
# )
# set_target_properties(perceptualcolor PROPERTIES SOVERSION
#     ${PROJECT_MAJOR_VERSION}
# )
# target_link_libraries(perceptualcolor ${LIBS})
# install(
#     TARGETS perceptualcolor
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
# )
# # TODO Install headers to ${CMAKE_INSTALL_INCLUDEDIR}





################# Test bed #################
enable_testing ()

# The following library is like the target “perceptualcolor”, with the
#following difference: It exports all symbols (also private ones), and it
# does not install. This is used for the unit tests, because exporting all
# symbols is nececery for whitebox testing.
add_library(perceptualcolorexport SHARED
    ${perceptualcolor_SRC}
    ${perceptualcolor_HEADERS}
)
# Setting the symbol visibility to “visible”. Following GCC documentation,
# indeed the option for “visible” is called “default” (Sounds strange, but
# it's true.)
set_target_properties(perceptualcolorexport PROPERTIES CXX_VISIBILITY_PRESET
    default
)
target_link_libraries(perceptualcolorexport ${LIBS})
# TODO Do this only during development, not for release
# # Find iwyu (include-what-you-use), a tool to check for unnecessary
# # header includes:
# find_program(iwyu_path NAMES include-what-you-use iwyu)
# if(NOT iwyu_path)
#   message(FATAL_ERROR "Could not find the program include-what-you-use")
# endif()
# set_target_properties(perceptualcolor
#     PROPERTIES CXX_INCLUDE_WHAT_YOU_USE ${iwyu_path})

# Build a standalone application lauching the color dialog
add_executable(perceptualcolorpicker src/main.cpp)
target_link_libraries(perceptualcolorpicker ${LIBS} perceptualcolorexport)

# Define how to add unit tests.
# The argument “test_name” is expected to be the name of a .cpp test file
# in the test directory. For adding the unit test “test/testsomething.cpp”,
# you have to call “add_unit_test(testsomething)”.
function(add_unit_test test_name)
    add_executable (${test_name} test/${test_name}.cpp)
    target_link_libraries (${test_name} ${LIBS} Qt5::Test perceptualcolorexport)
    add_test (NAME ${test_name} COMMAND ${test_name})
endfunction(add_unit_test)

add_unit_test(testabstractdiagram)
add_unit_test(testchromahuediagram)
add_unit_test(testchromahueimage)
add_unit_test(testcolordialog)
add_unit_test(testcolorpatch)
add_unit_test(testcolorwheelimage)
add_unit_test(testconstpropagatinguniquepointer)
add_unit_test(testconstpropagatingrawpointer)
add_unit_test(testextendeddoublevalidator)
add_unit_test(testfallbackiconengine)
add_unit_test(testhelper)
add_unit_test(testlabdouble)
add_unit_test(testlchdouble)
add_unit_test(testlchvalues)
add_unit_test(testmultispinbox)
add_unit_test(testpolarpointf)
add_unit_test(testrgbdouble)
