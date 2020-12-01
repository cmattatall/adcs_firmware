#!/bin/bash
rm -r build 
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=toolchain.cmake -DTARGET_MCU=ON
cmake --build build 