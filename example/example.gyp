{
  "targets": [{
    "target_name": "uv_link_t-example",
    "type": "executable",

    "include_dirs": [
      "src"
    ],

    "dependencies": [
      "../test/deps/libuv/uv.gyp:libuv",
      "../uv_link_t.gyp:uv_link_t"
    ],

    "sources": [
      "src/main.c",
      "src/middle.c",
    ],
  }],
}
