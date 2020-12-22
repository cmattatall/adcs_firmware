################################################################################
# @brief SCRIPT TO INSTALL THE MSP430 TOOLCHAIN AND UTILS ON YOUR SYSTEM       # 
# @author: Carl Mattatall (cmattatall2@gmail.com)                              # 
#                                                                              # 
# @note MUST BE RUN WITH ROOT PERMISSIONS ON LINUX                             # 
#                                                                              # 
################################################################################
import pip
import sys
import platform
import os
import wget
import requests
import math
import errno
import zipfile
import shutil


###############################################################################
## CONSTANTS. DON'T EVER TOUCH THESE
###############################################################################
 
toolchain_url_base = "http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSPGCC/9_2_0_0/export"

support_files_folder_name = "msp430-gcc-support-files"
support_files_archive_ext = "-1.208.zip"
support_files_archive_name = support_files_folder_name + support_files_archive_ext
support_files_url="https://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSPGCC/latest/exports/%s" % (support_files_archive_name)

###############################################################################

# @brief abort if not python 3
def checkPythonVersion():
    if sys.version_info.major < 3: # python 3 must be the runtime
        raise Exception(os.path.basename(__file__) + " must be executed using Python 3")


# @brief detect if python runtime is executing in 32 bit or 64 bit environment
def systemIs32Bit():

    ## THERE HAS TO BE A BETTER WAY OF DOING THIS...
    if sys.maxsize > 2**32:
        return False
    else:
        return True


# @brief forcibly create symlink to @target at location @link_name
# @note overwrites existing symlink with name == @name
def symlink_force(target, link_name):
    try:
        os.symlink(target, link_name)
    except OSError as e:
        if e.errno == errno.EEXIST:
            os.remove(link_name)
            os.symlink(target, link_name)
        else:
            raise e

# @brief query wrapper to safely get y/n input from user
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


# @brief install toolchain on windows
def install_windows():
    # IN ADDITION TO THE INSTALL SCRIPT, WE HAVE TO INSTALL THE USB DRIVERS 
    #https://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430_FET_Drivers/latest/index_FDS.html


    print("installing msp430 toolchain for " + platform.system())

    archive_ext = ".zip"
    if systemIs32Bit():
        toolchain_folder = "msp430-gcc-9.2.0.50_win32"
    else:
        toolchain_folder = "msp430-gcc-9.2.0.50_win64"
    
    windows_install_dir = "D:\\Desktop\\msp430_toolchain"
    #windows_install_dir = "C:\\msp430_toolchain\\"
    if os.path.exists(windows_install_dir):
        os.rmdir(windows_install_dir)
    os.mkdir(windows_install_dir)
    os.chdir(windows_install_dir)

    toolchain_archive_name = toolchain_folder + archive_ext
    download_url = toolchain_url_base + "/" + toolchain_archive_name
    toolchain_zipdata = requests.get(download_url)
    toolchain_zip = open(toolchain_archive_name, "wb")
    toolchain_zip.write(toolchain_zipdata.content)
    toolchain_zip.close()

    with zipfile.ZipFile(toolchain_archive_name, 'r') as zip_ref:
        zip_ref.extractall()
    
    if os.path.exists(toolchain_archive_name):
        os.remove(toolchain_archive_name)
    #os.system("rm \"%s\"" % (toolchain_archive_name))
    current_workdir = os.getcwd() # save to restore later
    os.chdir(windows_install_dir + "\\" + toolchain_folder + "\\" + "bin")
    
    # NOW SHIT LIKE !!! THIS !!! is why programmers bitch and moan about windows being outdated and garbage
    # equivalent linux command is literally just PATH=$PATH;new/thing/to/add
    #os.system("$PATH = [Environment]::GetEnvironmentVariable(\"PATH\")")
    #os.system("$path_to_add = \"%s\"" % (os.getcwd()))
    #os.system("[Environment]::SetEnvironmentVariable(\"PATH\", \"$PATH;$path_to_add\", \"Machine\")")
    os.chdir(windows_install_dir)

    supprtfiles_zipdata = requests.get(support_files_url)
    supportfiles_zip = open(support_files_archive_name, "wb")
    supportfiles_zip.write(supprtfiles_zipdata.content)
    supportfiles_zip.close()
    with zipfile.ZipFile(support_files_archive_name, "r") as zip_ref:
        zip_ref.extractall()
    if os.path.exists(support_files_archive_name):
        os.remove(support_files_archive_name)

    # install mspdebug
    os.system("git clone https://github.com/dlbeer/mspdebug.git")
    os.chdir("mspdebug")
    os.system("make && make install")

    os.chdir(windows_install_dir)
    print("install_windows script is not finished yet!!")
    exit(1)



# @brief install toolchain on linux
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

    os.system("apt-get update -y")
    os.system("apt-get install -y libusb-dev")
    
    # some string literals that have been abstracted out. May want to make 
    # these configurable in the future
    linux_install_dir = "/opt/msp430_toolchain"
    executable_symlink_dir = "/usr/local/bin"
    bashrc_filename = os.getenv("HOME") + "/" + ".bashrc"

    # save current directly to restore later because we dont have pushd/popd
    current_workdir = os.getcwd() 

    # delete old installation
    if os.path.exists(linux_install_dir):
        shutil.rmtree(linux_install_dir)
    os.mkdir(linux_install_dir)
    os.chdir(linux_install_dir)

    # download msp430-elf-gcc toolchain
    # and symlink to user local binary so they're in PATH
    download_url = toolchain_url_base + "/" + toolchain_folder + archive_ext
    os.system("wget -qO- %s | tar -xj" % (download_url))
    os.chdir(linux_install_dir + "/" + toolchain_folder + "/bin")
    for executable in os.listdir(os.getcwd()):
        symlink_force(os.path.abspath(executable), executable_symlink_dir + "/" + executable)
    os.chdir(linux_install_dir)

    # download mcu linker scripts and device header support files
    os.system("wget %s" % (support_files_url))
    os.system("unzip " + support_files_archive_name)
    if os.path.exists(support_files_archive_name):
        os.remove(support_files_archive_name)
        #shutil.rmtree doesn't work because the Inode for a compressed file
        #APPEARS as if it has no children 
        #(so a .zip file appears to be a single file, not an archive)

    # install mspdebug
    os.system("apt-get update -y")
    os.system("apt-get install -y libusb libusb-dev")
    os.system("apt-get install -y libreadline-dev") # for strange line endings
    os.system("git clone https://github.com/dlbeer/mspdebug.git")
    os.chdir("mspdebug")
    os.system("make && make install")
    symlink_force(os.path.abspath("mspdebug"), executable_symlink_dir + "/" + "mspdebug")

    # set up USB rules and udev, then retrigger so we don't need to relog
    os.system("udevadm control --reload-rules && udevadm trigger")
        # we may also need to set up USB and UDEV user groups here
        # if I remember correctly from past troubleshooting,
        # plugin, usb, and dialout groups are the ones required

    # go back to directory we started in
    os.chdir(current_workdir) 


# @brief install toolchain on apple
# @todo ACTUALLY IMPLEMENT THE DAMN THING 
#      (I don't have a macbook so idk what to do yet)
def install_apple():
    print("installing msp430 toolchain for " + platform.system())
    toolchain_folder = "msp430-gcc-full-osx-installer-9.2.0.0.app.zip"
    download_url = toolchain_url_base + "/" + toolchain_folder

    print("install_apple script is not finished yet!!")
    exit(1)


# @brief MAIN INSTALLATION WRAPPER
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