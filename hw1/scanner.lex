%{
#include <iostream>
#include <string>
using namespace std;
void showToken(string);
void inUpperCase(string);
void showString();
void check_string_errors();
void hex_error(string);
%}

%option yylineno
%option noyywrap

WHITESPACE 		([\n\r\t ])
LETTER 			([a-zA-Z])
DIGIT 			([0-9])

RESERVED_WORDS ("void"|"int"|"byte"|"b"|"bool"|"and"|"or"|"not"|"true"|"false"|"return"|"if"|"else"|"while"|"break"|"continue")
SC 				";"
COMMA 			","
RLPAREN 		"("
RPAREN 			")"
LBRACE 			"{"
RBRACE 			"}"
ASSIGN 			"="
RELOP 			("=="|"!="|"<"|">"|"<="|">=")
BINOP 			("+"|"-"|"*"|"/")
COMMENT 		"//".*
ID 				{LETTER}({LETTER}|{DIGIT})*
NUM 			[1-9]({DIGIT})*
PRINTABLE 		(" "|"!"|[#-/]|[1-~])
PRINTABLE 		(" "|"!"|[#-~]|"\\\"")
STRING 			"\""({PRINTABLE})*"\""

%%

{RESERVED_WORDS} 	inUpperCase(yytext);
{SC} 				showToken("SC");
{COMMA} 			showToken("COMMA");
{RLPAREN} 			showToken("RLPAREN");
{LBRACE} 			showToken("LBRACE");
{RBRACE} 			showToken("RBRACE");
{ASSIGN} 			showToken("ASSIGN");
{RELOP} 			showToken("RELOP");
{BINOP} 			showToken("BINOP");
{ID} 				showToken("ID");
{NUM} 				showToken("NUM");
{STRING} 			showString();


%%

void showToken(string name){
	cout << yylineno + " " + name + " " + yytext << endl;
}

void inUpperCase(string lexeme){
	char token[yyleng+1];
	for(int i=0; i < yyleng ; i++){
		token[i] = lexeme[i] + ('A' - 'a');
	}
	showToken(token);
}

void showString(){
	check_string_errors();
	string str;
	int i_lexeme = 1;
	char cur_char = yytext[i_lexeme];
	while(cur_char != '\"'){
		if(cur_char != '\\'){
			str += cur_char;
	        i_lexeme++;
		}
		else{
			switch(yytext[i_lexeme+1]){
				case '\\':
					str += '\\';
					break;
				case '\"':
					str += '\"';
					break;
				case 'n':
					str += '\n';
					break;
				case 'r':
					str += '\r';
					break;	
				case 't':
					str += '\t';
					break;
				case '0':
					str += '\0';
					break;
				case 'x':{
					string hex_char = "";
					if(yytext[i_lexeme + 2] != '\"')
						hex_char += yytext[i_lexeme + 2];
					if(yytext[i_lexeme + 3] && yytext[i_lexeme + 3] != '\"')
						hex_char += yytext[i_lexeme + 3];
					if(hex_char.size() < 2)
						hex_error(hex_char);
					try{
						char c = stoul(hex_char, nullptr, 16);
						str += c;
						i_lexeme += 2;
					}
					catch(const invalid_argument& ia){
						hex_error(hex_char);
					}
       }
					break;
				default:{
					char c = yytext[i_lexeme+1];
					cout << "Error undefined escape sequence " << c << endl;
					exit(0);
		    }
      }
			i_lexeme += 2;
	  }
		cur_char = yytext[i_lexeme];	
	}
	cout << yylineno + " STRING " + str << endl;	
}

void check_string_errors(){
	int index = 0;
	if(yytext[yyleng - 2] == '\\'){
		cout << "Error unclosed string" << endl;
		exit(0);
	}
	
	while(yytext[index]){
		if(yytext[index] == '\n'){
			cout << "Error unclosed string" << endl;
			exit(0);
		}
		index++;
	}
}

void hex_error(string error){
	cout << "Error undefined escape sequence x" + error << endl;
	exit(0);
}

int main(){
	yylex();
	return 0;
}