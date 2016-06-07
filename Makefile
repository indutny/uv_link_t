test: dist
	./out/Release/uv_link_t-test

example: dist
	./out/Release/uv_link_t-example

dist:
	gypkg gen uv_link_t.gyp
	make -C out/ -j8

.PHONY: test example dist
