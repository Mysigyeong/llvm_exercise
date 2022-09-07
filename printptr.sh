#!/bin/bash

clang-15 -O0 -S -emit-llvm test.c -o test.ll
opt-15 -load-pass-plugin ./build/libPrintPtr.so -passes=print-ptr test.ll -o test.opt.bc
llc-15 -filetype=obj test.opt.bc
clang-15 test.opt.bc -o test
./test