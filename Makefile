SOURCES = $(wildcard src/*.cpp) $(wildcard src/*/*.cpp) $(wildcard src/*/*.c) $(wildcard src/*/*/*.cpp)

.PHONY: mingw

list:
	echo $(SOURCES)

icon.res:
	windres icon.rc -O coff -o icon.res

vs: icon.res
	g++ -g -std=c++17 $(SOURCES) icon.res -I include -I vsc_base/include -L lib -L vscbase/lib -lSDL2 -lSDL2main -lSDL2_image -lfreetype `pkg-config freetype2 --cflags --libs` -o SmashScoreboard.exe

mingw: icon.res
	g++ -g -std=c++17 -o SmashScoreboard.exe $(SOURCES) icon.res -I include -I mingw_base/include -L lib -L mingw_base/lib `./mingw_base/bin/sdl2-config --cflags --libs --static-libs` -lSDL2_image `pkg-config freetype2 --cflags --libs`