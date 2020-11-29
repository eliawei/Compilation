%{
#include "parser.hpp"
#include "parser.tab.hpp"
#include "output.hpp"
using namespace output;
%}

%option yylineno
%option noyywrap

DIGIT           ([0-9])
LETTER          ([a-zA-Z])
RELOP_EQ        ("=="|"!=")
RELOP_REST      ("<"|">"|"<="|">=")
RELOP_IN        "in"
BINOP_PM        ("+"|"-")
BINOP_MD        ("*"|"/")
ID              {LETTER}({LETTER}|{DIGIT})*
NUM             ("0"|([1-9])({DIGIT})*)
STRING          (\"([^\n\r\"\\]|\\[rnt"\\])+\")
WHITESPACE      ([\n\r\t ])
LINE_COMMENT    "//"[^\r\n]*[ \r|\n|\r\n]?

%%

"void"                                          {yylval = new Token(yytext); return VOID;}
"int"                                           {yylval = new Token(yytext); return INT;}
"byte"                                          {yylval = new Token(yytext); return BYTE;}
"b"                                             {yylval = new Token(yytext); return B;}
"bool"                                          {yylval = new Token(yytext); return BOOL;}
"set"                                           {yylval = new Token(yytext); return SET;}
"and"                                           {yylval = new Token(yytext); return AND;}
"or"                                            {yylval = new Token(yytext); return OR;}
"not"                                           {yylval = new Token(yytext); return NOT;}
"true"                                          {yylval = new Token(yytext); return TRUE;}
"false"                                         {yylval = new Token(yytext); return FALSE;}
"return"                                        {yylval = new Token(yytext); return RETURN;}
"if"                                            {yylval = new Token(yytext); return IF;}
"else"                                          {yylval = new Token(yytext); return ELSE;}
"while"                                         {yylval = new Token(yytext); return WHILE;}
"break"                                         {yylval = new Token(yytext); return BREAK;}
"continue"                                      {yylval = new Token(yytext); return CONTINUE;}
";"                                             {yylval = new Token(yytext); return SC;}
","                                             {yylval = new Token(yytext); return COMMA;}
"("                                             {yylval = new Token(yytext); return LPAREN;}
")"                                             {yylval = new Token(yytext); return RPAREN;}
"{"                                             {yylval = new Token(yytext); return LBRACE;}
"}"                                             {yylval = new Token(yytext); return RBRACE;}
"["                                             {yylval = new Token(yytext); return LBRACKET;}
"]"                                             {yylval = new Token(yytext); return RBRACKET;}
"="                                             {yylval = new Token(yytext); return ASSIGN;}
".."                                            {yylval = new Token(yytext); return DOTS;}
{RELOP_EQ}                                      {yylval = new Token(yytext); return RELOP_EQ;}
{RELOP_REST}                                    {yylval = new Token(yytext); return RELOP_REST;}
{RELOP_IN}                                      {yylval = new Token(yytext); return RELOP_IN;}
{BINOP_PM}                                      {yylval = new Token(yytext); return BINOP_PM;}
{BINOP_MD}                                      {yylval = new Token(yytext); return BINOP_MD;}
{ID}                                            {yylval = new Token(yytext); return ID;}
{NUM}                                           {yylval = new Token(yytext); return NUM;}
{STRING}                                        {yylval = new Token(yytext); return STRING;}
{WHITESPACE}                                    ;
{LINE_COMMENT}                                  ;
.                                               {errorLex(yylineno); exit(0);}

%%

