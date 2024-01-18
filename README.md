# Introduction

This repository attempts to provide all the necessary files needed to build the
Linux image for our FPGA, and this document attempts to explain the gist of the
commands that will build the image, without explaining details not relevant to
our hardware.

All of the instructions here were written in blood. The set of tools that build
the image for you is called
[Petalinux](https://www.xilinx.com/products/design-tools/embedded-software/petalinux-sdk.html)
, an AMD / Xilinx product. The sad news is that they are horribly written, and
it is very hard / impossible to port them to different platforms. Hence the
instructions below require you to run only Ubuntu 20.04, and the 2021.2 version
of Petalinux.

**If you like playing with Linux, you may survive the experience of trying to
use modern versions of Petalinux and Ubuntu to do this, but you'd likely
fail. Succeeding this adventure would be heroic.**

## Binary files included in the repository

This repository includes also some binary files that were generated by Vivado,
in Eliana's computer.

<!--TODO: Write a bit about Eliana's Experience and where did all the files came from-->

# Instructions

Again, only Ubuntu 20.04 has been tested successfully with these instructions!
Ubuntu 22.04 has been tested to fail. You can use Windows' Microsoft Store to
install a Windows Subsystem for Linux (WSL) version of Ubuntu 20.04, and run all
commands from there. To do that try out this link:

https://www.microsoft.com/store/productId/9MTTCL66CPXJ

If you are having trouble using the link, try running in a Windows Powershell:

```
wsl.exe --install Ubuntu-20.04
```

Almost all of the instructions below don't assume you are using WSL.

## Install dependencies:

```sh
sudo dpkg --add-architecture i386
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install \
  build-essential \
  net-tools \
  xterm \
  libncurses5 \
  libncurses5-dev \
  gcc-multilib \
  zlib1g \
  zlib1g-dev \
  bc \
  libtool \
  autoconf \
  automake \
  texinfo \
  zlib1g:i386
sudo ln -s /bin/bash /bin/sh
```

Download Petalinux version 2021.2 - **Only this version will work!** Use this link:

https://www.xilinx.com/member/forms/download/xef.html?filename=petalinux-v2021.2-final-installer.run

Install Petalinux tools:

```sh
bash ./petalinux-v2021.2-final-installer.run --dir petalinux --skip_license
echo source petalinux/settings.sh >> .bashrc
source petalinux/settings.sh
```

## Build the image

Clone this repository:

```sh
git clone --recursive https://gitlab.com/doronbehar/lab-molecular-duck molecular-duck && cd molecular-duck
```

Build and generate image files to put later in SD card. Please run these
commands one by each without the `$` prefix, and pray for them to work.

```sh
$ petalinux-build
$ petalinux-package --boot --fsbl --fpga --u-boot
```

You may need to [use the `--force`](https://en.wikipedia.org/wiki/The_Force) in
the last command, if you are building an image after a previous successful trial.

## Install the image

From the `molecular-duck/images/linux` directory, copy the following 3 files to
the SD card, while overriding them in the destination:

```
BOOT.BIN
boot.scr
image.ub
```

If you are using Windows Subsystem for Linux (WSL), use the following command
to copy them via Windows' file explorer user interface - assuming you are
inside the `molecular-duck` directory:

```sh
explorer.exe images/linux
```

# Debugging the image

Sometimes, before booting the image, you may wish to inspect the files and
directories that Petalinux puts in the Linux image. These files are part of the
`image.ub` file mentioned above. The command that can help you with inspecting
those files is `binwalk`.  

You can install `binwalk` using:

```sh
sudo apt-get install binwalk
```

Extract the contents of the image using:

```sh
binwalk -e image.ub
```

Find the CPIO archive in `_image.ub.extracted/` using:

```sh
file _image.ub.extracted/* | grep "ASCII cpio archive"
```

Then you can run any `cpio` command on the file printed above, to learn about
the Linux file tree. For example, if you wanted to check that the files you
wanted to get installed exist in the image, you can use this command: 

```
cpio --list < _image.ub.extracted/AC*
```

Assuming the CPIO archive file starts with `AC`. Usually the filename shouldn't
change between subsequent builds, and you should be able to run after every
build the following:

```sh
binwalk --extract image.ub && cpio --list < _image.ub.extracted/AC* | grep my-file
```

And it will search for a `my-file` in the Linux files tree.
