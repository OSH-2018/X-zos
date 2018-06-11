#!/usr/bin/env bash
echo "setting the interface"
echo "please run this script as root user !"

ip tuntap add tap0 mode tap
ip addr add 10.0.120.100/24 dev tap0
ip link set dev tap0 up
