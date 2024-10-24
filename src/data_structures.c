#include <stdio.h>
#include <stdlib.h>
#include "../include/data_structures.h"

// Função para criar um novo nó
Node* createNode(void *data, int numChildren) {
    Node *newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->numChildren = numChildren;
    newNode->children = (Node**)malloc(numChildren * sizeof(Node*));
    for (int i = 0; i < numChildren; i++) {
        newNode->children[i] = NULL;
    }
    return newNode;
}

// Função para adicionar um filho a um nó
void addChild(Node *parent, Node *child, int index) {
    if (index >= 0 && index < parent->numChildren) {
        parent->children[index] = child;
    } else {
        printf("Índice fora do intervalo\n");
    }
}

// Função para liberar a memória da árvore
void freeTree(Node *root) {
    if (root == NULL) return;
    for (int i = 0; i < root->numChildren; i++) {
        freeTree(root->children[i]);
    }
    free(root->children);
    free(root);
}

// Função para imprimir a árvore (exemplo de uso)
void printTree(Node *root, void (*printData)(void *)) {
    if (root == NULL) return;
    printData(root->data);
    for (int i = 0; i < root->numChildren; i++) {
        printTree(root->children[i], printData);
    }
}

// Função de exemplo para imprimir dados inteiros
void printInt(void *data) {
    printf("%d ", *(int*)data);
}