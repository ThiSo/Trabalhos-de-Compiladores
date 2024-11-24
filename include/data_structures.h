#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H


typedef struct asd_tree {
  char *tipo;       // Int ou float
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


typedef struct nodo_tabela_simbolos {
    conteudo_tabela_simbolos_t entrada;
    int num_entradas;
    struct nodo_tabela_simbolos *prox;
} nodo_tabela_simbolos_t;


typedef struct lista_tabela_simbolos {
    nodo_tabela_simbolos_t *cabeca;
} lista_tabela_simbolos_t;


typedef struct pilha {
    lista_tabela_simbolos_t *tabela_simbolos;
    struct pilha *prox;
} pilha_tabelas_t;


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
void empilhar(pilha_tabelas_t **pilha, lista_tabela_simbolos_t *tabela_simbolos);

/*
 * Função desempilhar, desempilha uma tabela de símbolos.
 */
void desempilhar(pilha_tabelas_t **pilha);

/*
 * Função buscar_entrada, busca uma entrada na pilha de tabelas de símbolos.
 */
conteudo_tabela_simbolos_t* buscar_entrada(pilha_tabelas_t *pilha, const char *valor);

/*
 * Função print_pilha, imprime todas as tabelas de símbolos na pilha.
 */
void print_pilha(pilha_tabelas_t *pilha);

/*
 * Função cria_lista_tabela_simbolos, cria uma lista vazia.
 */
lista_tabela_simbolos_t* cria_lista_tabela_simbolos();

/*
 * Função cria_entrada, cria uma entrada na tabela de símbolos.
 */
conteudo_tabela_simbolos_t cria_entrada(int linha, const char *natureza, const char *tipo, const char *valor);

/*
 * Função destroi_lista_tabela_simbolos, libera a memória alocada para a lista.
 */
void destroi_lista_tabela_simbolos(lista_tabela_simbolos_t *lista);

/*
 * Função adiciona_simbolo, adiciona um símbolo à lista.
 */
void adiciona_entrada(lista_tabela_simbolos_t *lista, conteudo_tabela_simbolos_t simbolo);

/*
 * Função print_lista_tabela_simbolos, imprime a lista de símbolos.
 */
void print_lista_tabela_simbolos(lista_tabela_simbolos_t *lista);

valor_lexico_t* cria_valor_lexico(int linha, const char* tipo_token, const char* valor);

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