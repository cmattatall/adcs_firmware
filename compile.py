import os
import sys
import pathlib
import subprocess
import argparse
import platform

def checkPythonVersion():
    if sys.version_info.major < 3: # python 3 must be the runtime
        raise Exception("%s must be executed using Python 3" % (os.path.basename(__file__)))

if __name__ == "__main__":
    checkPythonVersion() 

    # user changes this for now
    crossCompiling = "OFF"

    '''
    nativeOutputDirPathObj = pathlib.Path(args.output_dir)
    nativeBuildDirPathObj  = pathlib.Path(args.build_dir)
    nativeSourceDirPathObj = pathlib.Path(args.source_dir)

    nativeOutputDirPathObj = pathlib.Path(str(nativeOutputDirPathObj).replace(' ', '\ ').lstrip().rstrip())
    nativeBuildDirPathObj = pathlib.Path(str(nativeBuildDirPathObj).replace(' ', '\ ').lstrip().rstrip())
    nativeSourceDirPathObj = pathlib.Path(str(nativeSourceDirPathObj).replace(' ', '\ ').lstrip().rstrip())

    current_dir = os.getcwd()
    nativeCurrentDirPathObj = pathlib.Path(current_dir)
    
    posixCurrentDirPathObj = None
    posixBuildDirPathObj = None
    posixOutputDirPathObj = None
    posixCurrentDirPathObj = pathlib.PurePosixPath((nativeCurrentDirPathObj.root/(nativeCurrentDirPathObj.relative_to(nativeCurrentDirPathObj.anchor))).as_posix())
    posixCurrentDirPathObj = pathlib.PurePosixPath(pathlib.PurePosixPath(str(posixCurrentDirPathObj)).as_posix().replace(' ', ' ').lstrip().rstrip())

    posixBuildDirPathObj = pathlib.PurePosixPath((nativeBuildDirPathObj.root/(nativeBuildDirPathObj.relative_to(nativeBuildDirPathObj.anchor))).as_posix())
    posixBuildDirPathObj = pathlib.PurePosixPath(pathlib.PurePosixPath(str(posixBuildDirPathObj)).as_posix().replace(' ', ' ').lstrip().rstrip())

    posixOutputDirPathObj = pathlib.PurePosixPath((nativeOutputDirPathObj.root/(nativeOutputDirPathObj.relative_to(nativeOutputDirPathObj.anchor))).as_posix())
    posixOutputDirPathObj = pathlib.PurePosixPath(pathlib.PurePosixPath(str(posixOutputDirPathObj)).as_posix().replace(' ', ' ').lstrip().rstrip())
    '''

    if(platform.system() == "Windows"):
        os.system("cmake -S . -B build -G \"MinGW Makefiles\" -DCMAKE_TOOLCHAIN_FILE=\"D:\\Desktop\\MyDev\\adcs_firmware\\toolchain.cmake\" -DCMAKE_CROSSCOMPILING=\"%s\"" % (crossCompiling))
        os.system("cmake --build build")

    elif(platform.system() == "Linux"):
        os.system("cmake -S . -B build -G \"Unix Makefiles\" -DCMAKE_TOOLCHAIN_FILE=\"D:\\Desktop\\MyDev\\adcs_firmware\\toolchain.cmake\" -DCMAKE_CROSSCOMPILING=\"%s\"" % (crossCompiling))
    else:
        printf("%s not supported" % (platform.system()))
        exit(1)
    