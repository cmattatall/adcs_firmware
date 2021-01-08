#!/usr/bin/python3
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
import shutil
import multiprocessing

toolchain_file = "msp430toolchain.cmake" # just going to hard-code this for now

################################################################################
# SCRIPT START. DON'T TOUCH STUFF BEYOND THIS POINT
################################################################################

def checkPythonVersion():
    # python 3 must be the runtime
    if sys.version_info.major < 3: 
        raise Exception(os.path.basename(__file__) + " must be executed using Python 3")

def space_args(string):
    return " " + str(string) + " "


def configure(sdir=".", bdir="build", btype="Debug", cross_compile=False, build_tests=False, verbose=False):

    configure_string = space_args("cmake")

    if not os.path.exists(sdir):
        print("directory %s does not exist! Aborting!" % (sdir))
        exit(1)

    configure_string += space_args("-S %s" % (sdir))
    if not os.path.exists(bdir):
        # if we could check the return value of this, I would be sooo happy :(
        os.mkdir(bdir)
        if not os.path.exists(bdir):
            return -1

    configure_string += space_args("-B %s" % (bdir))
    configure_string += space_args("-DCMAKE_BUILD_TYPE=\"%s\"" % (btype))

    if cross_compile:
        configure_string += space_args("-DCMAKE_TOOLCHAIN_FILE=\"%s\"" % (toolchain_file))
        
    if build_tests:
        configure_string += space_args("-DBUILD_TESTING:BOOL=ON")
    else:
        configure_string += space_args("-DBUILD_TESTING:BOOL=OFF")

    return os.system(configure_string)

def make(bdir):
    retval = os.system("cmake --build \"%s\"" % (bdir))
    return retval

def run_tests(bdir, verbose = False):
    current_dir = os.getcwd()
    os.chdir(bdir)

    test_command = "ctest"

    if verbose:
        test_command += space_args("-V")
    else:
        test_command += space_args("-j %d" % (multiprocessing.cpu_count()))

    retval = os.system(test_command)
    os.chdir(current_dir)
    return retval


def main():
    checkPythonVersion()

    parser = argparse.ArgumentParser()
    parser.add_argument("--cross-compile", action="store_true", dest="cross", help="Option to cross compile for the target device")
    parser.add_argument("--build-type", action="store", dest="btype", choices=["Debug", "Release"], default ="Debug", help="String indicating the cmake build type")
    parser.add_argument("--source-dir", action="store", dest="sdir", default=os.getcwd(), help="name of directory that contains the top level CMakeLists.txt file")
    parser.add_argument("--run-tests", action="store_true", dest="run_tests", help="option to build and run the automated tests as well as the project source")
    parser.add_argument("--build-tests", action="store_true", dest="make_tests", help="Option to build (but not run) tests")
    parser.add_argument("--rebuild", action="store_true", default=False, dest="clean", help="Option to build clean")
    parser.add_argument("--build-dir", action="store", default="build", dest="bdir", help="String for the build directory")
    parser.add_argument("--verbose", action="store_true", default=False, dest="verbose", help="Option to emit verbose information during configuration, build, and test steps")

    args=parser.parse_args()

    if args.run_tests and not args.make_tests:
        # we can't run tests without building them first
        args.make_tests = True

    if args.clean:
        if os.path.exists(args.bdir):
            shutil.rmtree(args.bdir)
            os.mkdir(args.bdir)
            if not os.path.exists(args.bdir):
                if args.verbose:
                    print("COULD NOT CREATE DIRECTORY %s" % (args.bdir))
                exit(1)

    # test hooks don't override weak symbols for static
    # libraries on windows so we have to rebuild every time :(
    if platform.system() == "Windows" and args.run_tests:
            if os.path.exists(args.bdir):
                shutil.rmtree(args.bdir)
                os.mkdir(args.bdir)
                if not os.path.exists(args.bdir):
                    if args.verbose:
                        print("COULD NOT CREATE DIRECTORY %s" % (args.bdir))
                    exit(1)

    if 0 != configure(sdir=args.sdir, bdir=args.bdir,cross_compile=args.cross, btype=args.btype, build_tests=args.make_tests, verbose=args.verbose):
        if args.verbose:
            print("\nError configuring project!\n")
        exit(-1)
    
    if 0 != make(args.bdir):
        if args.verbose:
            print("\nError building the project!\n")
        exit(-1)

    if args.run_tests:
        if 0 != run_tests(args.bdir, verbose=args.verbose):
            if args.verbose:
                print("\nAt least one test failed!")
            exit(-1)

if __name__ == "__main__":
    main()