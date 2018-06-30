#!/usr/bin/env bash

echo "please make sure that you have x86_64-rumprun-netbsd-gcc in your PATH"

rumprun qemu -i \
        -I if,vioif,'-net tap,script=no,ifname=tap0' \
        -W if,inet,static,10.0.120.101/24 \
        demo_rumprun.bin
