//
// Created by Illay Hai on 2020-06-26.
//
#include <iostream>
#include "bp.hpp"
#include "parser.hpp"

#ifndef OURCODE_IR_HPP
#define OURCODE_IR_HPP

// registers counter
int register_counter = 0;

// current function stack counter
int function_counter = 0;

// number of functions
int func_number = 0;


string freshReg(){
    return "%r" + std::to_string(++register_counter);
}

string freshFuncPointerReg(){
    return "%funcArgs" + std::to_string(++function_counter);
}

string stringReg(){
    return "@.str" + std::to_string(++register_counter);
}

// wrap bp.hpp methods

//gets a pair<int,BranchLabelIndex> item of the form {buffer_location, branch_label_index} and creates a list for it
vector<pair<int,BranchLabelIndex>> makelist(pair<int,BranchLabelIndex> item){
    return CodeBuffer::instance().makelist(item);
}

//merges two lists of {buffer_location, branch_label_index} items
vector<pair<int,BranchLabelIndex>> merge(const vector<pair<int,BranchLabelIndex>> &l1,const vector<pair<int,BranchLabelIndex>> &l2){
    return CodeBuffer::instance().merge(l1, l2);
}

//writes command to the buffer, returns its location in the buffer
int emit(const std::string &command){
    return CodeBuffer::instance().emit(command);
}

//prints the content of the code buffer to stdout
void printCodeBuffer(){
    CodeBuffer::instance().printCodeBuffer();
}

//generates a jump location label for the next command, writes it to the buffer and returns it
std::string genLabel(){
    return CodeBuffer::instance().genLabel();
}

/* accepts a list of {buffer_location, branch_label_index} items and a label.
	For each {buffer_location, branch_label_index} item in address_list, backpatches the branch command
	at buffer_location, at index branch_label_index (FIRST or SECOND), with the label.
	note - the function expects to find a '@' char in place of the missing label.
	note - for unconditional branches (which contain only a single label) use FIRST as the branch_label_index.
	example #1:
	int loc1 = emit("br label @");  - unconditional branch missing a label. ~ Note the '@' ~
	bpatch(makelist({loc1,FIRST}),"my_label"); - location loc1 in the buffer will now contain the command "br label %my_label"
	note that index FIRST referes to the one and only label in the line.
	example #2:
	int loc2 = emit("br i1 %cond, label @, label @"); - conditional branch missing two labels.
	bpatch(makelist({loc2,SECOND}),"my_false_label"); - location loc2 in the buffer will now contain the command "br i1 %cond, label @, label %my_false_label"
	bpatch(makelist({loc2,FIRST}),"my_true_label"); - location loc2 in the buffer will now contain the command "br i1 %cond, label @my_true_label, label %my_false_label"
	*/
void bpatch(const vector<pair<int,BranchLabelIndex>>& address_list, const std::string &label){
    CodeBuffer::instance().bpatch(address_list, label);
}

//write a line to the global section
void emitGlobal(const string& dataLine){
    CodeBuffer::instance().emitGlobal(dataLine);
}

//print the content of the global buffer to stdout
void printGlobalBuffer(){
    CodeBuffer::instance().printGlobalBuffer();
}

void addFunctionToIr(Token* retType, Token* id, FormalList* formals){
    func_number++;
    string argumentList = "";
    FormalList* cur = formals;
    int i = 0;
    while(cur != nullptr){
        if(cur->formalDec != nullptr){
            ++i;
            argumentList += "i32 %func" + std::to_string(func_number) + "arg" + std::to_string(i);
            if(cur->next != nullptr){
                argumentList += ", ";
            }
        }
        cur = cur->next;
    }
    string ret = "void";
    if(retType->text.compare("VOID") != 0) ret = "i32";
    string toEmit = "define " + ret + " @" + id->text + "(" + argumentList +  "){";
    emit(toEmit);
    string stackPtr = freshFuncPointerReg();
    emit(stackPtr + "= alloca i32, i32 50");
    string argPointer;
    for(int j = 0; j < i; j++){
        argPointer = freshReg();
        // get element pointer to use for string on stack
        emit(argPointer + "= getelementptr inbounds i32, i32* " + stackPtr + ", i32 " + std::to_string(j));
        // store the argument on the stack
        emit("store i32 %func" + std::to_string(func_number) + "arg" + std::to_string(j + 1) + ", i32* " + argPointer);
    }
}

int boolAssignLabel = 0;

string boolAssignment(Node* boolExp){
    boolAssignLabel++;
    string phiResult = freshReg();
    string result_reg = freshReg();
    string fLabel = genLabel();;
    int falseAddress = emit("br label @");
    string tLabel = genLabel();
    int trueAddress = emit("br label @");

    bpatch(boolExp->truelist, tLabel);
    bpatch(boolExp->falselist, fLabel);

    string phiLabel = genLabel();
    bpatch(makelist({trueAddress,FIRST}), phiLabel);
    bpatch(makelist({falseAddress,FIRST}), phiLabel);

    string toLoad = freshReg();
    emit(phiResult + " = phi i1 [0, %"+fLabel+"], [1, %"+tLabel+"]");
    emit(toLoad + "= zext i1 "+ phiResult+" to i32");
    if(boolExp->label.compare("") != 0){
        emit("br label %" + boolExp->label);
    }
    return toLoad;
}

string addFunctionCallToIr(Call* call, Node* explist){
    string type = call->type;
    type = (type.compare("VOID") == 0) ? "void" : "i32";
    string retReg = (type.compare("void") == 0) ? "" : freshReg();
    string assign = (type.compare("void") == 0) ? "" : (retReg + "= ");
    string funcId = call->id->text;
    string arglist = "";
    ExpList* curr = call->expList;
    string loadBool;
    if(call->id->text.compare("print") != 0){
        while(curr != nullptr){
            if(curr->exp->type.compare("BOOL") == 0){
                loadBool = boolAssignment(curr->exp);
                arglist += "i32 " + loadBool;
            }
            else{
                arglist += "i32 " + curr->exp->registerName;
            }
            curr = curr->next;
            if(curr != nullptr)
                arglist += ", ";
        }
    }
    else{
            arglist = "i8* " + curr->exp->registerName;
    }
    int address = emit("br label @");
    string label = genLabel();
    if(explist != nullptr)
        bpatch(explist->nextlist, label);
    bpatch(makelist({address, FIRST}), label);
    string toEmit = assign + "call " + type + " @" + funcId + "(" + arglist + ")" ;
    emit(toEmit);
    if(call->type.compare("BOOL") == 0){
        string condBr = freshReg();
        emit(condBr + " = icmp eq i32 0, " + retReg);
        int addr = emit("br i1 " + condBr +", label @, label @");
        call->falselist = makelist({addr,FIRST});
        call->truelist = makelist({addr,SECOND});
        return "";
    }
    return retReg;
}

int label = 0;

void validateNotZeroDiv(Expression* exp){
    string reg = exp->registerName;
    string brReg = freshReg();
    emit(brReg + "= icmp eq i32 " + reg + ", 0");
    ++label;
    string trueLabel = "d" + std::to_string(label);
    string falseLabel = "n" + std::to_string(label);
    emit("br i1 " + brReg + ", label %" + trueLabel + ", label %" + falseLabel);
    emit(trueLabel + ":");
    string regForString = freshReg();
    emit(regForString + " = getelementptr [23 x i8], [23 x i8]* @.zeroDivision, i32 0, i32 0");
    emit("call void @print(i8* " + regForString + ")");
    emit("call void @exit(i32 1)");
    emit("br label %" + falseLabel);
    emit(falseLabel + ":");
}



#endif //OURCODE_IR_HPP
