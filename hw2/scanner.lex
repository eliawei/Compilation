%{
#include <iostream>
#include <string>
#include "parser.tab.hpp"
using namespace std;
void print_test();
%}

%option yylineno
%option noyywrap

%%

"void"                                          {return VOID;}
"int"                                           {return INT;}
"byte"                                          {return BYTE;}
"b"                                             {return B;}
"bool"                                          {return BOOL;}
"set"                                           {return SET;}
"and"                                           {return AND;}
"or"                                            {return OR;}
"not"                                           {return NOT;}
"true"                                          {return TRUE;}
"false"                                         {return FALSE;}
"return"                                        {return RETURN;}
"if"                                            {return IF;}
"else"                                          {return ELSE;}
"while"                                         {return WHILE;}
"break"                                         {return BREAK;}
"continue"                                      {return CONTINUE;}
";"                                             {return SC;}
","                                             {return COMMA;}
"("                                             {return LPAREN;}
")"                                             {return RPAREN;}
"{"                                             {return LBRACE;}
"}"                                             {return RBRACE;}
"["                                             {return LBRACKET;}
"]"                                             {return RBRACKET;}
"="                                             {return ASSIGN;}
"==" | "!=" | "<" | ">" | "<=" | ">=" | "in"    {return RELOP;}
"+" | "-" | "*" | "/"                           {return BINOP;}
".."                                            {return DOTS;}
([a-zA-Z]([a-zA-Z0-9])*)                        {return ID;}
"0" | [1-9]([0-9])*                             {return NUM;}
\"([^\n\r\"\\]|\\[rnt"\\])+\"                   {return STRING;}
\/\/[^\r\n]*[ \r|\n|\r\n]?                      {;}
([\n\r\t\.\s ])                                 {;}

%%

void print_test(){
    cout<<yylineno<<": "<<yytext<<endl;
}