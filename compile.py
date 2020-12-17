import os
import platform
import sys
import pathlib

def checkPythonVersion():
    if sys.version_info.major < 3: # python 3 must be the runtime
        raise Exception(os.path.basename(__file__) + " must be executed using Python 3")

def compile_for_linux():
    os.system("cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=toolchain.cmake -DCMAKE_CROSSCOMPILING=ON")

def compile_for_windows():
    print("Apple is not yet supported!!")
    exit(1)

def compile_for_apple():
    print("Apple is not yet supported!!")
    exit(1)

def compile_the_project():
    if platform.system() == "Windows":
        compile_for_windows()
    elif platform.system() == "Linux":
        compile_for_linux()
    elif platform.system() == "Apple":
        compile_for_apple()
    else:
        print(platform.system() + " is not a supported platform!!")
        exit(1)

if __name__ == "__main__":
    checkPythonVersion()
    def compile_the_project():








