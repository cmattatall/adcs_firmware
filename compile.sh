#!/bin/bash
rm -r build 
cmake -S . -B build -DTARGET_MCU=ON -DCMAKE_TOOLCHAIN_FILE=toolchain.cmake 
cmake --build build 