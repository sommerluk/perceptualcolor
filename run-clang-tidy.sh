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





################# Run clang-tidy #################

# We run “make” with the argument “--jobs” without specifying the number
# of parallel jobs. This means the number is jobs is maximum.
mkdir --parents build \
    && cd build \
    && echo "cmake…" \
    && nice --adjustment 19 cmake ../ > /dev/null \
    && echo "make…" \
    && nice --adjustment 19 make --jobs > /dev/null \
    && echo "cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON…" \
    && nice --adjustment 19 cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON .. \
    && echo "run-clang-tidy…" \
    && nice --adjustment 19 run-clang-tidy \
        | grep \
            --perl-regexp "^clang-tidy-10 -p=" \
            --invert-match
