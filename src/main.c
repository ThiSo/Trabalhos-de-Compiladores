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

  int data1 = 1, data2 = 2, data3 = 3, data4 = 4;
  Node *root = createNode(&data1, 2);
  arvore = root; // Atribui a raiz ao ponteiro arvore

  Node *child1 = createNode(&data2, 2);
  Node *child2 = createNode(&data3, 0);
  Node *child3 = createNode(&data4, 0);

  addChild(root, child1, 0);
  addChild(root, child2, 1);
  addChild(child1, child3, 0);

  printTree((Node *)arvore, printInt); // Converte arvore para Node* antes de passar para printTree
  printf("\n");

  freeTree((Node *)arvore); // Converte arvore para Node* antes de passar para freeTree
    
  return ret;
}
