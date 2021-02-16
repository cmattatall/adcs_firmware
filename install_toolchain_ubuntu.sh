#!/bin/bash

function cleanup()
{
    while(( "$?" == 0 ))
    do
        popd > /dev/null
    done
}
trap cleanup EXIT

declare -a apt_packages=("wget" "gcc" "g++" "git" "nano" "libssl-dev" "tar" "cut" "curl" "software-properties-common" "gnupg-agent" "ca-certificates" "apt-transport-https" "automake" "autoconf" "libudev-dev" "udev" "autotools-dev" "libtool" "libusb-1.0.0-dev" "pkg-config" "make" "vim" "python3")


declare -a snap_packages()

