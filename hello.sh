#!/bin/bash

clang-15 -O0 -S -emit-llvm hello.c -o hello.ll
opt-15 -load-pass-plugin ./build/libHelloWorld.so -passes=hello-world -disable-output hello.ll