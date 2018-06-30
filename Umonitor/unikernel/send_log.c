/*
  this is the file that implement send_log(...)function.
*/
#include "umonitor.h"
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

int send_log(debug_link *thelink, char *format, ...) {
  va_list args;
  va_start(args, format);
  char buff[256];
  memset(buff, 0, 256);

  buff[0] = 3;

  vsprintf((char *)buff + 1, format, args);
  va_end(args);

  int n = write(thelink->host_sockfd, buff, strlen(buff));
  if (n < 0)
    printf("error on sending\n");

  return 0;
}

int send_error(debug_link *thelink, char *format, ...) {
  va_list args;
  va_start(args, format);
  char buff[256];
  memset(buff, 0, 256);
  buff[0] = 2;
  vsprintf((char *)buff + 1, format, args);

  int n = write(thelink->host_sockfd, buff, strlen(buff));
  if (n < 0)
    printf("error on sending\n");

  return 0;
}

int send_warning(debug_link *thelink, char *format, ...) {
  va_list args;
  va_start(args, format);
  char buff[256];
  memset(buff, 0, 256);
  buff[0] = 5;
  vsprintf((char *)buff + 1, format, args);

  int n = write(thelink->host_sockfd, buff, strlen(buff));
  if (n < 0)
    printf("error on sending\n");

  return 0;
}
