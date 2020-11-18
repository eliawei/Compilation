#include "tokens.hpp"
#include <iostream>
#include <string>
using namespace std;


void showToken(string);
void inUpperCase(char*);
void showString();
void check_string_errors();
void hex_error(string);

int main()
{
	int token;
	while(token = yylex()) {
	// Your code here
	}
	return 0;
	// inUpperCase("add");
}

void showToken(string name){
	string text=yytext;
	if(name == "COMMENT"){
		text = "//";
	}
	cout << yylineno << " " + name + " " + text << endl;
}

void inUpperCase(char* lexeme){
	string token;
	for (char* t = lexeme; *t != '\0'; t++) {
        token += char(*t + ('A'-'a'));
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
					// str += char(10);
					str += '\n';
					break;
				case 'r':
					// str += char(13);
					str += '\r';
					break;	
				case 't':
					// str += char(9);
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
	
	cout << yylineno << " STRING " + str2 << endl;	
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

void unkown_char_error(){
	cout << "Error " << yytext << endl;
}