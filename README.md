# GBEmu

## Important References:

https://gbdev.io/pandocs/

https://problemkaputt.de/pandocs.htm

https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html

https://archive.org/details/GameBoyProgManVer1.1/page/n85/mode/2up

https://github.com/rockytriton/LLD_gbemu/raw/main/docs/The%20Cycle-Accurate%20Game%20Boy%20Docs.pdf

https://github.com/rockytriton/LLD_gbemu/raw/main/docs/gbctr.pdf

NOTE: Designed to run on Linux, but you can build on Windows with MSYS2 and mingw-w64

Windows Environment Setup:

Install MSYS2: https://www.msys2.org/

Follow instructions 1 through 7 on the MSYS2 page.

pacman -S cmake

pacman -S mingw64/mingw-w64-x86_64-SDL2 mingw64/mingw-w64-x86_64-SDL2_mixer mingw64/mingw-w64-x86_64-SDL2_image mingw64/mingw-w64-x86_64-SDL2_ttf mingw64/mingw-w64-x86_64-SDL2_net

pacman -S mingw-w64-x86_64-check

After above steps you should be able to build from Windows using MSYS2 just like in the videos.

## Build For Original Version

	cd build
	make
	./gbemu.exe
	# then you can drag rom to the window to play, asdw for d-pad and jk for ab and enter for start

## Build For Wasm Version

	# First you should install emsdk for the build
	cd build
	make clean
	make wasm
	make deploywasm
	cd ../js
	# Then you can startup a web server to execute the js code, for simple you can just start a http server by python
	like under cmd
	python -m http.server

	# You can change the rom in the js/single.js

