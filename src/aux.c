#include "aux.h"
#include <stdio.h>
#include <stdlib.h>

void printa_arvore(asd_tree_t *arvore)
{
  if (arvore == NULL) {
        return;
  }

  for (int i = 0; i < arvore->number_of_children; i++) {
        printf("%p, %p\n", (void*)arvore, (void*)arvore->children[i]);
        printa_arvore(arvore->children[i]);
  }
}


void printa_labels(asd_tree_t *arvore)
{
  if (arvore == NULL) {
        return;
  }

  printf("%p [label=\"%s\"]; \n", (void*)arvore, arvore->label);

  for (int i = 0; i < arvore->number_of_children; i++) {
        printa_labels(arvore->children[i]);
  }
}

void destroi_arvore(asd_tree_t *arvore)
{
    if (arvore == NULL) {
        return;
    }

    for (int i = 0; i < arvore->number_of_children; i++) {
        destroi_arvore(arvore->children[i]);
    }

    free(arvore->children);
    free(arvore->label);
    free(arvore);
}

void exporta(void *arvore)
{
    asd_tree_t *arv = (asd_tree_t*) arvore;

    printa_arvore(arv);
    printf("\n");
    printa_labels(arv);
    destroi_arvore(arv);
}