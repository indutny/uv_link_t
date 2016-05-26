#include <string.h>

#include "common.h"

int uv_link_init(uv_loop_t* loop, uv_link_t* link) {
  memset(link, 0, sizeof(*link));
  return 0;
}


void uv_link_close(uv_link_t* link) {
}
