#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

void getTime(char * timestr) {
  time_t now;
  struct tm *tm_now;

  time(&now);
  tm_now = localtime(&now);

  sprintf(timestr, "%d-%d %d:%d:%d", tm_now->tm_mon, tm_now->tm_mday, tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec);
  return;
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    FILE * target;
    char filename[48];

    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];

    portno = atoi(argv[3]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("ERROR opening socket");
	exit(1);
    }
    server = gethostbyname(argv[2]);
    if (server == NULL) {
        printf("ERROR, no such host\n");
        exit(0);
    }
    else printf("host success.\n");

    sprintf(filename, "%s.log", argv[1]);
    target = fopen(filename, "w");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    printf("connecting\n");
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
        printf("ERROR connecting");
	exit(1);
    }
    else printf("connect success\n");
    bzero(buffer,256);
    n = read(sockfd,buffer,255);

    char msg[512];
    char t[48];
    bzero(t, 48);
    bzero(msg, 512);

    while (n > 0) {
      getTime(t);
      sprintf(msg, "%s --- %s", t, buffer);
      fprintf(target, "%s\n", msg);
      bzero(buffer, 256);
      bzero(msg, 512);
      bzero(t, 48);
      n = read(sockfd, buffer, 255);
    }

    exit(0);
}
