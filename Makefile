# Define as variáveis de diretórios
SRC_DIR = src
OBJ_DIR = obj
INC_DIR = include

# Define os arquivos .c que precisam ser compilados
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Define as dependências específicas
DEPS = $(OBJ_DIR)/main.o $(OBJ_DIR)/parser.tab.o $(OBJ_DIR)/lex.yy.o $(OBJ_DIR)/data_structures.o $(OBJ_DIR)/aux.o

# Alvo principal
all: etapa3

# Alvo para compilar o programa final
etapa3: $(DEPS)
	gcc -o etapa3 $(DEPS) -lfl

# Regra genérica para compilar todos os arquivos .o
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR) $(SRC_DIR)/parser.tab.h
	gcc -c $< -o $@ -I$(INC_DIR)

# Regra para gerar o diretório obj caso ele não exista
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Regras para gerar o arquivo parser.tab.c e parser.tab.h com bison
$(SRC_DIR)/parser.tab.c $(SRC_DIR)/parser.tab.h: $(SRC_DIR)/parser.y
	bison -d $(SRC_DIR)/parser.y -o $(SRC_DIR)/parser.tab.c

# Regras para gerar o arquivo lex.yy.c com flex
$(SRC_DIR)/lex.yy.c: $(SRC_DIR)/scanner.l
	flex -o $(SRC_DIR)/lex.yy.c $(SRC_DIR)/scanner.l

# Limpeza dos arquivos gerados
clean:
	rm -rf $(OBJ_DIR) $(SRC_DIR)/parser.tab.c $(SRC_DIR)/parser.tab.h $(SRC_DIR)/lex.yy.c etapa3 saida.dot graph.png

.PHONY: all clean