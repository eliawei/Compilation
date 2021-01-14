//
// Created by Illay Hai on 2020-05-26.
//

#ifndef HW3_SYMBOL_TABLE_HPP
#define HW3_SYMBOL_TABLE_HPP

#include <vector>
#include "parser.hpp"

using namespace std;


struct func_rec{
    Token* name;
    vector<Var*>* params;
    Token* return_type;
    func_rec(Token* func_name, vector<Var*>* params, Token* return_type){
        this->name = func_name;
//        this->params = new vector<Var*>(params);
        this->params = params;
        this->return_type = return_type;
    }
};

struct symrec{
    Var* var;
    func_rec* func;
    bool is_func;

    symrec(Var* variable){
        this->var = variable;
        this->func = nullptr;
        this->is_func = false;
    }
    symrec(Token* func_name, vector<Var*>* params, Token* return_type){
        func_rec* funcRec = new func_rec(func_name, params, return_type);
        this->func = funcRec;
        this->var = nullptr;
        this->is_func = true;
    }
    Token* getName();
};

struct symscope{
    bool is_while;
    func_rec* func_record;
    vector<symrec*>* records;
    int startingOffset;
    int currOffset;
    bool isFunc;

    symscope(bool is_while, int offset){
        this->is_while = is_while;
        this->func_record = nullptr;
        this->records = new vector<symrec*>();
        this->startingOffset = offset;
        this->currOffset = offset;
        this->isFunc = false;

    }
    symscope(func_rec* func_rec1){
        this->is_while = false;
        this->func_record = new func_rec(func_rec1->name, func_rec1->params, func_rec1->return_type);
        this->records = new vector<symrec*>();
        this->startingOffset = 0;
        this->currOffset = 0;
        this->isFunc = true;
    }
    bool wasDeclaredInScope(Token* name);
};

class symtable{
    vector<symscope*>* scopes;

public:
    symtable(){
        scopes = new vector<symscope*>();
        this->addScope(false);


        Token* name = new Token("print");
        FormalList* formalList = new FormalList(new Var(new Token("STRING"), new Token("first")));
        Token* ret_type = new Token("VOID");
        this->addFunction(name, formalList, ret_type);
        this->scopes->pop_back();


        name = new Token("printi");
        formalList = new FormalList(new Var(new Token("INT"), new Token("first")));
        this->addFunction(name, formalList, ret_type);
        this->scopes->pop_back();
    }

    void addRecord(Var* variable);
    void addScope(bool is_while);
    void addFunction(Token* name, FormalList* params, Token* return_type);
    void removeScope();
    bool wasDeclared(Token* name);
    void validateDeclared(Token* name, int lineNumber, bool isFunction);
    void validateNotDeclared(Token* name, int lineNumber);
    symrec* getRecord(Token* name);
    string getType(Token* name, int lineNumber, bool isFunction);
    void validateFuncArguments(Token* funcName, ExpList* argList, int lineNumber);
    bool isWhile();
    void validateMain();
    void validateReturnType(Expression* exp, int lineNumber);
    void validateParamList(Var* param, FormalList* list, int lineNumber);
    int getStackOffset(Token* name);
};

#endif //HW3_SYMBOL_TABLE_HPP
