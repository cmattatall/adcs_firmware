################################################################################
# @brief SCRIPT TO INSTALL THE MSP430 TOOLCHAIN AND UTILS ON YOUR SYSTEM       # 
# @author: Carl Mattatall (cmattatall2@gmail.com)                              # 
#                                                                              # 
# @note MUST BE RUN WITH ROOT PERMISSIONS ON LINUX                             # 
#                                                                              # 
################################################################################
import sys
import platform
import os
import wget
import requests
import math
import errno

def checkPythonVersion():
    if sys.version_info.major < 3: # python 3 must be the runtime
        raise Exception(os.path.basename(__file__) + " must be executed using Python 3")

# shitty way of checking for 32 bit vs 64 bit arch
def systemIs32Bit():
    if sys.maxsize > 2**32:
        return False
    else:
        return True

def symlink_force(target, link_name):
    try:
        os.symlink(target, link_name)
    except OSError as e:
        if e.errno == errno.EEXIST:
            os.remove(link_name)
            os.symlink(target, link_name)
        else:
            raise e

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

toolchain_url = "http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSPGCC/9_2_0_0/export"

def install_windows():
    print("installing msp430 toolchain for " + platform.system())
    download_url = toolchain_url + "/" + toolchain_folder

    print("install_windows script is not finished yet!!")
    exit(1)

def install_linux():
    if os.geteuid() != 0:
        print(os.path.basename(__file__) + " must be executed with root permission")
        exit(1)
    else:
        print("installing msp430 toolchain for " + platform.system())

    archive_ext = ".tar.bz2"
    if systemIs32Bit():
        toolchain_folder = "msp430-gcc-9.2.0.50_linux32"
    else:
        toolchain_folder = "msp430-gcc-9.2.0.50_linux64"
    
    install_dir = "/opt"
    download_url = toolchain_url + "/" + toolchain_folder + archive_ext
    os.system("wget -qO- %s | tar -xj" % (download_url))
    os.system("rm -r %s/%s" % ( install_dir, toolchain_folder))
    os.system("mv %s %s " % (toolchain_folder, install_dir))
    current_workdir = os.getcwd() # save to restore later
    os.chdir(install_dir + "/" + toolchain_folder + "/bin")

    # build symbolic links
    for executable in os.listdir(os.getcwd()):
        symlink_force(os.path.abspath(executable), "/usr/local/bin/" + executable)

def install_apple():
    print("installing msp430 toolchain for " + platform.system())
    toolchain_folder = "msp430-gcc-full-osx-installer-9.2.0.0.app.zip"
    download_url = toolchain_url + "/" + toolchain_folder

    print("install_apple script is not finished yet!!")
    exit(1)

def install_toolchain():
    checkPythonVersion()
    if platform.system() == "Windows":
        install_windows()
    if platform.system() == "Linux":
        install_linux()
    elif platform.system() == "Apple":
        install_apple()        

if __name__ == "__main__":
    install_toolchain()