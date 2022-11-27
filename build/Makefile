target = gbemu
csources = ../src/gbemu/main.c $(wildcard ../src/lib/*.c)
objects = $(csources:.c=.o)
CFLAGS= -g -s -std=c17 -Wall -fwrapv -I../windows_deps/sdl2/include/SDL2 -I../src/include

SHELL=/bin/bash

.PHONY: all
all: $(target)

$(target): Makefile $(objects)
	$(CC) $(CFLAGS) $(objects) -o $(target) ../windows_deps/sdl2/lib/x64/SDL2.lib -mwindows

.PHONY: wasm
wasm: EM-Makefile
	$(MAKE) -f EM-Makefile

.PHONY: clean
clean:
	rm -Rf $(target) ../src/lib/*.o ../src/gbemu/*.o

.PHONY: deploy
deploywasm:
	cp gbemu.* ../js/
