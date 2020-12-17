import os
import sys
import subprocess
import argparse
import platform
import pathlib
import shutil


def checkPythonVersion():
    if sys.version_info.major < 3: # python 3 must be the runtime
        raise Exception(os.path.basename(__file__) + " must be executed using Python 3")

def checkPlatform():
    supported_platforms = [ "Windows", "Linux"]
    if platform.system() not in supported_platforms:
        raise Exception(platform.system() + " is not a supported platform")

def query_yes_no(question, default="yes"):
    valid = {"yes": True, "y": True, "ye": True, "no": False, "n": False}
    if default is None:
        prompt = " [y/n] "
    elif default == "yes":
        prompt = " [Y/n] "
    elif default == "no":
        prompt = " [y/N] "
    else:
        raise ValueError("invalid default answer: '%s'" % default)

    while True:
        sys.stdout.write(question + prompt)
        choice = None

        # python 3 versus python 2 shenanigans
        if sys.version_info.major < 3:
            choice = raw_input().lower()
        else:
            choice = input().lower()

        if default is not None and choice == '':
            return valid[default]
        elif choice in valid:
            return valid[choice]
        else:
            sys.stdout.write("Please respond with 'yes' or 'no' (or 'y' or 'n').\n")

def compile():
    checkPythonVersion()
    checkPlatform()

    if query_yes_no("Are you cross compiling?") == True:
        crossCompiling="ON"
    else:
        crossCompiling="OFF"

    if(platform.system() == "Windows"):
        os.system("cmake -S . -B build -G \"MinGW Makefiles\" -DCMAKE_TOOLCHAIN_FILE=\"D:\\Desktop\\MyDev\\adcs_firmware\\toolchain.cmake\" -DCMAKE_CROSSCOMPILING=\"%s\"" % (crossCompiling))
        os.system("cmake --build build")

    elif(platform.system() == "Linux"):
        os.system("cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=\"toolchain.cmake\" -DCMAKE_CROSSCOMPILING=\"%s\"" % (crossCompiling))
    else:
        print("platform : %s not supported" % (platform.system()))
        exit(1)
    
    os.system("cmake --build build")


if __name__ == "__main__":
    compile()
    