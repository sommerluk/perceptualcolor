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

# Run doxygen, but only show errors
doxygen > /dev/null

CODEDIRECTORIES="include src test"

# Search for some patterns that should not be used in the source code. If
# these patterns are found, a message is displayed. Otherwise, nothing is
# displayed. Pattern list:

# -> Do not use the “code” and “endcode” tags for Doxygen documentation. Use
#    @snippet instead! That allows that the example code is actually compiled
#    and that helps detecting errors.
grep --fixed-strings --recursive "\\code" $CODEDIRECTORIES
grep --fixed-strings --recursive "\\endcode" $CODEDIRECTORIES
grep --fixed-strings --recursive "@code" $CODEDIRECTORIES
grep --fixed-strings --recursive "@endcode" $CODEDIRECTORIES

# -> Doxygen style: Do not use “@em xyz”. Prefer instead “<em>xyz</em>” which
#    might be longer, but has a clearer start point and end point, which is
#    better when non-letter characters are involved. The @ is reserved
#    for @ref with semantically tested references.
# -> Same thing for “@c xyz”: Prefer instead “<tt>xyz</tt>”
grep --fixed-strings --recursive "\\em" $CODEDIRECTORIES
grep --fixed-strings --recursive "@em" $CODEDIRECTORIES
grep --fixed-strings --recursive "\\c" $CODEDIRECTORIES
grep --fixed-strings --recursive "@c" $CODEDIRECTORIES

# -> Coding style: Do not use the “NULL” macro, but its counterpart “nullptr”
#    which is more type save.
grep --fixed-strings --recursive "NULL" $CODEDIRECTORIES

# -> In some Qt classes, devicePixelRatio() returns in integer.
#    Don’t do that and use floating point precision instead. Often,
#    devicePixelRatioF() is an alternative that provides
#    a qreal return value.
grep --perl-regexp --recursive "devicePixelRatio(?!F)" $CODEDIRECTORIES

# Qt’s documentation about QImage::Format says: For optimal performance only
# use the format types QImage::Format_ARGB32_Premultiplied,
# QImage::Format_RGB32 or QImage::Format_RGB16. Any other format, including
# QImage::Format_ARGB32, has significantly worse performance.
grep --perl-regexp --recursive "QImage::Format_(?!(ARGB32_Premultiplied|RGB32|RGB16))" $CODEDIRECTORIES

echo && echo && echo && echo "Run unit tests…" && cd build && cmake ../ > /dev/null && make > /dev/null && make test
