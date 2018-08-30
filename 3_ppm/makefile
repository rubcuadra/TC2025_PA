#Ruben Cuadra A01019102
# make
# make test
INPUTFILE=ninja.ppm
OUTPUTFILE=ninjaEdit.ppm
SCALE_FACTOR=200
NEGATIVE = 1

MAIN=main
IDIR =.
CC=gcc
CFLAGS=-I$(IDIR)

ODIR=.
LDIR =.

LIBS=-lm

_DEPS = ppmReader.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(MAIN): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~ 

test:
ifeq ($(NEGATIVE), 1)
	./$(MAIN) -i $(INPUTFILE) -o $(OUTPUTFILE) -s $(SCALE_FACTOR) -n 1
else
	./$(MAIN) -i $(INPUTFILE) -o $(OUTPUTFILE) -s $(SCALE_FACTOR)		
endif
