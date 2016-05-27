#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "uv.h"
#include "uv_link_t.h"

/* Declaration of `middle_methods` */
#include "middle.h"

typedef struct client_s client_t;

static uv_tcp_t server;

struct client_s {
  uv_tcp_t tcp;
  uv_link_source_t source;
  uv_link_t middle;
  uv_link_observer_t observer;
};

static void read_cb(uv_link_observer_t* observer,
                    ssize_t nread,
                    const uv_buf_t* buf) {
  client_t* client;

  client = observer->link.data;

  if (nread < 0) {
    fprintf(stderr, "error or close\n");
    uv_link_read_stop(&observer->link);
    uv_close((uv_handle_t*) &client->tcp, NULL);
    free(client);
    return;
  }

  fprintf(stderr, "read \"%.*s\"\n", (int) nread, buf->base);
}


static void connection_cb(uv_stream_t* s, int status) {
  int err;
  client_t* client;

  client = malloc(sizeof(*client));
  assert(client != NULL);

  err = uv_tcp_init(uv_default_loop(), &client->tcp);
  assert(err == 0);

  err = uv_accept(s, (uv_stream_t*) &client->tcp);
  assert(err == 0);

  err = uv_link_source_init(&client->source, (uv_stream_t*) &client->tcp);
  assert(err == 0);

  err = uv_link_init(&client->middle, &middle_methods);
  assert(err == 0);

  err = uv_link_chain(&client->source.link, &client->middle);
  assert(err == 0);

  err = uv_link_observer_init(&client->observer, &client->middle);
  assert(err == 0);

  client->observer.read_cb = read_cb;
  client->observer.link.data = client;

  err = uv_link_read_start(&client->observer.link);
  assert(err == 0);
}


int main() {
  static const int kBacklog = 128;

  int err;
  uv_loop_t* loop;
  struct sockaddr_in addr;

  loop = uv_default_loop();

  err = uv_tcp_init(loop, &server);
  assert(err == 0);

  err = uv_ip4_addr("0.0.0.0", 9000, &addr);
  assert(err == 0);

  err = uv_tcp_bind(&server, (struct sockaddr*) &addr, 0);
  assert(err == 0);

  fprintf(stderr, "Listening on 0.0.0.0:9000\n");

  err = uv_listen((uv_stream_t*) &server, kBacklog, connection_cb);

  err = uv_run(loop, UV_RUN_DEFAULT);
  assert(err == 0);

  return 0;
}
