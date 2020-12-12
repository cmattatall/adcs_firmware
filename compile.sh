#!/bin/bash
rm -r build 
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=toolchain.cmake 
cmake --build build 