%{
#include <iostream>
#include <string>
using namespace std;
void showToken(string);
void inUpperCase(string);
void showString();
void check_string_errors();
void hex_error(string);
void unkown_char_error();
void showComment();
void errorUnclosedString();
%}

%option yylineno
%option noyywrap

WHITESPACE 		([\n\r\t ])
LETTER 			([a-zA-Z])
DIGIT 			([0-9])

RESERVED_WORDS ("void"|"int"|"byte"|"b"|"bool"|"and"|"or"|"not"|"true"|"false"|"return"|"if"|"else"|"while"|"break"|"continue")
SC 				";"
COMMA 			","
LPAREN 		    "("
RPAREN 			")"
LBRACE 			"{"
RBRACE 			"}"
ASSIGN 			"="
RELOP 			("=="|"!="|"<"|">"|"<="|">=")
BINOP 			("+"|"-"|"*"|"/")
COMMENT 		"//".*
ID 				{LETTER}({LETTER}|{DIGIT})*
NUM 			("0"|[1-9]({DIGIT})*)
PRINTABLE 		(" "|"!"|[#-[]|[]-~]|"\\\\"|"\\\""|"\\"([a-zA-Z0-9])+)
STRING 			"\""({PRINTABLE})*"\""
QMARK           "\""

%%

{RESERVED_WORDS} 	inUpperCase(yytext);
{SC} 				showToken("SC");
{COMMA} 			showToken("COMMA");
{LPAREN} 			showToken("LPAREN");
{RPAREN} 			showToken("RPAREN");
{LBRACE} 			showToken("LBRACE");
{RBRACE} 			showToken("RBRACE");
{ASSIGN} 			showToken("ASSIGN");
{RELOP} 			showToken("RELOP");
{BINOP} 			showToken("BINOP");
{ID} 				showToken("ID");
{NUM} 				showToken("NUM");
{STRING} 			showString();
{COMMENT}           showComment();
{QMARK}             errorUnclosedString();
{WHITESPACE}        ;
.					unkown_char_error();

%%

