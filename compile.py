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

def configure_for_windows(sdir=".", bdir="build", btype="Debug", cross=False, defs=[], t=False):
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
    configure_string += space_args("-G \"MinGW Makefiles\"") 
    configure_string += space_args("-DCMAKE_TOOLCHAIN_FILE=\"%s\"" % (toolchain_file))
    configure_string += space_args("-DCMAKE_BUILD_TYPE=\"%s\"" % (btype))
    
    if cross == True:
        configure_string += space_args("-DCMAKE_CROSSCOMPILING:BOOL=ON")
    else:
        configure_string += space_args("-DCMAKE_CROSSCOMPILING:BOOL=OFF")

    if t == True:
        configure_string += space_args("-DBUILD_TESTING:BOOL=ON")
    else:
        configure_string += space_args("-DBUILD_TESTING:BOOL=OFF")

    for d in defs:
        configure_string += space_args("-D%s" % (d))
    return os.system(configure_string)

def configure_for_linux(sdir=".", bdir="build", btype="Debug", cross=False, defs=[], t=False):
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
    configure_string += space_args("-G \"Unix Makefiles\"")
    configure_string += space_args("-DCMAKE_TOOLCHAIN_FILE=\"%s\"" % (toolchain_file))
    configure_string += space_args("-DCMAKE_BUILD_TYPE=\"%s\"" % (btype))

    if cross == True:
        configure_string += space_args("-DCMAKE_CROSSCOMPILING:BOOL=ON")
    else:
        configure_string += space_args("-DCMAKE_CROSSCOMPILING:BOOL=OFF")

    if t == True:
        configure_string += space_args("-DBUILD_TESTING:BOOL=ON")
    else:
        configure_string += space_args("-DBUILD_TESTING:BOOL=OFF")

    for d in defs:
        configure_string += space_args("-D%s" % (d))
    retval = os.system(configure_string)


    return retval

def configure_for_apple(sdir=".", bdir="build", btype="Debug", cross=False, defs=[], t=False):
    print("Apple is not yet supported!!")
    exit(1)


def configure_the_project(sdir=".", bdir="build", btype="Debug", cross=False, defs=[], t=False):
    if platform.system() == "Windows":
        retval = configure_for_windows(cross=cross, btype=btype, defs=defs, t=t)
    elif platform.system() == "Linux":
        retval = configure_for_linux(cross=cross, btype=btype, defs=defs, t=t)
    elif platform.system() == "Apple":
        retval = configure_for_apple(cross=cross, btype=btype, defs=defs, t=t)
    else:
        print(platform.system() + " is not a supported platform!!")
        exit(-1)
    return retval

def build_the_project(bdir):
    retval = os.system("cmake --build \"%s\"" % (bdir))
    return retval

def run_tests(bdir, verbose = False):
    current_dir = os.getcwd()
    os.chdir(bdir)
    if verbose:
        retval = os.system("ctest -V")
    else:
        retval = os.system("ctest")
    os.chdir(current_dir)
    return retval


def main():
    checkPythonVersion()

    parser = argparse.ArgumentParser()
    parser.add_argument("--cross-compile", action="store_true", dest="cross_compile", help="Option to cross compile for the target device")
    parser.add_argument("--build-type", action="store", dest="build_type", choices=["Debug", "Release"], default ="Debug", help="String indicating the cmake build type")
    parser.add_argument("--definitions", action="store", dest="definitions", help="string of key-value pairs to be used for definitions. NOTE: NOT THOROUGHLY TESTED!", default="")
    parser.add_argument("--test", action="store_true", dest="test", help="option to build and run the automated tests as well as the project source")
    parser.add_argument("--rebuild", action="store_true", default=False, dest="rebuild", help="Option to build clean")
    parser.add_argument("--build-dir", action="store", default="build", dest="bdir", help="String for the build directory")
    parser.add_argument("--verbose", action="store_true", default=False, dest="verbose", help="Option to emit verbose information during configuration, build, and test steps")

    args = parser.parse_args()
    cross_compile = args.cross_compile
    build_type = args.build_type
    my_defs = args.definitions
    test=args.test
    rebuild=args.rebuild
    build_dir=args.bdir
    verbose=args.verbose

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

    if rebuild:
        if os.path.exists(build_dir):
            shutil.rmtree(build_dir)
            os.mkdir(build_dir)
            if not os.path.exists(build_dir):
                if verbose:
                    print("COULD NOT CREATE DIRECTORY %s" % (build_dir))
                exit(1)
    elif platform.system() == "Windows":
        if test:
            # test hooks don't override weak symbols for static
            # libraries on windows so we have to rebuild :(
            if os.path.exists(build_dir):
                shutil.rmtree(build_dir)
                os.mkdir(build_dir)
                if not os.path.exists(build_dir):
                    if verbose:
                        print("COULD NOT CREATE DIRECTORY %s" % (build_dir))
                    exit(1)


    cfg_retval = configure_the_project(cross=cross_compile, btype=build_type, defs=my_defs, t=test)
    if 0 != cfg_retval:
        if verbose:
            print("\nError configuring project! Process exited with code: " + str(cfg_retval) + "\n")
        exit(cfg_retval)

    build_retval = build_the_project(build_dir)
    if 0 != build_retval:
        if verbose:
            print("\nError building the project! Process exited with code:" + str(build_retval) + "\n")
        exit(15)

    if(test):
        test_retval = run_tests(build_dir, verbose=verbose)
        if 0 != test_retval:
            if verbose:
                print("\nOne or more automated tests did not pass! Exited with code:" + str(test_retval) + "\n")
            exit(test_retval)

if __name__ == "__main__":
    main()