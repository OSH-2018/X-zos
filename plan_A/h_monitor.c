#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "hmonitor.h"

h_link moni[64]; // the array of unikernel being monitored.
int num; // number of unikernel being monitored

void set_interface() {
  printf("input the ip address: ");
  fgets(stdin, moni[num].ip, 15);
  printf("input the port: ");
  scanf("%d", &(moni[num].portnum));
  struct hostent *client;
  int child = -1;

  client = gethostbyname(moni[num].ip);

  moni[num].hsock = socket(AF_INET, SOCK_STREAM, 0);

  moni[num].u_sockaddr.sin_family = AF_INET;
  moni[num].u_sockaddr.sin_addr.s_addr = htons(client->h_addr);
  moni[num].u_sockaddr.sin_port = htons(moni[num].portnum);

  if (connect(moni[num].hsock, (struct sockaddr *)&(moni[num].u_sockaddr),
      sizeof(struct sockaddr_in)) < 0) {
      printf("error on connecting, please try again\n");
      return ;
  }
  else {
    child = fork();
    if (child == 0) h_recv(num);
    else {
      num++;
      return;
    }
  }
}

void h_recv(int num) {
  FILE * logfile;
  char filename[16];
  sprintf(filename, "log%d", num);
  logfile = fopen(filename, "w");
  fclose(logfile);
  int m;
  char buff[256];
  m = recv(moni[num].hsock, buff, 256, 0);
  while(m > 0) {
    logfile = fopen(filename, "a+");
    fprintf(logfile, "%s\n", buff);
    fclose(logfile);
    memset(buff, 0, sizeof(buff));
    m = recv(moni[num].hsock, buff, 256, 0);
  }
}

void cat_log() {
}

int main() {
  // initialize global variable.
  memset(moni, 0, sizeof(moni));
  num = 0;

  while(1) {
    printf("please choose :\n");
    printf("1: set new unikernel ");
    printf("2: see the log\n");
    printf(">> ");
    int cho = 0;
    scanf("%d", &cho);

    switch (cho) {
      case 1: set_interface(); break;
      case 2: cat_log(); break;
      default: continue;
    }
  }
}
