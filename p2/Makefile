.PHONY: all clean

LDFLAGS = -lglut -lGL -lGLU -lX11 -lm -L/usr/X11R6/lib \
-framework OpenAL -framework Foundation

CPP = g++
CPPOPTS = -Wall -ggdb -I.

all: bloodstream

clean:
	rm -f bloodstream ./*.o

bloodstream: ../lib/glm.o bloodstream.o
	$(CPP) $(CPPOPTS) $^ $(LDFLAGS) -o $@

%.o: %.cpp
	$(CPP) -c $(CPPOPTS) $< -o $@

bloodstream.o: bloodstream.cpp ../lib/glm.h ../lib/formas.h ../lib/constantes.h ../lib/punto.h ../lib/objeto.h
../lib/glm.o: ../lib/glm.cpp ../lib/glm.h
