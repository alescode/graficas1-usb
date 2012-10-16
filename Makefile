.PHONY: all clean

# LDFLAGS = -lGL -lglut

# Mac OS X: OpenGL and GLUT are frameworks, override LDFLAGS above with these
LDFLAGS = -framework OpenGL -framework GLUT

CPP = g++
CPPOPTS = -Wall -ggdb -I.

all: proy01

clean:
	rm -f proy01 ./*.o

proy01: proy01.o lib/glm.o
	$(CPP) $(CPPOPTS) $^ $(LDFLAGS) -o $@

%.o: %.cpp
	$(CPP) -c $(CPPOPTS) $< -o $@

proy01.o: proy01.cpp lib/glm.h lib/formas.h lib/constantes.h lib/punto.h
lib/glm.o: lib/glm.cpp lib/glm.h