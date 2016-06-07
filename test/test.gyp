{
  "targets": [{
    "target_name": "uv_link_t-test",
    "type": "executable",

    "include_dirs": [
      "src"
    ],

    "variables": {
      "gypkg_deps": [
        "git://github.com/libuv/libuv.git@^1.9.0 => uv.gyp:libuv",
      ],
    },

    "dependencies": [
      "<!@(gypkg deps <(gypkg_deps))",
      "../uv_link_t.gyp:uv_link_t",
    ],

    "sources": [
      "src/main.c",
      "src/test-uv-link-source-t.c",
      "src/test-uv-link-observer-t.c",
      "src/test-defaults.c",
      "src/test-close.c",
      "src/test-strerror.c",
    ],
  }],
}
