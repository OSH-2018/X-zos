#!/usr/bin/env bash
ip tuntap add tap0 mode tap
ip addr add 10.0.120.1/24 dev tap0
ip link set dev tap0 up
