#!/bin/bash
# define which checks we want to run
export CLAZY_CHECKS="level2,no-qproperty-without-notify,assert-with-side-effects,container-inside-loop,detaching-member,heap-allocated-small-trivial-type,inefficient-qlist,isempty-vs-count,qhash-with-char-pointer-key,qproperty-type-mismatch,qrequiredresult-candidates,qstring-varargs,qt-keywords,qt4-qstring-from-array,qvariant-template-instantiation,raw-environment-function,reserve-candidates,signal-with-return-value,thread-with-slots,tr-non-literal,unneeded-cast"
# run clazy only for the targets perceptualcolor (the library) and perceptualcolorpicker (the application), but not for the unit tests
rm -rf build/* && cd build && cmake ../ -DCMAKE_CXX_COMPILER=clazy && make clean && make perceptualcolor && make perceptualcolorpicker && cd .. && rm -r build/*
