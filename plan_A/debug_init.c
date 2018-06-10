/*
  this is the file that defines the method used for debug
  initialization. we set up the server, wait for connection.
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include "umonitor.h"

void debug_init(debug_link *thelink) {

  memset(thelink, 0, sizeof(debug_link));

  thelink->unik_sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if(thelink->unik_sockfd < 0)
    fprintf(stderr, "ERROR ON CREATING SOCKET\n"); // this must be replaced.

  thelink->portnum = atoi("2333");

  thelink->unik_sockaddr.sin_family = AF_INET;
  thelink->unik_sockaddr.sin_addr.s_addr = INADDR_ANY;
  thelink->unik_sockaddr.sin_port = htons(thelink->portnum);

  int m = bind(thelink->unik_sockfd, (struct sockaddr *)&(thelink->unik_sockaddr), sizeof(struct sockaddr_in));

  if (m < 0)
    fprintf(stderr, "ERROR ON BINDING\n"); // replace .

  thelink->clilen = sizeof(struct sockaddr_in);
  listen(thelink->unik_sockfd, 5);
  thelink->host_sockfd = accept(thelink->unik_sockfd, (struct sockaddr *)&(thelink->host_sockaddr), &(thelink->clilen));

}
