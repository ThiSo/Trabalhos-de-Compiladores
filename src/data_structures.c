#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/data_structures.h"
#include "../include/erros.h"
#define ARQUIVO_SAIDA "saida.dot"


// -------------------------------------------
//            Funções Pilha
// -------------------------------------------

/*
 * Função cria_pilha, cria uma pilha vazia.
 */
pilha_tabelas_t* cria_pilha(tabela_simbolos_t *tabela_simbolos) {

    pilha_tabelas_t *ret = NULL;
    ret = (pilha_tabelas_t *) malloc(sizeof(pilha_tabelas_t));
    if (ret != NULL) {
        ret->tabela_simbolos = tabela_simbolos;
        ret->prox = NULL;
    } else {
        printf("Erro: %s não conseguiu alocar memória.\n", __FUNCTION__);
    }

    return ret;
}

/*
 * Função destroi_pilha, libera a memória alocada para a pilha.
 */
void destroi_pilha(pilha_tabelas_t *pilha) {
  while (pilha != NULL) {
    pilha_tabelas_t *temp = pilha;
    pilha = pilha->prox;
    if (temp->tabela_simbolos != NULL) {
      destroi_tabela_simbolos(temp->tabela_simbolos);
    }
    free(temp);
  }
}


/*
 * Função empilhar, adiciona uma nova tabela de símbolos no topo da pilha.
 */
void empilhar(pilha_tabelas_t **pilha, tabela_simbolos_t *tabela_simbolos) {
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
  destroi_tabela_simbolos(temp->tabela_simbolos);
  *pilha = (*pilha)->prox;
  free(temp);
}

/*
 * Função busca_entrada_pilha, busca uma entrada na pilha de tabelas de símbolos.
 */
conteudo_tabela_simbolos_t *busca_entrada_pilha(pilha_tabelas_t *pilha, const char *valor)
{ 
  while (pilha != NULL) {
    conteudo_tabela_simbolos_t *entrada = busca_entrada_tabela(pilha->tabela_simbolos, valor);
    if (entrada != NULL) {
      return entrada;
    }
    pilha = pilha->prox;
  }
  return NULL;
}

/*
 * Função printa_pilha, imprime todas as tabelas de símbolos na pilha.
 */
void printa_pilha(pilha_tabelas_t *pilha) {
    while (pilha != NULL) {
        printa_tabela_simbolos(pilha->tabela_simbolos);
        pilha = pilha->prox;
    }
}

// -------------------------------------------
//            Funções Tabela de símbolos
// -------------------------------------------

tabela_simbolos_t* cria_tabela_simbolos() {
  tabela_simbolos_t *ret = (tabela_simbolos_t *)calloc(1, sizeof(tabela_simbolos_t));
  if (ret == NULL) {
    fprintf(stderr, "Erro: %s não conseguiu alocar memória.\n", __FUNCTION__);
    return NULL;
  }
  ret->numero_de_entradas = 0;
  ret->entradas = NULL;
  return ret;
}


conteudo_tabela_simbolos_t* cria_entrada(int linha, const char *natureza, const char *tipo, const char *valor) {
  conteudo_tabela_simbolos_t *ret = (conteudo_tabela_simbolos_t *)calloc(1, sizeof(conteudo_tabela_simbolos_t));
  if (ret != NULL) {
    ret->linha = linha;
    ret->natureza = strdup(natureza);
    ret->tipo = strdup(tipo);
    ret->valor = strdup(valor);
  } else {
    fprintf(stderr, "Erro: %s não conseguiu alocar memória.\n", __FUNCTION__);
  }
  return ret;
}


conteudo_tabela_simbolos_t *busca_entrada_tabela(tabela_simbolos_t *tabela, const char *valor)
{
  if (tabela == NULL) {
    printf("Erro: %s recebeu parâmetro tabela = %p.\n", __FUNCTION__, tabela);
    return NULL;
  }

  for (int i = 0; i < tabela->numero_de_entradas; i++) {
    if (strcmp(tabela->entradas[i]->valor, valor) == 0) {
      return tabela->entradas[i];
    }
  }

  return NULL;
}


void atribui_tipo (tabela_simbolos_t *tabela, char *tipo) {
	if (tabela != NULL) {
		for (int i = 0; i < tabela->numero_de_entradas; i++) {
			if (strcmp(tabela->entradas[i]->tipo, "ATRIBUIR DEPOIS") == 0) {
			      	tabela->entradas[i]->tipo = tipo;
			}
		}
	}
}


void adiciona_entrada(tabela_simbolos_t *tabela, conteudo_tabela_simbolos_t *entrada) {
  if (tabela != NULL && entrada != NULL) {
        conteudo_tabela_simbolos_t *ret = busca_entrada_tabela(tabela, entrada->valor);
        if (ret != NULL) {
            printa_erro(ERR_DECLARED, entrada->valor, entrada->linha, ret->linha);
        }
        tabela->numero_de_entradas++;
        tabela->entradas = realloc(tabela->entradas, tabela->numero_de_entradas * sizeof(conteudo_tabela_simbolos_t *));   
        tabela->entradas[tabela->numero_de_entradas-1] = entrada;
    } else {
        printf("Erro: %s recebeu parâmetro tabela = %p / entrada = %p.\n", __FUNCTION__, tabela, entrada);
    }
}



void destroi_tabela_simbolos(tabela_simbolos_t *tabela)
{
  if (tabela == NULL) {
    printf("Erro: %s recebeu parâmetro tabela = %p.\n", __FUNCTION__, tabela);
    return;
  }
  free(tabela->entradas);
  free(tabela);
}


void printa_tabela_simbolos(tabela_simbolos_t *tabela)
{
    if (tabela != NULL) {
        for (int i = 0; i < tabela->numero_de_entradas; i++) {
            char *tipo;
            if (strcmp(tabela->entradas[i]->tipo, "int") == 0) {
                tipo = "INT";
            } else if (strcmp(tabela->entradas[i]->tipo, "float") == 0) {
                tipo = "FLOAT";
            } else if(strcmp(tabela->entradas[i]->tipo, "ATRIBUIR DEPOIS") == 0){
                tipo = "ATRIBUIR DEPOIS";
            } else {
                tipo = "ERRO";
            }
            char *natureza = strcmp(tabela->entradas[i]->natureza, "FUNCAO") == 0 ? "FUNCAO" : "IDENTIFICADOR";
            printf("Linha: %3d\tValor: %s\tTipo: %s\tNatureza: %s\n", tabela->entradas[i]->linha, tabela->entradas[i]->valor, tipo, natureza);
        }
    } else {
        printf("Erro: %s recebeu parâmetro tabela = %p.\n", __FUNCTION__, tabela);
    }
}


char* infere_tipo(const char *tipo_op_1, const char *tipo_op_2){
    // printf("tipo_op_1: %s, tipo_op_2: %s\n", tipo_op_1, tipo_op_2);
    if (strcmp(tipo_op_1, "INT") == 0 && strcmp(tipo_op_2, "INT") == 0) {
        return "INT";
    } else if ((strcmp(tipo_op_1, "INT") == 0 && strcmp(tipo_op_2, "FLOAT") == 0) || (strcmp(tipo_op_1, "FLOAT") == 0 && strcmp(tipo_op_2, "INT") == 0)) {
        return "FLOAT";
    } else if (strcmp(tipo_op_1, "FLOAT") == 0 && strcmp(tipo_op_2, "FLOAT") == 0) {
        return "FLOAT";
    } else {
        exit(1);
    }
}


void printa_erro(int erro, const char *valor, int linha, int linha2) {
    switch (erro) {
        case ERR_UNDECLARED:
            fprintf(stderr, "Erro semântico na linha %d: identificador %s não declarado\n", linha, valor);
            exit(ERR_UNDECLARED);
            break;
        case ERR_DECLARED:
            fprintf(stderr, "Erro semântico na linha %d: identificador %s já declarado anteriormente na linha %d\n", linha, valor, linha2);
            exit(ERR_DECLARED);
            break;
        case ERR_VARIABLE:
            fprintf(stderr, "Erro semântico na linha %d: identificador %s do tipo VARIAVEL sendo usado como %s\n", linha, valor, "FUNCAO");
            exit(ERR_VARIABLE);
            break;
        case ERR_FUNCTION:
            fprintf(stderr, "Erro semântico na linha %d: identificador %s do tipo FUNCAO sendo usado como %s\n", linha, valor, "VARIAVEL");
            exit(ERR_FUNCTION);
            break;
        default:
            fprintf(stderr, "Erro semântico na linha %d: erro desconhecido\n", linha);
            exit(1);
            break;
    }
}

asd_tree_t* corrige_ordem_filhos(asd_tree_t *tree, int minimo_filhos)
{
  if (tree != NULL) {
    asd_tree_t *node = tree;
    while (node->number_of_children > minimo_filhos) {
      node = node->children[node->number_of_children - 1];
    }
    return node;
  }
  else return NULL;
}


asd_tree_t *cria_tipo(const char *tipo)
{
  asd_tree_t *ret = calloc(1, sizeof(asd_tree_t));
  if (ret == NULL) {
    fprintf(stderr, "Erro: %s não conseguiu alocar memória.\n", __FUNCTION__);
    return NULL;
  }
  ret->tipo = strdup(tipo);
  return ret;
}


// -------------------------------------------
//            Funções etapa 3
// -------------------------------------------


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
    ret->tipo = NULL;
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