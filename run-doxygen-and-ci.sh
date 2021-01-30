#!/bin/bash

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





################# Compliance with REUSE specification #################
# Test if we provide all licenses as required by the “reuse” specification.
# This check needs the “reuse” application installed in your local bin
# directory. If you do not have that, you can install it with:
# pip3 install --user reuse
# Then, you have to make available $HOME/.local/bin/reuse in your path.
# Or, you can install it as root:
# sudo pip3 install reuse
# Then, you do not have to add it manually to the path.
reuse lint > /dev/null
if [ $? -eq 0 ];
then
    # Everything is fine. No message is printed.
    echo
else
    # “reuse lint” found problems. We call it again to print its messages.
    reuse lint
fi





################# Doxygen #################
# Run doxygen, but only show errors, no normal messages.
doxygen > /dev/null





################# Static code check #################
PUBLIC_HEADERS="include"
CODE_WITHOUT_UNIT_TESTS="include src"
CODE_AND_UNIT_TESTS="include src test"

# Search for some patterns that should not be used in the source code. If
# these patterns are found, a message is displayed. Otherwise, nothing is
# displayed.

# We do not include LittleCMS headers like lcms2.h in the public API of our
# library. But it is only be an internal dependency; library users should
# not need to care about that. Therefore, we grab all lines that contain
# identifiers starting with “cms” (except when in lines starting with
# “using”). This search is not done for all code directories, but only
# for files within the include directory (public API).
grep \
    --recursive \
    --perl-regexp "^cms" \
    $PUBLIC_HEADERS
grep \
    --recursive \
    --perl-regexp "[^a-zA-Z]cms[a-zA-Z0-9]" \
    $PUBLIC_HEADERS \
    | grep \
        --perl-regexp "(:using)|(\<tt\>cms)" \
        --invert-match

# -> Do not use the “code” and “endcode” tags for Doxygen documentation. Use
#    @snippet instead! That allows that the example code is actually compiled
#    and that helps detecting errors.
grep \
    --recursive \
    --fixed-strings "\\code" \
    $CODE_AND_UNIT_TESTS
grep \
    --recursive \
    --fixed-strings "\\endcode" \
    $CODE_AND_UNIT_TESTS
grep \
    --recursive \
    --fixed-strings "@code" \
    $CODE_AND_UNIT_TESTS
grep \
    --recursive \
    --fixed-strings "@endcode" \
    $CODE_AND_UNIT_TESTS

# -> Doxygen style: Do not use “@em xyz”. Prefer instead “<em>xyz</em>” which
#    might be longer, but has a clearer start point and end point, which is
#    better when non-letter characters are involved. The @ is reserved
#    for @ref with semantically tested references.
# -> Same thing for “@c xyz”: Prefer instead “<tt>xyz</tt>”.
grep \
    --recursive \
    --fixed-strings "\\em" \
    $CODE_AND_UNIT_TESTS
grep \
    --recursive \
    --fixed-strings "@em" \
    $CODE_AND_UNIT_TESTS
grep \
    --recursive \
    --fixed-strings "\\c" \
    $CODE_AND_UNIT_TESTS
grep \
   --recursive  \
    --perl-regexp "@c(?=([^a-zA-Z]|$))" \
    $CODE_AND_UNIT_TESTS

# -> Coding style: Do not use the “NULL” macro, but its counterpart “nullptr”
#    which is more type save.
grep \
    --recursive \
    --fixed-strings "NULL" \
    $CODE_AND_UNIT_TESTS

# -> Coding style: Do not use inline functions. If used in a header,
#    once exposed, they cannot be changed without breaking binary
#    compatibility, because applications linking against the library
#    will always execute the inline function version against they where
#    compiled, and never the inline function of the library version
#    against they are linking at run-time.
grep \
    --recursive \
    --fixed-strings "inline" \
    $CODE_AND_UNIT_TESTS

# -> In some Qt classes, devicePixelRatio() returns in integer.
#    Don’t do that and use floating point precision instead. Often,
#    devicePixelRatioF() is an alternative that provides
#    a qreal return value.
grep \
    --recursive \
    --perl-regexp "devicePixelRatio(?!F)" \
    $CODE_WITHOUT_UNIT_TESTS

# Qt’s documentation about QImage::Format says: For optimal performance only
# use the format types QImage::Format_ARGB32_Premultiplied,
# QImage::Format_RGB32 or QImage::Format_RGB16. Any other format, including
# QImage::Format_ARGB32, has significantly worse performance.
grep \
    --recursive \
    --perl-regexp "QImage::Format_(?!(ARGB32_Premultiplied|RGB32|RGB16))" \
    $CODE_AND_UNIT_TESTS





################# Unit tests #################
cd build \
    && cmake ../ > /dev/null \
    && make --jobs > /dev/null \
    && make --jobs test
