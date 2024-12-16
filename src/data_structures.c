#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "data_structures.h"
#include "erros.h"
#define ARQUIVO_SAIDA "saida.dot"
int contador_temp_global = 0;
int contador_label_global = 0;


// -------------------------------------------
//            Funções etapa 5
// -------------------------------------------

ret_instr_t gera_codigo(asd_tree_t *f1, asd_tree_t *f2) {
  ret_instr_t retorno;
  
  if (f1 == NULL)
    retorno.codigo = concatena2(cria_instrucao("nop", NULL, NULL, NULL), f2->codigo);
  else if (f2 == NULL)
    retorno.codigo = f1->codigo;
  else
    retorno.codigo = concatena2(f1->codigo, f2->codigo);  // Essa linha realiza a concatenação de multiplos comandos  
    
  return retorno;
}


ret_instr_t gera_codigo_expressao(valor_lexico_t *identificador, pilha_tabelas_t *pilha_tabelas)
{
  ret_instr_t retorno;
  retorno.temporario  = gera_temp();

  if (strcmp(identificador->tipo_token, "IDENTIFICADOR") == 0) {
    char buffer[20]; 
    sprintf(buffer, "%d", busca_entrada_pilha(pilha_tabelas, identificador->valor)->deslocamento); 
    retorno.codigo = cria_instrucao("loadAI", "rfp", buffer, retorno.temporario);
  } else {
    retorno.codigo  = cria_instrucao("loadI", identificador->valor, NULL, retorno.temporario);
  }

  return retorno;
}


ret_instr_t gera_codigo_atribuicao(valor_lexico_t *identificador, asd_tree_t *f1, pilha_tabelas_t *pilha_tabelas)
{
  char buffer[20]; 
  sprintf(buffer, "%d", busca_entrada_pilha(pilha_tabelas, identificador->valor)->deslocamento);
  char* instr_store = cria_instrucao("storeAI", f1->local, "rfp", buffer);
  
  ret_instr_t retorno;
  retorno.codigo = concatena2(f1->codigo, instr_store);

  return retorno;
}


ret_instr_t gera_codigo_exp_un(char* mneumonico, asd_tree_t *f1) 
{
  if (strcmp(mneumonico, "!") == 0){
    char *zero = gera_temp();
    char *temporario = gera_temp();
    char *resultado = gera_temp();
    char *label_true = gera_label();
    char *label_false = gera_label();
    char *label_prox = gera_label();
    char *instr_loadI = cria_instrucao("loadI", "0", NULL, zero);
    char *instr_EQ = cria_instrucao("cmp_EQ", f1->local, zero, temporario);
    char *instr_cbr = cria_instrucao("cbr", temporario, label_true, label_false);
    char *atribui_zero = cria_instrucao("loadI", "0", NULL, resultado);
    char *atribui_um = cria_instrucao("loadI", "1", NULL, resultado);
    char *instr_jump = cria_instrucao("jumpI", NULL, NULL, label_prox);
    
    ret_instr_t retorno;
    retorno.codigo  = concatena2(f1->codigo, instr_loadI);
    retorno.codigo  = concatena2(retorno.codigo , instr_EQ);
    retorno.codigo  = concatena2(retorno.codigo , instr_cbr);
    retorno.codigo  = concatena3(retorno.codigo , label_true, ":");
    retorno.codigo  = concatena2(retorno.codigo , "\n");
    retorno.codigo  = concatena2(retorno.codigo , atribui_um);
    retorno.codigo  = concatena2(retorno.codigo , instr_jump);
    retorno.codigo  = concatena3(retorno.codigo , label_false, ":");
    retorno.codigo  = concatena2(retorno.codigo , "\n");
    retorno.codigo  = concatena2(retorno.codigo , atribui_zero);
    retorno.codigo  = concatena3(retorno.codigo , label_prox, ":");
    retorno.codigo  = concatena2(retorno.codigo , "\n");

    retorno.temporario = resultado;

    return retorno;
  } else {
    char *temporario_neg = gera_temp();
    char* instr_neg = cria_instrucao("multI", f1->local, "-1", temporario_neg);

    ret_instr_t retorno;
    retorno.codigo = concatena2(f1->codigo, instr_neg);

    retorno.temporario = temporario_neg;

    return retorno;
  }
}


ret_instr_t gera_codigo_exp_bin(char* mneumonico, asd_tree_t *f1, asd_tree_t *f2) 
{
  char *temporario = gera_temp();
  char* instr = cria_instrucao(mneumonico, f1->local, f2->local, temporario);
  ret_instr_t retorno;
  retorno.codigo = concatena3(f1->codigo, f2->codigo, instr);
  retorno.temporario = temporario;

  return retorno;
}


ret_instr_t gera_codigo_if(asd_tree_t *f1, asd_tree_t *f2)
{
	ret_instr_t retorno;
	char *temporario = gera_temp();
	char *label_true = gera_label();
	char *label_false = gera_label();
	char* instr_if = cria_instrucao("cbr", f1->local, label_true, label_false);
	char* instr_nop = cria_instrucao("nop", NULL, NULL, NULL);
	
	if(f2 != NULL) {
	  retorno.codigo = concatena2(f1->codigo, instr_if);
	  retorno.codigo = concatena3(retorno.codigo , label_true, ":");
	  retorno.codigo = concatena2(retorno.codigo , "\n");
	  retorno.codigo = concatena2(retorno.codigo , f2->codigo);
	  retorno.codigo = concatena3(retorno.codigo , label_false, ":");
	  retorno.codigo = concatena2(retorno.codigo , "\n");

	  retorno.temporario = temporario;

	  return retorno;
	}
	else {
	  retorno.codigo = concatena2(f1->codigo, instr_if);
	  retorno.codigo = concatena3(retorno.codigo , label_true, ":");
	  retorno.codigo = concatena2(retorno.codigo , "\n");
	  retorno.codigo = concatena2(retorno.codigo , instr_nop);
	  retorno.codigo = concatena3(retorno.codigo , label_false, ":");
	  retorno.codigo = concatena2(retorno.codigo , "\n");

	  retorno.temporario = temporario;
	  
	  return retorno;
	}
}


ret_instr_t gera_codigo_if_else(asd_tree_t *f1, asd_tree_t *f2, asd_tree_t *f3)
{
  char *temporario = gera_temp();
  char *label_true = gera_label();
  char *label_false = gera_label();
  char *label_prox = gera_label();
  char* instr_if = cria_instrucao("cbr", f1->local, label_true, label_false);
  char* instr_jump = cria_instrucao("jumpI", NULL, NULL, label_prox);
  char* instr_nop = cria_instrucao("nop", NULL, NULL, NULL);
  ret_instr_t retorno;
  
  if(f2 != NULL && f3 != NULL) {
	  retorno.codigo = concatena2(f1->codigo, instr_if);
	  retorno.codigo = concatena3(retorno.codigo, label_true, ":");
	  retorno.codigo = concatena2(retorno.codigo, "\n");
	  retorno.codigo = concatena2(retorno.codigo, f2->codigo);
	  retorno.codigo = concatena2(retorno.codigo, instr_jump);
	  retorno.codigo = concatena3(retorno.codigo, label_false, ":");
	  retorno.codigo = concatena2(retorno.codigo, "\n");
	  retorno.codigo = concatena2(retorno.codigo, f3->codigo);
	  retorno.codigo = concatena3(retorno.codigo, label_prox, ":");
	  retorno.codigo = concatena2(retorno.codigo, "\n");

	  retorno.temporario = temporario;

	  return retorno;
  }
  else if(f2 == NULL && f3 != NULL) {
  	retorno.codigo = concatena2(f1->codigo, instr_if);
	  retorno.codigo = concatena3(retorno.codigo, label_true, ":");
	  retorno.codigo = concatena2(retorno.codigo, "\n");
	  retorno.codigo = concatena2(retorno.codigo , instr_nop);
	  retorno.codigo = concatena2(retorno.codigo, instr_jump);
	  retorno.codigo = concatena3(retorno.codigo, label_false, ":");
	  retorno.codigo = concatena2(retorno.codigo, "\n");
	  retorno.codigo = concatena2(retorno.codigo, f3->codigo);
	  retorno.codigo = concatena3(retorno.codigo, label_prox, ":");
	  retorno.codigo = concatena2(retorno.codigo, "\n");

	  retorno.temporario = temporario;

	  return retorno;
  }
  else if(f2 != NULL && f3 == NULL) {
  	retorno.codigo = concatena2(f1->codigo, instr_if);
	  retorno.codigo = concatena3(retorno.codigo, label_true, ":");
	  retorno.codigo = concatena2(retorno.codigo, "\n");
	  retorno.codigo = concatena2(retorno.codigo, f2->codigo);
	  retorno.codigo = concatena2(retorno.codigo, instr_jump);
	  retorno.codigo = concatena3(retorno.codigo, label_false, ":");
	  retorno.codigo = concatena2(retorno.codigo, "\n");
	  retorno.codigo = concatena2(retorno.codigo , instr_nop);
	  retorno.codigo = concatena3(retorno.codigo, label_prox, ":");
	  retorno.codigo = concatena2(retorno.codigo, "\n");

	  retorno.temporario = temporario;

	  return retorno;
  }
  else {
  	retorno.codigo = concatena2(f1->codigo, instr_if);
	  retorno.codigo = concatena3(retorno.codigo, label_true, ":");
	  retorno.codigo = concatena2(retorno.codigo, "\n");
	  retorno.codigo = concatena2(retorno.codigo , instr_nop);
	  retorno.codigo = concatena2(retorno.codigo, instr_jump);
	  retorno.codigo = concatena3(retorno.codigo, label_false, ":");
	  retorno.codigo = concatena2(retorno.codigo, "\n");
	  retorno.codigo = concatena2(retorno.codigo , instr_nop);
	  retorno.codigo = concatena3(retorno.codigo, label_prox, ":");
	  retorno.codigo = concatena2(retorno.codigo, "\n");

	  retorno.temporario = temporario;

	  return retorno;
  }
}


ret_instr_t gera_codigo_while(asd_tree_t *f1, asd_tree_t *f2)
{
  char *temporario = gera_temp();
  char *label_inicio = gera_label();
  char *label_true = gera_label();
  char *label_false = gera_label();
  char *instr_while = cria_instrucao("cbr", f1->local, label_true, label_false);
  char* instr_jump = cria_instrucao("jumpI", NULL, NULL, label_inicio);
  char* instr_nop = cria_instrucao("nop", NULL, NULL, NULL);
  ret_instr_t retorno;
  
  if(f2 != NULL) {
	  retorno.codigo = concatena2(label_inicio, ":");
	  retorno.codigo = concatena2(retorno.codigo, "\n");
	  retorno.codigo = concatena2(retorno.codigo, f1->codigo);
	  retorno.codigo = concatena2(retorno.codigo, instr_while);
	  retorno.codigo = concatena3(retorno.codigo, label_true, ":");
	  retorno.codigo = concatena2(retorno.codigo, "\n");
	  retorno.codigo = concatena2(retorno.codigo, f2->codigo);
	  retorno.codigo = concatena2(retorno.codigo, instr_jump);
	  retorno.codigo = concatena3(retorno.codigo, label_false, ":");
	  retorno.codigo = concatena2(retorno.codigo, "\n");

	  retorno.temporario = temporario;

	  return retorno;
  }
  else {
  	  retorno.codigo = concatena2(label_inicio, ":");
	  retorno.codigo = concatena2(retorno.codigo, "\n");
	  retorno.codigo = concatena2(retorno.codigo, f1->codigo);
	  retorno.codigo = concatena2(retorno.codigo, instr_while);
	  retorno.codigo = concatena3(retorno.codigo, label_true, ":");
	  retorno.codigo = concatena2(retorno.codigo, "\n");
	  retorno.codigo = concatena2(retorno.codigo , instr_nop);
	  retorno.codigo = concatena2(retorno.codigo, instr_jump);
	  retorno.codigo = concatena3(retorno.codigo, label_false, ":");
	  retorno.codigo = concatena2(retorno.codigo, "\n");

	  retorno.temporario = temporario;

	  return retorno;
  }
}


char* gera_temp() {
    char buffer[20]; 
    sprintf(buffer, "%d", contador_temp_global);

    size_t tamanho = strlen(buffer) + strlen("r") + 1;

    char* resultado = malloc(tamanho);
    if (resultado == NULL) {
        fprintf(stderr, "Erro de alocação de memória.\n");
        exit(EXIT_FAILURE);
    }

    strcpy(resultado, "r");
    strcat(resultado, buffer);
    
    contador_temp_global++;

    return resultado;
}


char* gera_label() {
    char buffer[20]; 
    sprintf(buffer, "%d", contador_label_global);

    size_t tamanho = strlen(buffer) + strlen("L")  + 1;

    char* resultado = malloc(tamanho);
    if (resultado == NULL) {
        fprintf(stderr, "Erro de alocação de memória.\n");
        exit(EXIT_FAILURE);
    }

    strcpy(resultado, "L");
    strcat(resultado, buffer);
    
    contador_label_global++;

    return resultado;
}


char* cria_instrucao(char* instrucao, char* parametro1, char* parametro2, char* parametro3) {
	size_t tamanho = strlen(instrucao);
	    
	if (parametro1 != NULL) tamanho += strlen(parametro1);
	if (parametro2 != NULL) tamanho += strlen(parametro2);
	if (parametro3 != NULL) tamanho += strlen(parametro3);

	tamanho += 10; 
	char* resultado = malloc(tamanho);
	
	if (resultado == NULL) {
		fprintf(stderr, "Erro de alocação de memória.\n");
		exit(EXIT_FAILURE);
    	}
    	
      if (parametro2 == NULL && parametro1 != NULL) {
        snprintf(resultado, tamanho, "%s  %s => %s\n", instrucao, parametro1, parametro3);
      }
      else if (parametro1 == NULL && parametro2 != NULL) {
        snprintf(resultado, tamanho, "%s  %s => %s\n", instrucao, parametro2, parametro3);
      }
      else if (parametro1 == NULL && parametro2 == NULL && strcmp(instrucao, "jumpI") != 0 && parametro3 != NULL) {
        snprintf(resultado, tamanho, "%s => %s\n", instrucao, parametro3);
      }
      else if (parametro1 == NULL && parametro2 == NULL && strcmp(instrucao, "jumpI") == 0 && parametro3 != NULL) {
        snprintf(resultado, tamanho, "%s -> %s\n", instrucao, parametro3);
      }
      else if (parametro1 == NULL && parametro2 == NULL && parametro3 == NULL){
      	snprintf(resultado, tamanho, "%s\n", instrucao);
      }
    	
    	// adicionar condicionais para gerar os outros formatos de instruções
    	else {
        if (strcmp(instrucao, "storeAI") == 0 || strcmp(instrucao, "storeA0") == 0 || 
            strcmp(instrucao, "cstoreAI") == 0 || strcmp(instrucao, "cstoreA0") == 0){
            snprintf(resultado, tamanho, "%s  %s => %s, %s\n", instrucao, parametro1, parametro2, parametro3);
        } 
        else if(strcmp(instrucao, "cbr") == 0) {
            snprintf(resultado, tamanho, "%s  %s -> %s, %s\n", instrucao, parametro1, parametro2, parametro3);
        }
        else {
            snprintf(resultado, tamanho, "%s  %s, %s => %s\n", instrucao, parametro1, parametro2, parametro3);
          }
    	}
    	
    	return resultado;
}


char* concatena3(char* parametro1, char* parametro2, char* instr) {
    // Verificar se algum parâmetro é NULL
    if (parametro1 == NULL) {
        fprintf(stderr, "Erro: parâmetro 1 inválido.\n");
        return NULL;
    }
    else if (parametro2 == NULL) {
        fprintf(stderr, "Erro: parâmetro 2 inválido.\n");
        return NULL;
    }
    else if (instr == NULL) {
        fprintf(stderr, "Erro: parâmetro instr inválido.\n");
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
    if (parametro1 == NULL) {
        fprintf(stderr, "Erro: parâmetro 1 inválido.\n");
        return NULL;
    }
    else if (instr == NULL) {
        fprintf(stderr, "Erro: parâmetro instr inválido.\n");
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
	
	if (tabela->numero_de_entradas == 0) {
                entrada->deslocamento = 0;
        }
        else {
        	conteudo_tabela_simbolos_t *ultima_entrada = tabela->entradas[tabela->numero_de_entradas - 1];
        	entrada->deslocamento = ultima_entrada->deslocamento + 4;
        }
        
        tabela->numero_de_entradas++;
        tabela->entradas = realloc(tabela->entradas, tabela->numero_de_entradas * sizeof(conteudo_tabela_simbolos_t *));
        if (tabela->entradas == NULL) {
            fprintf(stderr, "Erro: falha ao realocar memória para as entradas da tabela.\n");
            return;
        }
        tabela->entradas[tabela->numero_de_entradas - 1] = entrada;

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

asd_tree_t* cria_nodo_expressao(char *expressao, asd_tree_t *filho_1, asd_tree_t *filho_2)
{
  asd_tree_t* nodo = asd_new(expressao);
  asd_add_child(nodo, filho_1);
  asd_add_child(nodo, filho_2);
  char* tipo = infere_tipo(filho_1->tipo, filho_2->tipo);
  nodo->tipo = tipo;
  return nodo;
}

asd_tree_t* cria_nodo_expressao_unaria(char *expressao, asd_tree_t *filho_1)
{
  asd_tree_t* nodo = asd_new(expressao);
  if(filho_1 != NULL) 
    asd_add_child(nodo, filho_1);
  nodo->tipo = filho_1->tipo;
  return nodo;
}

asd_tree_t* processa_chamada_funcao(valor_lexico_t *identificador, asd_tree_t *filho_2, pilha_tabelas_t *pilha_tabelas) 
{
  char buffer[256]; 
  snprintf(buffer, sizeof(buffer), "call %s", identificador->valor);
  asd_tree_t* nodo = asd_new(buffer);
  asd_add_child(nodo, filho_2);

  conteudo_tabela_simbolos_t *checa_id = busca_entrada_pilha(pilha_tabelas, identificador->valor);
  if (checa_id == NULL)
    printa_erro(ERR_UNDECLARED, identificador->valor, identificador->linha, identificador->linha);
  else if(strcmp(checa_id->natureza, "IDENTIFICADOR") == 0)
    printa_erro(ERR_VARIABLE, checa_id->valor, identificador->linha, identificador->linha);

  return nodo;
}

asd_tree_t* processa_atribuicao(valor_lexico_t *identificador, asd_tree_t *filho_2, pilha_tabelas_t *pilha_tabelas)
{
  asd_tree_t* nodo = asd_new("=");
  asd_add_child(nodo, asd_new(identificador->valor)); 
  asd_add_child(nodo, filho_2); 
  conteudo_tabela_simbolos_t *checa_id = busca_entrada_pilha(pilha_tabelas, identificador->valor);
  if (checa_id == NULL)
    printa_erro(ERR_UNDECLARED, identificador->valor, identificador->linha, identificador->linha);
  else if(strcmp(checa_id->natureza, "FUNCAO") == 0)
    printa_erro(ERR_FUNCTION, checa_id->valor, identificador->linha, identificador->linha);
  nodo->tipo = checa_id->tipo;

  return nodo;
}

asd_tree_t* processa_expressao(valor_lexico_t *identificador, pilha_tabelas_t *pilha_tabelas)
{
  asd_tree_t* nodo = asd_new(identificador->valor); 
  conteudo_tabela_simbolos_t *checa_id = busca_entrada_pilha(pilha_tabelas, identificador->valor);
  if (checa_id == NULL)
    printa_erro(ERR_UNDECLARED, identificador->valor, identificador->linha, identificador->linha);
  else if(strcmp(checa_id->natureza, "FUNCAO") == 0)
    printa_erro(ERR_FUNCTION, checa_id->valor, identificador->linha, identificador->linha);
  nodo->tipo = checa_id->tipo; 

  return nodo;
}
