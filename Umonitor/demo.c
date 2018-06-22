#include <stdio.h>
#include "umonitor.h"

debug_link thelink;

int foo() {
  #ifndef NDEBUG
    send_log(&thelink, "funcion: %s\n", __func__);
  #endif
  return 0;
}

int main() {
  #ifndef NDEBUG
    debug_init(&thelink);
  #endif

  foo();

  return 0;
}
