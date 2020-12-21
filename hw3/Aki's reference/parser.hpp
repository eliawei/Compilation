#ifndef __SCANNER_HPP
#define __SCANNER_HPP

#include <string>
#include "hw3_output.hpp"
#include <iostream>

using namespace std;

typedef struct {
} Node;

struct Token : public Node{
    string text;

    Token(string desc){
        text = desc;
        if(text.compare("int") == 0)
            text = "INT";
        if(text.compare("byte") == 0)
            text = "BYTE";
        if(text.compare("bool") == 0)
            text = "BOOL";
        if(text.compare("void") == 0)
            text = "VOID";
        if(text.compare("string") == 0)
            text = "STRING";
    }
};

struct Expression : public Node{
    string type;

    Expression(){
        type = "";
    }

    Expression(string type){
        this->type = type;
    }
};

struct Relop : public Expression{
        Expression* first;
        Expression* second;
        Token* relop;

        Relop(Expression* firstExp, Token* op, Expression* secondExp){
            first = firstExp;
            second = secondExp;
            relop = op;
            type = "BOOL";
        }
};

struct Binop : public Expression{
    Expression* first;
    Expression* second;
    Token* op;

    Binop(Expression* firstExp, Token* binop, Expression* secondExp){
        first = firstExp;
        second = secondExp;
        op = binop;
        if(firstExp->type.compare("INT") == 0 || secondExp->type.compare("INT") == 0)
            type = "INT";
        else
            type = "BYTE";
    }
};

struct And : public Expression{
    Expression* first;
    Expression* second;

    And(Expression* firstExp, Expression* secondExp){
        first = firstExp;
        second = secondExp;
        type = "BOOL";
    }
};

struct Or : public Expression{
    Expression* first;
    Expression* second;

    Or(Expression* firstExp, Expression* secondExp){
        first = firstExp;
        second = secondExp;
        type = "BOOL";
    }
};

struct Not : public Expression{
    Expression* exp;

    Not(Expression* expression){
        exp = expression;
        type = "BOOL";
    }
};

struct Num : public Expression{
    Token* value;

    Num(Token* val){
        value = val;
        type = "INT";
    }
};

struct Bnum : public Expression{
    Num* num;

    Bnum(Num* number){
        num = number;
        type = "BYTE";
    }
};


struct ExpList : public Node{
    ExpList* next;
    Expression* exp;

    ExpList(Expression* expression){
        exp = expression;
        next = nullptr;
    }

    ExpList(ExpList* nextList, Expression* expression){
        next = nextList;
        exp = expression;
    }
};

struct Call : public Expression{
    Token* id;
    ExpList* expList;

    Call(Token* identifier, string returnType){
        id = identifier;
        expList = nullptr;
        type = returnType;
    }

    Call(Token* identifier, ExpList* expressionList, string returnType){
        id = identifier;
        expList = expressionList;
        type = returnType;
    }
};

struct Bool : public Expression{
    string value;

    Bool(Token* val){
        value = val->text;
        type = "BOOL";
    }
};

struct StringExp : public Expression{
    string value;

    StringExp(Token* val){
        value = val->text;
        type = "STRING";
    }
};

struct While : public Node{
    Node* cond;
    Node* statements;
    Node* elseStatements;

    While(Node* condition, Node* st){
        cond = condition;
        statements = st;
        elseStatements = nullptr;
    }

    While(Node* condition, Node* st, Node* elseSt){
        cond = condition;
        statements = st;
        elseStatements = elseSt;
    }
};

struct If : public Node{
    Node* cond;
    Node* statements;
    Node* elseStatements;

    If(Node* condition, Node* st){
        cond = condition;
        statements = st;
        elseStatements = nullptr;
    }

    If(Node* condition, Node* st, Node* elseSt){
        cond = condition;
        statements = st;
        elseStatements = elseSt;
    }
};

struct Return : public Node{
    Node* exp;

    Return(){
        exp = nullptr;
    }

    Return(Node* expression){
        exp = expression;
    }
};

struct Assign : public Node{
    Node* id;
    Node* val;

    Assign(Node* identifier, Node* value){
        id = identifier;
        val = value;
    }
};

struct Var : public Node{
    Token* type;
    Token* id;
    Token* val;

    Var(Token* t, Token* identifier){
        type = t;
        id = identifier;
        val = nullptr;


    }
    Var(Token* t, Token* identifier, Token* value){
        type = t;
        id = identifier;
        val = value;
    }
};

struct Statements : public Node{
    Node* next;
    Node* statement;

    Statements(Node* s){
        statement = s;
        next = nullptr;
    }

    Statements(Node* statements, Node* s){
        next = statements;
        statement = s;
    }
};

struct FormalList : public Node{
    FormalList* next;
    Var* formalDec;

    FormalList(Var* formal){
        formalDec = formal;
        next = nullptr;
    }

    FormalList(Var* dec, FormalList* formalList){
        next = formalList;
        formalDec = dec;
    }
};

struct RecType : public Node{
    Node* type;
    Node* id;
    Node* formals;
    Node* statements;

    RecType(Node* t, Node* i, Node* f, Node* s){
        type = t;
        id = i;
        formals = f;
        statements = s;
    }
};

struct Funcs : public Node{
    Node* funcs;
    Node* decl;

    Funcs(Node* d, Node* f){
        decl = d;
        funcs = f;
    }
};


#define YYSTYPE Node*

#endif
