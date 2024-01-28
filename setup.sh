#!/bin/bash

git pull
sudo rmmod firewall
make clean
make
insmod firewall.ko
gcc test.c
./a.out