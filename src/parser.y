%{
    #include <stdio.h>
    #include <string.h>
    #include "data_structures.h" // Necessário incluir aqui por conta da pilha global na linha 8
    int yylex(void);
    extern int get_line_number();
    extern void *arvore;
    extern pilha_tabelas_t *pilha_tabelas;
    void yyerror (char const *mensagem);
%}

%code requires {
    #include "aux.h"
    #include "erros.h"
    #include "data_structures.h"
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
%type<arvore> bloco_comandos_func
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

%define parse.error verbose

%%

// -----------------------------------------------------------------------------------
// Escopo global
// -----------------------------------------------------------------------------------  

inicializacao: abre_escopo_global programa fecha_escopo_global         

abre_escopo_global:                                                    
{ tabela_simbolos_t *tabela_global = cria_tabela_simbolos();
  // Pilha deve ser inicializada com a tabela global (atualizaçao na spec)
  pilha_tabelas = cria_pilha(tabela_global); 
};

abre_escopo_func:                                                      
{ tabela_simbolos_t *tabela_func = cria_tabela_simbolos();
  empilhar(&pilha_tabelas, tabela_func); 
};

abre_escopo_aninhado:                                                  
{ tabela_simbolos_t *tabela_aninhada = cria_tabela_simbolos();
  empilhar(&pilha_tabelas, tabela_aninhada); 
};

fecha_escopo_global:                                                   { destroi_pilha(pilha_tabelas); };

fecha_escopo_func:                                                     { desempilhar(&pilha_tabelas); };

fecha_escopo_aninhado:                                                 { desempilhar(&pilha_tabelas); };

programa: lista_de_funcoes 	                                           { $$ = $1; arvore = $$; }
    |                                                                  { $$ = NULL; arvore = $$; };
                             
lista_de_funcoes: funcao fecha_escopo_func lista_de_funcoes 	         { $$ = $1; if($3 != NULL) asd_add_child($$, $3); }          
        | funcao fecha_escopo_func		                                 { $$ = $1; };         

funcao: cabecalho corpo				                                         { $$ = $1; if($2 != NULL) asd_add_child($$, $2); };	    

// -----------------------------------------------------------------------------------
// Escopo da funcao 
// abre apos o '=' e nao apos as primeiras chaves	
// -----------------------------------------------------------------------------------	

// { /* segundo o professor, colocar na tabela que está EM BAIXO (global) $1 e $6 */ };
cabecalho: TK_IDENTIFICADOR '=' abre_escopo_func lista_parametros '>' tipo_variavel 	{ $$ = asd_new($1->valor);
                                                                                        conteudo_tabela_simbolos_t *entrada = cria_entrada($1->linha, "FUNCAO", $6->tipo, $1->valor);
                                                                                        adiciona_entrada(pilha_tabelas->prox->tabela_simbolos, entrada);};    


lista_parametros: parametro TK_OC_OR lista_parametros               { $$ = NULL; }
        | parametro 			                                          { $$ = NULL; }
        |                                                           { $$ = NULL; };            


 // { /* coloca na tabela que esta no topo $1 e $4 */ }; 
parametro: TK_IDENTIFICADOR '<' '-' tipo_variavel                   { $$ = NULL; 
                                                                      conteudo_tabela_simbolos_t *entrada = cria_entrada($1->linha, "IDENTIFICADOR", $4->tipo, $1->valor);
                                                                      adiciona_entrada(pilha_tabelas->tabela_simbolos, entrada);};

// necessária criação de um nodo com tipo aqui, pois os nodos criados com asd_new sempre vem com tipo = NULL, e na etapa 3 essa regra era apenas $$ = NULL
tipo_variavel: TK_PR_INT		                             { $$ = cria_tipo("INT"); } 
             | TK_PR_FLOAT 		                           { $$ = cria_tipo("FLOAT"); };   

corpo: bloco_comandos_func                               { $$ = $1; };    

bloco_comandos_func: '{' lista_comandos '}'              { $$ = $2; }
                   | '{' '}'                             { $$ = NULL; };


bloco_comandos: '{' abre_escopo_aninhado lista_comandos fecha_escopo_aninhado '}'              { $$ = $3; }
              | '{' abre_escopo_aninhado fecha_escopo_aninhado'}'                              { $$ = NULL; };

// -----------------------------------------------------------------------------------
// Escopo aninhado
// -----------------------------------------------------------------------------------

lista_comandos: comando 		                                           { $$ = $1; }       // Tratamento para as variaveis não inicializadas
              | comando lista_comandos	            	                 { $$ = $1; if(($$ != NULL) && ($2 != NULL)) asd_add_child($$, $2); else if ($2 != NULL ) $$ = $2; } 
              | declaracao_variavel ';' lista_comandos		             { $$ = $1; if ($$ != NULL) { if($3 != NULL) asd_add_child(corrige_ordem_filhos($$), $3); } else $$ = $3; }
              | declaracao_variavel ';' 	                             { $$ = $1; };

comando: bloco_comandos ';' 		                                       { $$ = $1; }
       | retorno ';' 			                                             { $$ = $1; }
       | atribuicao ';' 		                                           { $$ = $1; }
       | chamada_funcao ';' 		                                       { $$ = $1; }
       | controle_fluxo ';'		                                         { $$ = $1; };

// Atribuir tipos deixados na tabela como "atribuir depois" aqui
declaracao_variavel: tipo_variavel lista_variaveis	                     { $$ = $2;
                                                                           atribui_tipo(pilha_tabelas->tabela_simbolos, $1->tipo); };   

                                                                         // variaveis não inicializadas não entram na AST (mesmo comando)
lista_variaveis: variavel ',' lista_variaveis 		                       { if($1 == NULL) $$ = $3; else { $$ = $1; if ($3 != NULL) asd_add_child($$, $3); }};       
               | variavel				                                         { $$ = $1; };     

                                                                         // variaveis não inicializadas não entram na AST (comandos separados)
variavel: TK_IDENTIFICADOR 				                                       { $$ = NULL;
                                                                           conteudo_tabela_simbolos_t *entrada = cria_entrada($1->linha, "IDENTIFICADOR", "ATRIBUIR DEPOIS", $1->valor);
                                                                           adiciona_entrada(pilha_tabelas->tabela_simbolos, entrada); }

        | TK_IDENTIFICADOR TK_OC_LE literal		                           { $$ = asd_new("<="); asd_add_child($$, asd_new($1->valor)); asd_add_child($$, $3); 
                                                                           // Talvez usar $3->tipo em vez de ATRIBUIR_DEPOIS aqui? - perguntar pro professor
                                                                           conteudo_tabela_simbolos_t *entrada = cria_entrada($1->linha, "IDENTIFICADOR", "ATRIBUIR DEPOIS", $1->valor);
                                                                           adiciona_entrada(pilha_tabelas->tabela_simbolos, entrada); };

literal: TK_LIT_INT   					                                         { $$ = asd_new($1->valor);
                                                                           $$->tipo = strdup("INT"); }  
       | TK_LIT_FLOAT 					                                         { $$ = asd_new($1->valor);
                                                                           $$->tipo = strdup("FLOAT"); };                                         

retorno: TK_PR_RETURN expressao				                                   { $$ = asd_new("return"), asd_add_child($$, $2); };                                                                    
                                                     
controle_fluxo: ifs                     		                             { $$ = $1; }
              | whiles                  		                             { $$ = $1; };                                                                

ifs: TK_PR_IF '(' expressao ')' bloco_comandos                           { $$ = asd_new("if"); asd_add_child($$,$3); if($5 != NULL){ asd_add_child($$, $5); }}
   | TK_PR_IF '(' expressao ')' bloco_comandos TK_PR_ELSE bloco_comandos { $$ = asd_new("if"); asd_add_child($$,$3); if($5 != NULL){ asd_add_child($$, $5); } if($7 != NULL){ asd_add_child($$, $7); }};                        

whiles: TK_PR_WHILE '(' expressao ')' bloco_comandos                     { $$ = asd_new("while"); asd_add_child($$,$3);if($5 != NULL){ asd_add_child($$, $5); } };                                            

// --------------------------------------------------------------

// checar aqui se identificador ja foi criado procurando na tabela no topo da pilha e nas abaixo
// checar também se identificador está sendo usado corretamente (erro function -> se função é usada como variável)
// comando de atribuição deve ter inferência de tipo e "passar pra cima" o seu tipo com base na tabela  
atribuicao: TK_IDENTIFICADOR '=' expressao                               { $$ = asd_new("="); asd_add_child($$, asd_new($1->valor)); asd_add_child($$, $3); 
                                                                           conteudo_tabela_simbolos_t *checa_id = busca_entrada_pilha(pilha_tabelas, $1->valor);
                                                                           if (checa_id == NULL)
                                                                                printa_erro(ERR_UNDECLARED, $1->valor, $1->linha, $1->linha);
                                                                           else if(strcmp(checa_id->natureza, "FUNCAO") == 0)
                                                                                printa_erro(ERR_FUNCTION, checa_id->valor, $1->linha, $1->linha);
                                                                           $$->tipo = checa_id->tipo;
                                                                         };                                     

// checar aqui tambem se undeclared
// além disso, checar se está sendo usado corretamente (erro variable -> variavel utilizada como função)
chamada_funcao: TK_IDENTIFICADOR '(' lista_argumentos ')'                { char buffer[256]; snprintf(buffer, sizeof(buffer), "call %s", $1->valor); $$ = asd_new(buffer); asd_add_child($$, $3); 
                                                                           conteudo_tabela_simbolos_t *checa_id = busca_entrada_pilha(pilha_tabelas, $1->valor);
                                                                           if (checa_id == NULL)
                                                                                printa_erro(ERR_UNDECLARED, $1->valor, $1->linha, $1->linha);
                                                                           else if(strcmp(checa_id->natureza, "IDENTIFICADOR") == 0)
                                                                                printa_erro(ERR_VARIABLE, checa_id->valor, $1->linha, $1->linha);
                                                                         };           

lista_argumentos: expressao                                              { $$ = $1; }
                | expressao ',' lista_argumentos                         { $$ = $1; asd_add_child($$, $3); };

// Adicionar inferência de tipo aqui
// (int, int) -> int
// (float, float) -> float
// (float, int) -> float
expressao: expressao TK_OC_OR prec6     { $$ = asd_new("|"); asd_add_child($$, $1); asd_add_child($$, $3);
                                          char* tipo = infere_tipo($1->tipo, $3->tipo);
                                          $$->tipo = tipo; };

expressao: prec6                        { $$ = $1; };

prec6: prec6 TK_OC_AND prec5            { $$ = asd_new("&"); asd_add_child($$, $1); asd_add_child($$, $3);
                                          char* tipo = infere_tipo($1->tipo, $3->tipo);
                                          $$->tipo = tipo; }; 

prec6: prec5                            { $$ = $1; };

prec5: prec5 TK_OC_EQ prec4             { $$ = asd_new("=="); asd_add_child($$, $1); asd_add_child($$, $3);
                                          char* tipo = infere_tipo($1->tipo, $3->tipo);
                                          $$->tipo = tipo; };

prec5: prec5 TK_OC_NE prec4             { $$ = asd_new("!="); asd_add_child($$, $1); asd_add_child($$, $3);
                                          char* tipo = infere_tipo($1->tipo, $3->tipo);
                                          $$->tipo = tipo; };

prec5: prec4                            { $$ = $1; };

prec4: prec4 '<' prec3                  { $$ = asd_new("<"); asd_add_child($$, $1); asd_add_child($$, $3);
                                          char* tipo = infere_tipo($1->tipo, $3->tipo);
                                          $$->tipo = tipo; };

prec4: prec4 '>' prec3                  { $$ = asd_new(">"); asd_add_child($$, $1); asd_add_child($$, $3);
                                          char* tipo = infere_tipo($1->tipo, $3->tipo);
                                          $$->tipo = tipo; };

prec4: prec4 TK_OC_LE prec3             { $$ = asd_new("<="); asd_add_child($$, $1); asd_add_child($$, $3);
                                          char* tipo = infere_tipo($1->tipo, $3->tipo);
                                          $$->tipo = tipo; };

prec4: prec4 TK_OC_GE prec3             { $$ = asd_new(">="); asd_add_child($$, $1); asd_add_child($$, $3);
                                          char* tipo = infere_tipo($1->tipo, $3->tipo);
                                          $$->tipo = tipo; };

prec4: prec3                            { $$ = $1; };

prec3: prec3 '+' prec2                  { $$ = asd_new("+"); asd_add_child($$, $1); asd_add_child($$, $3);
                                          char* tipo = infere_tipo($1->tipo, $3->tipo);
                                          $$->tipo = tipo; };

prec3: prec3 '-' prec2                  { $$ = asd_new("-"); asd_add_child($$, $1); asd_add_child($$, $3);
                                          char* tipo = infere_tipo($1->tipo, $3->tipo);
                                          $$->tipo = tipo; };

prec3: prec2                            { $$ = $1; };  

prec2: prec2 '*' prec1                  { $$ = asd_new("*"); asd_add_child($$, $1); asd_add_child($$, $3); 
                                          char* tipo = infere_tipo($1->tipo, $3->tipo);
                                          $$->tipo = tipo; };

prec2: prec2 '/' prec1                  { $$ = asd_new("/"); asd_add_child($$, $1); asd_add_child($$, $3);
                                          char* tipo = infere_tipo($1->tipo, $3->tipo);
                                          $$->tipo = tipo; };

prec2: prec2 '%' prec1                  { $$ = asd_new("%"); asd_add_child($$, $1); asd_add_child($$, $3);
                                          char* tipo = infere_tipo($1->tipo, $3->tipo);
                                          $$->tipo = tipo; };

prec2: prec1                            { $$ = $1; };

prec1:'!' prec1                         { $$ = asd_new("!"); if($2 != NULL) { asd_add_child($$, $2);}
                                          $$->tipo = $2->tipo; }
    | '-' prec1                         { $$ = asd_new("-"); if($2 != NULL) { asd_add_child($$, $2);} 
                                          $$->tipo = $2->tipo; };

prec1: '(' expressao ')'                { $$ = $2;};

// checar aqui também se undeclared, da mesma forma que na atribuição
prec1:TK_IDENTIFICADOR                  { $$ = asd_new($1->valor); 
                                          conteudo_tabela_simbolos_t *checa_id = busca_entrada_pilha(pilha_tabelas, $1->valor);
                                          if (checa_id == NULL)
                                            printa_erro(ERR_UNDECLARED, $1->valor, $1->linha, $1->linha);
                                          else if(strcmp(checa_id->natureza, "FUNCAO") == 0)
                                            printa_erro(ERR_FUNCTION, checa_id->valor, $1->linha, $1->linha);
                                          $$->tipo = checa_id->tipo; };

    | TK_LIT_INT                        { $$ = asd_new($1->valor); 
                                          $$->tipo = strdup("INT"); }

    | TK_LIT_FLOAT                      { $$ = asd_new($1->valor); 
                                          $$->tipo = strdup("FLOAT"); }
                                          
    | chamada_funcao                    { $$ = $1; };
                          
%%

void yyerror(char const *mensagem)
{
    fprintf(stderr, "Erro %s na linha: %d\n", mensagem, get_line_number());
}
