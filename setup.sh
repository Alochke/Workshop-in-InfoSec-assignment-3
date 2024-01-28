#!/bin/bash

git pull
sudo rmmod firewall
make clean
make
insmod firewall 