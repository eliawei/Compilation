//
// Created by Illay Hai on 2020-05-26.
//

#include "symbol_table.hpp"
#include <string>
#include "hw3_output.hpp"
#include <iostream>
using namespace std;

Token* symrec::getName() {
    if(is_func) return this->func->name;
    return this->var->id;
}


bool symscope::wasDeclaredInScope(Token *name) {
    for(symrec* record: *(this->records)){
        if(record->getName()->text.compare(name->text) == 0){
            return true;
        }
    }
    if(this->isFunc){
        for(Var* param : *(this->func_record->params)){
            if(param->id->text.compare(name->text) == 0){
                return true;
            }
        }
    }
    return false;
}



void symtable::addRecord(Var* variable) {
    symrec* record = new symrec(variable);
    this->scopes->back()->records->push_back(record);
    this->scopes->back()->currOffset++;
}

void symtable::addFunction(Token* name, FormalList *params, Token* return_type) {
    // Add Function Record
    FormalList* curr = params;
    if(curr->formalDec == nullptr)
        curr = nullptr;
    vector<Var*>* params_ = new vector<Var*>();
    while(curr != nullptr){
        params_->push_back(curr->formalDec);
        curr = curr->next;
    }
    symrec* record = new symrec(name, params_, return_type);
    this->scopes->back()->records->push_back(record);
    this->scopes->back()->currOffset++;

    // Add Function Scope
    symscope* scope = new symscope(record->func);
    this->scopes->push_back(scope);
}

void symtable::addScope(bool is_while) {
    vector<symscope*>* scopes = this->scopes;
    int offset = 0;
    if(!scopes->empty()){
        offset = scopes->back()->currOffset;
    }
    symscope* scope = new symscope(is_while, offset);
    this->scopes->push_back(scope);
}

void symtable::removeScope() {
    output::endScope();
    symscope* scope = this->scopes->back();
    if(scope->isFunc){
        vector<Var*>* params = scope->func_record->params;
        for(int currParam = 0; currParam < params->size(); currParam++){
            output::printID((*params)[currParam]->id->text, -(currParam + 1), (*params)[currParam]->type->text);
        }
    }
    int curr = scope->startingOffset;
    for(symrec* record : *scope->records) {
        vector<string> argTypes = vector<string>();
        string type;
        if(record->is_func){
            for(Var* param : *record->func->params)
                argTypes.push_back(param->type->text);
            type = output::makeFunctionType(record->func->return_type->text, argTypes);
            output::printID(record->getName()->text, 0, type);
        }
        else{
            type = record->var->type->text;
            output::printID(record->getName()->text, curr, type);
        }
        curr++;
    }
    this->scopes->pop_back();
}

bool symtable::wasDeclared(Token *name) {
    for(symscope* scope: *this->scopes){
        if(scope->wasDeclaredInScope(name)){
            return true;
        }
    }
    return false;
}

void symtable::validateDeclared(Token *name, int lineNumber, bool isFunction) {
    if(!wasDeclared(name)){
        if(isFunction)
            output::errorUndefFunc(lineNumber, name->text);
        else
            output::errorUndef(lineNumber, name->text);
        exit(1);
    }
}

void symtable::validateNotDeclared(Token *name, int lineNumber) {
    if(wasDeclared(name)){
        output::errorDef(lineNumber, name->text);
        exit(1);
    }
}

symrec* symtable::getRecord(Token *name) {
    for(symscope* scope: *this->scopes){
        if(scope->isFunc){
            for(Var* param : *scope->func_record->params){
                if(param->id->text.compare(name->text) == 0)
                    return new symrec(param);
            }
        }
        for(symrec* record: *scope->records){
            if(record->getName()->text.compare(name->text) == 0)
                return record;
        }
    }
    return nullptr;
}

string symtable::getType(Token *name, int lineNumber, bool isFunction) {
    if(!wasDeclared(name)){
        if(isFunction)
            output::errorUndefFunc(lineNumber, name->text);
        else
            output::errorUndef(lineNumber, name->text);
        exit(1);
    }
    symrec* record = this->getRecord(name);
    if(!isFunction && record->is_func){
        output::errorUndef(lineNumber, name->text);
        exit(1);
    }
    else if (isFunction && !record->is_func){
        output::errorUndefFunc(lineNumber, name->text);
        exit(1);
    }
    if(isFunction)
        return record->func->return_type->text;
    return record->var->type->text;
}

void symtable::validateFuncArguments(Token *funcName, ExpList *argList, int lineNumber) {
    symrec* funcRecord = this->getRecord(funcName);
    vector<string>* expectedTypes = new vector<string>();
    for(Var* arg : *funcRecord->func->params){
        expectedTypes->push_back(arg->type->text);
    }
    ExpList* curr = argList;
    for(string type : *expectedTypes){
        if(curr == nullptr){
            output::errorPrototypeMismatch(lineNumber, funcName->text, *expectedTypes);
            exit(1);
        }
        else if(curr->exp->type.compare(type) != 0 && (curr->exp->type.compare("BYTE") != 0 || type.compare("INT") != 0)){
            output::errorPrototypeMismatch(lineNumber, funcName->text, *expectedTypes);
            exit(1);
        }
        else
            curr = curr->next;
    }
    if(curr != nullptr){
        output::errorPrototypeMismatch(lineNumber, funcName->text, *expectedTypes);
        exit(1);
    }
}

bool symtable::isWhile() {
    for(symscope* scope : *this->scopes) {
        if(scope->is_while)
            return true;
    }
    return false;
}

void symtable::validateMain() {
    Token* mainToken = new Token("main");
    if(!wasDeclared(mainToken) || !this->getRecord(mainToken)->is_func ||
        this->getRecord(mainToken)->func->return_type->text.compare("VOID") != 0 ||
        this->getRecord(mainToken)->func->params->size() != 0){
        output::errorMainMissing();
        exit(1);
    }
}

void symtable::validateReturnType(Expression* exp, int lineNumber) {
    for(symscope* scope : *this->scopes) {
        if(scope->isFunc){
            if(scope->func_record->return_type->text.compare("VOID") == 0){
                if(exp != nullptr){
                    output::errorMismatch(lineNumber);
                    exit(1);
                }
                return;
            }
            if(scope->func_record->return_type->text.compare("INT") == 0) {
                if(exp == nullptr || (exp->type.compare("INT") != 0 && exp->type.compare("BYTE") != 0)){
                    output::errorMismatch(lineNumber);
                    exit(1);
                }
            }
            if(scope->func_record->return_type->text.compare("BYTE") == 0) {
                if(exp == nullptr || exp->type.compare("BYTE") != 0){
                    output::errorMismatch(lineNumber);
                    exit(1);
                }
            }
            if(scope->func_record->return_type->text.compare("BOOL") == 0) {
                if(exp == nullptr || exp->type.compare("BOOL") != 0){
                    output::errorMismatch(lineNumber);
                    exit(1);
                }
            }
            return;
        }
    }
}

void symtable::validateParamList(Var* param, FormalList *list, int lineNumber) {
    this->validateNotDeclared(param->id, lineNumber);
    FormalList* curr = list;
    while(curr != nullptr){
        if(list->formalDec->id->text.compare(param->id->text) == 0){
            output::errorDef(lineNumber, param->id->text);
            exit(1);
        }
        curr = curr->next;
    }
}