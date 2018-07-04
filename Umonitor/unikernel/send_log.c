/*
  this is the file that implement send_log(...)function.
*/
#include "umonitor.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

int send_log(debug_link *thelink, int type, char *format, ...) {
  va_list args;
  va_start(args, format);
  char buff[256];
  memset(buff, 0, 256);

  if(type == MESSAGE)
    buff[0] = 'm';
  else if(type == WARNING)
    buff[0] = 'w';
  else if(type == FAILT)
    buff[0] = 'f';
  else if(type == RETURN)
    buff[0] = 'r';

  vsprintf((char *)buff + 1, format, args);
  va_end(args);

  int n = write(thelink->host_sockfd, buff, strlen(buff));
  if (n < 0)
    printf("error on sending\n");

  return 0;
}
