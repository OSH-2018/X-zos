/**
 * this file contains the main program that is 
 * responsible for communicate with the user.
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "host.h"

uni_info info[512];
int c_num = 0;
int num = 0;
pthread_t connects[512];

void string_phrase(char *string) {
  int i = 0;
  for(i = 0; string[i] != '\n' && string[i] != 0; i++)
  ;
  string[i] = 0;
}

void allocate_ip(int num) {
  sprintf(info[num].ip, "10.0.120.%d", 2+num);
  return ;
}

void allocate_port(int num) {
  info[num].port = 2333+num;
  return ;
}

void load_config(char *fname) {
  FILE *fp;
  fp = fopen(fname, "r");
  char buf[64];
  if (fp == NULL) {
    printf("no such file !\n");
    exit(1);
  }
  else {
    while(!feof(fp)) {
      fgets(buf, 64, fp);
      string_phrase(buf);
      if (access(buf, F_OK) == -1) {
        printf("unikernel %s doesn't exist, please verify\n", buf);
      }
      strcpy(info[num].name, buf);
      allocate_ip(num);
      allocate_port(num);
      pthread_create(&connects[c_num], NULL, &uni_connect, (void *)&(info[num]));
      num++;
      c_num++;
    }
  }
}

int run_script(int num) {
  char run_cmd[128];
  sprintf(run_cmd, "qemu -i -I if,vioif, '-net tap, script=no,ifname=tap0' -W if,inet,static, %s, %s", info[num].ip, info[num].name);

  return system(run_cmd);
}

int main(int argc, char *argv[]) {

  char cmd[32];

  memset(info, 0, sizeof(info));

  system("./ifsetup.sh");

  if(argc == 2) {
    load_config(argv[1]);
  }

  while(1) {
    memset(cmd, 0, sizeof(cmd));
    printf(">> ");
    fgets(cmd, 32, stdin);
    int i = 0;
    for (; cmd[i] != '\n'; i++)
    ;
    cmd[i] = 0;

    // first command: new unikernel.
    if (strcmp(cmd, "new") == 0) {
      printf("input the file name: ");
      fgets(info[num].name, 32, stdin);
      for (i = 0; info[num].name[i] != '\n'; i++)
      ;
      info[num].name[i] = 0;

      if (access(info[num].name, F_OK) == -1) {
        printf("unikernel doesn't exist, please try again\n");
      }
      else {
        char ip_t[20];
        sprintf(info[num].ip, "10.0.120.%d", 2+num);
        info[num].port = 2333;

        printf("ip addr: 10.0.120.%d, port = %d\n", 2+num, 2333+num);
        num ++;
      }
    }

    else if (strcmp(cmd, "run") == 0) {
      char name[32];
      printf("input the image name: ");
      fgets(name, 32, stdin);
      for (i = 0; name[i] != '\n'; i++)
      ;
      name[i] = 0;

      int j = 0;
      for (j = 0; j <= num - 1; j++)
        if (strcmp(info[j].name, name) == 0)
          break;
      run_script(j);
      pthread_create(&connects[c_num], NULL, &uni_connect, (void *)&(info[j]));
      //pthread_join(connects[c_num], NULL);

      c_num ++;
    }

    else if (strcmp(cmd, "list") == 0) {
      int k = 0;
      for (k = 0; k <= num - 1; k ++) {
        printf("%s\t", info[k].name);
      }
    }
  }
}
