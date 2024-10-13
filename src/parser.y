%{
    #include <stdio.h>
    int yylex(void);
    void yyerror (char const *mensagem);
%}

%token TK_PR_INT
%token TK_PR_FLOAT
%token TK_PR_IF
%token TK_PR_ELSE
%token TK_PR_WHILE
%token TK_PR_RETURN
%token TK_OC_LE
%token TK_OC_GE
%token TK_OC_EQ
%token TK_OC_NE
%token TK_OC_AND
%token TK_OC_OR
%token TK_IDENTIFICADOR
%token TK_LIT_INT
%token TK_LIT_FLOAT
%token TK_ERRO

%define parse.error verbose

%%

/*  Um programa na linguagem é composto por uma
    lista de funções, sendo esta lista opcional. */

programa: lista_de_funcoes | ;                                              // ok
lista_de_funcoes: lista_de_funcoes funcao | funcao;                         // ok
funcao: cabecalho corpo;                                                    // ok
cabecalho: TK_IDENTIFICADOR '=' lista_parametros '>' tipo_retorno;          // ok
lista_parametros: lista_parametros TK_OC_OR parametro | parametro | ;       // ok
parametro: TK_IDENTIFICADOR '<' '-' (TK_PR_INT | TK_PR_FLOAT)               // ok
tipo_retorno: TK_PR_INT | TK_PR_FLOAT;                                      // ok

corpo: '{' '}';

%%

void yyerror(char const *mensagem)
{
    fprintf(stderr, "%s\n", mensagem);
}