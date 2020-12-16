import os
import sys
import argparse
import shutil
import platform
import pathlib
import shutil
import json
import subprocess

def checkPythonVersion():
    if sys.version_info.major < 3: # python 3 must be the runtime
        raise Exception("%s must be executed using Python 3" % (os.path.basename(__file__)))

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

# This updates the vscode intellisense file compile_commands.json emitted in the docker container 
# to work on the native path
# the compiler toolchain arm-none-eabi-gcc should be present in the user's PATH (absolute, executable, hardlink, or symlink)
# the compiler toolchain sysroot must also be present somewhere in the user's system
def update_intellisense(PathLibObject, old_dir, new_dir):
    if PathLibObject.exists():
        if PathLibObject.is_file():
            text = PathLibObject.read_text()
            compileCommandsJsonObject = json.loads(text)
            for command in compileCommandsJsonObject:
                for key in command.keys():
                    command[key] = str(command[key]).replace(old_dir, new_dir)
                    if key == "command":
                        tmp = str(command[key])
                        compilerPathObj = pathlib.Path(tmp.split(' ', 1)[0])
                        oldCompilerPath = compilerPathObj.as_posix()
                        newCompilerPath = compilerPathObj.name
                        tmp = tmp.replace(oldCompilerPath, newCompilerPath)
                        command[key] = tmp
            PathLibObject.write_text(json.dumps(indent=2, obj=compileCommandsJsonObject))
        else:
            print("%s exists but is NOT a file" % (str(PathLibObject)))
    else:
        print("%s does not exist so could not configure intellisense." % (str(PathLibObject)))



def do_build():
    checkPythonVersion() 
    if platform.system() != "Windows" and platform.system() != "Linux":
        print(nameOfThisFile + " only supports builds from Windows or Linux environments")
        exit(1)
    
    # configure CLI args
    parser = argparse.ArgumentParser(description="parse command line args for docker build script")
    parser.add_argument("--output_dir", action="store", dest="output_dir", default="bin", help="The output directory name for compiled binaries")
    parser.add_argument("--build_type", action="store", dest="buildTypeString", default="Debug", help="Build mode : [ Debug, Release, MinSizeRel, RelWithDebInfo ]")
    parser.add_argument("--build_dir", action="store", dest="build_dir", default="objects", help="directory name in which to generate the cmake build pipeline")
    parser.add_argument("--image_repo", action="store", dest="docker_repo", default="local_images", help="docker image repository to contain the docker image nametag")
    parser.add_argument("--image_tag", action="store", dest="docker_nametag", default="docker_build_image", help="the name of the docker image")
    parser.add_argument("--source_dir", action="store", dest="source_dir", default=os.getcwd(), help="the name of the directory containing top-level CMakeLists.txt")
    parser.add_argument("--verbose", action="store", dest="build_verbose", default=False, help="Whether the build should be performed using a verbose makefile")
    parser.add_argument("--overwrite", action="store", dest="overwrite", default=False, help="Ignore warning prompts when overwriting output and build directories")
    parser.add_argument("--load", action="store", dest="image_file", default=None, help="instead of building a docker image, load a docker build image")
    parser.add_argument("--clean", action="store", dest="rebuild", default=False, help="rebuild all source files instead of just deltas")
    parser.add_argument("--cwd", action="store", dest="workdir", default=os.getcwd(), help="set the working execution directory for the build script")
    parser.add_argument("--cross_compile", action="store", dest="cross_compile", default=False, help="Flag to indicate cross compilation for target device (default is build for host machine using native toolchain)")

    # parse CLI args
    args = parser.parse_args()
    workdir = args.workdir
    nativeOutputDirPathObj = pathlib.Path(args.output_dir)
    nativeBuildDirPathObj  = pathlib.Path(args.build_dir)
    nativeSourceDirPathObj = pathlib.Path(args.source_dir)
    buildTypeString = str(args.buildTypeString)
    docker_tag   = "%s:%s" % (args.docker_repo, args.docker_nametag)
    docker_image_filename = "%s_%s" % (args.docker_repo, args.docker_nametag)
    container = "temporary_build_container"
    build_verbose = args.build_verbose
    overwrite = args.overwrite
    image_file = args.image_file
    rebuild = args.rebuild
    current_dir = pathlib.Path(os.getcwd())/pathlib.Path(str(__file__))

    #print("current_dir = %s" % (current_dir))
    #print("workdir = %s" % (workdir))
    if(args.cross_compile):
        cross_compile = "ON"
    else:
        cross_compile = "OFF"

    # validate CLI args
    if not nativeSourceDirPathObj.exists():
        print("[ERROR] source code directory %s does not exist" % (str(nativeSourceDirPathObj)))
        exit(1)

    if nativeOutputDirPathObj.exists():
        if overwrite == False:
            should_continue = query_yes_no("[WARNING] output directory: %s exists and will be overwritten post-build. Continue?" % (str(nativeOutputDirPathObj)), "yes")
            if False == should_continue:
                print("Aborting...")
                exit(0)

    if nativeBuildDirPathObj.exists():
        if overwrite == False:
            should_continue = query_yes_no("[WARNING] build directory: %s exists and will be overwritten post-build. Continue?" % (str(nativeBuildDirPathObj)), "yes")
            if False == should_continue:
                print("Aborting...")
                exit(0)

    # normalize CLI args that are paths
    nativeOutputDirPathObj = pathlib.Path(str(nativeOutputDirPathObj).replace(' ', '\ ').lstrip().rstrip())
    nativeBuildDirPathObj = pathlib.Path(str(nativeBuildDirPathObj).replace(' ', '\ ').lstrip().rstrip())
    nativeSourceDirPathObj = pathlib.Path(str(nativeSourceDirPathObj).replace(' ', '\ ').lstrip().rstrip())

    if image_file != None:
        os.system("docker load -i %s" % (image_file))  # load docker image from file
    elif 0 != subprocess.call(["docker", "inspect", docker_tag], stdout=subprocess.DEVNULL):
        if True == query_yes_no("docker image %s does not exists on your system. Build from dockerfile?", "yes"):
            os.system("docker build . -t %s" % (docker_tag)) # build docker image from scatch (or from cache)
        elif True == query_yes_no("Would you like to provide the name of a docker image file instead?", "yes"):
            os.system("docker load -i %s" % (input("Enter the path to the docker image file:")))
        else:
            print("docker image %s will not be built from Dockerfile and does not exist on system. Build cannot continue")
            exit (0)
    
    os.system("docker container stop %s" % (container))
    os.system("docker container rm %s " % (container))
    os.system("docker create -it --name %s %s" % (container, docker_tag))
    os.system("docker container start %s" % (container))



    # THIS NEXT SECTION IS LITERALLY JUST PATH WRANGLING SO THAT 
    # WE CAN EXECUTE THE DOCKER BUILD AND HAVE INTELLISENSE WORK IN A PLATFORM-AGNOSTIC MANNGER
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

    os.system("docker exec -t %s mkdir -p \"%s\"" % (container, str(posixCurrentDirPathObj)))

    #os.system("docker exec -t -w \"%s\" %s dos2unix \"%s/build_linux.sh\"" % (str(posixCurrentDirPathObj), container, scripts_dir))

    if nativeOutputDirPathObj.exists() and rebuild == False:
        print("\n copying existing build pipeline back into docker container \n")
        os.system("docker cp \"%s\" %s:\"%s\"" % (str(nativeBuildDirPathObj), container, str(posixCurrentDirPathObj)))

    toolchainFileName="toolchain.cmake"
    # Build the firmware
    os.system("docker cp \"%s\" %s:\"%s\"" % (nativeSourceDirPathObj, container, str(posixCurrentDirPathObj)))
    project_build_string = "sh -c \"cmake -S \"%s\" -B \"%s\" -DCMAKE_TOOLCHAIN_FILE=\"%s\" -DCMAKE_CROSSCOMPILING=\"%s\" -DCMAKE_BUILD_TYPE=\"%s\"" % (str(nativeSourceDirPathObj), str(nativeBuildDirPathObj), toolchainFileName, cross_compile, buildTypeString)


    os.system("docker exec -t -w \"%s\" %s %s" % (str(posixCurrentDirPathObj), container, "pwd && ls"))

    exit(0)
    
    print("project_build_string = %s" % (project_build_string))
    
   # % (buildTypeString, str(nativeOutputDirPathObj), str(nativeBuildDirPathObj), str(nativeSourceDirPathObj))
    os.system("docker exec -t -w \"%s\" %s %s" % (str(posixCurrentDirPathObj), container, project_build_string))

    # after build, copy built-objects out of container
    os.system("docker cp %s:\"%s\" \"%s\"" % (container, str(posixCurrentDirPathObj/posixBuildDirPathObj), str(nativeCurrentDirPathObj)))
    os.system("docker cp %s:\"%s\" \"%s\"" % (container, str(posixCurrentDirPathObj/posixOutputDirPathObj), str(nativeCurrentDirPathObj)))
    os.system("docker container stop %s" % (container))
    os.system("docker container rm %s" % (container))
    
    ## THIS NEXT SECTION IS MODIFYING THE EXPORTED JSON SO THAT INTELLISENSE WORKS ON THE NATIVE PLATFORM
    nativeCompileCommandsPathObj = nativeBuildDirPathObj/pathlib.Path("compile_commands.json")
    oldPath = str(posixCurrentDirPathObj)
    newPath = nativeCurrentDirPathObj.drive + oldPath
    update_intellisense(nativeCompileCommandsPathObj, oldPath, newPath)

if __name__ == "__main__":
    do_build()