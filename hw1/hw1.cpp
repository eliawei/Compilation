#include "tokens.hpp"
#include <iostream>
#include <string>
using namespace std;


void showToken(string);
void inUpperCase(string);
void showString();
void check_string_errors();
void hex_error(string);
void showComment();
void errorUnclosedString();

int main()
{
	int token;
	while(token = yylex()) {
	// Your code here
	}
	return 0;
}

void showToken(string name){
	cout << yylineno << " " + name + " " + yytext << endl;
}

void inUpperCase(string lexeme){
	char token[yyleng+1];
	for(int i=0; i < yyleng ; i++){
		token[i] = lexeme[i] + ('A' - 'a');
	}
	token[yyleng] = '\0';
	showToken(token);
}

void showComment(){
	cout << yylineno << " COMMENT //" << endl;
}

void showString(){
	string str = "";
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
					str += '@';
					break;
				case 'x':{
					string hex_char = "";
					for (int j = i_lexeme + 2; j < i_lexeme + 4 && yytext[j] != '\"'; j++)
					{
						hex_char += yytext[j];
					}
				
					if(hex_char.size() < 2)
						hex_error(hex_char);
					try{
						int c = stoul(hex_char, nullptr, 16);
						if (c < 0 || c > 127){
							hex_error(hex_char);
						}
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
	string str2 = "";
	for (int i = 0; i < str.size(); i++)
	{
		if (str[i] == '@')
		{	
			break;
		}
		str2 += str[i];
		
	}
	cout << yylineno << " STRING ";
	cout << str2 << endl;	
}

void check_string_errors(){
	int index = 0;
	while(yytext[index]){
		if(yytext[index] == '\n'){
			errorUnclosedString();
		}
		index++;
	}
}

void errorUnclosedString(){
	cout << "Error unclosed string" << endl;
	exit(0);
}

void hex_error(string error){
	cout << "Error undefined escape sequence x" + error << endl;
	exit(0);
}

void unkown_char_error(){
	cout << "Error " << yytext << endl;
	exit(0);
}