# This sample Makefile allows you to make an OpenGL application
#   whose source is exactly one .c or .cpp file under Linux.
#   check the location of the X libraries on your implementation.
#
# To use this Makefile, you type:
#
#        make xxxx
# where
#       xxxx.c is the name of the file you wish to compile 
# A binary named xxxx will be produced
# Libraries are assumed to be in the default search paths
# as are any required include files

CC = gcc
CP = g++

LDLIBS = -lglut -lGL -lGLU -lX11  -lm -L/usr/X11R6/lib

.c:
	$(CC)  -g $@.c $(LDLIBS) -o $@

.cpp:
	$(CP)  -g $@.cpp $(LDLIBS) -o $@
