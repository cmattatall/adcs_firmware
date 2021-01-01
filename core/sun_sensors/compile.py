# PYTHON SCRIPT TO HELP WITH CALLING CMAKE FOR USERS WHO ARE NOT USED TO
# USING CMAKE VIA CLI 

import os
import sys
import platform
import argparse
import shutil

def checkPythonVersion():
    # python 3 must be the runtime
    if sys.version_info.major < 3: 
        raise Exception(os.path.basename(__file__) + " must be executed using Python 3")

if __name__ == "__main__":
    checkPythonVersion()

    parser = argparse.ArgumentParser()
    parser.add_argument("--test", action="store_true", default=False, dest="test", help="Option to build and run the tests")
    parser.add_argument("--rebuild", action="store_true", default=False, dest="rebuild", help="Option to build clean")
    parser.add_argument("--build-dir", action="store", default="build", dest="bdir", help="String for the build directory")

    args = parser.parse_args()
    test= args.test
    rebuild=args.rebuild
    build_dir=args.bdir

    if rebuild:
        if os.path.exists(build_dir):
            shutil.rmtree(build_dir)
            os.mkdir(build_dir)

    if test:
        test_string = "ON"
    else:
        test_string = "OFF"
        
    os.system("cmake -S . -B \"%s\" -DBUILD_TESTING=%s" % (build_dir, test_string))
    os.system("cmake --build \"%s\"" % (build_dir))
    current_dir = os.getcwd()
    os.chdir(build_dir)
    os.system("ctest")
    os.chdir(current_dir)







