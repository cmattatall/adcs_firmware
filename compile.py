import os
import sys
import subprocess
import argparse
import platform
import pathlib


def checkPythonVersion():
    if sys.version_info.major < 3: # python 3 must be the runtime
        raise Exception("%s must be executed using Python 3" % (os.path.basename(__file__)))

if __name__ == "__main__":
    checkPythonVersion() 

    # user changes this for now
    crossCompiling = "OFF"

    if(platform.system() == "Windows"):
        os.system("cmake -S . -B build -G \"MinGW Makefiles\" -DCMAKE_TOOLCHAIN_FILE=\"D:\\Desktop\\MyDev\\adcs_firmware\\toolchain.cmake\" -DCMAKE_CROSSCOMPILING=\"%s\"" % (crossCompiling))
        os.system("cmake --build build")

    elif(platform.system() == "Linux"):
        os.system("rm -r build")
        os.system("cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=\"toolchain.cmake\" -DCMAKE_CROSSCOMPILING=\"%s\"" % (crossCompiling))
    else:
        printf("%s not supported" % (platform.system()))
        exit(1)
    
    os.system("cmake --build build")
    