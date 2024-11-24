SRC_DIR = src
OBJ_DIR = obj
INC_DIR = include

SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

DEPS = $(OBJ_DIR)/main.o $(OBJ_DIR)/parser.tab.o $(OBJ_DIR)/lex.yy.o $(OBJ_DIR)/data_structures.o $(OBJ_DIR)/aux.o

all: etapa4

etapa4: $(DEPS)
	gcc -o etapa4 $(DEPS) -lfl

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR) $(SRC_DIR)/parser.tab.h
	gcc -c $< -o $@ -I$(INC_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Gera o arquivo parser.tab.c e parser.tab.h 
$(SRC_DIR)/parser.tab.c $(SRC_DIR)/parser.tab.h: $(SRC_DIR)/parser.y
	bison -d $(SRC_DIR)/parser.y -o $(SRC_DIR)/parser.tab.c

# Gera o arquivo lex.yy.c 
$(SRC_DIR)/lex.yy.c: $(SRC_DIR)/scanner.l
	flex -o $(SRC_DIR)/lex.yy.c $(SRC_DIR)/scanner.l

clean:
	rm -rf $(OBJ_DIR) $(SRC_DIR)/parser.tab.c $(SRC_DIR)/parser.tab.h $(SRC_DIR)/lex.yy.c etapa4 saida.dot graph.png

.PHONY: all clean