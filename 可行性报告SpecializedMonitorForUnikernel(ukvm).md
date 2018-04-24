# 可行性报告 : Specialized Monitor for Unikernel (ukvm)^[1]^

## 构建一个 Specialized Monitor (如 ukvm) 的目的

- 使接口最少化：现有的操作系统，虚拟机或者容器（都可称为monitor），如docker，qemu，大都是 general-purpose monitor，它们采用了通用的接口。通用的接口是为了能够满足不同应用程序的不同需求而设计的，因此通用系统的接口数量必然多于某个程序所需的最少接口数量，这样在实用中造成了的浪费。另一方面冗余的接口可能使系统更加脆弱，若一些冗余的接口存在漏洞，就可能被利用来攻击系统，这增大了受攻击的面。相对的，若能为特定的程序构建一个 specialized monitor，接口为程序的需求量身定做，除去没有被用到的接口，就能免除上述通用系统的坏处。
- 简化 monitor 的设计：general-purpose monitor 因为需要支持能满足各种程序需求的接口，monitor 内部需要实现的操作和功能势必也会非常复杂，而且存在对某特定程序来说非必要的部分。尽管通常较为底部的接口导致的复杂性会降低，但较为底层的 general-purpose monitor 仍然有相当的不必要的复杂性，因为它们提供的虚拟化硬件必须遵循一些通用的标准，如BIOS, PCI devices, DMA address restrictions, memory holes, 等等，这样大多数程序才能在其上正常运行。相对地，specialized monitor 只需加入一些接口与在其上运行的程序交流，就能支持虚拟化硬件时用到的 introspection techniques 并且解决 VM 中棘手的同步问题。同时，省去上述通用的标准能简化程序与 general-purpose monitor 配合的部分，简化程序的开发
- 更快的启动速度：因为 specialized monitor 理论上更接近 monitor 的最小体积和最简单结构，在启动速度方面 specialized monitor 在启动速度上远胜 general-purpose monitor。

这些在最近在 unikernel 领域被很好地实践，并展现出了不错的前景。

## 构建设想和方法

Specialized Monitor for Unikernel 的提出者 Dan Williams 和 Ricardo Koller 在他们的论文中给出了他们设想的并已经通过一个设计原型实现的 specialized monitor for unikernel 构建方法。

设想中 specialized monitor 应该完成两个主要任务：1.为 unikernel 的运行提供独立隔离的环境；2.当 unikernel 退出/结束运行时采取适当的操作，其中可能的操作就包括清除这个 unikernel。最直观的一个 specialized monitor 的角色是专门化的虚拟机管理器。

这个 monitor 的默认工作方式是维护 unikernel 的完全隔离独立的运行环境，包括划定硬件隔离环境和运行 unikernel，而不提供和支持任何与 unikernel 的接口。一旦 unikernel 退出，monitor 立刻清除这个 unikernel 并取回它占用的资源，这时 monitor 可以安全地退出。

当然，完全隔离的 unikernel 可能适用场景有限，为此，可以依据程序的需要在 monitor 与 unikernel 间提供接口而不需要遵循一些既有的通用的标准。接口可以利用 monitor 可以访问 unikernel 的内存空间的特性来设计。如下面是一个发送 network packet 的接口，将这样的一个结构体的地址指针写入通过 UKVM_PORT_NETWRITE 定义的 I/O port 后，unikernel 退出，monitor 随后直接访问这个指针指向的地址，处理后得到要发送的 network packet 最后发送到物理网络中。

```c
/* UKVM_PORT_NETWRITE */
struct ukvm_netwrite {
void data; / IN */
int len; /* IN */
int ret; /* OUT */
};
```

构建过程

一些 unikernel 通过采用精明的包管理策略和对包依赖的追踪 （dependency tracking） 使得构建的 Library OS 的体量达到近似的最小值。例如 MirageOS，与 OCaml 的包管理器，OPAM 联动来追踪包依赖，它能生成基于 OCaml（Objective Caml，是 Caml 编程语言的主要实现）的 unikernel。追踪管理的包依赖覆盖面非常广，即使是一些通常会被操作系统包含的模块也会被追踪，如 TCP stack。下面的例子里，app 需要 TCP，因此在编译的时候，toolchain 会选择 TCP 模块和一个网络接口驱动 net-front 加入 unikernel 中。因为 这个 app 不需要使用文件系统，toolchain 从构建的 unikernel 中除去了 filesystem modules 和 block device driver。 这就是一个 unikernel 通常的构建和精简过程。

![1524589226061](C:\Users\wo\AppData\Local\Temp\1524589226061.png)

论文中他们提出将通过修改toolchain 和包管理器将 dependency tracking 的范围扩展到 monitor 的层次。上图的 （b）展示的就是这样的想法，不像标准的 unikernel 和其 monitor，（b）的unikernel 和 monitor 不是为了虚拟化一般性的网络设备操作而构建的。unikernel 中的 TCP 只有一个用 TAP 设备实现的网络操作，所以，构建时，toolchain 不仅删去了 unikernel 中的 filesystem 模块和 设备驱动模块，也删去了 monitor 中相应的模块，只在 monitor 中剩下 TAP 和 guest setup 模块。其中的 guest setup 模块是 monitor 中默认会包含的模块，负责引导启动 unikernel 和运行结束是清除 unikernel。这样构建的 specialized unikernel monitor 和 unikernel 代码行数与常见的 general purpose monitor 和 unikernel 的代码行数对比如下，可以见到 monitor 部分，专门化的 monitor 代码量大大缩减。

![1524591685870](C:\Users\wo\AppData\Local\Temp\1524591685870.png)

为了实现这样一个系统，还有一些问题需要解决。如何确定或者另外编写模块，尤其是那些跨越接口的？ 包或者模块应该要多大？ 如何通过这些包或者模块自动构建一个完整的 unikernel monitor？

## 原型 ukvm 的现状

论文中提出了他们编写的 specialized unikernel monitor 的原型，ukvm。Ukvm 代替虚拟化工具如 KVM/QEMU，引导，启动并管理基于Solo5（一个用 C 编写的开源 unikernel base，支持 MirageOS runtime and components） 的 unikernel，Mirage 应用程序的二进制文件（从 OCaml 的代码编译而来）与 Solo5 kernel 静态连接。他们的 ukvm 还没有达到完善，还不能在构建 monitor 时自动选择最小的包配置，自动选择的也还仅限与 MirageOS 的组成部件。

Ukvm 工作时装载 kernel ELF executable (solo5+mirage)，创建 KVMVCPU，并且配置内存和寄存器以便 Solo5 kernel 能像普通的 C main() 函数一样启动运行。内存和寄存器的配置包括设立一个线性页表（unikernel 只有一个单独的地址空间），栈，还有装载一些参数到寄存器（如内存大小）。

下面的是比较 ukvm，lkvm(最近推出的一种精简容器)，QEMU 在执行相同的任务时的初始化，响应和执行时间。可以见到测试中 ukvm 都取得了很好的结果。

![1524591934745](C:\Users\wo\AppData\Local\Temp\1524591934745.png)



# Reference

[1]Dan Williams&Ricardo Koller :Unikernel Monitors: Extending Minimalism Outside of the Box
	IBM T.J. Watson Research Center