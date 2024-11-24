#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/data_structures.h"
#define ARQUIVO_SAIDA "saida.dot"

/*
 * Função cria_pilha, cria uma pilha vazia.
 */
pilha_tabelas_t* cria_pilha() {
    pilha_tabelas_t *nova_pilha = (pilha_tabelas_t*)malloc(sizeof(pilha_tabelas_t));
    nova_pilha->tabela_simbolos = NULL;
    nova_pilha->prox = NULL;
    return nova_pilha;
}

/*
 * Função destroi_pilha, libera a memória alocada para a pilha.
 */
void destroi_pilha(pilha_tabelas_t *pilha) {
    while (pilha != NULL) {
        pilha_tabelas_t *temp = pilha;
        pilha = pilha->prox;
        destroi_lista_tabela_simbolos(temp->tabela_simbolos);
        free(temp);
    }
}

/*
 * Função empilhar, adiciona uma nova tabela de símbolos no topo da pilha.
 */
void empilhar(pilha_tabelas_t **pilha, lista_tabela_simbolos_t *tabela_simbolos) {
    pilha_tabelas_t *nova_pilha = (pilha_tabelas_t*)malloc(sizeof(pilha_tabelas_t));
    nova_pilha->tabela_simbolos = tabela_simbolos;
    nova_pilha->prox = *pilha;
    *pilha = nova_pilha;
}

/*
 * Função desempilhar, remove a tabela de símbolos do topo da pilha.
 */
void desempilhar(pilha_tabelas_t **pilha) {
  if (*pilha == NULL) {
    return;
  }
  pilha_tabelas_t *temp = *pilha;
  destroi_lista_tabela_simbolos(temp->tabela_simbolos);
  *pilha = (*pilha)->prox;
  free(temp);
}

/*
 * Função buscar_entrada, busca uma entrada na pilha de tabelas de símbolos.
 */
conteudo_tabela_simbolos_t* buscar_entrada(pilha_tabelas_t *pilha, const char *valor) {
    while (pilha != NULL) {
        nodo_tabela_simbolos_t *nodo = pilha->tabela_simbolos->cabeca;
        while (nodo != NULL) {
            if (strcmp(nodo->entrada.valor, valor) == 0) {
                return &nodo->entrada;
            }
            nodo = nodo->prox;
        }
        pilha = pilha->prox;
    }
    return NULL;
}

/*
 * Função print_pilha, imprime todas as tabelas de símbolos na pilha.
 */
void print_pilha(pilha_tabelas_t *pilha) {
    while (pilha != NULL) {
        print_lista_tabela_simbolos(pilha->tabela_simbolos);
        pilha = pilha->prox;
    }
}

lista_tabela_simbolos_t* cria_lista_tabela_simbolos() {
  lista_tabela_simbolos_t *lista = (lista_tabela_simbolos_t*)malloc(sizeof(lista_tabela_simbolos_t));
  lista->cabeca = NULL;
  return lista;
}


conteudo_tabela_simbolos_t cria_entrada(int linha, const char *natureza, const char *tipo, const char *valor) {
  conteudo_tabela_simbolos_t entrada;
  entrada.linha = linha;
  entrada.natureza = strdup(natureza);
  entrada.tipo = strdup(tipo);
  entrada.valor = strdup(valor);
  return entrada;
}


void adiciona_entrada(lista_tabela_simbolos_t *lista, conteudo_tabela_simbolos_t entrada) {
  nodo_tabela_simbolos_t *novo_nodo = (nodo_tabela_simbolos_t*)malloc(sizeof(nodo_tabela_simbolos_t));
  novo_nodo->entrada = entrada;
  novo_nodo->prox = lista->cabeca;
  lista->cabeca = novo_nodo;
}


void print_lista_tabela_simbolos(lista_tabela_simbolos_t *lista) {
  nodo_tabela_simbolos_t *atual = lista->cabeca;
  while (atual != NULL) {
    printf("Linha: %d, Natureza: %s, Tipo: %s, Valor: %s\n", atual->entrada.linha, atual->entrada.natureza, atual->entrada.tipo, atual->entrada.valor);
    atual = atual->prox;
  }
}

void destroi_lista_tabela_simbolos(lista_tabela_simbolos_t *lista) {
  nodo_tabela_simbolos_t *atual = lista->cabeca;
  nodo_tabela_simbolos_t *prox;
  while (atual != NULL) {
    prox = atual->prox;
    free(atual);
    atual = prox;
  }
  free(lista);
}

valor_lexico_t* cria_valor_lexico(int linha, const char* tipo_token, const char* valor) {
	valor_lexico_t *valor_lexico = (valor_lexico_t *)malloc(sizeof(valor_lexico_t));
	if (valor_lexico == NULL) {
		fprintf(stderr, "Erro ao alocar memória para valor_lexico\n");
		exit(1);
	}
	valor_lexico->linha = linha;
	valor_lexico->tipo_token = strdup(tipo_token);
	valor_lexico->valor = strdup(valor);
	return valor_lexico;
}

asd_tree_t *asd_new(const char *label)
{
  asd_tree_t *ret = NULL;
  ret = calloc(1, sizeof(asd_tree_t));
  if (ret != NULL){
    ret->label = strdup(label);
    ret->number_of_children = 0;
    ret->children = NULL;
  }
  return ret;
}

void asd_free(asd_tree_t *tree)
{
  if (tree != NULL){
    int i;
    for (i = 0; i < tree->number_of_children; i++){
      asd_free(tree->children[i]);
    }
    free(tree->children);
    free(tree->label);
    free(tree);
  }else{
    printf("Erro: %s recebeu parâmetro tree = %p.\n", __FUNCTION__, tree);
  }
}

void asd_add_child(asd_tree_t *tree, asd_tree_t *child)
{
  if (tree != NULL && child != NULL){
    tree->number_of_children++;
    tree->children = realloc(tree->children, tree->number_of_children * sizeof(asd_tree_t*));
    tree->children[tree->number_of_children-1] = child;
  }else{
    printf("Erro: %s recebeu parâmetro tree = %p / %p.\n", __FUNCTION__, tree, child);
  }
}

static void _asd_print (FILE *foutput, asd_tree_t *tree, int profundidade)
{
  int i;
  if (tree != NULL){
    fprintf(foutput, "%d%*s: Nó '%s' tem %d filhos:\n", profundidade, profundidade*2, "", tree->label, tree->number_of_children);
    for (i = 0; i < tree->number_of_children; i++){
      _asd_print(foutput, tree->children[i], profundidade+1);
    }
  }else{
    printf("Erro: %s recebeu parâmetro tree = %p.\n", __FUNCTION__, tree);
  }
}

void asd_print(asd_tree_t *tree)
{
  FILE *foutput = stderr;
  if (tree != NULL){
    _asd_print(foutput, tree, 0);
  }else{
    printf("Erro: %s recebeu parâmetro tree = %p.\n", __FUNCTION__, tree);
  }
}

static void _asd_print_graphviz (FILE *foutput, asd_tree_t *tree)
{
  int i;
  if (tree != NULL){
    fprintf(foutput, "  %ld [ label=\"%s\" ];\n", (long)tree, tree->label);
    for (i = 0; i < tree->number_of_children; i++){
      fprintf(foutput, "  %ld -> %ld;\n", (long)tree, (long)tree->children[i]);
      _asd_print_graphviz(foutput, tree->children[i]);
    }
  }else{
    printf("Erro: %s recebeu parâmetro tree = %p.\n", __FUNCTION__, tree);
  }
}

void asd_print_graphviz(asd_tree_t *tree)
{
  FILE *foutput = fopen(ARQUIVO_SAIDA, "w+");
  if(foutput == NULL){
    printf("Erro: %s não pude abrir o arquivo [%s] para escrita.\n", __FUNCTION__, ARQUIVO_SAIDA);
  }
  if (tree != NULL){
    fprintf(foutput, "digraph grafo {\n");
    _asd_print_graphviz(foutput, tree);
    fprintf(foutput, "}\n");
    fclose(foutput);
  }else{
    printf("Erro: %s recebeu parâmetro tree = %p.\n", __FUNCTION__, tree);
  }
}