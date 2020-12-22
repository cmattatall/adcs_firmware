#!/bin/bash
# BASH SCRIPT TO EVENTUALLY BE USED IN A CI PIPELINE

declare -a build_types=("Debug" "Release")

for type in "${build_types[@]}"
do
    python3 ./compile.py --rebuild --cross-compile --build-type ${type}
    if [ "$?" -ne 0 ]; then
        exit -1
    fi

    python3 ./compile.py --rebuild --cross-compile --build-type ${type} --test
    if [ "$?" -ne 0 ]; then
        exit -1
    fi
done