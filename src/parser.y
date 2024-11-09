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
%type<arvore> literal               
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

programa: lista_de_funcoes 			                                { printf("Árvore final gerada!\n"); asd_print_graphviz($1); asd_free($1); $$ = $1; arvore = $$;}
        |                                                           { printf("teste"); $$ = NULL; arvore = $$; };                                           

lista_de_funcoes: funcao lista_de_funcoes 	                        { $$ = $1; asd_add_child($$, $2); } // Rec. a direita para listas
                | funcao			                                { $$ = $1; };         

funcao: cabecalho corpo				                                { $$ = $2; };	    // Ignora o cabecalho da função e coloca na arvore somente o corpo

cabecalho: TK_IDENTIFICADOR '=' lista_parametros '>' tipo_variavel 	{ $$ = NULL; };		// NULL porque não está sendo utilizado no momento, colocar outro valor se for utilizar

lista_parametros: parametro TK_OC_OR lista_parametros               { $$ = $1; asd_add_child($$, $3); }
                | parametro 			                            { $$ = $1; }
                |                                                   { $$ = NULL; };            

parametro: TK_IDENTIFICADOR '<' '-' tipo_variavel   { $$ = $4; asd_add_child($$, asd_new($1->valor)); };  

tipo_variavel: TK_PR_INT		                    { $$ = asd_new($1->valor); } 
             | TK_PR_FLOAT 		                    { $$ = asd_new($1->valor); };   

corpo: bloco_comandos                               { $$ = $1; };                                                   

bloco_comandos: '{' lista_comandos '}'              { $$ = $2; }
              | '{' '}'                             { $$ = NULL; };

lista_comandos: comando 		                    { $$ = $1; }
              | comando lista_comandos	            { $$ = $1; if($$ != NULL) asd_add_child($$, $2); };

comando: bloco_comandos ';' 		                { $$ = $1; }
       | declaracao_variavel ';' 	                { $$ = $1; }
       | retorno ';' 			                    { $$ = $1; }
       | atribuicao ';' 		                    { $$ = $1; }
       | chamada_funcao ';' 		                { $$ = $1; }
       | controle_fluxo ';'		                    { $$ = $1; };

declaracao_variavel: tipo_variavel lista_variaveis	{ $$ = $1; asd_add_child($$, $2); };   

lista_variaveis: variavel ',' lista_variaveis 		{ $$ = $1; if($3 != NULL) asd_add_child($$, $3); };
               | variavel				            { $$ = $1; };     

variavel: TK_IDENTIFICADOR 				            { $$ = asd_new($1->valor); }
        | TK_IDENTIFICADOR TK_OC_LE literal		    { $$ = asd_new("<="); asd_add_child($$, asd_new($1->valor)); asd_add_child($$, $3); };

literal: TK_LIT_INT   					            { $$ = asd_new($1->valor); }  
       | TK_LIT_FLOAT 					            { $$ = asd_new($1->valor); };                                         

retorno: TK_PR_RETURN expressao				        { $$ = asd_new("return"), asd_add_child($$, $2); };                                                                    
                                                     
controle_fluxo: ifs                     		    { $$ = $1; }
              | whiles                  		    { $$ = $1; };                                                                

ifs: TK_PR_IF '(' expressao ')' bloco_comandos                           { $$ = asd_new("if"); asd_add_child($$,$3); if($5 != NULL){ asd_add_child($$, $5); }}
   | TK_PR_IF '(' expressao ')' bloco_comandos TK_PR_ELSE bloco_comandos { $$ = asd_new("if"); asd_add_child($$,$3); asd_add_child($$,$5); if($5 != NULL){ asd_add_child($$, $7); }};                        

whiles: TK_PR_WHILE '(' expressao ')' bloco_comandos                     { $$ = asd_new("while"); asd_add_child($$,$3);if($5 != NULL){ asd_add_child($$, $5); } };                                            

atribuicao: TK_IDENTIFICADOR '=' expressao                               { $$ = asd_new("="); asd_add_child($$, asd_new($1->valor)); asd_add_child($$, $3); };                                     

chamada_funcao: TK_IDENTIFICADOR '(' lista_argumentos ')'                { $$ = asd_new(strcat("call ", $1->valor)); asd_add_child($$, $3); };          

lista_argumentos: expressao                                              { $$ = $1; }
                | expressao ',' lista_argumentos                         { $$ = $1; asd_add_child($$, $3); };



expressao: expressao TK_OC_OR prec6     { $$ = asd_new("|"); asd_add_child($$, $1); asd_add_child($$, $3); }; 
expressao: prec6                        { $$ = $1; };

prec6: prec6 TK_OC_AND prec5            { $$ = asd_new("&"); asd_add_child($$, $1); asd_add_child($$, $3); }; 
prec6: prec5                            { $$ = $1; };

prec5: prec5 TK_OC_EQ prec4             { $$ = asd_new("=="); asd_add_child($$, $1); asd_add_child($$, $3); };
prec5: prec5 TK_OC_NE prec4             { $$ = asd_new("!="); asd_add_child($$, $1); asd_add_child($$, $3); };
prec5: prec4                            { $$ = $1; };

prec4: prec4 '<' prec3                  { $$ = asd_new("<"); asd_add_child($$, $1); asd_add_child($$, $3); };
prec4: prec4 '>' prec3                  { $$ = asd_new(">"); asd_add_child($$, $1); asd_add_child($$, $3); };
prec4: prec4 TK_OC_LE prec3             { $$ = asd_new("<="); asd_add_child($$, $1); asd_add_child($$, $3); };
prec4: prec4 TK_OC_GE prec3             { $$ = asd_new(">="); asd_add_child($$, $1); asd_add_child($$, $3); };
prec4: prec3                            { $$ = $1; };

prec3: prec3 '+' prec2                  { $$ = asd_new("+"); asd_add_child($$, $1); asd_add_child($$, $3); };
prec3: prec3 '-' prec2                  { $$ = asd_new("-"); asd_add_child($$, $1); asd_add_child($$, $3); };
prec3: prec2                            { $$ = $1; };  

prec2: prec2 '*' prec1                  { $$ = asd_new("*"); asd_add_child($$, $1); asd_add_child($$, $3); };
prec2: prec2 '/' prec1                  { $$ = asd_new("/"); asd_add_child($$, $1); asd_add_child($$, $3); };
prec2: prec2 '%' prec1                  { $$ = asd_new("%"); asd_add_child($$, $1); asd_add_child($$, $3); };
prec2: prec1                            { $$ = $1; };

prec1: unario fator			            { if($1 != NULL) { 
						                        $$ = $1; 
						                        asd_add_child($$, $2);
					                        }
					                        else {
					  	                        $$ = $2;
					                        } };

fator: '(' expressao ')'                { $$ = $2;};

fator: TK_IDENTIFICADOR                 { $$ = asd_new($1->valor); }
     | TK_LIT_INT                       { $$ = asd_new($1->valor); }
     | TK_LIT_FLOAT                     { $$ = asd_new($1->valor); }
     | chamada_funcao                   { $$ = $1; };

unario: unario '!' 			            { $$ = asd_new("!"); if($1 != NULL) { asd_add_child($$, $1);} }
     |  unario '-' 			            { $$ = asd_new("-"); if($1 != NULL) { asd_add_child($$, $1);} }
     | 					                { $$ = NULL;};                            

%%

void yyerror(char const *mensagem)
{
    fprintf(stderr, "Erro %s na linha: %d\n", mensagem, get_line_number());
}
