IDIR =./include
CC=gcc
CFLAGS =-I$(IDIR)

ODIR=obj
LDIR=lib

_DEPS = tokens.h linked_list.h lexer.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o linked_list.o lexer.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: src/%.c $(DEPS)
				$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
				$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
				rm -f $(ODIR)/*.o *~ core $(IDIR)/*~ 
