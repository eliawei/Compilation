#include <iostream>
#include "bp.hpp"
#include "parser.hpp"

using namespace std;

int reg_cnt = 0;

string freshReg(){
    reg_cnt++;
    return "%r" + to_string(reg_cnt);
}

int emit(const std::string &command){
    return CodeBuffer::instance().emit(command);
}

void arithmeticCalc(Expression* res, Expression* exp1, Token* oper, Expression* exp2){
    bool is_int = (res->type.compare("INT") == 0);
    string res_register = is_int ? res->register_name : freshReg();
    string op;
    if(oper->token.compare("+") == 0) op = "add";
    if(oper->token.compare("-") == 0) op = "sub";
    if(oper->token.compare("*") == 0) op = "mul";
    if(oper->token.compare("/") == 0) {
        op = "sdiv";
        //TODO: check if not divided by zero
    }

    emit(res_register + " = " + op + " i32 " + exp1->register_name + ", " +exp2->register_name);
    if(!is_int){
        string temp_reg = freshReg();
        emit(temp_reg + " = trunc i32 " + res_register + " to i8");
        emit(res->register_name + " = zext i8 " + temp_reg + " to i32");
    }

}