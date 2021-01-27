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

toolchain_file = "msp430gcc_toolchain.cmake" # just going to hard-code this for now

################################################################################
# SCRIPT START. DON'T TOUCH STUFF BEYOND THIS POINT
################################################################################

def checkPythonVersion():
    # python 3 must be the runtime
    if sys.version_info.major < 3: 
        raise Exception(os.path.basename(__file__) + " must be executed using Python 3")

def space_args(string):
    return " " + str(string) + " "


def configure(source_dir=".", bdir="build", btype="Debug", cross_compile=False, build_tests=False, verbose=False, build_examples=False, analyze=False):

    configure_string = space_args("cmake")

    if not os.path.exists(source_dir):
        print("directory %s does not exist! Aborting!" % (source_dir))
        exit(1)

    configure_string += space_args("-S %s" % (source_dir))
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
        if not cross_compile:
            # test executables typically cannot fit on an mcu
            configure_string += space_args("-DBUILD_TESTING:BOOL=ON")
        else:
            configure_string += space_args("-DBUILD_TESTING:BOOL=OFF")
    else:
        configure_string += space_args("-DBUILD_TESTING:BOOL=OFF")

    if build_examples:
        configure_string += space_args("-DBUILD_EXAMPLES:BOOL=ON")
    else:
        configure_string += space_args("-DBUILD_EXAMPLES:BOOL=OFF")

    # sadly, static analysis tools for microcontrollers frequently flag 
    # errors when they shouldnt...
    if(analyze):
        configure_string += space_args("-DANALYZE:BOOL=ON")
    else:
        configure_string += space_args("-DANALYZE:BOOL=OFF")

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
    parser.add_argument("--cross-compile", action="store_true", dest="cross", help="Cross compile for the target device.")
    parser.add_argument("--build-type", action="store", dest="btype", choices=["Debug", "Release"], default ="Debug", help="String indicating the cmake build type.")
    parser.add_argument("--source-dir", action="store", dest="source_dir", default=os.getcwd(), help="Path to the top level CMakeLists.txt file.")
    parser.add_argument("--run-tests", action="store_true", dest="run_tests", help="Build and run tests in addition to project.")
    parser.add_argument("--build-tests", action="store_true", dest="make_tests", help="Build (but don't run) tests.")
    parser.add_argument("--rebuild", action="store_true", default=False, dest="clean", help="Clean existing object files before building.")
    parser.add_argument("--build-dir", action="store", default="build", dest="bdir", help="String for the build directory.")
    parser.add_argument("--verbose", action="store_true", default=False, dest="verbose", help="Option to emit verbose information during generation.")
    parser.add_argument("--build-examples", action="store_true", default=False, dest="build_examples", help="Build the usage and API examples.")
    parser.add_argument("--analyze", action="store_true", default=False, dest="analyze", help="Run static analysis after the generation step.")
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

    if 0 != configure(source_dir=args.source_dir, bdir=args.bdir,cross_compile=args.cross, btype=args.btype, build_tests=args.make_tests, verbose=args.verbose, build_examples=args.build_examples, analyze=args.analyze):
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