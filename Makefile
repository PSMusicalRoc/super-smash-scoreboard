SOURCES = $(wildcard src/*.cpp) $(wildcard src/*/*.cpp) $(wildcard src/*/*.c)

.PHONY: list

list:
	echo $(SOURCES)

vs:
	g++ -g -std=c++17 $(SOURCES) -I include -I vsc_base/include -L lib -L vscbase/lib -lSDL2 -lSDL2main -lSDL2_image -lfreetype `pkg-config freetype2 --cflags --libs` -o Output.exe

mingw:
	g++ -g -std=c++17 -o Output.exe $(SOURCES) -I include -I mingw_base/include -L lib -L mingw_base/lib `./mingw_base/bin/sdl2-config --cflags --libs --static-libs` -lSDL2_image `pkg-config freetype2 --cflags --libs`