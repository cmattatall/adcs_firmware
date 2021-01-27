#!/usr/bin/python3
import os

if __name__ == "__main__":
    build_types=["Debug", "Release"]
    for btype in build_types:
        retval = os.system("python3 compile.py --cross-compile --rebuild --build-type %s --build-examples --analyze" % (btype))
        if 0 != retval:
            exit(retval)

        retval = os.system("python3 ./compile.py --rebuild --build-type %s --run-tests --build-examples --analyze" % (btype))
        if 0 != retval:
            exit(retval)    
            
    