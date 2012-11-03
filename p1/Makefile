.PHONY: all clean

LDFLAGS = -lglut -lGL -lGLU -lX11  -lm -L/usr/X11R6/lib

CPP = g++
CPPOPTS = -Wall -ggdb -I.

all: espacio_modelos

clean:
	rm -f espacio_modelos ./*.o

espacio_modelos: espacio_modelos.o ../lib/glm.o
	$(CPP) $(CPPOPTS) $^ $(LDFLAGS) -o $@

%.o: %.cpp
	$(CPP) -c $(CPPOPTS) $< -o $@

espacio_modelos.o: espacio_modelos.cpp ../lib/glm.h ../lib/formas.h ../lib/constantes.h ../lib/punto.h
../lib/glm.o: ../lib/glm.cpp ../lib/glm.h
