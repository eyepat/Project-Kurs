# Makefile for Windows
SRCDIR=./src
INCDIR=./include
CC=gcc
INCLUDE = C:\msys64\mingw64\include\SDL2

CFLAGS = -g -I$(INCLUDE) -I$(INCDIR) -c 
LDFLAGS = -lmingw32 -lSDL2main -lSDL2_image -lSDL2_ttf -lSDL2 -mwindows -lm

Football: main.o controller.o model.o view.o network.o
	$(CC) main.o controller.o model.o view.o network.o -o Football $(LDFLAGS)

network.o: $(SRCDIR)/network.c
	$(CC) $(CFLAGS) $(SRCDIR)/network.c

main.o: $(SRCDIR)/main.c
	$(CC) $(CFLAGS) $(SRCDIR)/main.c

controller.o: $(SRCDIR)/controller.c
	$(CC) $(CFLAGS) $(SRCDIR)/controller.c

model.o: $(SRCDIR)/model.c
	$(CC) $(CFLAGS) $(SRCDIR)/model.c

view.o: $(SRCDIR)/view.c
	$(CC) $(CFLAGS) $(SRCDIR)/view.c

clean:
	rm -f *.o Football
