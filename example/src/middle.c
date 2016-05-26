#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "uv_link_t.h"

static int read_start_impl(uv_link_t* link) {
  return uv_link_read_start(link->parent);
}


static int read_stop_impl(uv_link_t* link) {
  return uv_link_read_stop(link->parent);
}


static void alloc_cb_override(uv_link_t* link,
                              size_t suggested_size,
                              uv_buf_t* buf) {
  buf->base = malloc(suggested_size);
  assert(buf->base != NULL);
  buf->len = suggested_size;
}


static void read_cb_override(uv_link_t* link,
                             ssize_t nread,
                             const uv_buf_t* buf) {
  const char* res;
  uv_buf_t tmp;

  if (nread >= 0) {
    if (nread == 9 && strncmp(buf->base, "password\n", 9) == 0)
      res = "welcome";
    else
      res = "go away";
  } else {
    res = "error";
  }

  free(buf->base);

  uv_link_invoke_alloc_cb(link, strlen(res), &tmp);
  assert(tmp.len >= strlen(res));

  memcpy(tmp.base, res, strlen(res));
  uv_link_invoke_read_cb(link, strlen(res), &tmp);
}


uv_link_methods_t middle_methods = {
  .read_start = read_start_impl,
  .read_stop = read_stop_impl,

  /* Other doesn't matter in this example */
  .alloc_cb_override = alloc_cb_override,
  .read_cb_override = read_cb_override
};
