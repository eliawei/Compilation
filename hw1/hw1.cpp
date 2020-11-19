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
	//cout << "yytext" << yytext <<endl;
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
					if(i_lexeme == yyleng - 2){
						cout << "in line 59" << endl;
						errorUnclosedString();
					}
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
					if(yytext[i_lexeme + 2] != '\"')
						hex_char += yytext[i_lexeme + 2];
					if(yytext[i_lexeme + 3] && yytext[i_lexeme + 3] != '\"')
						hex_char += yytext[i_lexeme + 3];
					if(hex_char.size() < 2)
						hex_error(hex_char);
					try{
						char c = stoul(hex_char, nullptr, 16);
						if ((c < 32 || c > 126) && c != '\t' && c != '\r' && c != '\n'){
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
			cout << "in line 122" << endl;
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