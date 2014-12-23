#!/bin/bash
set -eu

make clean && make
sudo insmod ./aufs.ko

touch image.test
mkdir -p dir.test

sudo mount -o loop -t aufs ./image.test ./dir.test
sudo umount ./dir.test
rmdir ./dir.test
rm ./image.test

sudo rmmod aufs
dmesg | tail
echo "Tests passed"
