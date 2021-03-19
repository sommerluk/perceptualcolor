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

# TODO Update to newer Clazy version, which might provide more checks.

# Define which checks we want to run.
# Specific checks of level0-2 can be disabled.
# Exampele: no-qproperty-without-notify will disable the
# qproperty-without-notify check.
# Currently disabled are:
# no-inefficient-qlist-soft: In Qt6, QList and QVector will be aliases
# anyway. And, in Qt6 QList will be the default type. So we will
# follow this recommendation and always use QList.
export CLAZY_CHECKS="level2,\
no-inefficient-qlist-soft,\
assert-with-side-effects,\
container-inside-loop,\
detaching-member,\
heap-allocated-small-trivial-type,\
no-ifndef-define-typo,\
no-inefficient-qlist,\
isempty-vs-count,\
qhash-with-char-pointer-key,\
qproperty-type-mismatch,\
qrequiredresult-candidates,\
qstring-varargs,\
qt-keywords,\
qt4-qstring-from-array,\
qvariant-template-instantiation,\
raw-environment-function,\
reserve-candidates,\
signal-with-return-value,\
thread-with-slots,\
tr-non-literal,\
unneeded-cast"
# TODO Activate also:
# qt6-header-fixes
# qt6-qhash-signature
# qt6-fwd-fixes
# missing-qobject-macro
# which are recommandet for Qt6 porting
# at https://www.qt.io/blog/porting-from-qt-5-to-qt-6-using-clazy-checks
# but require Clazy 1.10

# We run “make” with the argument “--jobs” without specifying the number
# of parallel jobs. This means the number is jobs is maximum.
rm -rf build/* \
    && cd build \
    && cmake ../ -DCMAKE_CXX_COMPILER=clazy > /dev/null \
    && make clean \
    && make --jobs > /dev/null \
    && cd .. \
    && rm -r build/*
