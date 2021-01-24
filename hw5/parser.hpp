#ifndef _PARSER_HPP
#define _PARSER_HPP

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include "bp.hpp"
using namespace std;

struct Node{
    vector<pair<int,BranchLabelIndex>> truelist;
    vector<pair<int,BranchLabelIndex>> falselist;
    vector<pair<int,BranchLabelIndex>> nextlist;
    vector<pair<int,BranchLabelIndex>> continuelist;
    vector<pair<int,BranchLabelIndex>> breaklist;
    string register_name;
    string label;

    Node() : truelist(vector<pair<int,BranchLabelIndex>>()), falselist(vector<pair<int,BranchLabelIndex>>()),
            nextlist(vector<pair<int,BranchLabelIndex>>()), continuelist(vector<pair<int,BranchLabelIndex>>()),
            breaklist(vector<pair<int,BranchLabelIndex>>()){
                register_name = "";
                label = "";
            }
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

struct Marker : public Node{
    Marker(string m_label){
        label = m_label;
    } 
};

struct Type : public Node{
    Token* type;

    Type(Token* type){
        // this->type = new Token(type->token);
        this->type = type;
    }
};

struct Set : public Type{
    Token* from_num;
    Token* to_num;
    int size;
    int total_size;

    Set(Token* from_num, Token* to_num) : Type(new Token("set")){
        this->from_num = new Token(from_num->token);
        this->to_num = new Token(to_num->token);
        this->size = 0;
        this->total_size = 0;
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

    Funcs() : func(nullptr), next(nullptr) {}
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

    FormalsList() : value(nullptr), next(nullptr) {}
    FormalsList(Node* value) : value(value), next(nullptr) {}
    FormalsList(Node* value, Node* next) : value(value), next(next) {}
};

struct Statements : public Node{
    Node* value;
    Node* next;

    Statements(Node* value) : value(value), next(nullptr) {}
    Statements(Node* value, Node* next) : value(value), next(next) {}
};


struct Declaration : public Node{
    Token* type;
    Token* var_name;
    Token* value;

    Declaration(Type* type, Token* var_name) : type(type->type), var_name(var_name), value(nullptr) {}
    Declaration(Type* type, Token* var_name, Token* value) : type(type->type), var_name(var_name), value(value) {}
};

struct Assignment : public Node{
    Node* var_name;
    Node* value;

    Assignment(Node* var_name, Node* value) : var_name(var_name), value(value) {}
};

struct Return : public Expression{
    Expression* arg;

    Return() : arg(nullptr) {}
    Return(Expression* arg) : arg(arg) {
        type= arg->type;
    }
};

struct If : public Node{
    Node* condition;
    Node* to_do;
    Node* else_do;

    If(Node* condition, Node* to_do) : condition(condition), to_do(to_do), else_do(nullptr) {}
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

struct Binop : public Expression{
    Expression* exp1;
    Expression* exp2;
    Token* op;

    Binop(Expression* exp1, Token* op, Expression* exp2) : exp1(exp1), op(op), exp2(exp2) {
        if( (exp1->type == "SET" && (exp2->type == "INT" || exp2->type == "BYTE")) || (exp2->type == "SET" && (exp1->type == "INT" || exp1->type == "BYTE")))
            type = "SET";
        else {
            if(exp1->type == "INT" || exp2->type == "INT"){
                type = "INT";
            }else
                type = "BYTE";
            }
    }
};

struct Relop : public Expression{
    Expression* exp1;
    Expression* exp2;
    Token* op;

    Relop(Expression* exp1, Token* op, Expression* exp2) : exp1(exp1), op(op), exp2(exp2) {
        type = "BOOL";
    }
};


struct Num : public Expression{
    Token* num;

    Num(Token* num_t) : num(num_t) {
        type = "INT";
    }
};
struct NumB : public Expression{
    Num* num;

    NumB(Num* num) : num(num) {
        type = "BYTE";
    }
};
struct Bool : public Expression{
    Expression* val;

    Bool(Expression* val) : val(val) {
        type = "BOOL";
    }
};

struct Not : public Expression{
    Expression* exp;

    Not(Expression* exp) : exp(exp){
        type = exp->type;
    }
};

struct And : public Expression{
    Expression* exp1;
    Expression* exp2;

    And(Expression* exp1, Expression* exp2) : exp1(exp1), exp2(exp2){
        type = "BOOL";
    }
};
struct StringExp : public Expression{
    string str;

    StringExp(Token* lex) : str(lex->token){
        type = "STRING";
    }
};

struct Or : public Expression{
    Expression* exp1;
    Expression* exp2;

    Or(Expression* exp1, Expression* exp2) : exp1(exp1), exp2(exp2) {
        type = "BOOL";
    }
};

#define YYSTYPE Node*

#endif