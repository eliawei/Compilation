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
    bool FuncsArgsTypesValid(string id, vector<string>* func_args, ExpList* given_args){
        if(!given_args || !func_args){
            return false;
        }
        ExpList* curr = given_args;
        for (int i = 0; i < func_args->size(); i++)
        {
            if(!curr || !curr->value){
                return false;
            }
            if(!(func_args->at(i).compare(curr->value->type) == 0 || ((func_args->at(i)) == "INT" && (curr->value->type) == "BYTE"))) return false;
            curr = curr->next;
        }
        if(curr && curr->value) return false; // if anything left in given args list so too many args were passed.
        return true;
    }
    void errorPrototypeMismatchAux(int lineno, Node* id1, Node* argTypes){
        string id = ((Token*)id1)->token;
        ExpList* args = (ExpList*)(argTypes);
        vector<string>* arg_types = new vector<string>();
        while(args){
            arg_types->push_back(args->value->type); // here memory error
            args = args->next;
        }    
        output::errorPrototypeMismatch(lineno,id,*arg_types);
        delete arg_types;
    }
}

#endif
