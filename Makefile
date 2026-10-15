.PHONY: build
# Define the compiler
CC = gcc

# Define the target executable name
TARGETLIN = bin/Game
TARGETWIN = bin/Game.exe
DATE := $(shell date +%d.%m)

# Define the source file
SRC = src/main.c src/game.c src/init.c src/menus.c external/cJSON/cJSON.c

# Compiler flags
CFLAGS = -Iexternal/raylib -Iexternal/raygui -Iexternal/cJSON -Iinclude

#libs
LIBS = -Llib/linux -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -O2 

LIBSWIN = -Llib/mingw -lraylib -lm -lpthread -lopengl32 -lwinmm -lgdi32 -mwindows -O2

#debug
DFLAGS = -fsanitize=address -g -O0 -Wall -Werror

# Build target
$(TARGETLIN): $(SRC)
	$(CC) -o $(TARGETLIN) $(SRC) $(CFLAGS) $(LIBS)

debug:
	$(CC) $(DFLAGS) -o $(TARGETLIN) $(SRC) $(CFLAGS) $(LIBS)

win:
	x86_64-w64-mingw32-gcc -o $(TARGETWIN) $(SRC) $(CFLAGS) $(LIBSWIN)

winDebug:
	x86_64-w64-mingw32-gcc -o $(TARGETWIN) $(SRC) $(CFLAGS) $(LIBSWIN)

# Clean target to remove the compiled binary
clean:
	rm -f $(TARGETLIN)

cleanWin:
	rm -f $(TARGETWIN)

build:
	mkdir -p build/$(DATE)/
	cp $(TARGETLIN) build/$(DATE)/
	cp $(TARGETWIN) build/$(DATE)/
	cp -r bin/assets build/$(DATE)/
