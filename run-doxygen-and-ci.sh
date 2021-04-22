#!/bin/bash

# SPDX-License-Identifier: MIT
#
# Copyright (c) 2020 Lukas Sommer sommerluk@gmail.com
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
mkdir --parents doc
cp doxyfile.internal Doxyfile
# We are not interested in the normal Doxygen output, but only in the errors.
# We have to filter the errors, because Doxygen produces errors where it
# should not (https://github.com/doxygen/doxygen/issues/7411 errors on
# missing documentation of return value for functions that return “void”).
# Therefore, first we redirect Doxygen’s stderr to stdout (the pipe) to
# be able to filter it with grep. And we redirect stdout to /dev/null
# (without changing where stderr is going):
doxygen 2>&1 >/dev/null \
    | grep \
        --invert-match \
        --perl-regexp "warning: return type of member .* is not documented" \
           | sed 's/^/Doxygen “public API and internals” documentation: /'
cp doxyfile.external Doxyfile
doxygen 2>&1 >/dev/null \
    | grep \
        --invert-match \
        --perl-regexp "warning: return type of member .* is not documented" \
           | sed 's/^/Doxygen “public API” documentation: /'





################# Static code check #################
PUBLIC_HEADERS="include"
CODE_WITHOUT_UNIT_TESTS="include src"
CODE_AND_UNIT_TESTS="include src test"
NON_PUBLIC_CODE="src test"
UNIT_TESTS="test"

# Search for files that do not start with a byte-order-mark (BOM).
# We do this because Microsoft’s compiler does require a BOM at the start
# of the file in order to interpretate it as UTF-8.
grep \
    --recursive \
    --files-without-match $'\xEF\xBB\xBF' \
    $CODE_AND_UNIT_TESTS \
         | sed 's/^/Missing byte-order-mark: /'

# All header files in src/ should have an “@internal” in
# the Doxygen documentation, because when it would be public,
# the header file would not be in src/
grep \
    --recursive \
    --files-without-match $'@internal' \
    src/*.h \
         | sed 's/^/Missing “@internal” statement in non-public header: /'

# All public header files in include/ should use
# the PERCEPTUALCOLOR_IMPORTEXPORT macro.
grep \
    --recursive \
    --files-without-match $'PERCEPTUALCOLOR_IMPORTEXPORT' \
    $PUBLIC_HEADERS \
         | sed 's/^/Missing PERCEPTUALCOLOR_IMPORTEXPORT macro in public header: /'

# All files should use perceptualcolorglobal.h
grep \
    --recursive \
    --files-without-match $'perceptualcolorglobal.h' \
    $CODE_AND_UNIT_TESTS \
         | sed 's/^/Missing include of perceptualcolorglobal.h: /'

# All non-public code should not use the PERCEPTUALCOLOR_IMPORTEXPORT macro.
grep \
    --recursive \
    --files-with-matches $'PERCEPTUALCOLOR_IMPORTEXPORT' \
    $NON_PUBLIC_CODE \
         | sed 's/^/Internal files may not use PERCEPTUALCOLOR_IMPORTEXPORT macro: /'

# All non-public code should use perceptualcolorinternal.h
grep \
    --recursive \
    --files-without-match $'perceptualcolorinternal.h' \
    $NON_PUBLIC_CODE \
         | sed 's/^/Internal files must include perceptualcolorinternal.h: /'

# Do not use constexpr in public headers as when we change the value
# later, compile time value and run time value might be different, and
# that might be dangerous.
grep \
    --recursive \
    --fixed-strings "constexpr" \
    $PUBLIC_HEADERS \
         | sed 's/^/Public headers may not use constexpr: /'

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
        --perl-regexp "\<tt\>cms" \
        --invert-match
grep \
    --recursive \
    --fixed-strings "lcms2.h" \
    $PUBLIC_HEADERS
    
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
#    against they are linking at run-time. This make maintaining binary
#    compatibility much harder, for little benefit.
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

# When using snippets, don’t do this within a namespace. As they are
# meant for documentation, they should always contain fully-qualified
# names to make sure that they always work:
# 1) Display all the files that are unit tests (adding “/*” to the UNIT_TESTS
#    variable to make sure to get a file list that “cat” will understand.
# 2) For each file, get all lines starting from the first occurence
#    of “namespace”, using sed.
# 3) Now, filter only those who actually contain snippet definitions
#    starting with “//!”, using grep.
for FILE in $UNIT_TESTS/*
do
    cat $FILE \
        | sed -n -e '/namespace/,$p' \
        | grep --fixed-strings "//!"
done





################# Unit tests #################
mkdir --parents build \
    && cd build \
    && cmake ../ > /dev/null \
    && make --jobs > /dev/null
ctest --verbose \
    | grep --invert-match --perl-regexp "^\d+: PASS   : " \
    | grep --invert-match --perl-regexp "^\d+: Test command: " \
    | grep --invert-match --perl-regexp "^\d+: Test timeout computed to be: " \
    | grep --invert-match --perl-regexp "^\d+: \*\*\*\*\*\*\*\*\*" \
    | grep --invert-match --perl-regexp "^\d+: Config: " \
    | grep --invert-match --perl-regexp "^\d+: Totals: " \
    | grep --invert-match --perl-regexp "^\s*\d+/\d+\sTest\s*#\d+:\s*\w+\s*\.*\s*Passed" \
    | grep --invert-match --perl-regexp "^test \d+" \
    | grep --invert-match --perl-regexp "^UpdateCTestConfiguration  from :" \
    | grep --invert-match --perl-regexp "^Test project " \
    | grep --invert-match --perl-regexp "^Constructing a list of tests" \
    | grep --invert-match --perl-regexp "^Done constructing a list of tests" \
    | grep --invert-match --perl-regexp "^Updating test list for fixtures" \
    | grep --invert-match --perl-regexp "^Added \d+ tests to meet fixture requirements" \
    | grep --invert-match --perl-regexp "^Checking test dependency graph\.\.\." \
    | grep --invert-match --perl-regexp "^Checking test dependency graph end" \
    | grep --invert-match --perl-regexp "^\d+:\s*[\.0123456789]* msecs per iteration" \
    | grep --invert-match --perl-regexp "^\d+: RESULT : " \
    | grep --invert-match --perl-regexp "^$"
