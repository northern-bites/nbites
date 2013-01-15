# Main man makefile
ATOM_TOOLCHAIN = cmake/atom.cmake

.PHONY: build
build: build/Makefile
	cd build; make --no-print-directory

.PHONY: cross
cross:
	mkdir -p build
	cd build; cmake -DCMAKE_TOOLCHAIN_FILE=$(ATOM_TOOLCHAIN) ..; ccmake ..

.phony: clean
clean:
	rm -rf build/*

