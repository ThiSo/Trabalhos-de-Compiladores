/*
Função principal para realização da análise sintática.

Este arquivo será posterioremente substituído, não acrescente nada.
*/
#include <stdio.h>
#include "parser.tab.h" //arquivo gerado com bison -d parser.y
#include "data_structures.h"
#include "aux.h"

extern int yyparse(void);
extern int yylex_destroy(void);
void exporta(void *arvore);

void *arvore = NULL; // Inicializa o ponteiro como NULL
pilha_tabelas_t *pilha_tabelas = NULL;

int main (int argc, char **argv)
{
  int ret = yyparse();
  yylex_destroy();   
  return ret;
}
