
## DEVELOPMENT DEPENDENCIES

- Python3
- CMake 3.16 or newer (It may work on versions as old as 3.10 but I haven't tested those)
- msp430-elf-gcc (provided by the toolchain install script)
- msp430 TI device header support files (provided by the toolchain install script)
- gcc (on windows, this means you'll have to install mingw)

## TOOLCHAIN INSTALLATION

    #python3 install_msp430_toolchain.py.

On linux, you'll have to run it with root permissions (as root or using something like sudo)

On windows, you'll have to run it from a powershell instance with administrator permissions

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



