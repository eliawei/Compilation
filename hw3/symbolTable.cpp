#include "symbolTable.hpp"
#include "hw3_output.hpp"
#include <iostream>
using namespace std;

/*------------------------------------------ScopeRow------------------------------------*/
ScopeRow::ScopeRow(string name, string type, int offset) : name(name), type(type), offset(offset){
}

ScopeRow::ScopeRow(string name, string type) : name(name), type(type), offset(0){
}

string ScopeRow::getType(string id){
    if(this->name == id) return this->type;
    return {}; // empty string 
}



/*------------------------------------------Scope--------------------------------------*/

Scope::Scope(int offset) {
    scope_st = new vector<ScopeRow*>();
    cur_offset = offset;
    is_while = false;
}

Scope::~Scope(){
    scope_st->clear();
    delete scope_st;
}

void Scope::addRow(string name, string type){
    ScopeRow* scope_row = new ScopeRow(name, type, cur_offset);
    cur_offset++;
    scope_st->push_back(scope_row);
}

void Scope::addRow(string name, string type, int offset) {
    ScopeRow* scope_row = new ScopeRow(name, type, offset);
    scope_st->push_back(scope_row);
}

void Scope::addFunction(string name, string type) {
    ScopeRow* scope_row = new ScopeRow(name, type);
    scope_st->push_back(scope_row);
}

bool Scope::isIdentifierInScope(string id){
    for(int i = 0; i < scope_st->size(); i++){
        ScopeRow* current_row = (*scope_st)[i];
        if(id.compare(current_row->name) == 0)
            return true;
    }
    return false;
}

string Scope::getType(string id){
    for(int i = 0; i < scope_st->size(); i++){
        ScopeRow* current_row = (*scope_st)[i];
        string type = current_row->getType(id);
        if(!type.empty()) return type;
    }
    return {};// empty string 
}



/*------------------------------------------SymbolTable------------------------------------*/
SymbolTable::SymbolTable(){
    tables_stack = new vector<Scope*>();
    cur_offset = 0;
    Scope* global_scope = new Scope(cur_offset);

    //adding print
    string func_name = "print";
    string ret_type = "VOID";
    vector<string> args_types {"STRING"};
    string function_type = output::makeFunctionType(ret_type, args_types);
    global_scope->addFunction(func_name, function_type);

    //adding printi
    func_name = "printi";
    args_types = {"INT"};
    function_type = output::makeFunctionType(ret_type, args_types);
    global_scope->addFunction(func_name, function_type);

    tables_stack->push_back(global_scope);

}
SymbolTable::~SymbolTable(){
    tables_stack->clear();
    delete tables_stack;
}

void SymbolTable::enterScope(bool is_while) {
    Scope* scope = new Scope(cur_offset);
    scope->is_while = is_while;
    tables_stack->push_back(scope);
}

void SymbolTable::enterFunctionScope(vector<string> types, vector<string> names) {
    Scope* scope = new Scope(cur_offset);
    int index = -1;
    for(int i = 0; i < types.size(); i++){
        scope->addRow(names[i], types[i], index);
        index--;
    }
    tables_stack->push_back(scope);
}

void SymbolTable::exitScope(){
    output::endScope();
    Scope* scope = tables_stack->back();
    for (ScopeRow* row : *(scope->scope_st)){
        output::printID(row->name, row->offset, row->type);
        row->offset--;
    }
    tables_stack->pop_back();
}


//TODO: type checking 
void SymbolTable::insert(Node* type, Node* name) {
    string var_type = ((Token*)type)->token;
    string var_name = ((Token*)name)->token;
    Scope* current_scope = tables_stack->back();
    cur_offset++;
    current_scope->addRow(var_name, var_type);
}

void SymbolTable::insertFunction(Node* name, Node* ret_type, Node* args){
    string func_name = ((Token*)name) -> token;
    string func_ret_type = ((Token*)ret_type) -> token;
    FormalsList* current_args_list = (FormalsList*)args;
    if(!current_args_list->value) //functions with no arguments
        current_args_list = nullptr;

    vector<string> args_types = vector<string>();
    vector<string> args_names = vector<string>();
    while(current_args_list){
       string type = ((Token*)((Declaration*)current_args_list->value)->type) -> token;
       args_types.push_back(type);
       string name = ((Token*)((Declaration*)current_args_list->value)->var_name) -> token;
       args_names.push_back(name);
       current_args_list = (FormalsList*)current_args_list->next;
    }

    string function_type = output::makeFunctionType(func_ret_type, args_types);
    Scope* global_scope = tables_stack->front();
    global_scope->addFunction(func_name, function_type);

        if(args_types.size() > 0){
        enterFunctionScope(args_types, args_names);
    }
    else{
        enterScope();
    }
}

bool SymbolTable::isWhileScope(){
    for(int i=tables_stack->size() - 1; i >= 0; i--){
        Scope* current_scope = (*tables_stack)[i];
        if(current_scope->is_while)
            return true;
    }
    return false;
}

bool SymbolTable::isIdentifierDeclared(string id){
    for(int i=tables_stack->size() - 1; i >= 0; i--){
        Scope* current_scope = (*tables_stack)[i];
        if(current_scope->isIdentifierInScope(id))
            return true;
    }
    return false;
}

string SymbolTable::getType(Node* id1, bool is_function, bool ret){
    string id = ((Token*)id1)->token;
    Scope* current_scope = nullptr;
    if(is_function){
        current_scope = tables_stack->front();
    }else{
        current_scope = tables_stack->back();
    }
    string type = current_scope->getType(id);

    if(!type.empty()){
        if(is_function){
            if(ret){
                type = SymbolTable::getRetType(type);
            }else{
                type = SymbolTable::getArgsTypes(type);
            }
        }
        return type;
    }
    return {}; // empty string
}