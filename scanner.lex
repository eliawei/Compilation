%{
#include <stdio.h>
void showToken(char*);
char* inUpperCase(char*);
%}

%option yylineno

WHITESPACE ([\n\r\t ])
LETTER ([a-zA-Z])

RESERVED_WORDS ("void"|"int"|"byte"|"b"|"bool"|"and"|"or"|"not"|"true"|"false"|"return"|"if"|"else"|"while"|"break"|"continue")
SC ";"
COMMA ","
RLPAREN "("
RPAREN ")"
LBRACE "{"
RBRACE "}"
ASSIGN "="
RELOP ("=="|"!="|"<"|">"|"<="|">=")
BINOP ("+"|"-"|"*"|"/")
COMMENT "//".*
ID {LETTER}({LETTER}|{DIGIT})*
NUM [1-9]({DIGIT})*
PRINTABLE (" "|"!"|[#-/]|[1-[]|[]-~]|"\x"{DIGIT}{DIGIT})
STRING "\""({PRINTABLE})*"\""

%%

{BYTE} showToken(yytext);
{RESERVED_WORDS} inUpperCase(yytext);

%%

void showToken(char* name){
	printf("%d %s %s", yylineno, name, yytext);
}

void inUpperCase(char* lexeme){
	char token[yyleng+1];
	for(i=0; i < yyleng ; i++){
		token[i] = lexeme[i] + ('A' - 'a');
	}
	showToken(token);
}



