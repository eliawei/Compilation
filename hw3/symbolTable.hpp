#include <string>
#include "parser.hpp"

using namespace std;

struct ScopeRow{
    string name;
    string type;
    int offset;

    ScopeRow(string name, string type, int offset);
    ScopeRow(string name, string type);
};


struct Scope{
    vector<ScopeRow*>* scope_st;
    int start_offset;
    int cur_offset;
    bool is_while;

    
    Scope(int offset);
    void addRow(string name, string type);
    void addRow(string name, string type, int offset); //used for function args - negative values
    void addFunction(string name, string type);
    bool isIdentifierInScope(string id);


};


class SymbolTable{
    vector<Scope*>* tables_stack;
    int cur_offset;

public:
    SymbolTable();
    void enterScope(bool is_while=false);
    void enterFunctionScope(vector<string> types, vector<string> names);
    void exitScope();
    void insert(Node* type, Node* name);
    void insertFunction(Node* name, Node* ret_type, Node* args);
    bool isWhileScope();
    bool isIdentifierDelared(string id);



};
