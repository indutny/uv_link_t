#ifndef INCLUDE_UV_LINK_H_
#define INCLUDE_UV_LINK_H_

#include "uv.h"

typedef struct uv_link_s uv_link_t;
typedef struct uv_link_source_s uv_link_source_t;

typedef void (*uv_link_alloc_cb)(uv_link_t* link,
                                 size_t suggested_size,
                                 uv_buf_t* buf);
typedef void (*uv_link_read_cb)(uv_link_t* link,
                                ssize_t nread,
                                const uv_buf_t* buf);
typedef void (*uv_link_write_cb)(uv_link_t* link, int status);
typedef void (*uv_link_shutdown_cb)(uv_link_t* link, int status);

struct uv_link_s {
  uv_link_t* parent;
  uv_link_t* child;

  uv_link_alloc_cb alloc_cb;
  uv_link_read_cb read_cb;

  /* Sort of virtual table */
  int (*read_start)(uv_link_t* link);
  int (*read_stop)(uv_link_t* link);

  int (*write)(uv_link_t* link,
               const uv_buf_t bufs[],
               unsigned int nbufs,
               uv_stream_t* send_handle,
               uv_link_write_cb cb);
  int (*try_write)(uv_link_t* link,
                   const uv_buf_t bufs[],
                   unsigned int nbufs);

  int (*shutdown)(uv_link_t* link, uv_link_shutdown_cb cb);
};

struct uv_link_source_s {
  uv_link_t link;

  uv_stream_t* stream;
};

UV_EXTERN int uv_link_init(uv_loop_t* loop, uv_link_t* link);
UV_EXTERN void uv_link_close(uv_link_t* link);

UV_EXTERN int uv_link_chain(uv_link_t* from, uv_link_t* to);
UV_EXTERN int uv_link_unchain(uv_link_t* from, uv_link_t* to);

/* Source */

/* NOTE: uses `stream->data` field */
UV_EXTERN int uv_link_source_init(uv_loop_t* loop,
                                  uv_link_source_t* source,
                                  uv_stream_t* stream);
UV_EXTERN void uv_link_source_close(uv_link_source_t* source);

#endif  /* INCLUDE_UV_LINK_H_ */
