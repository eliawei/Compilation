#ifndef _SEMANTIC_HPP
#define _SEMANTIC_HPP

#include <vector>
#include <string>
#include <iostream>
#include "parser.hpp"
using namespace std;

namespace semantic{
    static int i = 0;
    bool AssignValid(Token* type, Expression* exp){
        string type1 = type->token;
        string type2 = exp->type;
        if(type1 == type2 || type1 == "INT" && type2 == "BYTE") return true;
        return false;
    }
    // func_args = "a,b,c", given_args = (value ,next)
    bool FuncsArgsTypesValid(string func_args, ExpList* given_args){
        if(!given_args){
            return false;
        }
        ExpList* curr = given_args;

        string s = func_args;
        string delimiter = ",";
        size_t pos = 0;
        string type;
        while ((pos = s.find(delimiter)) != string::npos && curr) {
            type = s.substr(0, pos);
            if(!(type.compare(curr->value->type) == 0 || (type == "INT" && curr->value->type == "BYTE"))) return false;
            s.erase(0, pos + delimiter.length());
            curr = curr->next;
        }
        return true;
    }
    void errorPrototypeMismatchAux(int lineno, Node* id1, Node* argTypes){
        string id = ((Token*)id1)->token;
        ExpList* args = (ExpList*)(argTypes);
        vector<string>* arg_types = new vector<string>();
        while(args){
            arg_types->push_back(args->value->type); // here memory error
            cout<<"done"<<endl;
            args = args->next;
        }    
        output::errorPrototypeMismatch(lineno,id,*arg_types);
        delete arg_types;
    }
}

#endif
