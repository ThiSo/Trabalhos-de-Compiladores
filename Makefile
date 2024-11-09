all: etapa3

etapa3: obj/main.o obj/parser.tab.o obj/lex.yy.o obj/data_structures.o obj/aux.o
	gcc -o etapa3 obj/main.o obj/parser.tab.o obj/lex.yy.o obj/data_structures.o obj/aux.o -lfl

obj/main.o: src/main.c | src/parser.tab.h include/data_structures.h include/aux.h
	gcc -c src/main.c -o obj/main.o -Iinclude

obj/parser.tab.o: src/parser.tab.c
	gcc -c src/parser.tab.c -o obj/parser.tab.o

obj/lex.yy.o: src/lex.yy.c
	gcc -c src/lex.yy.c -o obj/lex.yy.o

obj/data_structures.o: src/data_structures.c include/data_structures.h
	gcc -c src/data_structures.c -o obj/data_structures.o -Iinclude

obj/aux.o: src/aux.c include/aux.h
	gcc -c src/aux.c -o obj/aux.o -Iinclude

src/parser.tab.c src/parser.tab.h: src/parser.y
	bison -d src/parser.y -o src/parser.tab.c

src/lex.yy.c: src/scanner.l
	flex -o src/lex.yy.c src/scanner.l

clean:
	rm -f obj/*.o src/parser.tab.c src/parser.tab.h src/lex.yy.c etapa3 saida.dot graph.png

.PHONY: all clean
