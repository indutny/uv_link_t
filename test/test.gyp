{
  "targets": [{
    "target_name": "uv_link_t-test",
    "type": "executable",

    "include_dirs": [
      "src"
    ],

    "dependencies": [
      "deps/libuv/uv.gyp:libuv",
      "../uv_link_t.gyp:uv_link_t"
    ],

    "sources": [
      "src/main.c",
      "src/test-uv-link-source-t.c",
      "src/test-uv-link-observer-t.c",
      "src/test-defaults.c",
      "src/test-close.c",
    ],
  }],
}
