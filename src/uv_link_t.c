#include <string.h>

#include "common.h"

int uv_link_init(uv_loop_t* loop, uv_link_t* link) {
  memset(link, 0, sizeof(*link));
  return 0;
}


void uv_link_close(uv_link_t* link) {
}


int uv_link_chain(uv_link_t* from,
                  uv_link_t* to,
                  uv_link_alloc_cb alloc_cb,
                  uv_link_read_cb read_cb) {
  if (from->child != NULL || to->parent != NULL)
    return UV_EINVAL;

  if (from->alloc_cb != NULL || from->read_cb != NULL)
    return UV_EINVAL;

  from->child = to;
  to->parent = from;

  from->alloc_cb = alloc_cb;
  from->read_cb = read_cb;

  return 0;
}


int uv_link_unchain(uv_link_t* from, uv_link_t* to) {
  if (from->child != to || to->parent != from)
    return UV_EINVAL;

  from->child = NULL;
  to->parent = NULL;

  from->alloc_cb = NULL;
  from->read_cb = NULL;

  return from->read_stop(from);
}


void uv_link_invoke_alloc_cb(uv_link_t* link,
                             size_t suggested_size,
                             uv_buf_t* buf) {
  uv_link_t* target;

  target = link;
  if (link->child != NULL)
    target = link->child;

  link->alloc_cb(target, suggested_size, buf);
}


void uv_link_invoke_read_cb(uv_link_t* link,
                            ssize_t nread,
                            const uv_buf_t* buf) {
  uv_link_t* target;

  target = link;
  if (link->child != NULL)
    target = link->child;

  link->read_cb(target, nread, buf);
}
