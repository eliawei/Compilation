#include <string>
#include "parser.hpp"

using namespace std;
class ScopeRow{

    public:
        string name;
        string type;
        int offset;
        bool is_function;
        ScopeRow(string name, string type, int offset, bool is_function=false);
        string getType(string id);


};

class FunctionRow : public ScopeRow{
    public:
        vector<string>* args_types;
        string ret_type;
        FunctionRow(string name, string type, string ret_type, vector<string>& args_types);
        ~FunctionRow();
};


class Scope{

    public:
        vector<ScopeRow*>* scope_st;
        //int start_offset;
        int cur_offset;
        bool is_while;
        bool is_function;
        FunctionRow* func;

        
        Scope(int offset);
        ~Scope();
        void addRow(string name, string type);
        void addRow(string name, string type, int offset); //used for function args - negative values
        void addFunction(string name, string type, string ret_type, vector<string>& args_types);
        bool isIdentifierInScope(string id, bool is_new_dec);
        string getType(string id);
        vector<string>* getFuncArgsTypes(string id);
};


class SymbolTable{

    vector<Scope*>* tables_stack;
    int cur_offset;
    string getRetType(string func_type){
        string delimiter = "->";
        size_t pos = func_type.find(delimiter);
        string ret_type = func_type.substr(pos+2, func_type.length() - (pos+3));
        return ret_type;
    }
    string getArgsTypes(string func_type){
        size_t pos1 = func_type.find("(");
        string temp = func_type.substr(pos1+1, func_type.length() - (pos1+2));
        size_t pos2 = func_type.find(")");
        string args_types = temp.substr(0, pos2-1).append(",");
        return args_types;
    }

    public:
    SymbolTable();
    ~SymbolTable();
    void enterScope(bool is_while=false);
    void enterFunctionScope(vector<string> types, vector<string> names);
    void exitScope();
    void insert(Node* type, Node* name);
    void insertFunction(Node* name, Node* ret_type, Node* args);
    bool isWhileScope();
    bool isIdentifierDeclared(string id, bool is_new_dec=false);
    string getType(Node* id1, bool is_function=false);
    vector<string>* getFuncArgs(Node* id1);
    void validateRetType(Node* exp, int line_num);
    int getStackOffset(Token* name);
    bool validateNumeric(Node* exp1, Node* exp2);
    bool validateBoolean(Node* exp1, Node* exp2);
    bool validateBoolean(Node* exp);
    bool validateSet(Node* exp1, Node* exp2);
    bool validateRelopInTypes(Node* exp1, Node* exp2);
    bool validateByteLen(Node* num);
    void validateSetRange(Node* num1, Node* num2, int line_num);
    void validateMainFunc();
    void validateFunctionCall(Node* fun_name, int line_num);
    bool validateConversion(Node* type, Node* exp);
};
