# Makefile for NSGA2

# Compiler
CC = gcc

# Compiler options
CFLAGS = -g -Wall -pedantic

# all object files
SEL_OBJECTS = nsga2_functions.o nsga2_io.o nsga2.o

nsga2 : $(SEL_OBJECTS)
	$(CC) $(CFLAGS) -lm $(SEL_OBJECTS) -o nsga2

nsga2_functions.o : nsga2_functions.c nsga2.h
	$(CC) $(CFLAGS) -c nsga2_functions.c 

nsga2_io.o : nsga2_io.c nsga2.h
	$(CC) $(CFLAGS) -c nsga2_io.c

nsga2.o : nsga2.c nsga2.h
	$(CC) $(CFLAGS) -c nsga2.c

clean:
	rm -f *~ *.o

