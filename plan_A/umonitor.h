#ifndef UMONITOR_H_INCLUDED
#define UMONITOR_H_INCLUDED

#include <sys/socket.h>
#include <sys/types.h>

typedef struct {
  int unik_sockfd,
  int host_sockfd,
  sockaddr_in unik_sockaddr, host_sockaddr,
  int portnum,
  int clilen
} debug_link;

extern void debug_init(debug_link *thelink);
extern int send_log(debug_link *thelink, ...);

#endif
