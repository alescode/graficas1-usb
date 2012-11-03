.PHONY: all clean

LDFLAGS = -lglut -lGL -lGLU -lX11 -lSDL -lm -L/usr/X11R6/lib

CPP = g++
CPPOPTS = -Wall -ggdb -I.

all: main

clean:
	rm -f main ./*.o

main: main.o ../lib/glm.o
	$(CPP) $(CPPOPTS) $^ $(LDFLAGS) -o $@

%.o: %.cpp
	$(CPP) -c $(CPPOPTS) $< -o $@

main.o: main.cpp ../lib/glm.h ../lib/formas.h ../lib/constantes.h ../lib/punto.h
../lib/glm.o: ../lib/glm.cpp ../lib/glm.h
