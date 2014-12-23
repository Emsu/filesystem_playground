#!/bin/bash
set -eu

sudo insmod ./aufs.ko
sudo rmmod aufs
dmesg | tail
