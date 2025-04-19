CC=gcc
SRCDIR=src
CFLAGS=-I$(SRCDIR) -g -DDEBUG_MODEc -Wall -Werror -Wpedantic -fsanitize=address

ODIR=obj
_OBJ=main.o qoi_header.o qoi_encode.o qoi_decode.o errmsg.o qoi_operations.o qoi_channels.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

_DEPS=qoi_header.h qoi_decode.h qoi_encode.h errmsg.h qoi_operations.h qoi_channels.h
DEPS=$(patsubst %,$(SRCDIR)/%,$(_DEPS))

$(ODIR)/%.o: $(SRCDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

qoify: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o $(ODIR)/*/*.o *~ core
