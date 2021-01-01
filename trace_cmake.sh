#!/bin/bash
rm -r log.txt; rm -r build/; cmake -S . -B build --trace  -DCMAKE_TOOLCHAIN_FILE=msp430toolchain.cmake -DCMAKE_CROSSCOMPILING=ON > log.txt 2>&1