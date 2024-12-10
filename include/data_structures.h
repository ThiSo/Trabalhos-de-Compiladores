#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H


typedef struct asd_tree {
  char *tipo;       // Int, float ou atribuir depois
  char *label;
  char *codigo;
  char *local;
  char *t;
  char *f;
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
    int deslocamento;
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


typedef struct {
    char *codigo;
    char *temporario;
  } ret_instr_t;

// ----------------------------------------------------------
//                      Funções pilha 
// ----------------------------------------------------------

ret_instr_t gera_codigo_expressao(valor_lexico_t *identificador, pilha_tabelas_t *pilha_tabelas);
ret_instr_t gera_codigo_atribuicao(valor_lexico_t *identificador, asd_tree_t *f1, pilha_tabelas_t *pilha_tabelas);
ret_instr_t gera_codigo_exp_un(char* mneumonico, asd_tree_t *f1);
ret_instr_t gera_codigo_exp_bin(char* mneumonico, asd_tree_t *f1, asd_tree_t *f2);
ret_instr_t gera_codigo_if(asd_tree_t *f1, asd_tree_t *f2);
ret_instr_t gera_codigo_while(asd_tree_t *f1, asd_tree_t *f2);
ret_instr_t gera_codigo_if_else(asd_tree_t *f1, asd_tree_t *f2, asd_tree_t *f3);

/*
 * Função cria_pilha, cria uma pilha vazia.
 */
pilha_tabelas_t* cria_pilha(tabela_simbolos_t *tabela_simbolos);

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
 * Função buscar_entrada, busca uma entrada na pilha de tabelas de símbolos.
 */
conteudo_tabela_simbolos_t* busca_entrada_pilha(pilha_tabelas_t *pilha, const char *valor);

/*
 * Função printa_pilha_tabelas, DEBUG.
 */
void printa_pilha_tabelas(pilha_tabelas_t *pilha);


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
 * Função atribui_tipo, percorre tabela buscando entradas sem tipo e atribui o tipo correspondente.
 */
void atribui_tipo (tabela_simbolos_t *tabela, char *tipo);


/*
 * Função adiciona_entrada, adiciona um símbolo à lista.
 */
void adiciona_entrada(tabela_simbolos_t *tabela, conteudo_tabela_simbolos_t *entrada);

/*
 * Função calcula_deslocamentos, calcula deslocamentos das entradas da tabela que não puderam ser calculados em algum momento.
 */
void calcula_deslocamentos(tabela_simbolos_t *tabela);


/*
 * Função destroi_tabela_simbolos, libera a memória alocada para a lista.
 */
void destroi_tabela_simbolos(tabela_simbolos_t *tabela);


/*
*  Função infere_tipo, infere o tipo de uma operação.
*/
char* infere_tipo(const char *tipo_op_1, const char *tipo_op_2);

/*
*  Função printa_erro, imprime o erro semntico.
*/
void printa_erro(int erro, const char *valor, int linha, int linha2);

/*
 * Função corrige_ordem_filhos, necessária para corrigir erro na etapa 3.
 */
asd_tree_t* corrige_ordem_filhos(asd_tree_t *arvore);

asd_tree_t *cria_tipo(const char *tipo);

// ----------------------------------------------------------
//                      Funções etapa 5 
// ----------------------------------------------------------

char* gera_temp();

char* gera_label();

char* cria_instrucao(char* instrucao, char* parametro1, char* parametro2, char* parametro3);

char* concatena3(char* parametro1, char* parametro2, char* instr);

char* concatena2(char* parametro1, char* instr);

// ----------------------------------------------------------
//                      Funções etapa 3 
// ----------------------------------------------------------

/*
 * Função cria_valor_lexico, cria um valor léxico.
 */
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

asd_tree_t* cria_nodo_expressao(char *expressao, asd_tree_t *filho_1, asd_tree_t *filho_2);

asd_tree_t* cria_nodo_expressao_unaria(char *expressao, asd_tree_t *filho_1);

asd_tree_t* processa_chamada_funcao(valor_lexico_t *identificador, asd_tree_t *filho_2, pilha_tabelas_t *pilha_tabelas);

asd_tree_t* processa_atribuicao(valor_lexico_t *identificador, asd_tree_t *filho_2, pilha_tabelas_t *pilha_tabelas);

asd_tree_t* processa_expressao(valor_lexico_t *identificador, pilha_tabelas_t *pilha_tabelas);

#endif // DATA_STRUCTURES_H

