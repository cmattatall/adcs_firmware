
## DEVELOPMENT DEPENDENCIES

- Python3
- CMake 3.16 or newer (It may work on versions as old as 3.10 but I haven't tested those)
- msp430-elf-gcc (provided by the toolchain install script)
- msp430 TI device header support files (provided by the toolchain install script)
- gcc (on windows, this means you'll have to install mingw)

## TOOLCHAIN INSTALLATION



### LINUX (UBUNTU 18.04 OR NEWER)

    #python3 ./install_msp430_toolchain.py.

### Windows

1. First install MinGW gcc from http://mingw-w64.org/doku.php/download 
2. You'll have to make sure that you have gcc added to your PATH
3. Download and install python 3 (or newer). It can be installed from the windows store on windows 10 and above.
4. Start an administrator instance of powershell, navigate to this directory and launch the python installer
5. The installer can be executed with 
    #python3 .\install_msp430_toolchain.py
## Building

run python3 ./compile.py

The build script has various options available. To view options,

    $python3 ./compile.py --help


## TASKS

Instead of using the command line, there are also several tasks that have
been automated using visual studio code integration
There are several tasks available in the environment project (more can easily be added)

- Build native
- Build target
- Flash MCU


CTRL + SHIFT + P opens the command palette.

If you select "Run tasks", a dropdown will appear of tasks

<B>TODO: EXAMPLE IMAGES OF THE TASKS</B>

## Debugging



