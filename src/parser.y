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
{ 
  tabela_simbolos_t *tabela_global = cria_tabela_simbolos();
  pilha_tabelas = cria_pilha(tabela_global); 
};


abre_escopo_func:                                                      
{ 
  tabela_simbolos_t *tabela_func = cria_tabela_simbolos();
  empilhar(&pilha_tabelas, tabela_func); 
};


abre_escopo_aninhado:                                                  
{ 
  tabela_simbolos_t *tabela_aninhada = cria_tabela_simbolos();
  empilhar(&pilha_tabelas, tabela_aninhada); 
};


fecha_escopo_global:                                                   
{ 
  destroi_pilha(pilha_tabelas); 
};


fecha_escopo_func:                                                     
{ 
  desempilhar(&pilha_tabelas);
};


fecha_escopo_aninhado:                                                 
{ 
  desempilhar(&pilha_tabelas);
};


programa: lista_de_funcoes { 
  $$ = $1; 
  arvore = $$;
  // printf("%s\n", $$->codigo);
}
        |  { 
  $$ = NULL; 
  arvore = $$;
};


lista_de_funcoes: funcao fecha_escopo_func lista_de_funcoes { 
  $$ = $1; 
  if($3 != NULL) 
    asd_add_child($$, $3);
}          
                | funcao fecha_escopo_func { 
  $$ = $1; 
};         


funcao: cabecalho corpo	{ 
  $$ = $1; 
  if($2 != NULL) 
    asd_add_child($$, $2); 
  printf("%s\n", $2->codigo);
};	    


// -----------------------------------------------------------------------------------
// Escopo da funcao 
// abre apos o '=' e nao apos as primeiras chaves	
// -----------------------------------------------------------------------------------	


cabecalho: TK_IDENTIFICADOR '=' abre_escopo_func lista_parametros '>' tipo_variavel { 
  $$ = asd_new($1->valor);
  conteudo_tabela_simbolos_t *entrada = cria_entrada($1->linha, "FUNCAO", $6->tipo, $1->valor);
  adiciona_entrada(pilha_tabelas->prox->tabela_simbolos, entrada);
};    


lista_parametros: parametro TK_OC_OR lista_parametros { 
  $$ = NULL; 
}
                | parametro { 
  $$ = NULL; 
}
                | { 
  $$ = NULL; 
};            


parametro: TK_IDENTIFICADOR '<' '-' tipo_variavel  { 
  $$ = NULL; 
  conteudo_tabela_simbolos_t *entrada = cria_entrada($1->linha, "IDENTIFICADOR", $4->tipo, $1->valor);
  adiciona_entrada(pilha_tabelas->tabela_simbolos, entrada);
};


tipo_variavel: TK_PR_INT { 
  $$ = cria_tipo("INT"); 
} 
             | TK_PR_FLOAT { 
  $$ = cria_tipo("FLOAT"); 
};   


corpo: bloco_comandos_func { 
  $$ = $1;
};    


bloco_comandos_func: '{' lista_comandos '}' { 
  $$ = $2;
}
                   | '{' '}' { 
  $$ = NULL; 
};


bloco_comandos: '{' abre_escopo_aninhado lista_comandos fecha_escopo_aninhado '}' { 
  $$ = $3; 
}
              | '{' abre_escopo_aninhado fecha_escopo_aninhado'}' { 
  $$ = NULL; 
};


// -----------------------------------------------------------------------------------
// Escopo aninhado
// -----------------------------------------------------------------------------------


lista_comandos: comando { 
  $$ = $1;    // Tratamento para as variaveis não inicializadas
}       
              | comando lista_comandos { 
  $$ = $1; 
  if(($$ != NULL) && ($2 != NULL))
    asd_add_child($$, $2);
  else if ($2 != NULL ) 
    $$ = $2;
  $$->codigo = concatena2($1->codigo, $2->codigo);  // Essa linha realiza a concatenação de multiplos comandos
} 
              | declaracao_variavel ';' lista_comandos { 
  $$ = $1; 
  if ($$ != NULL) { 
    if($3 != NULL) 
      asd_add_child(corrige_ordem_filhos($$), $3); 
  } 
  else $$ = $3; 
}
              | declaracao_variavel ';' { 
  $$ = $1; 
};


comando: bloco_comandos ';' { 
  $$ = $1; 
}
       | retorno ';' { 
  $$ = $1; 
}
       | atribuicao ';' { 
  $$ = $1;
}
       | chamada_funcao ';' { 
  $$ = $1; 
}
       | controle_fluxo ';' { 
  $$ = $1; 
};

declaracao_variavel: tipo_variavel lista_variaveis { 
  $$ = $2;
  atribui_tipo(pilha_tabelas->tabela_simbolos, $1->tipo);
  calcula_deslocamentos(pilha_tabelas->tabela_simbolos);
};   
                      
lista_variaveis: variavel ',' lista_variaveis { 
  if($1 == NULL) 
    $$ = $3; 
  else { 
    $$ = $1; 
    if ($3 != NULL) 
      asd_add_child($$, $3); 
  }
};       
               | variavel { 
  $$ = $1; 
};     


variavel: TK_IDENTIFICADOR { 
  $$ = NULL;
  conteudo_tabela_simbolos_t *entrada = cria_entrada($1->linha, "IDENTIFICADOR", "ATRIBUIR DEPOIS", $1->valor);
  adiciona_entrada(pilha_tabelas->tabela_simbolos, entrada); 
}
        | TK_IDENTIFICADOR TK_OC_LE literal {
  $$ = asd_new("<="); asd_add_child($$, asd_new($1->valor)); asd_add_child($$, $3); 
  conteudo_tabela_simbolos_t *entrada = cria_entrada($1->linha, "IDENTIFICADOR", "ATRIBUIR DEPOIS", $1->valor);
  adiciona_entrada(pilha_tabelas->tabela_simbolos, entrada); 
};


literal: TK_LIT_INT { 
  $$ = asd_new($1->valor);
  $$->tipo = strdup("INT"); 
}  
       | TK_LIT_FLOAT { 
  $$ = asd_new($1->valor);
  $$->tipo = strdup("FLOAT"); 
};                                         


retorno: TK_PR_RETURN expressao { 
  $$ = asd_new("return"), asd_add_child($$, $2); 
};                                                                    


controle_fluxo: ifs {
  $$ = $1; 
}
              | whiles { 
  $$ = $1; 
};                                                                


ifs: TK_PR_IF '(' expressao ')' bloco_comandos { 
  $$ = asd_new("if");
  asd_add_child($$,$3); 
  if($5 != NULL) 
    asd_add_child($$, $5);

  ret_instr_t ret = gera_codigo_if($3, $5);
  $$->codigo = ret.codigo;
  $$->local = ret.temporario;
}

   | TK_PR_IF '(' expressao ')' bloco_comandos TK_PR_ELSE bloco_comandos { 
  $$ = asd_new("if"); 
  asd_add_child($$,$3); 
  if($5 != NULL) 
    asd_add_child($$, $5);  
  if($7 != NULL) 
    asd_add_child($$, $7); 

  ret_instr_t ret = gera_codigo_if_else($3, $5, $7);
  $$->codigo = ret.codigo;
  $$->local = ret.temporario;
};                        

whiles: TK_PR_WHILE '(' expressao ')' bloco_comandos { 
  $$ = asd_new("while"); 
  asd_add_child($$,$3);
  if($5 != NULL)
    asd_add_child($$, $5); 
  
  ret_instr_t ret = gera_codigo_while($3, $5);
  $$->codigo = ret.codigo;
  $$->local = ret.temporario;
};                                            

// --------------------------------------------------------------

atribuicao: TK_IDENTIFICADOR '=' expressao { 
  $$ = processa_atribuicao($1, $3, pilha_tabelas);

  ret_instr_t ret = gera_codigo_atribuicao($1, $3, pilha_tabelas);
  $$->codigo = ret.codigo;
};                                     


chamada_funcao: TK_IDENTIFICADOR '(' lista_argumentos ')' { 
  $$ = processa_chamada_funcao($1, $3, pilha_tabelas);
};           


lista_argumentos: expressao { 
  $$ = $1; 
}
                | expressao ',' lista_argumentos { 
  $$ = $1; 
  asd_add_child($$, $3); 
};


expressao: expressao TK_OC_OR prec6 { 
  $$ = cria_nodo_expressao("|", $1, $3);
  
  ret_instr_t ret = gera_codigo_exp_bin("or", $1, $3);
  $$->codigo = ret.codigo;
  $$->local = ret.temporario;
};


expressao: prec6 { 
  $$ = $1; 
};


prec6: prec6 TK_OC_AND prec5 { 
  $$ = cria_nodo_expressao("&", $1, $3);
  
  ret_instr_t ret = gera_codigo_exp_bin("and", $1, $3);
  $$->codigo = ret.codigo;
  $$->local = ret.temporario;
}; 


prec6: prec5 { 
  $$ = $1; 
};


prec5: prec5 TK_OC_EQ prec4 { 
  $$ = cria_nodo_expressao("==", $1, $3);
  
  ret_instr_t ret = gera_codigo_exp_bin("cmp_EQ", $1, $3);
  $$->codigo = ret.codigo;
  $$->local = ret.temporario;
};


prec5: prec5 TK_OC_NE prec4 { 
  $$ = cria_nodo_expressao("!=", $1, $3);
  
  ret_instr_t ret = gera_codigo_exp_bin("cmp_NE", $1, $3);
  $$->codigo = ret.codigo;
  $$->local = ret.temporario;
};


prec5: prec4 { 
  $$ = $1; 
};


prec4: prec4 '<' prec3 { 
  $$ = cria_nodo_expressao("<", $1, $3);
  
  ret_instr_t ret = gera_codigo_exp_bin("cmp_LT", $1, $3);
  $$->codigo = ret.codigo;
  $$->local = ret.temporario;
};


prec4: prec4 '>' prec3 { 
  $$ = cria_nodo_expressao(">", $1, $3);
  
  ret_instr_t ret = gera_codigo_exp_bin("cmp_GT", $1, $3);
  $$->codigo = ret.codigo;
  $$->local = ret.temporario;
};


prec4: prec4 TK_OC_LE prec3 { 
  $$ = cria_nodo_expressao("<=", $1, $3);
  
  ret_instr_t ret = gera_codigo_exp_bin("cmp_LE", $1, $3);
  $$->codigo = ret.codigo;
  $$->local = ret.temporario;
};


prec4: prec4 TK_OC_GE prec3 { 
  $$ = cria_nodo_expressao(">=", $1, $3);
  
  ret_instr_t ret = gera_codigo_exp_bin("cmp_GE", $1, $3);
  $$->codigo = ret.codigo;
  $$->local = ret.temporario;
};


prec4: prec3 { 
  $$ = $1; 
};


prec3: prec3 '+' prec2 { 
  $$ = cria_nodo_expressao("+", $1, $3);
  
  ret_instr_t ret = gera_codigo_exp_bin("add", $1, $3);
  $$->codigo = ret.codigo;
  $$->local = ret.temporario;
};


prec3: prec3 '-' prec2 { 
  $$ = cria_nodo_expressao("-", $1, $3);
  
  ret_instr_t ret = gera_codigo_exp_bin("sub", $1, $3);
  $$->codigo = ret.codigo;
  $$->local = ret.temporario;
};


prec3: prec2 { 
  $$ = $1; 
};  


prec2: prec2 '*' prec1 { 
  $$ = cria_nodo_expressao("*", $1, $3);

  ret_instr_t ret = gera_codigo_exp_bin("mult", $1, $3);
  $$->codigo = ret.codigo;
  $$->local = ret.temporario;
};


prec2: prec2 '/' prec1 { 
  $$ = cria_nodo_expressao("/", $1, $3);
  
  ret_instr_t ret = gera_codigo_exp_bin("div", $1, $3);
  $$->codigo = ret.codigo;
  $$->local = ret.temporario;
};


prec2: prec2 '%' prec1 { 
  $$ = cria_nodo_expressao("%", $1, $3);
};


prec2: prec1 { 
  $$ = $1; 
};


prec1:'!' prec1 { 
  $$ = cria_nodo_expressao_unaria("!", $2);

  ret_instr_t ret = gera_codigo_exp_un("!", $2);
  $$->codigo = ret.codigo;
  $$->local = ret.temporario;
}
    | '-' prec1 { 
  $$ = cria_nodo_expressao_unaria("-", $2);

  ret_instr_t ret = gera_codigo_exp_un("-", $2);
  $$->codigo = ret.codigo;
  $$->local = ret.temporario;
};


prec1: '(' expressao ')' { 
  $$ = $2;
};


prec1:TK_IDENTIFICADOR { 
  $$ = processa_expressao($1, pilha_tabelas);

  ret_instr_t ret = gera_codigo_expressao($1, pilha_tabelas);
  $$->codigo = ret.codigo;
  $$->local = ret.temporario;
};

    | TK_LIT_INT { 
  $$ = asd_new($1->valor); 
  $$->tipo = strdup("INT"); 

  ret_instr_t ret = gera_codigo_expressao($1, pilha_tabelas);
  $$->codigo = ret.codigo;
  $$->local = ret.temporario;
}

    | TK_LIT_FLOAT { 
  $$ = asd_new($1->valor); 
  $$->tipo = strdup("FLOAT"); 
}
                                         
    | chamada_funcao { 
  $$ = $1;
};
                          
%%

void yyerror(char const *mensagem)
{
    fprintf(stderr, "Erro %s na linha: %d\n", mensagem, get_line_number());
}

