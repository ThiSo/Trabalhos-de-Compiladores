#ifndef DATA_STRUCTURES_H
#define DATA_STRUCTURES_H

// Estrutura para um nó da árvore n-ária
typedef struct Node {
    void *data;
    struct Node **children;
    int numChildren;
} Node;

// Função para criar um novo nó
Node* createNode(void *data, int numChildren);

// Função para adicionar um filho a um nó
void addChild(Node *parent, Node *child, int index);

// Função para liberar a memória da árvore
void freeTree(Node *root);

// Função para imprimir a árvore
void printTree(Node *root, void (*printData)(void *));

// Função de exemplo para imprimir dados inteiros
void printInt(void *data);

#endif // DATA_STRUCTURES_H