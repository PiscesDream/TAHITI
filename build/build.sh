#!/bin/bash

bootstrap=$1
setup=$2
system=$3
IMAGE=$4

# the system maximum size: 0x3000*16 bytes == 0x180 sectors (boot/setup.nasm)
SYS_SIZE=$((0x3000*16))


dd if=/dev/zero of=$IMAGE bs=512 count=2880

# bootstrap: 1 sector
[ ! -f "$bootstrap" ] && echo "there is no bootstrap binary file there" && exit -1
dd if=$bootstrap bs=512 count=1 of=$IMAGE conv=notrunc

# setup: 4 sector
[ ! -f "$setup" ] && echo "there is no setup binary file there" && exit -1
dd if=$setup seek=1 bs=512 count=5 of=$IMAGE conv=notrunc

# system: < SYS_SIZE
[ ! -f "$system" ] && echo "there is no system binary file there" && exit -1
system_size=`wc -c $system |cut -d" " -f1`
[ $system_size -gt $SYS_SIZE ] && echo "the system binary is too big" && exit -1
dd if=$system seek=6 bs=512 count=$((2888-1-5)) of=$IMAGE conv=notrunc

