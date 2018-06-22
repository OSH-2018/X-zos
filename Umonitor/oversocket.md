# 基于SOCKET通信的日志系统

我们已经实现了unikernel和host的通讯，基于socket技术，目前考虑使用该方法实现unikernel的日志系统。

## 实现平台

rumprun unikernel. qemu simulator(not necessary), kvm virtualization.

## 实现关键

关键在于把host, unikernel放在同一个网络中，这样才能实现连接，传输。

### host 部分

```bash
ip addr flush eth0
ip tuntap add tap0 mode tap
ip addr add 10.0.120.100/24 dev tap0
ip link set dev tap0 up
```

(参考了构建nginx in unikernel服务器的方法)

### unikernel 部分

```bash
rumprun qemu -i \
        -I if,vioif,'-net tap,script=no,ifname=tap0' \
        -W if,inet,static,10.0.120.101/24 \
        server_rump.bin
```

1. write an interface to set up unikernel.
2. an interface to monitor them.
3. an interface to see the log.
