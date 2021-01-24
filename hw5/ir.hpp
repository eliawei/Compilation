#include <iostream>
#include "bp.hpp"
#include "parser.hpp"
#include "symbolTable.hpp"

using namespace std;

int reg_cnt = 0;
int function_cnt = 0;
int bool_assign_cnt = 0;
int set_cnt = 0;
const string set_type = ""; 

string freshReg(){
    reg_cnt++;
    return "%r" + to_string(reg_cnt);
}

string freshStrReg(){
    reg_cnt++;
    return "@.str" + to_string(reg_cnt);
}

string freshSetReg(){
    set_cnt++;
    return "%s" + to_string(reg_cnt);
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
    emit("declare i8* @malloc(i32)");
    emit("declare void @free(i8*)");
    emit("declare void @llvm.memcpy.p0i8.p0i8.i32(i8*, i8*, i32, i1)");
    emitGlobal("@.int_specifier = constant [4 x i8] c\"%d\\0A\\00\"");
    emitGlobal("@.str_specifier = constant [4 x i8] c\"%s\\0A\\00\"");
    emitGlobal("@.zero_specifier = constant [23 x i8] c\"Error division by zero\\00\"");
    emit("define void @printi(i32) {");
    emit("call i32 (i8*, ...) @printf(i8* getelementptr ([4 x i8], [4 x i8]* @.int_specifier, i32 0, i32 0), i32 %0)");
    emit("ret void");
    emit("}");
    emit("define void @print(i8*) {");
    emit("call i32 (i8*, ...) @printf(i8* getelementptr ([4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0), i8* %0)");
    emit("ret void");
    emit("}");
}

int z_label = 0;

void ValidateNoZeroDiv(Expression* exp2){
    z_label++;
    string zeroLabel = "label_zero" + to_string(z_label);
    string nonZeroLabel = "label_non_zero" + to_string(z_label);
    string comp_reg = freshReg();
    //compare expression 2 to 0
    emit(comp_reg + " = " + "icmp eq i32 " + exp2->register_name +", 0");
    //if is zero than print error and exit else continue
    emit("br i1 " + comp_reg + ", label %" + zeroLabel + ", label %"+ nonZeroLabel);
    emit(zeroLabel + ":");
    emit("call void @print(i8* getelementptr ([23 x i8], [23 x i8]* @.zero_specifier, i32 0, i32 0))");
    emit("call void @exit(i32 0)");
    emit("br label %" + nonZeroLabel);
    emit(nonZeroLabel + ":");
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
        ValidateNoZeroDiv(exp2);
    }

    emit(res_register + " = " + op + " i32 " + exp1->register_name + ", " +exp2->register_name);
    if(!is_int){
        string temp_reg = freshReg();
        emit(temp_reg + " = trunc i32 " + res_register + " to i8");
        emit(res->register_name + " = zext i8 " + temp_reg + " to i32");
    }
}

int check = 0;
void checkSetRange(string set_ptr, string num_reg, string op){
    check++;
    string bad_label = "label_bad" + to_string(check);
    string good_label = "label_good" + to_string(check);
    string cont_label = "label_cont" + to_string(check);
    string first_elem_loc = freshReg();
    emit(first_elem_loc + " = getelementptr inbounds i32, i32* " + set_ptr + ", i32 1");
    string first_elem = freshReg();
    emit(first_elem + " = load i32, i32* " + first_elem_loc);

    string total_size_loc = freshReg();
    emit(total_size_loc + " = getelementptr inbounds i32, i32* " + set_ptr + ", i32 0");
    string total_size = freshReg();
    emit(total_size + " = load i32, i32* " + total_size_loc);

    string temp = freshReg();
    emit(temp + " = add i32 " + first_elem + ", " + total_size);
    string last_element = freshReg();
    emit(last_element + " = sub i32 " + temp + ", 4");

    string cond_bigger = freshReg();
    emit(cond_bigger + " = icmp sgt i32 " + num_reg + ", " + last_element);
    emit("br i1 " + cond_bigger + ", label %" + bad_label +", label %" + cont_label);

    emit(cont_label + ":");
    string cond_smaller = freshReg();
    emit(cond_smaller + " = icmp slt i32 " + num_reg + ", " + first_elem);
    int address_smaller = emit("br i1 " + cond_smaller + ", label %" + bad_label + ", label %" + good_label);
    
    emit(bad_label + ":");
    string err = freshStrReg();
    string str_len = (op.compare("in") == 0)? "31" : "30";
    emitGlobal(err + " = constant [" + str_len + " x i8] c\"Error out of set range. Op: " + op + "\\00\"");
    string to_print = freshReg();
    emit(to_print + " = getelementptr [" + str_len + " x i8], [" + str_len + " x i8]* " + err + ", i32 0, i32 0");

    emit("call void @print(i8* " + to_print + ")");
    emit("call void @exit(i32 1)");

    emit("br label %" + good_label);
    emit(good_label + ":");
}



string addToSet(Expression* set, Expression* num){
    string set_ptr = freshReg();
    emit(set_ptr + " = inttoptr i32 " + set->register_name + " to i32*");
    checkSetRange(set_ptr, num->register_name, "+");

    string tot_size_loc = freshReg();
    emit(tot_size_loc + " = getelementptr inbounds i32, i32* " + set_ptr + ", i32 0");
    string tot_size = freshReg();
    emit(tot_size + " = load i32, i32* " + tot_size_loc);

    string size_in_bytes = freshReg();
    size_in_bytes = freshReg();
    emit(size_in_bytes + " = mul i32 4, " + tot_size);

    string temp_reg = freshReg();
    emit(temp_reg + " = call i8* @malloc(i32 " + size_in_bytes + ")");
    string new_set_ptr = freshReg();
    emit(new_set_ptr + " = bitcast i8* " + temp_reg + " to i32*");
    string new_set_int = freshReg();
    emit(new_set_int + " = ptrtoint i32* " + new_set_ptr + " to i32");
    
    string src_reg = freshReg();
    emit(src_reg + " = bitcast i32* " + set_ptr + " to i8*");
    emit("call void @llvm.memcpy.p0i8.p0i8.i32(i8* " + temp_reg + ", i8* " + src_reg + ", i32 " + size_in_bytes + ", i1 1)");

    string first_elem_loc = freshReg();
    emit(first_elem_loc + " = getelementptr inbounds i32, i32* " + new_set_ptr + ", i32 1");
    string first_elem = freshReg();
    emit(first_elem + " = load i32, i32* " + first_elem_loc);
    
    string temp = freshReg();
    emit(temp + " = sub i32 " + num->register_name + ", " + first_elem);
    string location = freshReg();
    //emit(location + " = add i32 4, " + temp);
    emit(location + " = add i32 3, " + temp);

    string num_loc = freshReg();
    emit(num_loc + " = getelementptr inbounds i32, i32* " + new_set_ptr + ", i32 " + location);
    string cur_val = freshReg();
    emit(cur_val + " = load i32, i32* " + num_loc);

    string cond = freshReg();
    emit(cond + " = icmp eq i32 0, " + cur_val);
    int address = emit("br i1 " + cond + ", label@, label@");
    string label1 = genLabel();
    bpatch(makelist({address, FIRST}), label1);
    emit("store i32 1, i32* " + num_loc);

    string cur_size_loc = freshReg();
    emit(cur_size_loc + " = getelementptr inbounds i32, i32* " + new_set_ptr + ", i32 2");
    string cur_size = freshReg();
    emit(cur_size + " = load i32, i32* " + cur_size_loc);

    string add_size = freshReg();
    emit(add_size + " = add i32 1, " + cur_size);
    emit("store i32 " + add_size + ", i32* " + cur_size_loc);

    int addr_end_true = emit("br label @");
    string label2 = genLabel();
    bpatch(merge(makelist({address, SECOND}), makelist({addr_end_true, FIRST})), label2);

    return new_set_int;
}

string subFromSet(Expression* set, Expression* num){
    string set_ptr = freshReg();
    emit(set_ptr + " = inttoptr i32 " + set->register_name + " to i32*");
    checkSetRange(set_ptr, num->register_name, "-");

    string tot_size_loc = freshReg();
    emit(tot_size_loc + " = getelementptr inbounds i32, i32* " + set_ptr + ", i32 0");
    string tot_size = freshReg();
    emit(tot_size + " = load i32, i32* " + tot_size_loc);

    string size_in_bytes = freshReg();
    size_in_bytes = freshReg();
    emit(size_in_bytes + " = mul i32 4, " + tot_size);

    string temp_reg = freshReg();
    emit(temp_reg + " = call i8* @malloc(i32 " + size_in_bytes + ")");
    string new_set_ptr = freshReg();
    emit(new_set_ptr + " = bitcast i8* " + temp_reg + " to i32*");
    string new_set_int = freshReg();
    emit(new_set_int + " = ptrtoint i32* " + new_set_ptr + " to i32");
    
    string src_reg = freshReg();
    emit(src_reg + " = bitcast i32* " + set_ptr + " to i8*");
    emit("call void @llvm.memcpy.p0i8.p0i8.i32(i8* " + temp_reg + ", i8* " + src_reg + ", i32 " + size_in_bytes + ", i1 1)");

    string first_elem_loc = freshReg();
    emit(first_elem_loc + " = getelementptr inbounds i32, i32* " + new_set_ptr + ", i32 1");
    string first_elem = freshReg();
    emit(first_elem + " = load i32, i32* " + first_elem_loc);

    string temp = freshReg();
    emit(temp + " = sub i32 " + num->register_name + ", " + first_elem);
    string location = freshReg();
    emit(location + " = add i32 3, " + temp);

    string num_loc = freshReg();
    emit(num_loc + " = getelementptr inbounds i32, i32* " + new_set_ptr + ", i32 " + location);
    string cur_val = freshReg();
    emit(cur_val + " = load i32, i32* " + num_loc);

    string cond = freshReg();
    emit(cond + " = icmp eq i32 1, " + cur_val);
    int address = emit("br i1 " + cond + ", label@, label@");
    string label1 = genLabel();
    bpatch(makelist({address, FIRST}), label1);
    emit("store i32 0, i32* " + num_loc);

    string cur_size_loc = freshReg();
    emit(cur_size_loc + " = getelementptr inbounds i32, i32* " + new_set_ptr + ", i32 2");
    string cur_size = freshReg();
    emit(cur_size + " = load i32, i32* " + cur_size_loc);

    string sub_size = freshReg();
    emit(sub_size + " = sub i32 " + cur_size + ", 1");
    emit("store i32 " + sub_size + ", i32* " + cur_size_loc);

    int addr_end_true = emit("br label @");
    string label2 = genLabel();
    bpatch(merge(makelist({address, SECOND}), makelist({addr_end_true, FIRST})), label2);
    return new_set_int;
}

string setOperators(Expression* exp1, Token* op, Expression* exp2){
    if(op->token.compare("+") == 0){
        if(exp1->type.compare("SET") == 0){
            return addToSet(exp1, exp2);
        }
        else
            return addToSet(exp2, exp1);
    }
    else{
        if(exp1->type.compare("SET") == 0)
            return subFromSet(exp1, exp2);
        else
            return subFromSet(exp2, exp1);
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
    if(oper->token.compare("!=") == 0) op = "ne";
    emit(cond + " = icmp " + op + " i32 " + exp1->register_name + ", " + exp2->register_name);
    int address = emit("br i1 " + cond + ", label @, label @");
    res->truelist = makelist({address, FIRST});
    res->falselist = makelist({address, SECOND});
}

void isInSet(Expression* res, Expression* num, Expression* set){
    string set_ptr = freshReg();
    emit(set_ptr + " = inttoptr i32 " + set->register_name + " to i32*");
    checkSetRange(set_ptr, num->register_name, "in");

    
    string tot_size_loc = freshReg();
    emit(tot_size_loc + " = getelementptr inbounds i32, i32* " + set_ptr + ", i32 0");
    string tot_size = freshReg();
    emit(tot_size + " = load i32, i32* " + tot_size_loc);
    string first_elem_loc = freshReg();
    emit(first_elem_loc + " = getelementptr inbounds i32, i32* " + set_ptr + ", i32 1");
    string first_elem = freshReg();
    emit(first_elem + " = load i32, i32* " + first_elem_loc);

    string temp = freshReg();
    emit(temp + " = sub i32 " + num->register_name + ", " + first_elem);
    string location = freshReg();
    emit(location + " = add i32 3, " + temp);

    string num_loc = freshReg();
    emit(num_loc + " = getelementptr inbounds i32, i32* " + set_ptr + ", i32 " + location);
    string cur_val = freshReg();
    emit(cur_val + " = load i32, i32* " + num_loc);
    
    string cond = freshReg();
    emit(cond + " = icmp eq i32 1, " + cur_val);
    int address = emit("br i1 " + cond + ", label @, label @");
    res->truelist = makelist({address, FIRST});
    res->falselist = makelist({address, SECOND});
}

void emitString (Node* res, Token* str){
    string str_reg = freshStrReg();
    int string_len = str->token.length() + 1;
    emitGlobal(str_reg + " = constant [" + to_string(string_len-2) + " x i8] c\""+ str->token.substr(1,string_len-3) + "\\00\"");
    res->register_name = freshReg();
    emit(res->register_name + " = getelementptr [" + to_string(string_len-2) + " x i8], [" + to_string(string_len-2) +
                              " x i8]* " + str_reg + ", i32 0, i32 0");
}

string boolAssignment(Node* boolExp){
    bool_assign_cnt++;
    string phi_res = freshReg();
    string result_reg = freshReg();
    string false_label = genLabel();
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

void storeInStack(SymbolTable* symbol_table, Token* id, Expression* exp, Type* type){
    int stack_offset = symbol_table->getStackOffset(id);
    string stack_ptr = freshReg();
    string res = "";

    if(!exp)
        res = "0";
    else
        res = (type->type->token.compare("BOOL") != 0) ? exp->register_name : boolAssignment(exp); 
    
    emit(stack_ptr + " = getelementptr inbounds i32, i32* %f" + to_string(function_cnt) + "args, i32 "
                    + to_string(stack_offset));
    emit("store i32 " + res + ", i32* " + stack_ptr);
}

string copySet(SymbolTable* symbol_table, Token* id, Expression* exp){
    int stack_offset = symbol_table->getStackOffset(id);
    string stack_ptr = freshReg();
    emit(stack_ptr + " = getelementptr inbounds i32, i32* %f" + to_string(function_cnt) + "args, i32 "
                + to_string(stack_offset));
        
    string cur_set_i32 = freshReg();
    emit(cur_set_i32 + " = inttoptr i32 " + exp->register_name + " to i32*");
    string tot_size_loc = freshReg();
    emit(tot_size_loc + " = getelementptr inbounds i32, i32* " + cur_set_i32 + ", i32 0");
    string temp_size = freshReg();
    emit(temp_size + " = load i32, i32* " + tot_size_loc);
    string tot_size = freshReg();
    //emit(tot_size + " = add i32 4, " + temp_size);
    emit(tot_size + " = add i32 0, " + temp_size);
    string size_in_bytes = freshReg();
    size_in_bytes = freshReg();
    emit(size_in_bytes + " = mul i32 4, " + tot_size);

    string temp_reg = freshReg();
    emit(temp_reg + " = call i8* @malloc(i32 " + size_in_bytes + ")");
    string set_ptr = freshReg();
    emit(set_ptr + " = bitcast i8* " + temp_reg + " to i32*");

    string prev_ptr_int = freshReg();
    emit(prev_ptr_int + " = load i32, i32* " + stack_ptr);
    string prev_ptr_i32 = freshReg();
    emit(prev_ptr_i32 + " = inttoptr i32 " + prev_ptr_int + " to i32*");
    string prev_ptr_i8 = freshReg();
    emit(prev_ptr_i8 + " = bitcast i32* " + prev_ptr_i32 + " to i8*");
    emit("call void @free(i8* " + prev_ptr_i8 + ")");

    string set_ptr2 = freshReg();
    emit(set_ptr2 + " = ptrtoint i32* " + set_ptr + " to i32");
    emit("store i32 " + set_ptr2 + ", i32* " + stack_ptr);

    string src_reg = freshReg();
    emit(src_reg + " = bitcast i32* " + cur_set_i32 + " to i8*");
    emit("call void @llvm.memcpy.p0i8.p0i8.i32(i8* " + temp_reg + ", i8* " + src_reg + ", i32 " + size_in_bytes + ", i1 1)");

    return set_ptr2;
}


string storeSet(SymbolTable* symbol_table, Token* id, Expression* exp, Set* set){
    
    string size_in_bytes = "";
    if(exp){
        string set_ptr = freshReg();
        emit(set_ptr + " = inttoptr i32 " + exp->register_name + " to i32*");

        string tot_size_loc = freshReg();
        emit(tot_size_loc + " = getelementptr inbounds i32, i32* " + set_ptr + ", i32 0");
        string temp_size = freshReg();
        emit(temp_size + " = load i32, i32* " + tot_size_loc);
        string tot_size = freshReg();
        // emit(tot_size + " = add i32 4, " + temp_size);
        emit(tot_size + " = add i32 0, " + temp_size);

        size_in_bytes = freshReg();
        emit(size_in_bytes + " = mul i32 4, " + tot_size);
    }
    else{
        int start_num = stoi((set->from_num)->token);
        int end_num = stoi(set->to_num->token);
        set->total_size = end_num - start_num + 4;
        int size_in_byte = 4 * (end_num - start_num + 4);
        size_in_bytes = to_string(size_in_byte);

    }

    string res = "";
    string set_i8 = freshReg();
    emit(set_i8 + " = call i8* @malloc(i32 " + size_in_bytes + ")");

    string set_i32 = freshSetReg();
     emit(set_i32 + " = bitcast i8* " + set_i8 + " to i32*");

    int stack_offset = symbol_table->getStackOffset(id);
    string stack_ptr = freshReg();
    emit(stack_ptr + " = getelementptr inbounds i32, i32* %f" + to_string(function_cnt) + "args, i32 "
                + to_string(stack_offset));

    string set_ptr2 = freshReg();
    emit(set_ptr2 + " = ptrtoint i32* " + set_i32 + " to i32");

    emit("store i32 " + set_ptr2 + ", i32* " + stack_ptr);

    if(exp){
        string temp = freshReg();
        emit(temp + " = inttoptr i32 " + exp->register_name + " to i32*");
        string src_reg = freshReg();
        emit(src_reg + " = bitcast i32* " + temp + " to i8*");
        emit("call void @llvm.memcpy.p0i8.p0i8.i32(i8*" + set_i8 + ", i8* " + src_reg + ", i32 " + size_in_bytes + ", i1 1)");
    }

    else{
        string cur_reg = freshReg();
        emit(cur_reg + " = getelementptr inbounds i32, i32* " + set_i32 + ", i32 0");
        string total_size = freshReg();
        emit(total_size + " = add i32 0, " + to_string(set->total_size));
        emit("store i32 " + total_size + ", i32* " + cur_reg);

        cur_reg = freshReg();
        emit(cur_reg + " = getelementptr inbounds i32, i32* " + set_i32 + ", i32 1");
        string first_elem = freshReg();
        emit(first_elem + " = add i32 0, " + set->from_num->token);
        emit("store i32 " + first_elem + ", i32* " + cur_reg);
        
        for(int i = 2; i < set->total_size; i++){
            string cur_reg = freshReg();
            emit(cur_reg + " = getelementptr inbounds i32, i32* " + set_i32 + ", i32 " + to_string(i));
            emit("store i32 0, i32* " + cur_reg);
        }
    }

    //test
    // emit("%t1 = load i32, i32* " + stack_ptr);
    // emit("%t2 = inttoptr i32 %t1 to i32*");
    // emit("%t3 = getelementptr inbounds i32, i32* %t2, i32 0");
    // emit("%t4 = load i32, i32* %t3");
    // emit("call void @printi(i32 %t4)");

    //end test

    return set_ptr2;

}

string getSetSize(Expression* set){
    string set_ptr = freshReg();
    emit(set_ptr + " = inttoptr i32 " + set->register_name + " to i32*");
    string size_loc = freshReg();
    emit(size_loc + " = getelementptr inbounds i32, i32* " + set_ptr + ", i32 2");
    string size = freshReg();
    emit(size + " = load i32, i32* " + size_loc);
    return size;
}

void callFunction(Call* res, string type, Token* id, ExpList* params = nullptr){
    if(params){
        int address = emit("br label @");
        params->nextlist = makelist({address, FIRST});
    }
    string return_type = "void";
    string res_reg = "";
    string assign = "";
    if(type.compare("VOID") != 0){
        return_type = "i32";
        res_reg = freshReg();
        assign = res_reg + " = ";
    }
    res->register_name = res_reg;
    
    string params_list = "";
    ExpList* current_param = params;
    while(current_param){
        string param_type = "";
        string curr_reg_name = current_param->value->register_name;
        if(id->token.compare("print") == 0){
            param_type = "i8* ";
        }else{
            param_type = "i32 ";
            if((current_param->value->type.compare("BOOL") == 0)){
                string load_bool = boolAssignment(current_param->value);
                curr_reg_name = load_bool;
            }
        }
        params_list += param_type + curr_reg_name;
        if(current_param->next){
            params_list += ", ";
            current_param = (ExpList*)(current_param->next);
        }else{
            break;
        }
    }

    int address = emit("br label @");
    string label = genLabel();
    emit(assign + "call " + return_type + " @" + id->token + "(" + params_list + ")");

    if(params){
        bpatch(merge(makelist({address, FIRST}), params->nextlist), label);
    }
    else{
        bpatch(makelist({address, FIRST}), label);
    }
    if(res->type.compare("BOOL") == 0){
        string cond = freshReg();
        emit(cond + " = icmp eq i32 0, " + res_reg);
        int address2 = emit("br i1 " + cond + ", label @, label@");
        res->falselist = makelist({address2, FIRST});
        res->truelist = makelist({address2, SECOND});
    }
    
}

void enterFunctionIR (Token* type, Token* id, FormalsList* args){
    function_cnt++;
    string return_type = (type->token.compare("VOID") == 0 ) ? "void" : "i32";
    string arg_list = "";
    FormalsList* current_args_list = args;
    string stack_ptr = "%f" + to_string(function_cnt) + "args";
    int i = 0;
    if(current_args_list->value) //functions with no arguments
        while(current_args_list){
            i++;
            arg_list += "i32 %f" + to_string(function_cnt) + "n" + to_string(i);
            if(current_args_list->next){
                arg_list += ", ";
                current_args_list = (FormalsList*)(current_args_list->next);
            }else{
                break;
            }
    }
    // Function definition 
    emit("define " + return_type + " @" + id->token + "(" + arg_list + "){");
    // Allocate memory on stack for loval vars(max 50)
    emit(stack_ptr + " = alloca i32, i32 50");
    for(int j = 0; j < i; j++){
        string cur_arg_reg = freshReg();
        emit(cur_arg_reg + " = getelementptr inbounds i32, i32* " + stack_ptr + ", i32 " + to_string(j));
        emit("store i32 %f" + to_string(function_cnt) + "n" + to_string(j+1) + ", i32* " + cur_arg_reg);
    }
}

void exitFunctionIR(Token* type){
    if(type->token.compare("VOID") == 0)
        emit("ret void");
    else
        emit("ret i32 1");
}

void retFromFunction(Expression* exp = nullptr){
    if(!exp){
        emit("ret void");
        return;
    }
    string res_reg = exp->register_name;
    if(exp->type.compare("BOOL") == 0){
        res_reg = boolAssignment((Node*)exp);
    }
    emit("ret i32 " + res_reg);
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

