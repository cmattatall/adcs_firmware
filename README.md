# TABLE OF CONTENTS


- [TABLE OF CONTENTS](#table-of-contents)
- [WHAT IS THE LORIS PROJECT](#what-is-the-loris-project)
- [WHAT IS AN ADCS](#what-is-an-adcs)
- [DEVELOPMENT ENVIRONMENT](#development-environment)
  * [DEPENDENCIES](#dependencies)
  * [TOOLCHAIN INSTALLATION](#toolchain-installation)
    + [LINUX (UBUNTU 18.04 OR NEWER)](#linux--ubuntu-1804-or-newer-)
    + [Windows](#windows)
  * [Building](#building)
  * [TASKS](#tasks)
  * [Debugging](#debugging)

# WHAT IS THE LORIS PROJECT


The Low Orbit Reconnaissance & Imaging Satellite (LORIS) is a student project underway at Dalhousie University funded by the Canadian Space Agency (CSA). Dalhousie Space Systems Lab’s (DSS) mission is to deploy a two-unit CubeSat with the objective of developing skills in the areas of space systems engineering and of deploying a satellite capable of taking pictures of Earth and sending them back to a ground station at Dalhousie University. Since the project’s start, students at Dalhousie have been working on designing the subsystems required for the LORIS mission with guidance from the CSA and Dalhousie Faculty. In orbit, the satellite must be capable of maintaining a nadir pointing accuracy to ensure ground station communication and imaging requirements are met. The Attitude Determination and Control System (ADCS) is the subsystem responsible for determining and maintaining the attitude of the satellite and orienting its attitude with the requirements of the mission objective. Due to the pointing requirements, the ADCS subsystem is mission critical.  

The goal of this project is to develop, test, and deliver the hardware and firmware for ADCS on the LORIS CubeSat. Our project will review the current design of the ADCS system and provide manufacturable PCB designs and functioning firmware for the final ADCS flight boards of the LORIS mission. 

Once deployed, LORIS will orbit the Earth in a low earth orbit (LEO) at an altitude of about 400 km. The satellite mission’s payload is two cameras that will be used to take pictures of the Nova Scotia peninsula. Due to this imaging payload, the mission entails a high accuracy pointing requirement.

For more information, please visit
https://dalorbits.ca/2019/07/01/loris-2021/

![alt text](https://github.com/cmattatall/adcs_firmware/blob/dev/resources/images/loris.jpg?raw=true)


# WHAT IS AN ADCS

For spacecraft with mission critical pointing requirements, Attitude Determination and Control Systems (ADCS) are used. Attitude is the orientation of an aerospace vehicle with respect to an inertial frame of reference, in LORIS’s case this frame of reference is the Earth’s. LORIS requires  a nadir-pointing (the vector pointing to center of Earth) accuracy of ± 5° along the satellite’s Z axis during nominal operation. On a cubesat, the ADCS is one of the most mission critical subsystems.

![alt text](https://github.com/cmattatall/adcs_firmware/blob/dev/resources/images/nadir.jpg?raw=true)

# DEVELOPMENT ENVIRONMENT


## DEPENDENCIES

- Python3
- CMake 3.16 or newer (It may work on versions as old as 3.10 but I haven't tested those) https://cmake.org/download/
- msp430-elf-gcc (provided by the toolchain install script)
- msp430 TI device header support files (provided by the toolchain install script)
- gcc (on windows, this means you'll have to install mingw)

## TOOLCHAIN INSTALLATION



### LINUX (UBUNTU 18.04 OR NEWER)

```
    #python3 ./install_msp430_toolchain.py.
```

### Windows

1. First install MinGW gcc from http://mingw-w64.org/doku.php/download 
2. You'll have to make sure that you have gcc added to your PATH
3. Download and install python 3 (or newer). It can be installed from the windows store on windows 10 and above.
4. Start an administrator instance of powershell, navigate to this directory and launch the python installer
5. The installer can be executed with 

```    
    #python3 .\install_msp430_toolchain.py
```

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


## Test plan

Tests are automated using cmake and ctest. To produce software that is maintainable and scalable, as many features are tested independent of the target hardware as possible.


[![asciicast](https://asciinema.org/a/113463.png)](https://github.com/cmattatall/adcs_firmware/blob/dev/resources/captures/regression_test_asciinema_capture.cast)


