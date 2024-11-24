/*
Função principal para realização da análise sintática.

Este arquivo será posterioremente substituído, não acrescente nada.
*/
#include <stdio.h>
#include "parser.tab.h" //arquivo gerado com bison -d parser.y
#include "../include/data_structures.h"
#include "../include/aux.h"

extern int yyparse(void);
extern int yylex_destroy(void);
void exporta(void *arvore);

void *arvore = NULL; // Inicializa o ponteiro como NULL
pilha_tabelas_t *pilha_tabelas = NULL;

int main (int argc, char **argv)
{
  // Fazer aqui ou no parser
  // 1) criar pilha de tabelas de símbolos
  // 2) criar tabela do escopo global
  int ret = yyparse();
  // exporta(arvore);
  yylex_destroy();   
  return ret;
}
