#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data_structures.h"
#include "erros.h"
#define ARQUIVO_SAIDA "saida.dot"
int contador_global = 0;


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
 * Função printa_pilha_tabelas, DEBUG.
 */
void printa_pilha_tabelas(pilha_tabelas_t *pilha) {
    if (pilha == NULL) {
        printf("A pilha está vazia.\n");
        return;
    }

    int nivel = 0; // Para indicar o nível da tabela na pilha
    while (pilha != NULL) {
        printf("Tabela no nível %d:\n", nivel);
        tabela_simbolos_t *tabela = pilha->tabela_simbolos;

        if (tabela == NULL || tabela->entradas == NULL || tabela->numero_de_entradas == 0) {
            printf("  (Tabela vazia)\n");
        } else {
            for (int i = 0; i < tabela->numero_de_entradas; i++) {
                conteudo_tabela_simbolos_t *entrada = tabela->entradas[i];
                if (entrada != NULL) {
                    printf("  Entrada %d:\n", i);
                    printf("    Linha: %d\n", entrada->linha);
                    printf("    Deslocamento: %d\n", entrada->deslocamento);
                    printf("    Natureza: %s\n", entrada->natureza);
                    printf("    Tipo: %s\n", entrada->tipo);
                    printf("    Valor: %s\n", entrada->valor);
                } else {
                    printf("  Entrada %d: (nula)\n", i);
                }
            }
        }

        // Passa para o próximo nível da pilha
        pilha = pilha->prox;
        nivel++;
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
            return;
        }

        if (strcmp(entrada->tipo, "ATRIBUIR DEPOIS") == 0) {
            entrada->deslocamento = -1; 
        } else {
            if (tabela->numero_de_entradas == 0) {
                entrada->deslocamento = 0; 
            } else {
                conteudo_tabela_simbolos_t *ultima_entrada = tabela->entradas[tabela->numero_de_entradas - 1];
                if (ultima_entrada->deslocamento == -1) {
                    entrada->deslocamento = -1; 
                } else if (strcmp(ultima_entrada->tipo, "INT") == 0) {
                    entrada->deslocamento = ultima_entrada->deslocamento + 4;
                } else if (strcmp(ultima_entrada->tipo, "FLOAT") == 0) {
                    entrada->deslocamento = ultima_entrada->deslocamento + 6;
                } else {
                    fprintf(stderr, "Erro: tipo desconhecido '%s'.\n", ultima_entrada->tipo);
                    entrada->deslocamento = -1; 
                }
            }
        }
        
        tabela->numero_de_entradas++;
        tabela->entradas = realloc(tabela->entradas, tabela->numero_de_entradas * sizeof(conteudo_tabela_simbolos_t *));
        if (tabela->entradas == NULL) {
            fprintf(stderr, "Erro: falha ao realocar memória para as entradas da tabela.\n");
            return;
        }
        tabela->entradas[tabela->numero_de_entradas - 1] = entrada;

        if (entrada->deslocamento == -1) {
            for (int i = 0; i < tabela->numero_de_entradas; i++) {
                tabela->entradas[i]->deslocamento = -1;
            }
        }
    } else {
        fprintf(stderr, "Erro: %s recebeu parâmetros inválidos. tabela = %p, entrada = %p\n", __FUNCTION__, tabela, entrada);
    }
}

void calcula_deslocamentos(tabela_simbolos_t *tabela) {
    if (tabela == NULL || tabela->entradas == NULL) {
        fprintf(stderr, "Erro: tabela inválida em %s.\n", __FUNCTION__);
        return;
    }

    int deslocamento_atual = 0;

    for (int i = 0; i < tabela->numero_de_entradas; i++) {
        conteudo_tabela_simbolos_t *entrada = tabela->entradas[i];

        if (entrada->deslocamento != -1) {
            deslocamento_atual = entrada->deslocamento + 
                                 (strcmp(entrada->tipo, "INT") == 0 ? 4 : 6);
            continue;
        }

        if (strcmp(entrada->tipo, "INT") == 0) {
            entrada->deslocamento = deslocamento_atual;
            deslocamento_atual += 4;
        } else if (strcmp(entrada->tipo, "FLOAT") == 0) {
            entrada->deslocamento = deslocamento_atual;
            deslocamento_atual += 6;
        } else if (strcmp(entrada->tipo, "ATRIBUIR DEPOIS") == 0) {
            fprintf(stderr, "Aviso: tipo ainda não definido para '%s' na linha %d. Deslocamento permanecerá indefinido.\n",
                    entrada->valor, entrada->linha);
            entrada->deslocamento = -1;
            return; 
        } else {
            fprintf(stderr, "Erro: tipo desconhecido '%s' para '%s'.\n", entrada->tipo, entrada->valor);
            entrada->deslocamento = -1;
            return; 
        }
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

asd_tree_t* corrige_ordem_filhos(asd_tree_t *arvore)
{
  if (arvore != NULL) {
    asd_tree_t *nodo_a_corrigir = arvore;
    while (nodo_a_corrigir->number_of_children > 2) {
      nodo_a_corrigir = nodo_a_corrigir->children[nodo_a_corrigir->number_of_children - 1];
    }
    return nodo_a_corrigir;
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
//            Funções etapa 5
// -------------------------------------------

char* gera_temp() {
    char buffer[20]; 
    sprintf(buffer, "%d", contador_global);

    size_t tamanho = strlen(buffer) + strlen("t") + 1;

    char* resultado = malloc(tamanho);
    if (resultado == NULL) {
        fprintf(stderr, "Erro de alocação de memória.\n");
        exit(EXIT_FAILURE);
    }

    strcpy(resultado, "t");
    strcat(resultado, buffer);
    
    contador_global++;

    return resultado;
}

char* cria_instrucao(char* instrucao, char* parametro1, char* parametro2, char* parametro3) {
	size_t tamanho = strlen(instrucao);
	    
	if (parametro1 != NULL) tamanho += strlen(parametro1);
	if (parametro2 != NULL) tamanho += strlen(parametro2);
	if (parametro3 != NULL) tamanho += strlen(parametro3);

	tamanho += 5; 
	tamanho += 1;  
	char* resultado = malloc(tamanho);
	
	if (resultado == NULL) {
		fprintf(stderr, "Erro de alocação de memória.\n");
		exit(EXIT_FAILURE);
    	}
    	
    	if(parametro2 == NULL) {
    		strcpy(resultado, instrucao);
    		strcat(resultado, "  ");
    		strcat(resultado, parametro1);
    		strcat(resultado, " => ");
    		strcat(resultado, parametro3);
    		strcat(resultado, "\n");
    	}
    	
    	else if(parametro1 == NULL) {
    		strcpy(resultado, instrucao);
    		strcat(resultado, "  ");
    		strcat(resultado, parametro2);
    		strcat(resultado, " => ");
    		strcat(resultado, parametro3);
    		strcat(resultado, "\n");
    	}
    	
    	// adicionar condicionais para gerar os outros formatos de instruções
    	else {
    		if(strcmp(instrucao, "storeAI") == 0 || strcmp(instrucao, "storeA0") == 0 || strcmp(instrucao, "cstoreAI") == 0 || strcmp(instrucao, "cstoreA0") == 0) {
    			strcpy(resultado, instrucao);
	    		strcat(resultado, "  ");
	    		strcat(resultado, parametro1);
	    		strcat(resultado, " => ");
	    		strcat(resultado, parametro2);
	    		strcat(resultado, ", ");
	    		strcat(resultado, parametro3);
	    		strcat(resultado, "\n");
    		}
    		else {
    			strcpy(resultado, instrucao);
	    		strcat(resultado, "  ");
	    		strcat(resultado, parametro1);
	    		strcat(resultado, ", ");
	    		strcat(resultado, parametro2);
	    		strcat(resultado, " => ");
	    		strcat(resultado, parametro3);
	    		strcat(resultado, "\n");
    		}
    	}
    	
    	return resultado;
}

char* concatena3(char* parametro1, char* parametro2, char* instr) {
    // Verificar se algum parâmetro é NULL
    if (parametro1 == NULL || parametro2 == NULL || instr == NULL) {
        fprintf(stderr, "Erro: parâmetros inválidos.\n");
        return NULL;
    }

    // Calcular o tamanho total necessário para armazenar as três strings mais o '\0'
    size_t tamanho = strlen(parametro1) + strlen(parametro2) + strlen(instr) + 5;

    // Alocar memória para o resultado
    char* resultado = malloc(tamanho);
    if (resultado == NULL) {
        fprintf(stderr, "Erro de alocação de memória.\n");
        exit(EXIT_FAILURE);
    }

    // Copiar as strings para o resultado
    strcpy(resultado, parametro1);
    strcat(resultado, parametro2);
    strcat(resultado, instr);

    return resultado;
}

char* concatena2(char* parametro1, char* instr) {
    // Verificar se algum parâmetro é NULL
    if (parametro1 == NULL || instr == NULL) {
        fprintf(stderr, "Erro: parâmetros inválidos.\n");
        return NULL;
    }

    // Calcular o tamanho total necessário para armazenar as três strings mais o '\0'
    size_t tamanho = strlen(parametro1) + strlen(instr) + 5;

    // Alocar memória para o resultado
    char* resultado = malloc(tamanho);
    if (resultado == NULL) {
        fprintf(stderr, "Erro de alocação de memória.\n");
        exit(EXIT_FAILURE);
    }

    // Copiar as strings para o resultado
    strcpy(resultado, parametro1);
    strcat(resultado, instr);

    return resultado;
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
    ret->codigo = NULL;
    ret->local = NULL;
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
    free(tree->codigo);
    free(tree->local);
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
