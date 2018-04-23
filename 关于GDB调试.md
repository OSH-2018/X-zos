# 关于GDB调试  

  
## GDB工作原理  
### 一. 基于ptrace的系统调用  
  > 函数原型：
  ```C
    long ptrace(enum __ptrace_request request, pid_t pid,void *addr,void *data); 
  ```  
>  *equest参数的主要选项*：
1. **PTRACE_TRACEME**：由子进程调用，表示本进程将被其父进程跟踪，交付给这个进程的所有信号，即使信号是忽略处理的（除SIGKILL之外），都将使其停止，父进程将通过wait()获知这一情况。
2. **PTRACE_ATTACH**：attach到一个指定的进程，使其成为当前进程跟踪的子进程，而子进程的行为等同于它进行了一次PTRACE_TRACEME操作。但是，需要注意的是，虽然当前进程成为被跟踪进程的父进程，但是子进程使用getppid()的到的仍将是其原始父进程的pid。
这下子gdb的attach功能也就明朗了。当你在gdb中使用attach命令来跟踪一个指定进程/线程的时候，gdb就自动成为改进程的父进程，而被跟踪的进程则使用了一次PTRACE_TRACEME，gdb也就顺理成章的接管了这个进程。
3. **PTRACE_CONT**：继续运行之前停止的子进程。可同时向子进程交付指定的信号。       
  
  
  ptrace系统调用提供了一种方法，让父进程可以观察和控制其它进程的执行，检查和改变其核心映像及寄存器。主要用来实现断点调试和系统调用跟踪。  
    
###　二. gdb调试的三种方式
* attach并调试一个已经运行的进程： 
确定需要进行调试的进程id,运行gdb，输入attch pid，如：gdb 12345。gdb将对指定进行执行如下操作：ptrace（PTRACE_ATTACH，pid，0,0 )  
* 运行并调试一个新的进程:
运行gdb，通过命令行参数或file指定目标调试程序，如gdb ./test
输入run命令，gdb执行下述操作：
通过fork()系统调用创建一个新进程
在新创建的子进程中调用ptrace(PTRACE_TRACEME，0,0,0）
在子进程中通过execv（）系统调用加载用户指定的可执行文件   
* 远程调试目标主机上新创建的进程
gdb运行在调试机，gdbserver运行在目标机，通过二者之间定义的数据格式进行通信　　

### 其他　　
*　关于设置断点　　
信号是实现断点的基础，当用breakpoint 设置一个断点后，gdb会在=找到该位置对应的具体地址，然后向该地址写入断点指令INT3，即0xCC。
　　目标程序运行到这条指令时，就会触发SIGTRAP信号，gdb会首先捕获到这个信号。然后根据目标程序当前停止的位置在gdb维护的断点链表中查询，若存在，则可判定为命中断点。　　

## gdb 如何进行远程调试  
  
  gdb的远程调试与本地调试大抵相同，只是在gdb与目标程序之间加了一层gdbserver控制程序。　　
  ![gdb远程调试][]  
##  一个gdb远程调试unikernal的实例：
  > # Howto: Debugging Rumprun with gdb
  Generally speaking, use `rumprun [platform] -p -D [port]` and `target remote:[port]` in gdb.

> #### Debugging 64-bit unikernels under KVM/QEMU
Debugging a 64-bit rumprun unikernel under KVM/QEMU requires the following workaround due to a [GDB issue](https://sourceware.org/bugzilla/show_bug.cgi?id=13984):

> 1. Start the unikernel, leaving it paused and waiting for GDB to connect: 
  ````
  rumprun [kvm|qemu] -p -D 1234 [...]
  ````

> 2. Run the following GDB command:
  ````
  gdb -q -ex "target remote:1234" -ex "hbreak x86_boot" -ex "continue" -ex "disconnect" -ex "quit" unikernel.bin
  ````

>  This will cause the unikernel to proceed past the transition from 32-bit mode to long mode and remain in a paused state.
3. Re-launch GDB a second time as you would normally.

> #### Common gdb commands
- `disas` or `disas startaddr endaddr` - disassembles around current PC or specific address
- `info locals/args/variables` - prints out local variables, function arguments, all variables...
- `info registers` - prints out all registers and their values
- `break file:linenr` - sets a breakpoint on specific file/line
- `c` - continue execution
- `n` - step to next command in current stack frame
- `si` - step by single instruction
- `ni` - step by single instruction but if it is a function call, step until it returns
- `l` - prints out few lines of code around current PC
- `p $reg/variable` - prints values of registers or variables
- `x address` - print value at some address. Look at docs for more details on formatting

> #### Debugging example
We'll walk through debugging [hello_world Go application](https://github.com/deferpanic/gorump/tree/master/examples/hello_world). Process is the same whether you debug C, Go, Erlang or any other language/setup.

> Before running the kernel, start gdb in one terminal, from directory where the binary is located:
```bash
cd $HOME/gorump/examples/hello_world
gdb -ex 'target remote:1234' hello.bin
```
> In another terminal, run the kernel:
```bash
rumprun xen -D 1234 -p -i hello.bin
```

> gdb prompt should stop on `0x0000` and wait for input:
```
	info "(gdb)Auto-loading safe path"
Remote debugging using :1234
0x0000000000000000 in _text ()
(gdb)
```

> We can set a breakpoint on `runtime1.go:77`, which is `goenvs_unix` function in Go runtime. After that, we use `c` to run until the breakpoint.
```
(gdb) break runtime1.go:77
Breakpoint 2 at 0x4ac89: file /usr/local/go/src/runtime/runtime1.go, line 77.
(gdb) c
Continuing.

> Breakpoint 1, runtime.goenvs_unix () at /usr/local/go/src/runtime/runtime1.go:76
76		n := int32(0)
(gdb)
```

> Execution of the kernel stops at this point and gdb waits for our input. We can investigate the surrounding code:
```
(gdb) l
71
72	func goenvs_unix() {
73		// TODO(austin): ppc64 in dynamic linking mode doesn't
74		// guarantee env[] will immediately follow argv.  Might cause
75		// problems.
76		n := int32(0)
77		for argv_index(argv, argc+1+n) != nil {
78			n++
79		}
80
(gdb)
```

> `argv` and `argc` are global variables in `runtime` at this point, so we can't get their value with `p argc` but need to specify exactly:
```
(gdb) p 'runtime.argc'
$1 = 1
(gdb) p 'runtime.argv'
$2 = (uint8 **) 0x3d5250 <kludge_argv>
(gdb)
```

> We see that `argv` is a double pointer, so we can unpack that and look at the value:
```
(gdb) x /1a 'runtime.argv'          # /1a means "one as pointer" so it automatically reads what's on that address
0x3d5250 <kludge_argv>:	0x26d520
(gdb) x /1s 0x26d520                # /1s means "one string on that location"
0x26d520:	"argument"
(gdb)
```

> If we were to debug the stack of `main` (which is where `argc` and `argv` reside) we can use `x` to get multiple values:
```
(gdb) x /16w 0x3d5240
0x3d5240 <kludge_argc>:	0x00000001	0x00000000	0x00000000	0x00000000
0x3d5250 <kludge_argv>:	0x0026d520	0x00000000	0x0026d525	0x00000000
0x3d5260 <envp>:	0x0026d52b	0x00000000	0x00000000	0x00000000
0x3d5270:	0x00000000	0x00000000	0x00000000	0x00000000
(gdb)
```

> We can also debug our application of course. HelloWorld has a single .go file, [hello.go](https://github.com/deferpanic/gorump/blob/master/examples/hello_world/hello.go), so we set a breakpoint:
```
(gdb) break hello.go:11
Breakpoint 4 at 0x176b7: file /home/ubuntu/gorump/examples/hello_world/hello.go, line 11.
(gdb)
```

> Then, we just `c` to continue running until the next breakpoint, which should be the one in hello.go
```
(gdb) c
Continuing.

Breakpoint 4, main.damain () at /home/ubuntu/gorump/examples/hello_world/hello.go:11
11		fmt.Println("Hello, Rumprun.  This is Go.")
(gdb) l
6	func main() {
7	}
8
9	//export damain
10	func damain() {
11		fmt.Println("Hello, Rumprun.  This is Go.")
12	}
(gdb)
```

> Again, we can investigate variables, stack, code or machine instructions...  

# 关于unikernal的内存空间　　
*unikernal的内存空间是单地址空间(single address space)*
> In a Single Address Space OperatingSystem (SASOS) all processes run within a single global virtual address space; protection (if provided by the OS) is provided not through conventional address space boundaries, but through ProtectionDomains that dictate which pages of the global address space a process can reference. This operating system organization is facilitated by microprocessor architectures that support 64 bits of virtual address space and promotes efficient data sharing and cooperation, both between complex applications and between parts of the operating system itself.  

## Advantages
 Application View

    Simple naming mechanism - 64 bit address - supported by ``conventional'' hardware.
    User data structures can contain embedded references to other data.
    Eliminates excessive copying of data and software pointer translation.

SASOS Advantages: System View

    Simplifies data migration
    Simplifies process migration
    Orthogonality of translation and protection
    No need for file system -- all disk I/O is paging
    RAM is cache for VM -- unified buffer & disk cache management
    Easy to implement zero-copy operations
    In-place execution -- no need for position-independent code

==> Simplified system implementation and increased performance

### SASOS Advantages: Hardware View

    Virtual caches are no problem
    virtual address maps uniquely to physical address
    Hardware separating translation from protection could increase performance due to increased TLB coverage
    (e.g. IA-64 protection keys)
