#!/usr/bin/env bash

echo "please make sure that you have x86_64-rumprun-netbsd-gcc in your PATH"

x86_64-rumprun-netbsd-gcc -o demo-rumprun demo.c send_log.c debug_init.c
rumprun-bake hw_generic demo-rumprun.bin demo-rumprun

echo "booting.."

rumprun qemu -i \
        -I if,vioif,'-net tap,script=no,ifname=tap0' \
        -W if,inet,static,10.0.120.101/24 \
        demo-rumprun.bin &

echo "building the client"

gcc -o client client.c

$(pwd)/client
