#!/bin/bash
#building the applications
make
#inserting the module
sudo insmod ioctl.ko 
#creating a character device
sudo mknod /dev/ioctl c 42 5
#compiling my user program
gcc -o test ioclt_app.c 
#running my user program
sudo ./test 
#removing the device
sudo rmmod /dev/ioctl 