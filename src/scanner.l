
%{

	#include "parser.tab.h"
	#include "../include/aux.h"
	extern int yylineno;
	extern void *arvore;
	extern int get_line_number();

%}

%option yylineno

%%

int         { return TK_PR_INT; }
float       { return TK_PR_FLOAT; }
if         	{ return TK_PR_IF; }
else       	{ return TK_PR_ELSE; }
while       { return TK_PR_WHILE; }
return      { return TK_PR_RETURN; }

"<="		{ return TK_OC_LE; }
">="		{ return TK_OC_GE; }
"=="		{ return TK_OC_EQ; }
"!="		{ return TK_OC_NE; }
"&"			{ return TK_OC_AND; }
"|"			{ return TK_OC_OR; }

[!*/%+\-<>{}()=,;]	{ return yytext[0]; }

[0-9]+				{ yylval.valor_lexico = cria_valor_lexico(get_line_number(), "LITERAL", yytext);
                      return TK_LIT_INT; }

[0-9]*[.][0-9]+		{ yylval.valor_lexico = cria_valor_lexico(get_line_number(), "LITERAL", yytext);
                      return TK_LIT_FLOAT; }

[a-z_][a-z_0-9]*	{ yylval.valor_lexico = cria_valor_lexico(get_line_number(), "IDENTIFICADOR", yytext);
					  return TK_IDENTIFICADOR; }

[ \t\n]	    		{ }
"//".*				{ }

.					{ return TK_ERRO; }

%%

int get_line_number(void){

	return yylineno;
}
