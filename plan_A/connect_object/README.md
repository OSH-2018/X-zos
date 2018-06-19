# README

## 编译

gcc -o server connect_object.c

gcc -o client client.c

## 编译后分别在不同终端：

./server //只能启动一个，负责接收传输信息写入.log文件(还没做成可调用的函数，先凑合吧)

./client //可以同时启动多个，启动client后可以从stdin输入任意字符串经client传输到server，各个终端第一次传输的字符串作为文件名，各自生成.log文件
