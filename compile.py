################################################################################
# @brief python script to build the ADCS firmware platform independently
# @author: Carl Mattatall (cmattatall2@gmail.com)
#
################################################################################
import os
import platform
import sys
import pathlib
import argparse

toolchain_file = "toolchain.cmake" # just going to hard-code this for now

################################################################################
# SCRIPT START. DON'T TOUCH STUFF BEYOND THIS POINT
################################################################################

def checkPythonVersion():
    # python 3 must be the runtime
    if sys.version_info.major < 3: 
        raise Exception(os.path.basename(__file__) + " must be executed using Python 3")

def space_args(string):
    return " " + str(string) + " "

def configure_for_windows(sdir=".", bdir="build", btype="Debug", cross=False, defs=[]):
    configure_string = space_args("cmake")
    if not os.path.exists(sdir):
        print("directory %s does not exist! Aborting!" % (sdir))
        exit(1)
    configure_string += space_args("-S %s" % (sdir))
    if not os.path.exists(bdir):
        os.mkdir(bdir)
    configure_string += space_args("-B %s" % (bdir))
    configure_string += space_args("-G \"MinGW Makefiles\"") 
    configure_string += space_args("-DCMAKE_TOOLCHAIN_FILE=\"%s\"" % (toolchain_file))
    configure_string += space_args("-DCMAKE_BUILD_TYPE=\"%s\"" % (btype))
    if cross == True:
        configure_string += space_args("-DCMAKE_CROSSCOMPILING=\"ON\"")
    else:
        configure_string += space_args("-DCMAKE_CROSSCOMPILING=\"OFF\"")
    for d in defs:
        configure_string += space_args("-D%s" % (d))
    os.system(configure_string)

def configure_for_linux(sdir=".", bdir="build", btype="Debug", cross=False, defs=[]):
    configure_string = space_args("cmake")
    if not os.path.exists(sdir):
        print("directory %s does not exist! Aborting!" % (sdir))
        exit(1)
    configure_string += space_args("-S %s" % (sdir))
    if not os.path.exists(bdir):
        os.mkdir(bdir)
    configure_string += space_args("-B %s" % (bdir))
    configure_string += space_args("-G \"Unix Makefiles\"")
    configure_string += space_args("-DCMAKE_TOOLCHAIN_FILE=\"%s\"" % (toolchain_file))
    configure_string += space_args("-DCMAKE_BUILD_TYPE=\"%s\"" % (btype))
    if cross == True:
        configure_string += space_args("-DCMAKE_CROSSCOMPILING=\"ON\"")
    else:
        configure_string += space_args("-DCMAKE_CROSSCOMPILING=\"OFF\"")
    for d in defs:
        configure_string += space_args("-D%s" % (d))
    os.system(configure_string)

def configure_for_apple(sdir=".", bdir="build", btype="Debug", cross=False):
    print("Apple is not yet supported!!")
    exit(1)

def compile_the_project():

    parser = argparse.ArgumentParser()
    parser.add_argument("--cross-compile", action="store_true", dest="cross_compile", help="Option to cross compile for the target device")
    parser.add_argument("--build-type", action="store", dest="build_type", choices=["Debug", "Release"], default ="Debug", help="String indicating the cmake build type")
    parser.add_argument("--definitions", action="store", dest="definitions", help="string of key-value pairs to be used for definitions. NOTE: NOT THOROUGHLY TESTED!", default="")

    args = parser.parse_args()
    cross_compile = args.cross_compile
    build_type = args.build_type
    my_defs = args.definitions

    ############################################################################
    # okay, so for now, we're going to rebuild everything from scratch each 
    # time. 
    #
    # why you may ask? 
    # because I can't get cmake to recognize that certain cache variables
    # need to be repopulated / reprocessed from toolchain file in the 
    # edge-case scenario where the following occurs:
    # 
    # 1. build for target device
    # 2. reload shell <-- can happen for any number of reasons
    # 3. build for native system.
    # 4. build fails because it loads the OLD cache variables from
    #    the toolchain file instead of reprocessing the toolchain file
    #    when the "project" directive is hit as is supposed to happen according
    #    to the documentation
    #    
    #    NOTE 
    #    as of december 17, I have submitted a bug report with Kitware 
    #    and they are working to resolve it (it has been a known issue
    #    for the last few months) 
    ############################################################################
    os.system("rm -r build")

    if platform.system() == "Windows":
        configure_for_windows(cross=cross_compile, btype=build_type, defs=my_defs)
    elif platform.system() == "Linux":
        configure_for_linux(cross=cross_compile, btype=build_type, defs=my_defs)
    elif platform.system() == "Apple":
        configure_for_apple(cross=cross_compile, btype=build_type, defs=my_defs)
    else:
        print(platform.system() + " is not a supported platform!!")
        exit(1)
    os.system("cmake --build build")

def run_tests():
    print("running tests")

if __name__ == "__main__":
    checkPythonVersion()
    compile_the_project()