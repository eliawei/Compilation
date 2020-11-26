%{
#include <iostream>
#include <string>
using namespace std;
void showToken(string);
void inUpperCase(char*);
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
COMMENT 		("//"(.)*)
ID 				{LETTER}({LETTER}|{DIGIT})*
<<<<<<< HEAD
NUM 			(([1-9])({DIGIT})*|0)
PRINTABLE 		(" "|"!"|[#-~]|{WHITESPACE}|"\\\"")
STRING 			\"([^\n\r\"\\]|\\[rnt"\\]|\\x[0-9]+)+\"
=======
NUM 			("0"|[1-9]({DIGIT})*)
PRINTABLE 		(" "|"\t"|"!"|[#-[]|[]-~]|"\\".)
STRING 			"\""({PRINTABLE})*"\""
QMARK           "\""
>>>>>>> 294777583b1021c532a34da2e0d6db0d6383b3f0

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
{COMMENT}           showToken("COMMENT");
{BINOP} 			showToken("BINOP");
{ID} 				showToken("ID");
{NUM} 				showToken("NUM");
{STRING} 			showString();
<<<<<<< HEAD
{WHITESPACE}          ;
=======
{COMMENT}           showComment();
{QMARK}             errorUnclosedString();
{WHITESPACE}        ;
>>>>>>> 294777583b1021c532a34da2e0d6db0d6383b3f0
.					unkown_char_error();

%%

