#ifndef UMONITOR_H_INCLUDED
#define UMONITOR_H_INCLUDED

#define MESSAGE 0
#define WARNING 1
#define FAILT 2
#define RETURN 3

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

typedef struct {
  int unik_sockfd;
  int host_sockfd;
  struct sockaddr_in unik_sockaddr, host_sockaddr;
  int portnum;
  int clilen;
} debug_link;

extern void debug_init(debug_link *thelink);
extern int send_log(debug_link *thelink, int type, char *format, ...);

#endif
