![alt text](https://github.com/cmattatall/adcs_firmware/blob/dev/resources/images/loris.jpg?raw=true)

# Table of Contents
- [About the LORIS Project](#about-the-loris-project)
- [What is an ADCS](#what-is-an-adcs)
- [Firmware and Development Environment](#firmware-and-development-environment)
  * [Dependencies](#dependencies)
  * [Toolchain installation](#toolchain-installation)
    + [LINUX (UBUNTU 18.04 OR NEWER)](#linux--ubuntu-1804-or-newer-)
    + [Windows](#windows)
  * [Building](#building)
  * [Visual Studio Code Integration](#visual-studio-code-integration)
    + [Tasks and Command Palette](#tasks-and-command-palette)
    + [Debugging](#debugging)
    + [Intellisense](#intellisense)
  * [Verification and Validation](#verification-and-validation)
    + [Regression tests](#regression-tests)
    + [Pre-push hooking](#pre-push-hooking)
    + [Nightly builds](#nightly-builds)
    + [On-target tests](#on-target-tests)
    + [Target-independent tests](#target-independent-tests)

# About the LORIS Project


The Low Orbit Reconnaissance & Imaging Satellite (LORIS) is a student project underway at Dalhousie University funded by the Canadian Space Agency (CSA). Dalhousie Space Systems Lab’s (DSS) mission is to deploy a two-unit CubeSat with the objective of developing skills in the areas of space systems engineering and of deploying a satellite capable of taking pictures of Earth and sending them back to a ground station at Dalhousie University. Since the project’s start, students at Dalhousie have been working on designing the subsystems required for the LORIS mission with guidance from the CSA and Dalhousie Faculty. In orbit, the satellite must be capable of maintaining a nadir pointing accuracy to ensure ground station communication and imaging requirements are met. The Attitude Determination and Control System (ADCS) is the subsystem responsible for determining and maintaining the attitude of the satellite and orienting its attitude with the requirements of the mission objective. Due to the pointing requirements, the ADCS subsystem is mission critical.  

The goal of this project is to develop, test, and deliver the hardware and firmware for ADCS on the LORIS CubeSat. Our project will review the current design of the ADCS system and provide manufacturable PCB designs and functioning firmware for the final ADCS flight boards of the LORIS mission. 

Once deployed, LORIS will orbit the Earth in a low earth orbit (LEO) at an altitude of about 400 km. The satellite mission’s payload is two cameras that will be used to take pictures of the Nova Scotia peninsula. Due to this imaging payload, the mission entails a high accuracy pointing requirement.

For more information, please visit
https://dalorbits.ca/2019/07/01/loris-2021/



# What is an ADCS

For spacecraft with mission critical pointing requirements, Attitude Determination and Control Systems (ADCS) are used. Attitude is the orientation of an aerospace vehicle with respect to an inertial frame of reference, in LORIS’s case this frame of reference is the Earth’s. LORIS requires  a nadir-pointing (the vector pointing to center of Earth) accuracy of ± 5° along the satellite’s Z axis during nominal operation. On a cubesat, the ADCS is one of the most mission critical subsystems.


<p align="center">
  <img src="https://github.com/cmattatall/adcs_firmware/blob/dev/resources/images/nadir.jpg?raw=true" alt="Sublime's custom image"/>
</p>



The primary purpose ot the ADCS system is to maintain the satellite's nadir pointing. There are several modes of operation for the system (Eclipsed, Detumbling, burnwire, and pointing). The following gif is a visualization of reducing the angular rate of the satellite (post deployment) by operating the ADCS in detumbling mode. The gif was produced from orbital simulations of LORIS as part of Anna Wailand's master's thesis on detumbling a cubesat using the B-dot algorithm and proportional control of the earth's magnetic dipole moment.

![](https://github.com/cmattatall/adcs_firmware/blob/master/resources/captures/detumbling_sim.gif)

# Firmware and Development Environment


## Dependencies

- Python3
- CMake 3.16 or newer (It may work on versions as old as 3.10 but I haven't tested those) https://cmake.org/download/
- msp430-elf-gcc (provided by the toolchain install script)
- msp430 TI device header support files (provided by the toolchain install script)
- gcc (on windows, this means you'll have to install mingw)

## Toolchain installation

Depending on your platform, toolchain installation step will vary. 

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

Internally, the project builds using cmake. To ease non-cmake users into the development environment, a python script (./compile.py) has been provided. Various command line arguments can be provided to the script to configure and build for the target microcontroller (rather than on the developer's native system), set the build type, change the verbosity level, and so on.

The build script has various options available. To view options, the --help flag can be used:

    $python3 ./compile.py --help
    
As a brief example, the following asciinema recording can be found below:

[![asciicast](https://asciinema.org/a/PX7z1klDIJXAscdKr7vvV4MBJ.png)](https://asciinema.org/a/PX7z1klDIJXAscdKr7vvV4MBJ)


## Visual Studio Code Integration


### Tasks and Command Palette

Instead of using the command line, there are also several tasks that have
been automated using visual studio code integration
There are several tasks available in the environment project (more can easily be added)

- Build native
- Build target
- Flash MCU


CTRL + SHIFT + P opens the command palette.

If you select "Run tasks", a dropdown will appear of tasks

<B>TODO: EXAMPLE IMAGES OF THE TASKS</B>

### Debugging


### Intellisense

Intellisense is configured using compile_commands.json exported by cmake. For integrated development environments like Microsoft Visual Studio, the method of parsing will vary internally based on the project generator.


## Verification and Validation

As a safety critical and mission critical system component, the ADCS firmware for LORIS must be continually validated against stringent behavioural requirements. Ideally, as many of these requirements as possible should be tested indpendent of the target hardware. To achieve this, integration with the ctest test suite is used. Regression tests are configured as part of client side git hooks, where any pushes that do not pass the regression test suite are rejected.

### Regression tests

To run regression tests, a python script called regressions.py is provided. Below you can find a recording of a user manually running the test suite.

[![asciicast](https://asciinema.org/a/jMWEML6JvH3Ur1rznNz8hxJm0.png)](https://asciinema.org/a/jMWEML6JvH3Ur1rznNz8hxJm0)

### Pre-push hooking

Although free github does not support serverside hook events, client-side git hooks can be configured to automatically run the regression test suite before pushing. If the regression tests fail, the push will be rejected. To configure git hooks, a file called _pre-push_ in the .git/hooks directory can be configured to automatically run the regression tests as a prerequisite for performing a push. The file snippet of .git/hooks/pre-push is provided below:

```bash
# This file is called .git/hooks/pre-push
# It must also have executable group permissions
remote="$1"
url="$2"


echo ""
echo "Executing client side pre-push git hook..."
echo "If regression tests fail, push to $url will be rejected!"
echo ""

python3 ./regressions.py

if [ "$?" -ne "0" ]; then
        echo ""
        echo "[ERROR] One or more regression tests failed. Push was rejected!"
        echo ""
        exit -1
fi
```

Once the client-side hooks are configured, all pushes that introduce deltas which fail regression tests will be rejected from the client side.

[![asciicast](https://asciinema.org/a/fWnGNB6YzwGkHdMqTJC3I8Msp.png)](https://asciinema.org/a/fWnGNB6YzwGkHdMqTJC3I8Msp)

### Nightly builds

We are currently looking for an enterprise server to run the nightly builds. 

### On-target tests


### Target-independent tests

