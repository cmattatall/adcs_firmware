################################################################################
# @brief python script to build the ADCS firmware platform independently
# @author: Carl Mattatall (cmattatall2@gmail.com)
#
################################################################################
import os
import platform
import sys
import pathlib

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

def configure_for_windows(source_dir = ".", build_dir="build", definitions=[], cross=False):
    configure_string = space_args("cmake")
    if not os.path.exists(source_dir):
        print("directory %s does not exist! Aborting!" % (source_dir))
        exit(1)
    configure_string += space_args("-S %s" % (source_dir))
    if not os.path.exists(build_dir):
        os.mkdir(build_dir)
    configure_string += space_args("-B %s" % (build_dir))
    configure_string += space_args("-G \"MinGW Makefiles\"") 
    configure_string += space_args("-DCMAKE_TOOLCHAIN_FILE=\"%s\"" % (toolchain_file))
    if cross == True:
        configure_string += space_args("-DCMAKE_CROSSCOMPILING=\"ON\"")
    else:
        configure_string += space_args("-DCMAKE_CROSSCOMPILING=\"OFF\"")
    for d in definitions:
        configure_string += space_args("-D%s" % (d))
    os.system(configure_string)

def configure_for_linux(source_dir = ".", build_dir="build", definitions=[], cross=False):
    configure_string = space_args("cmake")
    if not os.path.exists(source_dir):
        print("directory %s does not exist! Aborting!" % (source_dir))
        exit(1)
    configure_string += space_args("-S %s" % (source_dir))
    if not os.path.exists(build_dir):
        os.mkdir(build_dir)
    configure_string += space_args("-B %s" % (build_dir))
    configure_string += space_args("-G \"Unix Makefiles\"")
    configure_string += space_args("-DCMAKE_TOOLCHAIN_FILE=\"%s\"" % (toolchain_file))
    configure_string += space_args("-DCMAKE_BUILD_TYPE=Debug")
    if cross == True:
        configure_string += space_args("-DCMAKE_CROSSCOMPILING=\"ON\"")
    else:
        configure_string += space_args("-DCMAKE_CROSSCOMPILING=\"OFF\"")
    for d in definitions:
        configure_string += space_args("-D%s" % (d))
    os.system(configure_string)

def configure_for_apple(source_dir = ".", build_dir="build", definitions=[], cross=False):
    print("Apple is not yet supported!!")
    exit(1)

def compile_the_project():
    cross_compiling = query_yes_no("Build for the microcontroller", default="no")

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
        configure_for_windows(cross = cross_compiling)
    elif platform.system() == "Linux":
        configure_for_linux(cross = cross_compiling)
    elif platform.system() == "Apple":
        configure_for_apple(cross = cross_compiling)
    else:
        print(platform.system() + " is not a supported platform!!")
        exit(1)
    os.system("cmake --build build")

def run_tests():
    print("running tests")

if __name__ == "__main__":
    checkPythonVersion()
    compile_the_project()







