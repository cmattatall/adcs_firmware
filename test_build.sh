#!/bin/bash
# BASH SCRIPT TO EXECUTE REGRESSION TESTS UPON PROJECT
# THIS IS CALLED BY A GIT HOOK AS A PRE-PUSH EVENT.
# IF ANY TESTS FAIL, THE PUSH WILL BE REJECTED.
# 
# IT IS ADVISED THAT DEVELOPERS USE THIS DURING THE DEVELOPMENT 
# PROCESS TO CONFIRM THAT PAST FEATURES ARE NOT BEING BROKEN 

declare -a build_types=("Debug" "Release")

for type in "${build_types[@]}"
do
    python3 ./compile.py --rebuild --cross-compile --build-type ${type}
    if [ "$?" -ne 0 ]; then
        exit -1
    fi

    python3 ./compile.py --rebuild --build-type ${type} --run-tests
    if [ "$?" -ne 0 ]; then
        exit -1
    fi
done