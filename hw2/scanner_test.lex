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

"void"                                print_test();//{return VOID;}
"int"                                 print_test();//{return INT;}
"byte"                                print_test();//{return BYTE;}
"b"                                   print_test();//{return B;}
"bool"                                print_test();//{return BOOL;}
"set"                                 print_test();//{return SET;}
"and"                                 print_test();//{return AND;}
"or"                                  print_test();//{return OR;}
"not"                                 print_test();//{return NOT;}
"true"                                print_test();//{return TRUE;}
"false"                               print_test();//{return FALSE;}
"return"                              print_test();//{return RETURN;}
"if"                                  print_test();//{return IF;}
"else"                                print_test();//{return ELSE;}
"while"                               print_test();//{return WHILE;}
"break"                               print_test();//{return BREAK;}
"continue"                            print_test();//{return CONTINUE;}
";"                                   print_test();//{return SC;}
","                                   print_test();//{return COMMA;}
"("                                  print_test();//{return LPAREN;}
")"                                  print_test();//{return RPAREN;}
"{"                                  print_test();//{return LBRACE;}
"}"                                  print_test();//{return RBRACE;}
"["                                  print_test();//{return LBRACKET;}
"]"                                   print_test();//{return RBRACKET;}
"="                                   print_test();//{return ASSIGN;}
"==" | "!=" | "<" | ">" | "<=" | ">=" | "in"      print_test();//{return RELOP;}
"+" | "-" | "*" | "/"                   print_test();//{return BINOP;}
".."                                  print_test();//{return DOTS;}
([a-zA-Z]([a-zA-Z0-9])*)                print_test();cout<<"id"<<endl;//{return ID;}
"0" | [1-9]([0-9])*                    print_test();//{return NUM;}
\"([^\n\r\"\\]|\\[rnt"\\])+\"       print_test();//{return STRING;}
\/\/[^\r\n]*[ \r|\n|\r\n]?          ;
([\n\r\t\.\s ])                                   ;

%%

void print_test(){
    cout<<yylineno<<": "<<yytext<<endl;
}