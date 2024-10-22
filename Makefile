all: etapa2

etapa2: obj/main.o obj/parser.tab.o obj/lex.yy.o
	gcc -o etapa2 obj/main.o obj/parser.tab.o obj/lex.yy.o -lfl

obj/main.o: src/main.c | src/parser.tab.h
	gcc -c src/main.c -o obj/main.o

obj/parser.tab.o: src/parser.tab.c
	gcc -c src/parser.tab.c -o obj/parser.tab.o

obj/lex.yy.o: src/lex.yy.c
	gcc -c src/lex.yy.c -o obj/lex.yy.o

src/parser.tab.c src/parser.tab.h: src/parser.y
	bison -d src/parser.y -o src/parser.tab.c

src/lex.yy.c: src/scanner.l
	flex -o src/lex.yy.c src/scanner.l

clean:
	rm -f obj/*.o src/parser.tab.c src/parser.tab.h src/lex.yy.c etapa2

.PHONY: all clean
