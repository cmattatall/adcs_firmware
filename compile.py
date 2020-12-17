# @brief python script to build the ADCS firmware platform independently
# @author: Carl Mattatall (cmattatall2@gmail.com)
#

import os
import platform
import sys
import pathlib

# just hard coding this for now
toolchain_file = "toolchain.cmake"

def checkPythonVersion():
    # python 3 must be the runtime
    if sys.version_info.major < 3: 
        raise Exception(os.path.basename(__file__) + " must be executed using Python 3")

def space_args(string):
    return " " + str(string) + " "

def configure_for_linux(source_dir = ".", build_dir="build", definitions=[], cross_compiling=False):
    configure_string = space_args("cmake")
    configure_string += space_args("-S %s" % (source_dir))
    configure_string += space_args("-B %s" % (build_dir))
    configure_string += space_args("-DCMAKE_TOOLCHAIN_FILE=\"%s\"" % (toolchain_file))
    configure_string += space_args("-G \"UNIX MakeFiles") # DON'T KNOW IF I NEED THIS ONE HERE OR NOT
    if cross_compiling == True:
        configure_string += space_args("-DCMAKE_CROSSCOMPILING=\"ON\"")
    else:
        configure_string += space_args("-DCMAKE_CROSSCOMPILING=\"OFF\"")
    for d in definitions:
        configure_string += space_args("-D%s" % (d))
    os.system(configure_string)


def configure_for_windows(source_dir = ".", build_dir="build", definitions=[], cross_compiling=False):
    configure_string = space_args("cmake")
    configure_string += space_args("-S %s" % (source_dir))
    configure_string += space_args("-B %s" % (build_dir))
    configure_string += space_args("-DCMAKE_TOOLCHAIN_FILE=\"%s\"" % (toolchain_file))
    configure_string += space_args("-G \"MinGW MakeFiles") # DON'T KNOW IF I NEED THIS ONE HERE OR NOT
    if cross_compiling == True:
        configure_string += space_args("-DCMAKE_CROSSCOMPILING=\"ON\"")
    else:
        configure_string += space_args("-DCMAKE_CROSSCOMPILING=\"OFF\"")
    for d in definitions:
        configure_string += space_args("-D%s" % (d))
    os.system(configure_string)


def configure_for_apple(source_dir = ".", build_dir="build", definitions=[], cross_compiling=False):
    print("Apple is not yet supported!!")
    exit(1)

def compile_the_project():
    if platform.system() == "Windows":
        configure_for_windows()
    elif platform.system() == "Linux":
        configure_for_linux()
    elif platform.system() == "Apple":
        configure_for_apple()
    else:
        print(platform.system() + " is not a supported platform!!")
        exit(1)
    os.system("cmake --build build")

def run_tests():
    print("running tests")

if __name__ == "__main__":
    checkPythonVersion()
    compile_the_project()







