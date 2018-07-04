/**
 * this file defiines the function that receives / send.
 */

#ifndef HOST_H_INCLUDED
#define HOST_H_INCLUDED
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct uni_info {
  char name[32];
  char ip[32];
  int port;
} uni_info;

extern int run_script(int num);

void msg_phrase(char *dst, char *msg) {
  // add some extra infomation to the msg.
  time_t now;
  struct tm *tm_now;

  time(&now);
  tm_now = localtime(&now);

  char sign[32];
  memset(sign, 0, 32);
  char l = msg[0];
  if (l == 'm')
    sprintf(sign, " \033[32m message: \033[0m ");
  else if(l == 'w')
    sprintf(sign, " \033[33m warning: \033[0m ");
  else if(l == 'f')
    sprintf(sign, " \033[31m error: \033[0m ");
  else if(l == 'r')
    sprintf(sign, " \033[32m return: \033[0m ");

  sprintf(dst, "%d-%d %d:%d:%d -- %s %s", tm_now->tm_mon, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec, sign, msg + 1);
  return ;
}

void* uni_connect(void *arg) {
// this function implements the connection between the host and unikernel.
  int ser_fd;
  struct sockaddr_in ser;
  pthread_t p1, p2;
  uni_info *info = (uni_info *) arg;
  FILE *fp;
  //-----------------------------//
  ser_fd = socket(AF_INET, SOCK_STREAM, 0);

  memset(&ser, 0, sizeof(ser));
  ser.sin_family = AF_INET;
  ser.sin_port = htons(info->port);
  inet_aton(info->ip, &ser.sin_addr);

  printf("connecting\n");
  int m = connect(ser_fd, (struct sockaddr *)&ser, sizeof(ser));
  if(m == 0) {
    printf("connect success\n");
  }

  char recv_buf[128];
  char msg[256];
  memset(recv_buf, 0, sizeof(recv_buf));
  memset(msg, 0, sizeof(msg));

  char fname[48];
  sprintf(fname, "%s.log", info->name);

  fp = fopen(fname, "a");

  int i = 0;
  while(recv(ser_fd, recv_buf, 128, 0) > 0) {
    msg_phrase(msg, recv_buf);
    fprintf(fp, "%s\n", msg);
    i ++;
    if (i == 10) fflush(fp);
  }

  fflush(fp);
  fclose(fp);
  return NULL;
}

#endif
