{
  "targets": [{
    "target_name": "uv_link_t-example",
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
      "src/middle.c",
    ],
  }],
}
