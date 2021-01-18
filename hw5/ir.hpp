#include <iostream>
#include "bp.hpp"
#include "parser.hpp"
#include "symbolTable.hpp"

using namespace std;

int reg_cnt = 0;
int function_cnt = 0;
int bool_assign_cnt = 0;

string freshReg(){
    reg_cnt++;
    return "%r" + to_string(reg_cnt);
}

string freshStrReg(){
    reg_cnt++;
    return "@.str" + to_string(reg_cnt);
}

// ---------------------------- Wrapping bp methods --------------------------------

vector<pair<int,BranchLabelIndex>> makelist(pair<int,BranchLabelIndex> item){
    return CodeBuffer::instance().makelist(item);
}

vector<pair<int,BranchLabelIndex>> merge(const vector<pair<int,BranchLabelIndex>> &l1,const vector<pair<int,BranchLabelIndex>> &l2){
    return CodeBuffer::instance().merge(l1, l2);
}

int emit(const std::string &command){
    return CodeBuffer::instance().emit(command);
}

void printCodeBuffer(){
    CodeBuffer::instance().printCodeBuffer();
}

std::string genLabel(){
    return CodeBuffer::instance().genLabel();
}

void bpatch(const vector<pair<int,BranchLabelIndex>>& address_list, const std::string &label){
    CodeBuffer::instance().bpatch(address_list, label);
}

void emitGlobal(const string& dataLine){
    CodeBuffer::instance().emitGlobal(dataLine);
}

void printGlobalBuffer(){
    CodeBuffer::instance().printGlobalBuffer();
}

//------------------------ end -----------------------

void declareInitialFuncs(){
    emit("declare i32 @printf(i8*, ...)");
    emit("declare void @exit(i32)");
    emitGlobal("@.int_specifier = constant [4 x i8] c\"%d\\0A\\00\"");
    emitGlobal("@.str_specifier = constant [4 x i8] c\"%s\\0A\\00\"");
    emit("define void @printi(i32) {");
    emit("call i32 (i8*, ...) @printf(i8* getelementptr ([4 x i8], [4 x i8]* @.int_specifier, i32 0, i32 0), i32 %0)");
    emit("ret void");
    emit("}");
    emit("define void @print(i8*) {");
    emit("call i32 (i8*, ...) @printf(i8* getelementptr ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i8* %0)");
    emit("ret void");
    emit("}");
}

void arithmeticCalc(Expression* res, Expression* exp1, Token* oper, Expression* exp2){
    bool is_int = (res->type.compare("INT") == 0);
    string res_register = is_int ? res->register_name : freshReg();
    string op = "";
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

void emitCondition(Node* res, Expression* exp1, Token* oper, Expression* exp2){
    string cond = freshReg();
    string op = "";
    if(oper->token.compare("==") == 0) op = "eq";
    if(oper->token.compare("<") == 0) op = "slt";
    if(oper->token.compare(">") == 0) op = "sgt";
    if(oper->token.compare("<=") == 0) op = "sle";
    if(oper->token.compare(">=") == 0) op = "sge";
    if(oper->token.compare("~=") == 0) op = "ne";
    emit(cond + " = icmp " + op + " i32 " + exp1->register_name + ", " + exp2->register_name);
    int address = emit("br i1 " + cond + ", label @, label @");
    res->truelist = makelist({address, FIRST});
    res->falselist = makelist({address, SECOND});
}

void emitString (Node* res, Token* str){
    string str_reg = freshStrReg();
    int string_len = str->token.length() + 1;
    emitGlobal(str_reg + " = constant [" + to_string(string_len) + " x i8] c\"" + str->token + "\\00\"");
    res->register_name = freshReg();
    emit(res->register_name + " = getelementptr [" + to_string(string_len) + " x i8], [" + to_string(string_len) +
                              " x i8]* " + str_reg + ", i32 0, i32 0");
}

string boolAssignment(Node* boolExp){
    bool_assign_cnt++;
    string phi_res = freshReg();
    string result_reg = freshReg();
    string false_label = genLabel();;
    int false_address = emit("br label @");
    string true_label = genLabel();
    int true_address = emit("br label @");

    bpatch(boolExp->truelist, true_label);
    bpatch(boolExp->falselist, false_label);

    string phi_label = genLabel();
    bpatch(makelist({true_address,FIRST}), phi_label);
    bpatch(makelist({false_address,FIRST}), phi_label);

    string to_load = freshReg();
    emit(phi_res + " = phi i1 [0, %"+false_label+"], [1, %"+true_label+"]");
    emit(to_load + "= zext i1 "+ phi_res + " to i32");
    if(boolExp->label.compare("") != 0){
        emit("br label %" + boolExp->label);
    }
    return to_load;
}
void storeInStack(SymbolTable* symbol_table, Token* id, Expression* exp, Token* type){
    int stack_offset = symbol_table->getStackOffset(id);
    string stack_ptr = freshReg();
    string res_reg = (type->token.compare("BOOL") != 0) ? exp->register_name : boolAssignment(exp);
    emit(stack_ptr + " = getelementptr inbounds i32, i32* %f" + to_string(function_cnt) + "args, i32 "
                    + to_string(stack_offset));
    emit("store i32 " + res_reg + ", i32* " + stack_ptr);
}

void enterFunctionIR (Token* type, Token* id, FormalsList* args){
    function_cnt++;
    string return_type = (type->token.compare("VOID") == 0 ) ? "void" : "i32";
    string arg_list = "";
    FormalsList* current_args_list = args;
    if(!current_args_list->value) //functions with no arguments
        current_args_list = nullptr;

    int i = 1;
    while(current_args_list){
        arg_list += "i32 %f" + to_string(function_cnt) + "n" + to_string(i);
        if(current_args_list->next){
            arg_list += ", ";
            current_args_list = (FormalsList*)(current_args_list->next);
        }else{
            break;
        }
        i++;

    }
    emit("define " + return_type + " @" + id->token + "(" + arg_list + "){");
    
    string stack_ptr = "%f" + to_string(function_cnt) + "args";
    emit(stack_ptr + " = alloca i32, i32 50");

    for(int j = 0; j < i; j++){
        string cur_arg_reg = freshReg();
        emit(cur_arg_reg + " = getelementptr inbounds i32, i32* " + stack_ptr + ", i32 " + to_string(j));
        emit("store i32 %f" + to_string(function_cnt) + "n" + to_string(j+1) + ", i32* " + cur_arg_reg);
    }
}

void exitFunctionIR(Token* type){
    if(type->token.compare("VOID") == 0){
        emit("ret void");
    }else{
        emit("ret i32 1");
    }
    emit("}");
}

void handleId(SymbolTable* symbol_table, Expression* res, Token* id){
    int offset = symbol_table->getStackOffset(id);
    string stack_ptr = freshReg();
    emit(stack_ptr + "= getelementptr inbounds i32, i32* %f" + to_string(function_cnt) + "args, i32 " + to_string(offset));
    string cur_reg = freshReg();
    emit(cur_reg + "= load i32, i32* " + stack_ptr);
    if(res->type.compare("BOOL") != 0){
        res->register_name = cur_reg;
    }
    else{
        string cond = freshReg();
        emit(cond + " = icmp eq i32 0, " + cur_reg);
        int address = emit("br i1 " + cond + ", label @, label @");
        res->falselist = makelist({address, FIRST});
        res->truelist = makelist({address, SECOND});
    }
}