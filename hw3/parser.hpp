#ifndef _PARSER_HPP
#define _PARSER_HPP

#include <string>
#include <vector>
#include <algorithm>
using namespace std;

struct Node{
};

struct Token : public Node{
    string token;

    Token(string lexeme) : token(lexeme) {
        vector<string> types {"int", "byte", "bool", "string", "set", "void"};
        if(find(types.begin(), types.end(), token) != types.end()){
            for (auto & c: token) c = toupper(c);
        }
    }
};

struct Expression : public Node{
    string type;

    Expression(): type(""){}

    Expression(string type): type(type){}
};
struct Funcs : public Node{
    Node* func;
    Node* next;

    Funcs() : func(NULL), next(NULL) {}
    Funcs(Node* func, Node* next) : func(func), next(next) {}
};

struct FuncDecl : public Node{
    Node* ret_type;
    Node* func_name;
    Node* args;
    Node* statements;

    FuncDecl(Node* ret_type, Node* func_name, Node* args, Node* statements) : ret_type(ret_type), 
                                                                                func_name(func_name),
                                                                                args(args),
                                                                                statements(statements) {}
};

struct FormalsList : public Node{
    Node* value;
    Node* next;

    FormalsList() : value(NULL), next(NULL) {}
    FormalsList(Node* value) : value(value), next(NULL) {}
    FormalsList(Node* value, Node* next) : value(value), next(next) {}
};

struct Statements : public Node{
    Node* value;
    Node* next;

    Statements(Node* value) : value(value), next(NULL) {}
    Statements(Node* value, Node* next) : value(value), next(next) {}
};


struct Declaration : public Node{
    Node* type;
    Node* var_name;
    Node* value;

    Declaration(Node* type, Node* var_name) : type(type), var_name(var_name), value(NULL) {}
    Declaration(Node* type, Node* var_name, Node* value) : type(type), var_name(var_name), value(value) {}
};

struct Assignment : public Node{
    Node* var_name;
    Node* value;

    Assignment(Node* var_name, Node* value) : var_name(var_name), value(value) {}
};

struct Return : public Node{
    Node* arg;

    Return() : arg(NULL) {}
    Return(Node* arg) : arg(arg) {}
};

struct If : public Node{
    Node* condition;
    Node* to_do;
    Node* else_do;

    If(Node* condition, Node* to_do) : condition(condition), to_do(to_do), else_do(NULL) {}
    If(Node* condition, Node* to_do, Node* else_do) : condition(condition), to_do(to_do), else_do(else_do) {}
};

struct While : public Node{
    Node* condition;
    Node* to_do;

    While(Node* condition, Node* to_do) : condition(condition), to_do(to_do) {}
};

struct ExpList : public Node{
    ExpList* next;
    Expression* value;

    ExpList(Expression* value) : value(value), next(nullptr) {}
    ExpList(Expression* value, ExpList* next) : value(value), next(next) {}
};
struct Call : public Expression{
    Token* func_id;
    ExpList* exp_args;

    Call(Token* func_id, string ret_type) : func_id(func_id), exp_args(nullptr) {
        type = ret_type;
    }
    Call(Token* func_id, ExpList* exp_args, string ret_type) : func_id(func_id), exp_args(exp_args) {
        type = ret_type;
    }
};



struct Set : public Node{
    Node* from_num;
    Node* to_num;

    Set(Node* from_num, Node* to_num) : from_num(from_num), to_num(to_num) {}
};

struct Binop : public Node{
    Node* exp1;
    Node* exp2;
    Node* op;

    Binop(Node* exp1, Node* op, Node* exp2) : exp1(exp1), op(op), exp2(exp2) {}
};

struct Relop : public Node{
    Node* exp1;
    Node* exp2;
    Node* op;

    Relop(Node* exp1, Node* op, Node* exp2) : exp1(exp1), op(op), exp2(exp2) {}
};


struct Num : public Expression{
    Node* num;

    Num(Node* num) : num(num) {
        type = "INT";
    }
};
struct NumB : public Expression{
    Node* num;

    NumB(Node* num) : num(num) {
        type = "BYTE";
    }
};
struct Bool : public Expression{
    Node* val;

    Bool(Node* val) : val(val) {
        type = "BOOL";
    }
};

struct Not : public Node{
    Node* exp;

    Not(Node* exp) : exp(exp){}
};

struct And : public Expression{
    Node* exp1;
    Node* exp2;

    And(Node* exp1, Node* exp2) : exp1(exp1), exp2(exp2){
        type = "BOOL";
    }
};

struct Or : public Node{
    Node* exp1;
    Node* exp2;

    Or(Node* exp1, Node* exp2) : exp1(exp1), exp2(exp2) {}
};

#define YYSTYPE Node*

#endif