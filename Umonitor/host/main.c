#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <stdlib.h>

typedef struct {
  char name[32];
  char ip_addr[16];
  char portno[8];
} uni_link;

uni_link link[256];

int main() {

  char cmd[32];
  int child;
  int num = 0;

  while(1) {
    memset(cmd, 0, sizeof(cmd));
    printf(">> ");
    fgets(cmd, 32, stdin);
    int i = 0;
    for (; cmd[i] != '\n'; i++)
    ;
    cmd[i] = 0;
    if (strcmp(cmd, "new") == 0) {
      printf("input the file name: ");
      fgets(link[num].name, 32, stdin);
      for (i = 0; link[num].name[i] != '\n'; i++)
      ;
      link[num].name[i] = 0;

      if (access(link[num].name, F_OK) == -1) {
        printf("unikernel doesn't exist, please try again\n");
      }
      else {
        itoa(2333 + i, link[num].portno, 10);
        sprintf(link[num].ipaddr, "10.0.120.%d", 1 + num);
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
        if (strcmp(link[j].name, name) == 0)
          break;
      child = fork();
      if (child == 0)
        execlp("monitor",link[j].name, link[j].ip_addr, link[j].portno);
    }

    else if (strcmp(cmd, "list") == 0) {
      int k = 0;
      for (k = 0; k <= num - 1; k ++) {
        printf("%s\t", link[k].name);
      }
    }
  }
}
