/*
Função principal para realização da análise sintática.

Este arquivo será posterioremente substituído, não acrescente nada.
*/
#include <stdio.h>
#include "parser.tab.h" //arquivo gerado com bison -d parser.y
#include "../include/data_structures.h"
extern int yylex_destroy(void);
void *arvore = NULL; // Inicializa o ponteiro como NULL

int main (int argc, char **argv)
{
  int ret = yyparse();
  yylex_destroy();   

  asd_tree_t *asd_mult = NULL, *asd_soma = NULL;
  asd_mult = asd_new("*");
  asd_add_child(asd_mult, asd_new("3"));
  asd_add_child(asd_mult, asd_new("4"));
  asd_add_child(asd_mult, asd_new("5"));
  
  asd_soma = asd_new("+");
  asd_add_child(asd_soma, asd_new("5"));
  asd_add_child(asd_soma, asd_mult);
  
  asd_print(asd_soma);
  asd_print_graphviz(asd_soma);
  asd_free(asd_soma);
   
  return ret;
}
