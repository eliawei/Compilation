%{

/* Declarations section */
#include <stdio.h>
#include "parser.hpp"
#include "parser.tab.hpp"
#include "hw3_output.hpp"

using namespace output;

%}

%option yylineno
%option noyywrap
DIGIT   		([0-9])
POS_DIGIT       ([1-9])
LETTER  		([a-zA-Z])
WHITESPACE		([\t\r\n ])
OTHER_RELOP     ("<"|">"|"<="|">=")
EQ_RELOP        ("=="|"!=")
MUL_BINOP       "*"|"/"
ADD_BINOP       "+"|"-"
STRING          ["]([^\n\r\"\\]|\\[rnt"\\])+["]
LPAREN          "("
RPAREN          ")"
LBRACE          "{"
RBRACE          "}"
LINE_COMMENT    "//"[^\r\n]*[ \r|\n|\r\n]?



%%

void              			    {yylval=new Token(yytext); return VOID;}
int                             {yylval=new Token(yytext); return INT;}
byte                            {yylval=new Token(yytext); return BYTE;}
b                               {yylval=new Token(yytext); return B;}
bool                            {yylval=new Token(yytext); return BOOL;}
and                             {yylval=new Token(yytext); return AND;}
or                              {yylval=new Token(yytext); return OR;}
not                             {yylval=new Token(yytext); return NOT;}
true                            {yylval=new Token(yytext); return TRUE;}
false                           {yylval=new Token(yytext); return FALSE;}
return                          {yylval=new Token(yytext); return RETURN;}
if                              {yylval=new Token(yytext); return IF;}
else                            {yylval=new Token(yytext); return ELSE;}
while                           {yylval=new Token(yytext); return WHILE;}
break                           {yylval=new Token(yytext); return BREAK;}
continue                        {yylval=new Token(yytext); return CONTINUE;}
;                               {yylval=new Token(yytext); return SC;}
,                               {yylval=new Token(yytext); return COMMA;}
{LPAREN}                        {yylval=new Token(yytext); return LPAREN;}
{RPAREN}                        {yylval=new Token(yytext); return RPAREN;}
{LBRACE}                        {yylval=new Token(yytext); return LBRACE;}
{RBRACE}                        {yylval=new Token(yytext); return RBRACE;}
=                               {yylval=new Token(yytext); return ASSIGN;}
{OTHER_RELOP}                   {yylval=new Token(yytext); return OTHER_RELOP;}
{EQ_RELOP}                      {yylval=new Token(yytext); return EQ_RELOP;}
{MUL_BINOP}                     {yylval=new Token(yytext); return MUL_BINOP;}
{ADD_BINOP}                     {yylval=new Token(yytext); return ADD_BINOP;}
{LETTER}({LETTER}|{DIGIT})*     {yylval=new Token(yytext); return ID;}
0|({POS_DIGIT}{DIGIT}*)         {yylval=new Token(yytext); return NUM;}
{STRING}                        {yylval=new Token(yytext); return STRING;}
{LINE_COMMENT}                  ;
{WHITESPACE}                    ;
.		                        {errorLex(yylineno); exit(1);};

%%
