SOURCES = src/main.c
LEX_SOURCE = src/scanner.l
LEX = lex.yy.c
EXEC = etapa2

all: $(EXEC)

lex.yy.c: $(LEX_SOURCE)
	flex $(LEX_SOURCE)

parser.tab.c parser.tab.h: src/parser.y
	bison -d src/parser.y

$(EXEC): $(LEX) $(SOURCES) parser.tab.c
	gcc -c src/main.c parser.tab.c lex.yy.c 
	gcc -o etapa2 main.o parser.tab.o lex.yy.o -lfl

run: $(EXEC)
	./$(EXEC)

clean:
	rm -f $(EXEC) $(LEX) parser.tab.c parser.tab.h
	rm -f *.o
