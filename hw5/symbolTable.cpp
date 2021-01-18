#include "symbolTable.hpp"
#include "hw3_output.hpp"
#include <iostream>
using namespace std;

/*------------------------------------------ScopeRow------------------------------------*/
ScopeRow::ScopeRow(string name, string type, int offset, bool is_function) : name(name), type(type), offset(offset), is_function(is_function){
}

string ScopeRow::getType(string id){
    if(this->name == id && this->is_function) return ((FunctionRow*)this)->ret_type;
    if(this->name == id && ! this->is_function) return this->type;
    return {}; // empty string 
}

FunctionRow::FunctionRow(string name, string type, string ret_type, vector<string>& args_types) : ScopeRow(name, type, 0, true){
    this->is_function = true;
    this->args_types = new vector<string>();
    *this->args_types = args_types;
    this->ret_type = ret_type;
}
FunctionRow::~FunctionRow(){
    delete args_types;
}



/*------------------------------------------Scope--------------------------------------*/

Scope::Scope(int offset) {
    scope_st = new vector<ScopeRow*>();
    cur_offset = offset;
    is_while = false;
    is_function = false;
    func = nullptr;
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

void Scope::addFunction(string name, string type, string ret_type, vector<string>& args_types) {
    FunctionRow* scope_row = new FunctionRow(name, type, ret_type, args_types);
    scope_st->push_back(scope_row);
}

bool Scope::isIdentifierInScope(string id, bool is_new_dec){
    for(int i = 0; i < scope_st->size(); i++){
        ScopeRow* current_row = (*scope_st)[i];
        if(id.compare(current_row->name) == 0){
            if(is_new_dec) return true;
            else{
                if(!current_row->is_function) return true;
            }
        }
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

vector<string>* Scope::getFuncArgsTypes(string id){
    for(int i = 0; i < scope_st->size(); i++){
        ScopeRow* current_row = (*scope_st)[i];
        if(current_row->name == id) return ((FunctionRow*)current_row)->args_types;
    }
    return nullptr;
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
    global_scope->addFunction(func_name, function_type, ret_type, args_types);

    //adding printi
    func_name = "printi";
    args_types = {"INT"};
    //function_type = output::makeFunctionType(ret_type, args_types);
    global_scope->addFunction(func_name, function_type, ret_type, args_types);

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
    scope->is_function = true;
    Scope* global_scope = tables_stack->front();
    scope->func = (FunctionRow*)global_scope->scope_st->back();
    int index = -1;
    for(int i = 0; i < types.size(); i++){
        scope->addRow(names[i], types[i], index);
        index--;
    }
    tables_stack->push_back(scope);
}

void SymbolTable::exitScope(){
    //output::endScope();
    Scope* scope = tables_stack->back();
    for (ScopeRow* row : *(scope->scope_st)){
        //output::printID(row->name, row->offset, row->type);
        if(row->offset>=0){
            this->cur_offset--;
        }
    }
    //delete scope;
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
    global_scope->addFunction(func_name, function_type, func_ret_type, args_types);
    enterFunctionScope(args_types, args_names);
}

bool SymbolTable::isWhileScope(){
    for(int i=tables_stack->size() - 1; i >= 0; i--){
        Scope* current_scope = (*tables_stack)[i];
        if(current_scope->is_while)
            return true;
    }
    return false;
}

bool SymbolTable::isIdentifierDeclared(string id, bool is_new_dec){
    for(int i=tables_stack->size() - 1; i >= 0; i--){
        Scope* current_scope = (*tables_stack)[i];
        if(current_scope->isIdentifierInScope(id, is_new_dec))
            return true;
    }
    return false;
}

string SymbolTable::getType(Node* id1, bool is_function){
    string id = ((Token*)id1)->token;
    Scope* current_scope = nullptr;
    if(is_function){
        current_scope = tables_stack->front();
        string type = current_scope->getType(id);
        if(!type.empty()){
            return type;
        }
    }else{
        for(int i=tables_stack->size() - 1; i > 0; i--){
            Scope* current_scope = (*tables_stack)[i];
            string type = current_scope->getType(id);
            if(!type.empty()){
                return type;
            }
        }
    }    
    return {}; // empty string
}
vector<string>* SymbolTable::getFuncArgs(Node* id1){
    string id = ((Token*)id1)->token;
    Scope* current_scope = tables_stack->front();
    return current_scope->getFuncArgsTypes(id);
}

void SymbolTable::validateRetType(Node* exp, int line_num){
    string func_ret_type;
    for(Scope* scope : *tables_stack){
        if(scope->is_function)
            func_ret_type = scope->func->ret_type;
    }
    if(!exp){
        if(func_ret_type.compare("VOID") != 0){
            output::errorMismatch(line_num);
			exit(1);
        }
    }
    else{
        string exp_type = ((Expression*)exp)->type;
        if(func_ret_type == "INT" && exp_type == "BYTE") return; 
        if(func_ret_type.compare(exp_type) == 0) return;
        else{
            output::errorMismatch(line_num);
			exit(1);
        }
    }
}

int SymbolTable::getStackOffset(Token* name){
    int offset = 0;
    int i = 0;
    for(; i < tables_stack->size(); i++){
        if((*tables_stack)[i]->is_function){
            break;
        }
    }
    for(int j = i; j < tables_stack->size(); j++){
        Scope* cur_scope = (*tables_stack)[j];
        for(ScopeRow* row : *cur_scope->scope_st){
            if(row->name.compare(name->token) == 0)
                return offset;
            offset++;
        }
    }
    return -1;
}

bool isNumeric(string type){
    return (type == "INT" || type == "BYTE");
}

bool SymbolTable::validateNumeric(Node* exp1, Node* exp2){
    string type1 = ((Expression*)exp1)->type;
    string type2 = ((Expression*)exp2)->type;
    if(isNumeric(type1) && isNumeric(type2))
        return true;
    return false;
}

bool SymbolTable::validateBoolean(Node* exp1, Node* exp2){
    string type1 = ((Expression*)exp1)->type;
    string type2 = ((Expression*)exp2)->type;
    if(type1 == "BOOL" && type2 == "BOOL"){
       return true;
    }
    return false;
}

bool SymbolTable::validateBoolean(Node* exp){
    string type = ((Expression*)exp)->type;
    if(type == "BOOL"){
       return true;
    }
    return false;
}

bool SymbolTable::validateSet(Node* exp1, Node* exp2){
    string type1 = ((Expression*)exp1)->type;
    string type2 = ((Expression*)exp2)->type;
    if(isNumeric(type1) && type2 == "SET")
        return true;
    if(type1 == "SET" && isNumeric(type2))
        return true;
    return false;
}

bool SymbolTable::validateRelopInTypes(Node* exp1, Node* exp2){
    string type1 = ((Expression*)exp1)->type;
    string type2 = ((Expression*)exp2)->type;
    if(isNumeric(type1) && type2 == "SET")
        return true;
    return false;
}

bool SymbolTable::validateByteLen(Node* num){
    string num_str = ((Token*)num)->token;
    int num_int = stoi(num_str);
    if(num_int > 255)
        return false;
    return true;
}

void SymbolTable::validateSetRange(Node* num1, Node* num2, int line_num){
    string num1_str = ((Token*)num1)->token;
    string num2_str = ((Token*)num2)->token;
    int num1_int = stoi(num1_str);
    int num2_int = stoi(num2_str);
    int diff = num2_int - num1_int;
    if(diff <=0  || diff > 255){
        output::errorSetTooLarge(line_num, num1_str, num2_str);
		exit(1);
    }
}

void SymbolTable::validateMainFunc(){
    Scope* global_scope = tables_stack->front();
    for(ScopeRow* row : *global_scope->scope_st){
        if(row->is_function){
            FunctionRow* func_row = (FunctionRow*)row;
            if(func_row->name == "main" && func_row->ret_type == "VOID")
                if(func_row->args_types->size() == 0)
                    return;
        }
    }
    output::errorMainMissing();
	exit(1);
}

void SymbolTable::validateFunctionCall(Node* fun_name, int line_num){
    string name = ((Token*)fun_name)->token;
    Scope* global_scope = tables_stack->front();
    for(ScopeRow* row : *global_scope->scope_st){
        if(row->is_function){
            FunctionRow* fun_row = (FunctionRow*)row;  
            if(name.compare(fun_row->name) == 0)
                return;     
        }
    }
    output::errorUndefFunc(line_num, name);
    exit(1);
}

bool SymbolTable::validateConversion(Node* type, Node* exp){
    string type1 = ((Token*)type)->token;
    string type2 = ((Expression*)exp)->type;

    if(type1 == "INT" && type2 == "SET")
        return true;
    return false;
}




