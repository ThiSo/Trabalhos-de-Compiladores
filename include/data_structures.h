#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

typedef struct asd_tree {
  char *label;
  int number_of_children;
  struct asd_tree **children;
} asd_tree_t;

typedef struct valor_lexico {
    int linha;
    char* tipo_token;
    char* valor;
} valor_lexico_t;

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