#!/bin/bash

#
# To be run as exec from puppet, easily check if module is installed
# on current kernel and compile/install if not
#
# No licence what so ever, use as you want and don't complain or hold
# anyone acountable.

# name of module
MODNAME=golden_tulip
MODDIR=/lib/modules/$(uname -r)/kernel/drivers/char

# where to find src (transfer to this directory with puppet
SOURCE=/opt/sysbase/sysbas_mpdrv.v23.0

if [ -f $MODDIR/$MODNAME.ko ]; then
    # if golden_tulip exists exit and dont think more about it
    exit 0
fi

# complain if we don't have gcc
if [ -n $(which gcc) ]; then
    :
else
    echo no gcc found
    exit 1
fi

# complain if we don't have make
if [ -n $(which make) ]; then
    :
else
    echo no make found
    exit 1
fi

cd $SOURCE
make modules
make install 
make clean

rmmod golden_tulip 2> /dev/null
depmod -a
modprobe golden_tulip
