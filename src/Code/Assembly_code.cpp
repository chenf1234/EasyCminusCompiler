#include "Assembly_code.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
using namespace std;

//edx寄存器存放临时变量，eax寄存器存放函数返回值，ecx寄存器存放if语句的测试条件值
//ebx寄存器用于加减乘运算保存运算左值，edx保存加减乘运算右值
//eax,esi寄存器用于处理除法运算，edx保存除法运算结果
//比较运算符，ebx保存第一操作数的值，edx保存第二操作数的值，结果保存在edx中
//edi保存数组元素在栈中地址

static int s_pos=0;//栈中位置
static string function_name="GLOBALS";
static int stack_size1=0;
static int param_size1=0;
static ofstream ass_out;
bool is_2(int num){//判断num是否为2的幂次方
    if(num&(num-1))return false;
    else return true;
}
int log2(int value){//求num是2的多少次方
    int x=0;  
    while(value>1)  
    {  
        value>>=1;  
        x++;  
    }  
    return x;  
}
void create_assembly_code(Instruction ins){
    switch (ins.op)
    {
        case FUNCTION_CODE:{//生成栈帧
            ass_out<<ins.label<<":"<<endl;
            ass_out<<"\tpushl\t%ebp"<<endl;
            ass_out<<"\tmovl\t%esp, %ebp"<<endl;
            //栈中多留16个给临时变量
            ass_out<<"\tsubl\t$"<<stack_size1+param_size1+16<<", %esp"<<endl;
            break;
        }
        case GLOBAL_VAL_CODE:{//全局变量声明，在bss段定义数据元素
            string var_name=ins.label;
            int size=sys_table["GLOBALS"][var_name].lines[0].sizes;
            if(size<32)
                ass_out<<"\t.comm  "<<var_name<<","<<size<<",4"<<endl;
            else ass_out<<"\t.comm  "<<var_name<<","<<size<<",32"<<endl;
            break;
        }
        case LABELS_CODE:{
            string var_name=ins.label;
            ass_out<<"."<<var_name<<":"<<endl;
            break;
        }
        case GOTO_CODE:{
            string label=ins.arg1.id_string;
            ass_out<<"\tjmp\t\t."<<label<<endl;
            break;
        }
        case RETURN_CODE:{
            Args arg1=ins.arg1;
            if(arg1.is_global_tmp==2)ass_out<<"\tmovl %edx,%eax"<<endl;//临时变量
            else if(arg1.is_global_tmp==0){//全局变量
                ass_out<<"\tmovl\t"<<arg1.id_string<<",%eax"<<endl;
            }
            else {//局部变量时
                if(arg1.is_num==true){//返回值为常数
                    ass_out<<"\tmovl\t$"<<arg1.loc<<",%eax"<<endl;
                }
                else{//返回值为本地声明,不存在返回数组指针
                    if(arg1.loc==-1){
                        ass_out<<"\tmovl\t"<<arg1.array_loc*4+4<<"(%ebp),%eax\n";
                    }//使用的是参数
                    else if(arg1.loc==-2){//函数返回值
                        ass_out<<"\tmovl\t%eax,%eax"<<endl;
                    }
                    else ass_out<<"\tmovl\t"<<arg1.loc+param_size1<<"(%esp),%eax"<<endl;
                }
            }
            s_pos=0;//return后，栈中临时变量清空
            break;//函数返回值保存在eax中
        }
        case CALL_CODE:{//函数调用
            string f_name=ins.arg1.id_string;
            if(f_name=="__isoc99_scanf"){
                ass_out<<"\tleal\t8(%esp),%eax"<<endl;
                ass_out<<"\tmovl\t%eax,4(%esp)"<<endl;
            }//输入函数input的输入值保存到8(%esp)中
            ass_out<<"\tcall\t"<<f_name<<endl;
            if(f_name=="__isoc99_scanf"){
                ass_out<<"\tmovl\t8(%esp),%eax"<<endl;
            }//将input的输入值保存到eax中作为函数返回值
            break;
        }
        case PUTPARAM_CODE:{//函数调用的参数入栈，注意数组，放置的为数组起始地址
            Args arg1=ins.arg1;
            if(arg1.is_global_tmp==2)ass_out<<"\tmovl\t%edx,"<<(ins.putparam_pos-1)*4<<"(%esp)"<<endl;//临时变量
            else if(arg1.is_global_tmp==0){//全局变量
                if(arg1.is_array==true){//全局数组
                    ass_out<<"\tmovl\t$"<<arg1.id_string<<","<<(ins.putparam_pos-1)*4<<"(%esp)"<<endl;
                }
                else{//非数组
                    ass_out<<"\tmovl\t"<<arg1.id_string<<",%ebx"<<endl;
                    ass_out<<"\tmovl\t%ebx,"<<(ins.putparam_pos-1)*4<<"(%esp)"<<endl;
                }
            }
            else {//局部变量时
                if(arg1.is_num==true){//返回值为常数
                    ass_out<<"\tmovl\t$"<<arg1.loc<<","<<(ins.putparam_pos-1)*4<<"(%esp)"<<endl;
                    
                }
                else{//本地声明,可能是数组指针
                    if(arg1.is_array==true){//数组指针作为参数
                        if(arg1.loc==-1){
                            ass_out<<"\tmovl\t"<<arg1.array_loc*4+4<<"(%ebp),%eax\n";
                            ass_out<<"\tmovl\t%eax,"<<(ins.putparam_pos-1)*4<<"(%esp)"<<endl;
                        }//使用的是参数
                        else{
                            ass_out<<"\tleal\t"<<arg1.loc+param_size1<<"(%esp),%eax"<<endl;
                            ass_out<<"\tmovl\t%eax,"<<(ins.putparam_pos-1)*4<<"(%esp)"<<endl;
                        }//函数中定义的变量
                    }
                    else{//普通变量
                        if(arg1.loc==-1){
                            ass_out<<"\tmovl\t"<<arg1.array_loc*4+4<<"(%ebp),%eax"<<endl;
                            ass_out<<"\tmovl\t%eax,"<<(ins.putparam_pos-1)*4<<"(%esp)"<<endl;
                        }//使用的是参数
                        else{
                            ass_out<<"\tmovl\t"<<arg1.loc+param_size1<<"(%esp),%eax"<<endl;
                            ass_out<<"\tmovl\t%eax,"<<(ins.putparam_pos-1)*4<<"(%esp)"<<endl;
                        }//函数中定义的变量
                    }
                }
            }
            s_pos=0;//压参完成后，栈中临时变量清空
            break;
        }
        //二元运算符两边都为临时变量的情况还不能处理，这里得使用寄存器分配算法，而我没使用
        case TIMES_CODE:
        case MINUS_CODE:
        case PLUS_CODE:{//不存在指针运算
            Args arg1=ins.arg1;
            Args arg2=ins.arg2;
            //cout<<stack_size1<<" "<<param_size1<<" "<<s_pos<<endl;
            //左右值都是临时变量,ebx存的都是左值
            if(arg1.is_global_tmp==2&&arg2.is_global_tmp==2){
                s_pos-=4;//临时变量的位置被释放
                ass_out<<"\tmovl\t"<<stack_size1+param_size1+s_pos<<"(%esp),%ebx"<<endl;
                goto L1;
            }
            //左值保存在%ebx中，右值保存在%edx中，结果保存在%edx中
            if(ins.op==TIMES_CODE){//乘法运算时，检查乘数是否存在数字
                if(arg1.is_num||arg2.is_num){//乘数存在数字
                    if(arg1.is_num&&is_2(arg1.loc)){//左值为数字并且数字为2的幂次方，交换左右值
                        Args tmp=arg1;
                        arg1=arg2;
                        arg2=tmp;
                    }
                    if(arg2.is_num&&is_2(arg2.loc)){//数字为2的幂次方,通过移位进行优化
                        if(arg1.is_global_tmp==0){//全局变量
                            ass_out<<"\tmovl\t"<<arg1.id_string<<",%edx\n";
                        }
                        else if(arg1.is_global_tmp==2){//临时变量
                            //ass_out<<"\tmovl\t%edx,%edx"<<endl;
                        }
                        else{
                            if(arg1.is_num==true){//数字
                                ass_out<<"\tmovl\t$"<<arg1.loc<<",%edx"<<endl;
                            }
                            else{
                                if(arg1.loc==-1){//参数
                                    ass_out<<"\tmovl\t"<<arg1.array_loc*4+4<<"(%ebp),%edx\n";
                                }
                                else if(arg1.loc==-2){//函数返回值
                                    ass_out<<"\tmovl\t%eax,%edx"<<endl;
                                }
                                else{//函数内部定义的变量
                                    ass_out<<"\tmovl\t"<<arg1.loc+param_size1<<"(%esp),%edx"<<endl;;
                                }
                            }
                        }
                        int mi=log2(arg2.loc);
                        ass_out<<"\tsal\t\t$"<<mi<<",%edx"<<endl;
                        break;
                    }
                }
            }
            if(arg1.is_global_tmp==0){//全局变量
                ass_out<<"\tmovl\t"<<arg1.id_string<<",%ebx\n";
            }
            else if(arg1.is_global_tmp==2){//临时变量
                ass_out<<"\tmovl\t%edx,%ebx"<<endl;
            }
            else{
                if(arg1.is_num==true){//数字
                    ass_out<<"\tmovl\t$"<<arg1.loc<<",%ebx"<<endl;
                }
                else{
                    if(arg1.loc==-1){//参数
                        ass_out<<"\tmovl\t"<<arg1.array_loc*4+4<<"(%ebp),%ebx\n";
                    }
                    else if(arg1.loc==-2){//函数返回值
                        ass_out<<"\tmovl\t%eax,%ebx"<<endl;
                    }
                    else{//函数内部定义的变量
                        ass_out<<"\tmovl\t"<<arg1.loc+param_size1<<"(%esp),%ebx"<<endl;;
                    }
                }
            }
            if(arg2.is_global_tmp==0){//全局变量
                ass_out<<"\tmovl\t"<<arg2.id_string<<",%edx\n";
            }
            else if(arg2.is_global_tmp==2){//临时变量
                //ass_out<<"\tmovl\t%edx,%edx"<<endl;
            }
            else{
                if(arg2.is_num==true){//数字
                    ass_out<<"\tmovl\t$"<<arg2.loc<<",%edx"<<endl;
                }
                else{
                    if(arg2.loc==-1){//参数
                        ass_out<<"\tmovl\t"<<arg2.array_loc*4+4<<"(%ebp),%edx\n";
                    }
                    else if(arg2.loc==-2){//函数返回值
                        ass_out<<"\tmovl\t%eax,%edx"<<endl;
                    }
                    else{//函数内部定义的变量
                        ass_out<<"\tmovl\t"<<arg2.loc+param_size1<<"(%esp),%edx"<<endl;;
                    }
                }
            }
            L1:
            if(ins.op==PLUS_CODE)ass_out<<"\taddl\t%ebx,%edx"<<endl;
            else if(ins.op==MINUS_CODE){
                ass_out<<"\tsubl\t%edx,%ebx"<<endl;
                ass_out<<"\tmovl\t%ebx,%edx"<<endl;
            }
            else{
                ass_out<<"\timull\t%ebx,%edx"<<endl;
            }
            
            if(ins.is_into_stack==1){
                ass_out<<"\tmovl\t%edx,"<<stack_size1+param_size1+s_pos<<"(%esp)"<<endl;
                s_pos+=4;
            }//值会被覆盖，存入栈中
            break;
        }
        case OVER_CODE:{//不存在数组指针运算
            
            Args arg1=ins.arg1;
            Args arg2=ins.arg2;
            //cout<<"chufa: "<<arg1.id_string<<endl;
            //左右值都是临时变量,eax存的都是被除数
            if(arg1.is_global_tmp==2&&arg2.is_global_tmp==2){
                s_pos-=4;//临时变量的位置被释放
                ass_out<<"\tmovl\t"<<stack_size1+param_size1+s_pos<<"(%esp),%eax"<<endl;
                ass_out<<"\tmovl\t%edx,%esi"<<endl;
                ass_out<<"\tcltd\n";//对edx 寄存器进行零扩展
                
                ass_out<<"\tdivl\t%esi"<<endl;
                ass_out<<"\tmovl\t%eax,%edx"<<endl;//除法结果要保存到edx中
                if(ins.is_into_stack==1){
                    ass_out<<"\tmovl\t%edx,"<<stack_size1+param_size1+s_pos<<"(%esp)"<<endl;
                    s_pos+=4;
                }//值会被覆盖，存入栈中
                break;
            }
            if(arg2.is_num==true&&is_2(arg2.loc)){//若除数为数字并且是2的幂次方，通过移位运算进行优化
                int mi=log2(arg2.loc);
                if(arg1.is_global_tmp==0){//全局变量
                    ass_out<<"\tmovl\t"<<arg1.id_string<<",%edx\n";
                }
                else if(arg1.is_global_tmp==2){//临时变量
                    //ass_out<<"\tmovl\t%edx,%edx"<<endl;
                }
                else{
                    if(arg1.is_num==true){//数字
                        ass_out<<"\tmovl\t$"<<arg1.loc<<",%edx"<<endl;
                    }
                    else{
                        if(arg1.loc==-1){//参数
                            ass_out<<"\tmovl\t"<<arg1.array_loc*4+4<<"(%ebp),%edx\n";
                        }
                        else if(arg1.loc==-2){//函数返回值
                            ass_out<<"\tmovl\t%eax,%edx"<<endl;
                        }
                        else{//函数内部定义的变量
                            ass_out<<"\tmovl\t"<<arg1.loc+param_size1<<"(%esp),%edx"<<endl;;
                        }
                    }
                }
                ass_out<<"\tshr\t\t$"<<mi<<",%edx"<<endl;//都是正数，所以移出位进行0扩展
                if(ins.is_into_stack==1){
                    ass_out<<"\tmovl\t%edx,"<<stack_size1+param_size1+s_pos<<"(%esp)"<<endl;
                    s_pos+=4;
                }//值会被覆盖，存入栈中
                 break;
            }
            else{//普通除法运算,先处理被除数再处理除数
                if(arg2.is_global_tmp==0){//全局变量
                    ass_out<<"\tmovl\t"<<arg2.id_string<<",%esi\n";
                }
                else if(arg2.is_global_tmp==2){//临时变量
                    ass_out<<"\tmovl\t%edx,%esi"<<endl;
                }
                else{
                    if(arg2.is_num==true){//数字
                        ass_out<<"\tmovl\t$"<<arg2.loc<<",%esi"<<endl;
                    }
                    else{
                        if(arg2.loc==-1){//参数
                            ass_out<<"\tmovl\t"<<arg2.array_loc*4+4<<"(%ebp),%esi\n";
                        }
                        else if(arg2.loc==-2){//函数返回值
                            ass_out<<"\tmovl\t%eax,%esi"<<endl;
                        }
                        else{//函数内部定义的变量
                            ass_out<<"\tmovl\t"<<arg2.loc+param_size1<<"(%esp),%esi"<<endl;;
                        }
                    }
                }
                if(arg1.is_global_tmp==0){//全局变量
                    ass_out<<"\tmovl\t"<<arg1.id_string<<",%eax\n";
                }
                else if(arg1.is_global_tmp==2){//临时变量
                    ass_out<<"\tmovl\t%edx,%eax"<<endl;
                }
                else{
                    if(arg1.is_num==true){//数字
                        ass_out<<"\tmovl\t$"<<arg1.loc<<",%eax"<<endl;
                    }
                    else{
                        if(arg1.loc==-1){//参数
                            ass_out<<"\tmovl\t"<<arg1.array_loc*4+4<<"(%ebp),%eax\n";
                        }
                        else if(arg1.loc==-2){//函数返回值
                            //ass_out<<"\tmovl\t%eax,%eax"<<endl;
                        }
                        else{//函数内部定义的变量
                            ass_out<<"\tmovl\t"<<arg1.loc+param_size1<<"(%esp),%eax"<<endl;;
                        }
                    }
                }
                ass_out<<"\tcltd\n";//对edx 寄存器进行零扩展
                ass_out<<"\tdivl\t%esi"<<endl;
                ass_out<<"\tmovl\t%eax,%edx"<<endl;//除法结果要保存到edx中
                if(ins.is_into_stack==1){
                    ass_out<<"\tmovl\t%edx,"<<stack_size1+param_size1+s_pos<<"(%esp)"<<endl;
                    s_pos+=4;
                }//值会被覆盖，存入栈中
                break;
            }
            break;
           
        }
        //比较操作符运算
        case EQ_CODE:
        case NEQ_CODE:
        case LT_CODE:
        case LTE_CODE:
        case HTE_CODE:
        case HT_CODE:{//不存在数组指针运算
            Args arg1=ins.arg1;
            Args arg2=ins.arg2;
            if(arg1.is_global_tmp==2&&arg1.is_global_tmp==2){
                s_pos-=4;//临时变量的位置被释放
                ass_out<<"\tmovl\t"<<stack_size1+param_size1+s_pos<<"(%esp),%ebx"<<endl;
                goto L2;
            }
            if(arg1.is_global_tmp==0){//全局变量
                ass_out<<"\tmovl\t"<<arg1.id_string<<",%ebx\n";
            }
            else if(arg1.is_global_tmp==2){//临时变量
                ass_out<<"\tmovl\t%edx,%ebx"<<endl;
            }
            else{
                if(arg1.is_num==true){//数字
                    ass_out<<"\tmovl\t$"<<arg1.loc<<",%ebx"<<endl;
                }
                else{
                    if(arg1.loc==-1){//参数
                        ass_out<<"\tmovl\t"<<arg1.array_loc*4+4<<"(%ebp),%ebx\n";
                    }
                    else if(arg1.loc==-2){//函数返回值
                        ass_out<<"\tmovl\t%eax,%ebx"<<endl;
                    }
                    else{//函数内部定义的变量
                        ass_out<<"\tmovl\t"<<arg1.loc+param_size1<<"(%esp),%ebx"<<endl;;
                    }
                }
            }
            if(arg2.is_global_tmp==0){//全局变量
                ass_out<<"\tmovl\t"<<arg2.id_string<<",%edx\n";
            }
            else if(arg2.is_global_tmp==2){//临时变量
                //ass_out<<"\tmovl\t%edx,%edx"<<endl;
            }
            else{
                if(arg2.is_num==true){//数字
                    ass_out<<"\tmovl\t$"<<arg2.loc<<",%edx"<<endl;
                }
                else{
                    if(arg2.loc==-1){//参数
                        ass_out<<"\tmovl\t"<<arg2.array_loc*4+4<<"(%ebp),%edx\n";
                    }
                    else if(arg2.loc==-2){//函数返回值
                        ass_out<<"\tmovl\t%eax,%edx"<<endl;
                    }
                    else{//函数内部定义的变量
                        ass_out<<"\tmovl\t"<<arg2.loc+param_size1<<"(%esp),%edx"<<endl;;
                    }
                }
            }
            L2:
            ass_out<<"\tcmpl\t%edx,%ebx"<<endl;
            if(ins.op==LT_CODE){
                ass_out<<"\tsetl\t%al"<<endl;
            }
            else if(ins.op==LTE_CODE){
                ass_out<<"\tsetle\t%al"<<endl;
            }
            else if(ins.op==HT_CODE){
                ass_out<<"\tsetg\t%al"<<endl;
            }
            else if(ins.op==HTE_CODE){
                ass_out<<"\tsetge\t%al"<<endl;
            }
            else if(ins.op==EQ_CODE){
                ass_out<<"\tsete\t%al"<<endl;
            }
            else{
                ass_out<<"\tsetne\t%al"<<endl;
                
            }
            ass_out<<"\tmovzbl\t%al,%eax"<<endl;
            ass_out<<"\tmovl\t%eax,%edx"<<endl;
            if(ins.is_into_stack==1){
                ass_out<<"\tmovl\t%edx,"<<stack_size1+param_size1+s_pos<<"(%esp)"<<endl;
                s_pos+=4;
            }//值会被覆盖，存入栈中
            break;
        }
        case IF_CODE:{//不存在指针运算,判断条件保存在ecx寄存器中
            Args arg1=ins.arg1;
            if(arg1.is_global_tmp==0){//全局变量
                ass_out<<"\tmovl\t"<<arg1.id_string<<",%ecx\n";
            }
            else if(arg1.is_global_tmp==2){//临时变量
                ass_out<<"\tmovl\t%edx,%ecx"<<endl;
            }
            else{
                if(arg1.is_num==true){//数字
                    ass_out<<"\tmovl\t$"<<arg1.loc<<",%ecx"<<endl;
                }
                else{
                    if(arg1.loc==-1){//参数
                        ass_out<<"\tmovl\t"<<arg1.array_loc*4+4<<"(%ebp),%ecx\n";
                    }
                    else if(arg1.loc==-2){//函数返回值
                        ass_out<<"\tmovl\t%eax,%ecx"<<endl;
                    }
                    else{//函数内部定义的变量
                        ass_out<<"\tmovl\t"<<arg1.loc+param_size1<<"(%esp),%ecx"<<endl;;
                    }
                }
            }
            ass_out<<"\tcmpl\t$0,%ecx"<<endl;
            Args arg2=ins.arg2;
            ass_out<<"\tjne\t\t."<<arg2.id_string<<endl;
            s_pos=0;//if条件判断后，栈中临时变量清空
            break;
        }
        //访问数组元素,edi保存数组首地址,ebx保存数组索引,结果保存在edx中
        case ARRAY_CODE:{
            Args arg1=ins.arg1;
            if(arg1.is_global_tmp==0){//全局变量
                ass_out<<"\tleal\t"<<arg1.id_string<<",%edi\n";
            }
            else if(arg1.is_global_tmp==1){//不存在临时变量，不存在数字
                
                    if(arg1.loc==-1){//参数,参数的值就是数组的地址
                        ass_out<<"\tmovl\t"<<arg1.array_loc*4+4<<"(%ebp),%edi\n";
                    }
                    else{//函数内部定义的变量，不存在函数返回值
                        ass_out<<"\tleal\t"<<arg1.loc+param_size1<<"(%esp),%edi"<<endl;;
                    }
                
            }
            Args arg2=ins.arg2;
            if(arg2.is_global_tmp==0){//全局变量
                ass_out<<"\tmovl\t"<<arg2.id_string<<",%ebx\n";
            }
            else if(arg2.is_global_tmp==2){//临时变量
                ass_out<<"\tmovl\t%edx,%ebx"<<endl;
            }
            else{
                if(arg2.is_num==true){//数字
                    ass_out<<"\tmovl\t$"<<arg2.loc<<",%ebx"<<endl;
                }
                else{
                    if(arg2.loc==-1){//参数
                        ass_out<<"\tmovl\t"<<arg2.array_loc*4+4<<"(%ebp),%ebx\n";
                    }
                    else if(arg2.loc==-2){//函数返回值
                        ass_out<<"\tmovl\t%eax,%ebx"<<endl;
                    }
                    else{//函数内部定义的变量
                        ass_out<<"\tmovl\t"<<arg2.loc+param_size1<<"(%esp),%ebx"<<endl;;
                    }
                }
            }
            ass_out<<"\tleal\t0(,%ebx,4),%edx"<<endl;//元素宽度为4,计算基于数组首地址的偏移量
            ass_out<<"\taddl\t%edi,%edx"<<endl;//求得数组元素栈中对应地址
            ass_out<<"\tmovl\t%edx,%edi"<<endl;//将地址的值保存到edi寄存器中
            ass_out<<"\tmovl\t(%edx),%edx"<<endl;//取出地址中的值存入edx中
            if(ins.is_into_stack==1){
                ass_out<<"\tmovl\t%edx,"<<stack_size1+param_size1+s_pos<<"(%esp)"<<endl;
                s_pos+=4;
            }//值会被覆盖，存入栈中
            break;

        }
        case ASSIGN_CODE:{//赋值运算，对数组元素赋值有点不同，不存在数组指针赋值
            //赋值符号左值地址保存在edi寄存器中，右值保存在esi寄存器中
            Args arg1=ins.arg1;
            Args arg2=ins.arg2;
            if(arg1.is_global_tmp==0){//全局变量
                ass_out<<"\tmovl\t"<<arg1.id_string<<",%esi\n";
            }
            else if(arg1.is_global_tmp==2){//临时变量
                ass_out<<"\tmovl\t%edx,%esi"<<endl;
            }
            else{
                if(arg1.is_num==true){//数字
                    ass_out<<"\tmovl\t$"<<arg1.loc<<",%esi"<<endl;
                }
                else{
                    if(arg1.loc==-1){//参数
                        ass_out<<"\tmovl\t"<<arg1.array_loc*4+4<<"(%ebp),%esi\n";
                    }
                    else if(arg1.loc==-2){//函数返回值
                        ass_out<<"\tmovl\t%eax,%esi"<<endl;
                    }
                    else{//函数内部定义的变量
                        ass_out<<"\tmovl\t"<<arg1.loc+param_size1<<"(%esp),%esi"<<endl;;
                    }
                }
            }
            if(arg2.is_global_tmp==0){//全局变量
                ass_out<<"\tleal\t"<<arg2.id_string<<",%edi\n";
            }
            else if(arg2.is_global_tmp==1){//局部变量只为数组元素赋值，已经保存在edi中
                
                if(arg2.loc==-1){//参数
                    ass_out<<"\tleal\t"<<arg2.array_loc*4+4<<"(%ebp),%edi\n";
                }
                else{//函数内部定义的变量
                    ass_out<<"\tleal\t"<<arg2.loc+param_size1<<"(%esp),%edi"<<endl;;
                }
                
            }
            ass_out<<"\tmovl\t%esi,(%edi)"<<endl;
            s_pos=0;//完成赋值后，栈中临时变量清空
            break;
        }
        default:
            break;
    }
}

/********************代码生成*********************/
void Generate_assembly_code(){
    ass_out.open("../Release/main.s");
    ass_out<<"\t.section\t\t.rodata"<<endl;
    ass_out<<".LC0:\n\t.string  \"%d\""<<endl;
    ass_out<<".LC1:\n\t.string  \"%d\\n\""<<endl;

    for(int i=0;i<(int)my_code.size();i++){

        function_name=my_code[i].fuction;//函数名称
        //cout<<function_name<<endl;
        stack_size1=my_code[i].stack_size;//栈大小
        param_size1=my_code[i].max_param_size;//参数空间，即变量的起始位置
        //cout<<stack_size1<<" : "<<param_size1<<endl;
        s_pos=0;
        
        if(function_name!="GLOBALS"){
            ass_out<<"\t.text"<<endl;
            ass_out<<"\t.globl  "<<function_name<<endl;
            ass_out<<"\t.type  "<<function_name<<",@function"<<endl;
        }
        for(int j=0;j<(int)my_code[i].ins.size();j++){
            create_assembly_code(my_code[i].ins[j]);
        }
        if(function_name!="GLOBALS"){
            ass_out<<"\tleave"<<endl;
            ass_out<<"\tret"<<endl;
            ass_out<<"\t.size\t"<<function_name<<",.-"<<function_name<<endl;
        }
    }
}
