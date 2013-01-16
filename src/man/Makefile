# Main man makefile
ATOM_TOOLCHAIN = cmake/atom.cmake

.PHONY: build
build: build/Makefile
	make -C build --no-print-directory

.PHONY: install
install: build/Makefile
	make install -C build --no-print-directory; build/upload.sh

.PHONY: cross
cross:
	mkdir -p build
	cd build; \
	cmake -DCMAKE_TOOLCHAIN_FILE=$(ATOM_TOOLCHAIN) -DCMAKE_INSTALL_PREFIX=install/ ..; \
	ccmake ..

.PHONY: clean
clean:
	rm -rf build/*

