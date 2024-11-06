%{
    #include <stdio.h>
    #include <string.h>
    int yylex(void);
    extern int get_line_number();
    extern void *arvore;
    void yyerror (char const *mensagem);
%}

%code requires {
    #include "../include/data_structures.h"
    #include "../include/aux.h"
}

%union {
  valor_lexico_t *valor_lexico;
  asd_tree_t *arvore;
}


%token<valor_lexico> TK_PR_INT
%token<valor_lexico> TK_PR_FLOAT
%token<valor_lexico> TK_PR_IF
%token<valor_lexico> TK_PR_ELSE
%token<valor_lexico> TK_PR_WHILE
%token<valor_lexico> TK_PR_RETURN
%token<valor_lexico> TK_OC_LE
%token<valor_lexico> TK_OC_GE
%token<valor_lexico> TK_OC_EQ
%token<valor_lexico> TK_OC_NE
%token<valor_lexico> TK_OC_AND
%token<valor_lexico> TK_OC_OR
%token<valor_lexico> TK_IDENTIFICADOR
%token<valor_lexico> TK_LIT_INT
%token<valor_lexico> TK_LIT_FLOAT
%token<valor_lexico> TK_ERRO

%type<arvore> programa
%type<arvore> lista_de_funcoes
%type<arvore> funcao
%type<arvore> cabecalho
%type<arvore> lista_parametros
%type<arvore> parametro
%type<arvore> tipo_variavel
%type<arvore> corpo
%type<arvore> bloco_comandos
%type<arvore> lista_comandos
%type<arvore> comando
%type<arvore> declaracao_variavel
%type<arvore> lista_variaveis
%type<arvore> variavel
%type<arvore> literal               // talvez seja <valor_lexico>
%type<arvore> retorno
%type<arvore> controle_fluxo
%type<arvore> ifs
%type<arvore> whiles
%type<arvore> atribuicao
%type<arvore> chamada_funcao
%type<arvore> lista_argumentos
%type<arvore> expressao
%type<arvore> prec6
%type<arvore> prec5
%type<arvore> prec4
%type<arvore> prec3
%type<arvore> prec2
%type<arvore> prec1
%type<arvore> fator
%type<arvore> unario

%define parse.error verbose

%%

programa: lista_de_funcoes 
        |                                             { $$ = NULL;};                                           

lista_de_funcoes: lista_de_funcoes funcao 
                | funcao;         

funcao: cabecalho corpo;  

cabecalho: TK_IDENTIFICADOR '=' lista_parametros '>' tipo_variavel; 

lista_parametros: lista_parametros TK_OC_OR parametro 
                | parametro 
                |                                     { $$ = NULL; };            

parametro: TK_IDENTIFICADOR '<' '-' tipo_variavel       

tipo_variavel: TK_PR_INT 
             | TK_PR_FLOAT;   

corpo: bloco_comandos;                                                                              

bloco_comandos: '{' lista_comandos '}'                { $$ = $2; }
              | '{' '}'                               { $$ = NULL; };

lista_comandos: comando 
              | comando lista_comandos;

comando: bloco_comandos ';' 
       | declaracao_variavel ';' 
       | retorno ';' 
       | atribuicao ';' 
       | chamada_funcao ';' 
       | controle_fluxo ';';

declaracao_variavel: tipo_variavel lista_variaveis ;   

lista_variaveis: lista_variaveis ',' variavel 
               | variavel;     

variavel: TK_IDENTIFICADOR 
        | TK_IDENTIFICADOR TK_OC_LE literal;

literal: TK_LIT_INT     
       | TK_LIT_FLOAT;                                         

retorno: TK_PR_RETURN expressao;                                                                    
                                                     
controle_fluxo: ifs           {$$ = $1;}
              | whiles        {$$ = $1;};                                                                

ifs: TK_PR_IF '(' expressao ')' bloco_comandos                           { $$ = asd_new("if"); asd_add_child($$,$3); asd_add_child($$,$5);}
   | TK_PR_IF '(' expressao ')' bloco_comandos TK_PR_ELSE bloco_comandos { $$ = asd_new("if"); asd_add_child($$,$3); asd_add_child($$,$5); asd_add_child($$,$7);};                        

whiles: TK_PR_WHILE '(' expressao ')' bloco_comandos                     { $$ = asd_new("while"); asd_add_child($$,$3); asd_add_child($$,$5);};                                            

atribuicao: TK_IDENTIFICADOR '=' expressao;                                                         

chamada_funcao: TK_IDENTIFICADOR '(' lista_argumentos ')';

lista_argumentos: lista_argumentos ',' expressao 
                | expressao;


expressao: expressao TK_OC_OR prec6;
expressao: prec6;

prec6: prec6 TK_OC_AND prec5;
prec6: prec5;

prec5: prec5 TK_OC_EQ prec4;
prec5: prec5 TK_OC_NE prec4;
prec5: prec4;

prec4: prec4 '<' prec3;
prec4: prec4 '>' prec3;
prec4: prec4 TK_OC_LE prec3;
prec4: prec4 TK_OC_GE prec3;
prec4: prec3;

prec3: prec3 '+' prec2;
prec3: prec3 '-' prec2;
prec3: prec2;

prec2: prec2 '*' prec1;
prec2: prec2 '/' prec1;
prec2: prec2 '%' prec1;
prec2: prec1;

prec1: unario fator { $$ = asd_new("!"); asd_add_child($$, $2);}
     | fator;

fator: '(' expressao ')' { $$ = $2;};

fator: TK_IDENTIFICADOR     
     | TK_LIT_INT           
     | TK_LIT_FLOAT         
     | chamada_funcao;

unario: unario '!' 
      | unario '-' 
      | '!' 
      | '-';

%%

void yyerror(char const *mensagem)
{
    fprintf(stderr, "Erro %s na linha: %d\n", mensagem, get_line_number());
}
