# Makefile for Windows
SRCDIR=./src
INCDIR=./include
CC=gcc

# Correct the paths to use MinGW-w64 installation
INCLUDE = C:/mingw64/include
LIBDIR = C:/mingw64/lib

# Update the compilation flags to use the correct include directories
CFLAGS = -g -I$(INCLUDE) -I$(INCDIR) -c 
LDFLAGS = -L$(LIBDIR) -lmingw32 -lSDL2main -lSDL2_image -lSDL2_net -lSDL2_ttf -lSDL2  -lm -lSDL2_mixer

# Your targets
Football: main.o controller.o model.o view.o network.o
	$(CC) main.o controller.o model.o network.o view.o  -o Football $(LDFLAGS)

main.o: $(SRCDIR)/main.c
	$(CC) $(CFLAGS) $(SRCDIR)/main.c

controller.o: $(SRCDIR)/controller.c
	$(CC) $(CFLAGS) $(SRCDIR)/controller.c

model.o: $(SRCDIR)/model.c
	$(CC) $(CFLAGS) $(SRCDIR)/model.c

view.o: $(SRCDIR)/view.c
	$(CC) $(CFLAGS) $(SRCDIR)/view.c

network.o: $(SRCDIR)/network.c
	$(CC) $(CFLAGS) $(SRCDIR)/network.c

clean:
	rm -f *.o Football.exe

