IDIR = include
CC = gcc
CFLAGS = -I$(IDIR)

ODIR = obj
SDIR = src

LIBS = -lfl

_DEPS = parser.tab.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = lex.yy.o parser.tab.o main.o data_structures.o aux.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS) | $(ODIR)
	$(CC) -c -o $@ $< $(CFLAGS)

etapa4: $(OBJ)
	$(CC) -g -o $@ $^ $(LIBS)

$(ODIR):
	mkdir -p $(ODIR)

$(SDIR)/parser.tab.c $(IDIR)/parser.tab.h: $(SDIR)/parser.y
	bison -d $(SDIR)/parser.y -o $(SDIR)/parser.tab.c
	mv $(SDIR)/parser.tab.h $(IDIR)/parser.tab.h

$(SDIR)/lex.yy.c: $(SDIR)/scanner.l
	flex -o $(SDIR)/lex.yy.c $(SDIR)/scanner.l

.PHONY: clean

clean:
	rm -rf $(ODIR) $(SDIR)/parser.tab.c $(IDIR)/parser.tab.h $(SDIR)/lex.yy.c etapa4