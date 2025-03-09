all: clean build
clean:
	rm -rf build obj-* || true
build:
	meson setup build
	ninja -C build
