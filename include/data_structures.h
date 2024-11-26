#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H


typedef struct asd_tree {
  char *tipo;       // Int, float ou atribuir depois
  char *label;
  int number_of_children;
  struct asd_tree **children;
} asd_tree_t;


typedef struct valor_lexico {
    int linha;
    char* tipo_token;
    char* valor;
} valor_lexico_t;


typedef struct conteudo_tabela_simbolos {
    int linha;
    char* natureza; // Identificador ou função
    char* tipo;
    char* valor;
} conteudo_tabela_simbolos_t;


typedef struct tabela_simbolos {
    int numero_de_entradas;
    conteudo_tabela_simbolos_t **entradas;
} tabela_simbolos_t;


typedef struct pilha {
    tabela_simbolos_t *tabela_simbolos;
    struct pilha *prox;
} pilha_tabelas_t;


// ----------------------------------------------------------
//                      Funções pilha 
// ----------------------------------------------------------

/*
 * Função cria_pilha, cria uma pilha vazia.
 */
pilha_tabelas_t* cria_pilha();

/*
 * Função destroi_pilha, libera a memória alocada para a pilha.
 */
void destroi_pilha(pilha_tabelas_t *pilha) ;

/*
 * Função empilhar, empilha uma tabela de símbolos.
 */
void empilhar(pilha_tabelas_t **pilha, tabela_simbolos_t *tabela_simbolos);

/*
 * Função desempilhar, desempilha uma tabela de símbolos.
 */
void desempilhar(pilha_tabelas_t **pilha);

/*
 * Função printa_pilha, imprime todas as tabelas de símbolos na pilha.
 */
void printa_pilha(pilha_tabelas_t *pilha);


/*
 * Função buscar_entrada, busca uma entrada na pilha de tabelas de símbolos.
 */
conteudo_tabela_simbolos_t* busca_entrada_pilha(pilha_tabelas_t *pilha, const char *valor);


// ----------------------------------------------------------
//                      Funções tabela 
// ----------------------------------------------------------
/*
 * Função cria_lista_tabela_simbolos, cria uma lista vazia.
 */
tabela_simbolos_t* cria_tabela_simbolos();

/*
 * Função cria_entrada, cria uma entrada na tabela de símbolos.
 */
conteudo_tabela_simbolos_t *cria_entrada(int linha, const char *natureza, const char *tipo, const char *valor);


/*
 * Função busca_entrada, busca uma entrada na tabela de símbolos.
 */
conteudo_tabela_simbolos_t *busca_entrada_tabela(tabela_simbolos_t *tabela, const char *valor);


/*
 * Função adiciona_simbolo, adiciona um símbolo à lista.
 */
void adiciona_entrada(tabela_simbolos_t *tabela, conteudo_tabela_simbolos_t *entrada);


/*
 * Função destroi_tabela_simbolos, libera a memória alocada para a lista.
 */
void destroi_tabela_simbolos(tabela_simbolos_t *tabela);


/*
 * Função printa_tabela_simbolos, imprime a lista de símbolos.
 */
void printa_tabela_simbolos(tabela_simbolos_t *tabela);


// ----------------------------------------------------------
//                      Funções etapa 3 
// ----------------------------------------------------------

/*
 * Função corrige_ordem_filhos, necessária para corrigir erro na etapa 3.
 */
asd_tree_t* corrige_ordem_filhos(asd_tree_t *tree, int min_number_of_children);

/*
 * Função cria_valor_lexico, cria um valor léxico.
 */
valor_lexico_t* cria_valor_lexico(int linha, const char* tipo_token, const char* valor);


asd_tree_t *asd_tipo(const char *tipo);

/*
 * Função asd_new, cria um nó sem filhos com o label informado.
 */
asd_tree_t *asd_new(const char *label);

/*
 * Função asd_tree, libera recursivamente o nó e seus filhos.
 */
void asd_free(asd_tree_t *tree);

/*
 * Função asd_add_child, adiciona child como filho de tree.
 */
void asd_add_child(asd_tree_t *tree, asd_tree_t *child);

/*
 * Função asd_print, imprime recursivamente a árvore.
 */
void asd_print(asd_tree_t *tree);

/*
 * Função asd_print_graphviz, idem, em formato DOT
 */
void asd_print_graphviz (asd_tree_t *tree);

#endif // DATA_STRUCTURES_H