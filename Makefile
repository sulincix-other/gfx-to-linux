all: clean build
clean:
	rm -rf build || true
build:
	meson setup build
	ninja -C build