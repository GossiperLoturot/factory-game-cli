FORM_SRC := $(shell find src include -type f \( -name '*.cc' -o -name '*.h' \))
TIDY_SRC := $(shell find src include -type f \( -name '*.cc' \))

.PHONY: build check compile run test

build:
	@meson setup --reconfigure build

check: build
	@clang-format -i $(FORM_SRC)
	# @clang-tidy $(TIDY_SRC) -p build

compile: build
	@meson compile -C build

run: compile
	@./build/main

test: build
	@meson test -C build -v
