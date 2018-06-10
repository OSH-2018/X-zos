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

void debug_init() {
  int debug_sockfd, new_sockfd, portnum, clilen;
  struct sockaddr_in serv_addr, cli_addr;
  protnum = 2333;

  debug_sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if(debug_sockfd < 0)
    fprintf(stderr, "ERROR ON CREATING SOCKET\n");

  bzero((char *)&sevr_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(protnum);

  int m = bind(debug_sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

  if (m < 0)
    fprintf(stderr, "ERROR ON BINDING\n");

  listen(debug_sockfd, 5);
  clilen = sizeof(cli_addr);
  new_sockfd = accept(debug_sockfd, (struct sockaddr *)&cli_addr, &clilen);

  return 0;
}
