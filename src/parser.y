%{
    #include <stdio.h>
    int yylex(void);
    extern int get_line_number();
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

programa: lista_de_funcoes | ;                                                                      
lista_de_funcoes: lista_de_funcoes funcao | funcao;                                                
funcao: cabecalho corpo;                                                                            
cabecalho: TK_IDENTIFICADOR '=' lista_parametros '>' tipo_variavel;                                 
lista_parametros: lista_parametros TK_OC_OR parametro | parametro | ;                               
parametro: TK_IDENTIFICADOR '<' '-' tipo_variavel                                                   
tipo_variavel: TK_PR_INT | TK_PR_FLOAT;                                                            
corpo: bloco_comandos;                                                                              

bloco_comandos: '{' lista_comandos '}' | '{' '}';

lista_comandos: comando | comando lista_comandos;

comando: bloco_comandos |                                                                           
         declaracao_variavel ';' | 
         retorno ';' | 
         atribuicao ';' | 
         chamada_funcao ';' |
         controle_fluxo;

declaracao_variavel: tipo_variavel lista_variaveis ;                                                
                     lista_variaveis: lista_variaveis ',' variavel | variavel;                      
                     variavel: TK_IDENTIFICADOR | TK_IDENTIFICADOR TK_OC_LE literal;                
                     literal: TK_LIT_INT | TK_LIT_FLOAT;                                            

retorno: TK_PR_RETURN expressao;                                                                    
                                                     
controle_fluxo: ifs | whiles;                                                                       

ifs: TK_PR_IF '(' expressao ')' bloco_comandos |                                                   
     TK_PR_IF '(' expressao ')' bloco_comandos TK_PR_ELSE bloco_comandos;                           

whiles: TK_PR_WHILE '(' expressao ')' bloco_comandos;                                               

atribuicao: TK_IDENTIFICADOR '=' expressao;                                                         

chamada_funcao: TK_IDENTIFICADOR '(' lista_argumentos ')';
                lista_argumentos: lista_argumentos ',' expressao | expressao;


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
prec1: '-' fator;
prec1: '!' fator;
prec1: fator;
fator: '(' expressao ')';
fator: TK_IDENTIFICADOR | TK_LIT_INT| TK_LIT_FLOAT;

%%

void yyerror(char const *mensagem)
{
    fprintf(stderr, "Erro %s na linha: %d\n", mensagem, get_line_number());
}
