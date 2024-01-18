# Introduction

<!--TODO: Write a bit about Eliana's Experience and where did all the files came from-->

# Instructions

<!--TODO: Write about the need for all versions of all involved software to be exact and specific-->

## Install dependencies:

```sh
sudo apt-get update
sudo apt-get upgrade
sudo apt-get install build-essential
sudo dpkg --add-architecture i386
sudo apt-get update
sudo apt-get install \
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
ln -s /bin/sh /bin/bash
```

Download petalinux version 2021.2 - **Only this version will work!** use this link:

https://www.xilinx.com/member/forms/download/xef.html?filename=petalinux-v2021.2-final-installer.run

Install petalinux program:

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
the last command, if you are building an image not for the first time.

## Install the image

From the `molecular-duck/images/linux` directory, copy the following 3 files to
the SD card, while overriding them in the destination:

```
BOOT.BIN
boot.scr
image.ub
```

Use the following command to copy them via Windows' file explorer user
interface - assuming you are inside the `molecular-duck` directory:

```sh
explorer.exe images/linux
```

# Debugging image

<!-- Write about misc-init-commands, and how to inspect the log of the
dac-tcp-server, and how to extract the contents of the Linux files tree with
cpio command-->
