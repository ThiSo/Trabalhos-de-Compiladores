SOURCES = src/main.c 
LEX_SOURCE = flex/scanner.l
LEX = lex.yy.c
EXEC = etapa1

all: $(EXEC)

lex.yy.c: $(LEX_SOURCE)
	flex $(LEX_SOURCE)

$(EXEC): $(LEX) $(SOURCES)
	gcc $(SOURCES) $(LEX) -o $(EXEC) -lfl

run: $(EXEC)
	./$(EXEC)

clean:
	rm -f $(EXEC) $(LEX)
