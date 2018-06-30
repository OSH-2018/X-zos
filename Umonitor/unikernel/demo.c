#include <stdio.h>
#include "umonitor.h"

debug_link thelink;

int foo1() {
  #ifndef NDEBUG
    send_log(&thelink, "funcion: %s", __func__);
  #endif
  return 0;
}

int foo2() {
  #ifndef NDEBUG
    send_error(&thelink, "function: %s divide by zero", __func__);
  #endif
    return 0;
}

int foo3() {
  #ifndef NDEBUG
    send_warning(&thelink, "function: %s null pointer", __func__);
  #endif
   return 0;
}

int main() {
  #ifndef NDEBUG
    debug_init(&thelink);
  #endif

  foo1();
  foo2();

  return 0;
}
