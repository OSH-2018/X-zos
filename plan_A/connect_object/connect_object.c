#include<sys/types.h>
#include<sys/wait.h>
#include<sys/socket.h>
#include<stdio.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/shm.h>
#include<signal.h>
#include<errno.h>

#define MYPORT 2333   //服务器端口
#define QUEUE 20        //等待队列大小
#define BUFFER_SIZE 1024 //缓冲区大小
//信号处理函数
void sigfunc(int signo)
{
    pid_t pid;
    __sighandler_t a = NULL;
    int status;
    while((pid=waitpid(-1,&status,WNOHANG))>0)
        printf("child %d terminated!\n",pid);
}

int main()
{
    //socket
    int server_sockfd = socket(AF_INET,SOCK_STREAM,0); 
     
    //定义sockaddr_in
    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(MYPORT);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //bind，成功返回0,出错返回-1
    if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1)
    {
        perror("bind error!\n");
        exit(1);
    }
    //lisen,成功返回0,出错返回-1
    if(listen(server_sockfd,QUEUE)==-1)
    {
        perror("lisen error!\n");
        exit(1);
    }
    int count=0;
    //信号监听  
    signal(SIGCHLD, sigfunc);
    while(1)
    {
        //客户端套接字
        char buffer[BUFFER_SIZE];
        struct sockaddr_in client_addr;
        socklen_t length = sizeof(client_addr);
        //建立连接 成功返回非负描述字，出错返回-1
        int conn = accept(server_sockfd,(struct sockaddr*)&client_addr,&length);
        if(conn<0)
        {
            perror("connect error!\n");
            exit(1);
        }
        int pid;
        //调用fork()函数时  如果是父进程调用则返回进程号，如果是子进程调用则返回0
        //这里父进程用来创建连接  子进程负责处理请求  所以当pid=0才执行
        if((pid=fork())==0)
        {
            //子进程会复制父进程的资源 这里我们关闭连接只是把子进程复制的连接关闭并不会真正关闭连接
            close(server_sockfd);
            int recv_file_name = 0;
    	    int fd = -1;
            while(1)
            {
                //清空缓冲区
                memset(buffer,0,sizeof(buffer));
                //接收客户端发送来的数据
                int len = recv(conn,buffer,sizeof(buffer),0);
                if (recv_file_name == 0){
                    recv_file_name = 1;
                    char file_name[BUFFER_SIZE + 10], *suffix;
                    strcpy(file_name, buffer);
                    suffix = strstr(file_name, ".");
                    if (suffix == NULL)
                        suffix = &file_name[len-1];
                    strcpy(suffix, ".log\0");
                    fd = open(file_name, O_WRONLY|O_CREAT, 00777);
                    if(fd == -1){
                        perror("file open error!\n");
                        exit(1);
                    }
                }
                else
                {   
                    if(strcmp(buffer,"exit\n")==0||len==0)
                    {
                        break;
                    }
                    if (write(fd, buffer, len) == -1){
                        perror("file write error!\n");
                        exit(1);
                    }
                }
                // fputs(buffer,stdout);   //for debug
            }
            close(fd);
            close(conn);
            exit(0);
        }
        close(conn);
        count++;
    }
    close(server_sockfd);
    return 0;
}
