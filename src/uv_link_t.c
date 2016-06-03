#include <stdlib.h>
#include <string.h>

#include "src/common.h"

static void uv_link_maybe_close(uv_link_t* link);

static void uv_link_def_alloc_cb(uv_link_t* link,
                                 size_t suggested_size,
                                 uv_buf_t* buf) {
  buf->base = malloc(suggested_size);
  buf->len = suggested_size;

  CHECK_NE(buf->base, NULL, "uv_link_t: alloc_cb failure");
}


static void uv_link_def_read_cb(uv_link_t* link,
                                ssize_t nread,
                                const uv_buf_t* buf) {
  if (buf != NULL)
    free(buf->base);
}


int uv_link_init(uv_link_t* link, uv_link_methods_t const* methods) {
  memset(link, 0, sizeof(*link));

  link->alloc_cb = uv_link_def_alloc_cb;
  link->read_cb = uv_link_def_read_cb;

  link->methods = methods;

  return 0;
}


#define CLOSE_WRAP(RES)                                                       \
    do {                                                                      \
      int err;                                                                \
      link->close_depth++;                                                    \
      err = (RES);                                                            \
      if (--link->close_depth == 0)                                           \
        uv_link_maybe_close(link);                                            \
      return err;                                                             \
    } while (0)


int uv_link_propagate_write(uv_link_t* link, uv_link_t* source,
                            const uv_buf_t bufs[], unsigned int nbufs,
                            uv_stream_t* send_handle,
                            uv_link_write_cb cb, void* arg) {
  if (link == NULL)
    return UV_EFAULT;
  CLOSE_WRAP(link->methods->write(link, source, bufs, nbufs, send_handle, cb,
                                  arg));
}


int uv_link_propagate_shutdown(uv_link_t* link,
                               uv_link_t* source,
                               uv_link_shutdown_cb cb,
                               void* arg) {
  if (link == NULL)
    return UV_EFAULT;
  CLOSE_WRAP(link->methods->shutdown(link, source, cb, arg));
}


int uv_link_read_start(uv_link_t* link) {
  if (link == NULL)
    return UV_EFAULT;
  CLOSE_WRAP(link->methods->read_start(link));
}


int uv_link_read_stop(uv_link_t* link) {
  if (link == NULL)
    return UV_EFAULT;
  CLOSE_WRAP(link->methods->read_stop(link));
}


int uv_link_try_write(uv_link_t* link,
                      const uv_buf_t bufs[],
                      unsigned int nbufs) {
  if (link == NULL)
    return UV_EFAULT;
  CLOSE_WRAP(link->methods->try_write(link, bufs, nbufs));
}


void uv_link_close(uv_link_t* link, uv_link_close_cb cb) {
  uv_link_propagate_close(link, link, cb);
}


static void uv_link_close_join(uv_link_t* link) {
  if (--link->close_waiting == 0)
    return link->saved_close_cb(link);
}


void uv_link_maybe_close(uv_link_t* link) {
  uv_link_t* source;

  if (link->pending_close_source == NULL)
    return;

  source = link->pending_close_source;

  link->pending_close_source = NULL;
  link->methods->close(link, source, uv_link_close_join);
}


void uv_link_propagate_close(uv_link_t* link, uv_link_t* source,
                             uv_link_close_cb cb) {
  uv_link_t* root;
  int count;

  CHECK_EQ(link->child, NULL, "uv_link_t: attempt to close chained link");

  /* Find root */
  count = 1;
  for (root = link; root->parent != NULL; root = root->parent)
    count++;

  /* NOTE: This is very important line. Only because we `+=` here the
   * recursive propagation is possible
   */
  source->close_waiting += count;

  source->saved_close_cb = cb;

  /* Go from the root to the leaf, disconnecting and closing everything */
  while (root != NULL) {
    uv_link_t* child;

    child = root->child;
    if (child != NULL)
      CHECK_EQ(uv_link_unchain(root, child), 0, "close unchain");

    if (root->close_depth == 0)
      root->methods->close(root, source, uv_link_close_join);
    else
      root->pending_close_source = source;
    root = child;
  }
}


int uv_link_chain(uv_link_t* from, uv_link_t* to) {
  if (from->child != NULL || to->parent != NULL)
    return UV_EINVAL;

  CHECK(to->methods->alloc_cb_override != NULL &&
            to->methods->read_cb_override != NULL,
        "Attempting to chain link without overriding callbacks");

  from->child = to;
  to->parent = from;

  from->saved_alloc_cb = from->alloc_cb;
  from->saved_read_cb = from->read_cb;
  from->alloc_cb = to->methods->alloc_cb_override;
  from->read_cb = to->methods->read_cb_override;

  return 0;
}


int uv_link_unchain(uv_link_t* from, uv_link_t* to) {
  if (from->child != to || to->parent != from)
    return UV_EINVAL;

  from->child = NULL;
  to->parent = NULL;

  from->alloc_cb = from->saved_alloc_cb;
  from->read_cb = from->saved_read_cb;
  from->saved_alloc_cb = NULL;
  from->saved_read_cb = NULL;

  return 0;
}


void uv_link_propagate_alloc_cb(uv_link_t* link,
                                size_t suggested_size,
                                uv_buf_t* buf) {
  uv_link_t* target;

  target = link;
  if (link->child != NULL)
    target = link->child;

  target->close_depth++;
  link->alloc_cb(target, suggested_size, buf);
  if (--target->close_depth == 0)
    uv_link_maybe_close(target);
}


void uv_link_propagate_read_cb(uv_link_t* link,
                               ssize_t nread,
                               const uv_buf_t* buf) {
  uv_link_t* target;

  target = link;
  if (link->child != NULL)
    target = link->child;

  target->close_depth++;
  link->read_cb(target, nread, buf);
  if (--target->close_depth == 0)
    uv_link_maybe_close(target);
}
