# Simple example of a Makefile
INPUT  = "assets/ppms/ninja.ppm"
OUTPUT = "assets/ppms/ninja2.ppm"
MATRIX = "assets/filters/edge_detect.txt" #blur down sharpen
### Variables for this project ###
# These should be the only ones that need to be modified
# The files that must be compiled, with a .o extension
OBJECTS = test_ppm.o ppms.o string_functions.o filters.o
# The header files
DEPENDS = ppms.h string_functions.h filters.h
# The executable program to be created
MAIN = test_ppm

### Variables for the compilation rules ###
# These should work for most projects, but can be modified when necessary
# The compiler program to use
CC = /usr/local/bin/gcc-8
# Options to use when compiling object files
CFLAGS = -Wall -g -std=c99 -pedantic -lpthread -fopenmp #-D DEBUG
# Options to use for the final linking process
# This one links the math library
LDFLAGS = -lm

### The rules ###
# These should work for most projects without change

# Default rule
all: $(MAIN)

# Rule to make the executable
$(MAIN): $(OBJECTS)
	$(CC) $^ -o $@ $(LDFLAGS) -fopenmp

# Rule to make the object files
%.o: %.c $(DEPENDS)
	$(CC) $< -c -o $@ $(CFLAGS)

# Clear the compiled files
clean:
	rm -rf *.o $(MAIN)
	
# Indicate the rules that do not refer to a file
.PHONY: clean all

test:
	./$(MAIN) -i $(INPUT) -o $(OUTPUT) -m $(MATRIX)