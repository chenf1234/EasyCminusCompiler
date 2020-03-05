#include<iostream>
#include<algorithm>
#include<fstream>
#include"Mid_Code.h"
using namespace std;

map<string,vector<Args> > tmp_var;
vector<code>  my_code;

/**************相关变量声明*****************/
static int labels=0; //标号不能重复，即使是不同的函数
static int scope1;//作用域
static vector<int> scopes1;//记录可以访问的作用域，栈顶元素为当前作用域
static int index1=0;//临时变量索引
static string fun_name1="GLOBALS";
code c;//一个函数中所有的三地址码

//设置无需入栈
void delete1(){
    int size=c.ins.size()-1;
    //从后开始寻找第二次出现的二元运算符
    for(int i=size;i>=0;i--){
        Mid_code_type c_type=c.ins[i].op;
        switch (c_type)
        {
        case ARRAY_CODE:
        case PLUS_CODE:
        case MINUS_CODE:
        case TIMES_CODE:
        case OVER_CODE:
        case LT_CODE:
        case LTE_CODE:
        case HTE_CODE:
        case EQ_CODE:
        case NEQ_CODE:
        case HT_CODE:{
            if(c.ins[i].is_left&&c.ins[i].is_into_stack==0){
                    c.ins[i].is_into_stack=-1;
                return;
            }//取消左值入栈
            break;
        }
        default:
            break;
        }
    }
}

//设置二元运算左值is_stack为true
void change1(){
    int size=c.ins.size()-1;
    //从后开始寻找第二次出现的二元运算符
    for(int i=size;i>=0;i--){
        Mid_code_type c_type=c.ins[i].op;
        switch (c_type)
        {
        case ARRAY_CODE:
        case PLUS_CODE:
        case MINUS_CODE:
        case TIMES_CODE:
        case OVER_CODE:
        case LT_CODE:
        case LTE_CODE:
        case HTE_CODE:
        case EQ_CODE:
        case NEQ_CODE:
        case HT_CODE:{
            if(c.ins[i].is_left&&c.ins[i].is_into_stack==0){
                    c.ins[i].is_into_stack=1;
                return;
            }//左值入栈
            break;
        }
           
        
        default:
            break;
        }
    }
    return ;
}

/***************二元运算符转相应的三地址码结点******************/
Mid_code_type Binary_to_Mid(string s){
    if(s=="<=")return LTE_CODE;
    if(s=="<") return LT_CODE;
    if(s==">") return HT_CODE;
    if(s==">=")return HTE_CODE;
    if(s=="==")return EQ_CODE;
    if(s=="!=")return NEQ_CODE;
    if(s=="+") return PLUS_CODE;
    if(s=="-") return MINUS_CODE;
    if(s=="*") return TIMES_CODE;
    if(s=="/") return OVER_CODE;
    return ERROR_CODE;
}

/*****************生成三地址码**************/
Args Op_code(TreeNode *t,int left){
    Args arg;
    arg.is_array=false;
    if(t==NULL||t->is_deal==true) return arg;
    t->is_deal=true;//指示该节点已经生成过中间代码
    
    switch(t->nodekind){
        case FUN_DECK:{//函数定义
        
            scope1=1;
            index1=0;//临时变量索引清零
            fun_name1=t->child[1]->val;
            //cout<<fun_name1<<endl;
            //修改相应的参数
            c.fuction=fun_name1;
            c.stack_size=fun_table[fun_name1].stack_size;
            //cout<<fun_table[fun_name1].stack_size<<endl;
            //cout<<c.stack_size<<endl;
            c.max_param_size=12;//对于输入输出函数需要压入两个参数，同时%esp+8的位置保存输入的值，所以为12
            c.ins.clear();

            Instruction singel_code;//单条三地址码
            //singel_code.line=t->lineno;
            singel_code.label=fun_name1;
            singel_code.op=FUNCTION_CODE;
            c.ins.push_back(singel_code);
            break;
        }
        case VAR_DECK:{
            if(fun_name1=="GLOBALS"){//全局函数声明时
                Instruction singel_code;//单条三地址码
                singel_code.op=GLOBAL_VAL_CODE;
                string var_name=t->child[1]->val;
                if(t->child[1]->nodekind==ARRK)var_name=t->child[1]->child[0]->val;//如果是数组
                singel_code.label=var_name;
                //singel_code.line=t->lineno;
                c.max_param_size=4;
                c.fuction=fun_name1;
                c.stack_size=0;
                c.ins.push_back(singel_code);
                my_code.push_back(c);//全局变量声明作为单独指令保存，局部变量声明不作为指令
                c.ins.clear();
            }
            break;
        }
        case ADDOPK:
        case MULOPK:
        case RELOPK:{//二元运算
           
            Args arg1=Op_code(t->child[0],1);
            Args arg2=Op_code(t->child[1],0);
            //二元运算两边都是临时变量，第一个临时变量的值入栈保存
            if(arg1.is_global_tmp==2&&arg2.is_global_tmp==2){
                change1();//设置二元运算的左值is_stack为true
            }
            else if(arg1.is_global_tmp==2)//左值存在临时变量
                delete1();
            Instruction singel_code;
            singel_code.label="";
            //singel_code.line=t->lineno;
            singel_code.op=Binary_to_Mid(t->val);//具体哪个运算符
            
            singel_code.is_left=left;
            if(arg1.is_num==true&&arg2.is_num==true){
                arg.id_string="NUMBER";
                arg.array_loc=0;
                arg.is_global_tmp=1;
                arg.is_num=true;
                switch (singel_code.op)
                {
                    case PLUS_CODE:{
                        arg.loc=arg1.loc+arg2.loc;
                        break;
                    }
                    case MINUS_CODE:{
                        arg.loc=arg1.loc-arg2.loc;
                        break;
                    }
                    case TIMES_CODE:{
                        arg.loc=arg1.loc*arg2.loc;
                        break;
                    }
                    case OVER_CODE:{
                        arg.loc=arg1.loc/arg2.loc;
                        break;
                    }
                    case EQ_CODE:{
                        arg.loc=(arg1.loc==arg2.loc);
                        break;
                    }
                    case NEQ_CODE:{
                        arg.loc=(arg1.loc!=arg2.loc);
                        break;
                    }
                    case LT_CODE:{
                        arg.loc=(arg1.loc<arg2.loc);
                        break;
                    }
                    case LTE_CODE:{
                        arg.loc=(arg1.loc<=arg2.loc);
                        break;
                    }
                    case HT_CODE:{
                        arg.loc=(arg1.loc>arg2.loc);
                        break;
                    }
                    case HTE_CODE:{
                        arg.loc=(arg1.loc>=arg2.loc);
                        break;
                    }
                    
                    default: {
                        arg.loc=0;
                        break;
                    }
                }
            }//常量折叠
            else{
                
                singel_code.arg1=arg1;
                singel_code.arg2=arg2;
                c.ins.push_back(singel_code);
                arg.id_string="tmp"+to_string(index1);//临时变量的名字
                arg.is_num=false;//非数字
                arg.is_global_tmp=2;//临时变量
                arg.array_loc=0;
                arg.loc=index1++;
                tmp_var[fun_name1].push_back(arg);//保存临时变量
            }
            break;
        }
        case ASSIGNK:{//可能存在连续赋值
           
            Args arg2=Op_code(t->child[1],0);//对于赋值运算，先处理赋值符号的右值
            Args arg1=Op_code(t->child[0],1);
            Instruction signel_code;
            
            signel_code.op=ASSIGN_CODE;
            
            signel_code.label="";
            signel_code.arg1=arg2;//三地址码，arg1为赋值符号右值，arg2为赋值符号左值
            signel_code.arg2=arg1;
            c.ins.push_back(signel_code);
            arg=arg1;
            break;
        }
        case NUMK:{//整数
            arg.is_num=true;//是整数
            arg.is_global_tmp=1;
            arg.id_string="NUMBER";
            arg.array_loc=0;
            arg.loc=atoi(t->val.c_str());//loc保存整数值
            break;
        }
        case ARR_ELEMK:{//结点为数组中的元素
            string names=t->child[0]->val;
            Args arg1=Op_code(t->child[0],0);
            Args arg2=Op_code(t->child[1],0);
            Instruction singel_code;
            
            singel_code.op=ARRAY_CODE;//数组元素访问操作
            singel_code.label="";
            singel_code.arg1=arg1;
            singel_code.arg2=arg2;
            singel_code.is_left=left;
            c.ins.push_back(singel_code);
            arg.array_loc=0;
            arg.is_num=false;
            arg.is_array=true;//用于处理数组元素赋值
            arg.is_global_tmp=2;//进行数组元素访问操作后，先生成临时变量
            arg.id_string="tmp"+to_string(index1);
            arg.loc=index1++;
            tmp_var[fun_name1].push_back(arg);//保存临时变量
            break;
        }
        case IDK:{//结点为ID时，找出正确的作用域定义下的ID
            string names=t->val;
            //cout<<t->lineno<<" ID:"<<names<<endl;
            int ans=-1;

            arg.id_string=names;
            arg.is_num=false;

            bool is_arg=false;//该变量是否属于函数的参数
            if(sys_table[fun_name1].find(names)!=sys_table[fun_name1].end()) {		
				Var_sym tmp=sys_table[fun_name1][names];
				for(int i=0;i<(int)tmp.lines.size();i++){
					//cout<<i<<endl;
					if(tmp.lines[i].scope==1){//当在参数中存在该ID
                        is_arg=true;
                        arg.array_loc=tmp.lines[i].loc;//保存参数的位置
                        if(tmp.lines[i].ty==Integer_arr)arg.is_array=true;
                        else arg.is_array=false;
                    }
					else if(find(scopes1.begin(),scopes1.end(),tmp.lines[i].scope)!=scopes1.end() ){
						ans=tmp.lines[i].loc;
                        is_arg=false;	
                        if(tmp.lines[i].ty==Integer_arr)arg.is_array=true;
                        else arg.is_array=false;
					}//参数在符号表最前面，符号表后面仍能匹配该ID，那么ID不处于参数中，保存内存位置
					
				}
			}
            //cout<<ans<<" "<<is_arg<<endl;
            if(ans==-1&&is_arg==false){//在函数的符号表中没找到该变量，那么就在全局域中
                arg.array_loc=0;
                arg.is_global_tmp=0;//全局域
                arg.loc=0;
            }
            else if(is_arg==true){//该ID处于函数的参数中
                arg.is_global_tmp=1;//全局域
                arg.loc=-1;
            }
            else{//ID在函数中定义
                arg.is_global_tmp=1;
                arg.loc=ans;
                arg.array_loc=0;
            }
            //cout<<arg.loc<<endl;
            break;
        }
        case CALLK:{//函数调用
            Args arg1=Op_code(t->child[0],0);
            TreeNode *tmp_node=t->child[1];
            string names=t->child[0]->val;//调用函数名
            int p_size=fun_table[names].p_num*4;
            if(p_size>c.max_param_size) c.max_param_size=p_size;
            if(tmp_node!=NULL){//存在参数
                //TreeNode* temp=tmp_node->child[0];
                int param_num=0;
                
                param_num=fun_table[names].p_num;
                int i=0;
                while(i!=param_num){//从最后一个参数开始压栈
                    TreeNode* temp=tmp_node->child[0];
                    int j=0;
                    while(j<(param_num-i-1)){
                        temp=temp->sibling;
                        j++;
                    }
                    Instruction singel_code;
                    Args arg2=Op_code(temp,0);
                    //singel_code.line=t->lineno;
                    singel_code.arg1=arg2;
                    singel_code.label="";
                    singel_code.op=PUTPARAM_CODE;//参数压栈
                    if(names=="output")singel_code.putparam_pos=2;//输出函数的特殊情况
                    else singel_code.putparam_pos=j+1;
                    c.ins.push_back(singel_code);
                    i++;
                }

            }
            if(names=="input"){//输入函数，格式a=input(),将输入值保存到a中
                Instruction singel_code;
                singel_code.op=PUTPARAM_CODE;
                //singel_code.line=t->lineno;
                singel_code.label="";
                Args arg12;
                arg12.id_string="$.LC0";
                arg12.is_array=false;
                arg12.is_num=false;
                arg12.array_loc=0;
                arg12.is_global_tmp=0;//设置为全局变量
                arg12.loc=0;
                singel_code.arg1=arg12;
                singel_code.putparam_pos=1;
                c.ins.push_back(singel_code);
                arg1.id_string="__isoc99_scanf";
            }
            else if(names=="output"){
                Instruction singel_code;
                //singel_code.line=t->lineno;
                singel_code.op=PUTPARAM_CODE;
                singel_code.label="";
                Args arg12;
                arg12.id_string="$.LC1";
                arg12.is_array=false;
                arg12.is_num=false;
                arg12.array_loc=0;
                arg12.is_global_tmp=0;//设置为全局变量
                arg12.loc=0;
                singel_code.arg1=arg12;
                singel_code.putparam_pos=1;
                c.ins.push_back(singel_code);
                arg1.id_string="printf";
            }//输出函数，格式output(a),将a的值和一个换行符一起打印到标准输出设备 
            Instruction singel_code;
           // singel_code.line=t->lineno;
            singel_code.arg1=arg1;

            singel_code.label="";
            singel_code.op=CALL_CODE;
            c.ins.push_back(singel_code);//参数压栈后进行函数调用
            arg.array_loc=0;
            arg.loc=-2;
            arg.is_num=false;
            arg.is_global_tmp=1;
            arg.id_string="Call_function";//函数调用的返回值
            break;
        }
        case WHILEK:{
            //label begin
            int temp_begin=labels;
            int temp_next=0;
            Instruction signel_code;
            //signel_code.line=t->lineno;
            signel_code.op=LABELS_CODE;
            signel_code.label="L"+to_string(labels++);
            c.ins.push_back(signel_code);
            int temp_label=0;//临时保存标签

            //if t goto L1:
            Args arg1=Op_code(t->child[0],0);//t
            Args arg2;
            arg2.id_string="L"+to_string(labels);
            arg2.is_num=false;
            arg2.loc=0;
            arg2.array_loc=0;
            arg2.is_global_tmp=1;
            signel_code.label="";
            signel_code.op=IF_CODE;//if
            signel_code.arg1=arg1;
            signel_code.arg2=arg2;
            c.ins.push_back(signel_code);

            temp_label=labels++;//保存临时标签
            temp_next=labels++;
            //goto next:
            signel_code.op=GOTO_CODE;
            arg2.id_string="L"+to_string(temp_next);
            signel_code.arg1=arg2;
            c.ins.push_back(signel_code);

            //label L1:
            signel_code.op=LABELS_CODE;
            signel_code.label="L"+to_string(temp_label);
            c.ins.push_back(signel_code);

            //while循环内部代码
            Op_code(t->child[1],0);

            //goto begin
            signel_code.label="";
            signel_code.op=GOTO_CODE;
            arg2.id_string="L"+to_string(temp_begin);
            signel_code.arg1=arg2;
            c.ins.push_back(signel_code);

            //label next
            signel_code.label="";
            signel_code.op=LABELS_CODE;
            signel_code.label="L"+to_string(temp_next);
            c.ins.push_back(signel_code);

            break;
        }
        case IFK:{
            Instruction signel_code;
            int temp_label=0;

            //if t goto L1:
            Args arg1=Op_code(t->child[0],0);//t
            Args arg2;
            arg2.id_string="L"+to_string(labels);
            arg2.is_num=false;
            arg2.loc=0;
            arg2.array_loc=0;
            arg2.is_global_tmp=1;
            signel_code.label="";
            signel_code.op=IF_CODE;//if
            signel_code.arg1=arg1;
            signel_code.arg2=arg2;
            c.ins.push_back(signel_code);
            temp_label=labels++;

            //goto L2:
            signel_code.label="";
            signel_code.op=GOTO_CODE;
            arg2.id_string="L"+to_string(labels);
            signel_code.arg1=arg2;
            c.ins.push_back(signel_code);

            //label L1:
            signel_code.label="";
            signel_code.op=LABELS_CODE;
            signel_code.label="L"+to_string(temp_label);
            c.ins.push_back(signel_code);
            temp_label=labels++;

            //then部分内部代码
            Op_code(t->child[1],0);

            //当if语句else部分存在时
            int temp_next=0;
            if(t->child[2]!=NULL){
                //goto next
                temp_next=labels++;
                signel_code.label="";
                signel_code.op=GOTO_CODE;
                arg2.id_string="L"+to_string(temp_next);
                signel_code.arg1=arg2;
                c.ins.push_back(signel_code);
            }

            //label L2:
            signel_code.label="";
            signel_code.op=LABELS_CODE;
            signel_code.label="L"+to_string(temp_label);
            c.ins.push_back(signel_code);

            //else部分内部代码
            if(t->child[2]!=NULL){
                Op_code(t->child[2],0);
            }

            //当if语句else部分存在时
            if(t->child[2]!=NULL){
                //label next
                signel_code.label="";
                signel_code.op=LABELS_CODE;
                signel_code.label="L"+to_string(temp_next);
                c.ins.push_back(signel_code);
            }
            break;
        }
        case RETURNK:{
            Instruction signel_code;
            Args arg1;
            if(t->child[0]!=NULL){//只有在return存在值时，生成return三地址码；否则不生成
                arg1=Op_code(t->child[0],0);
                signel_code.op=RETURN_CODE;
                signel_code.label="";
                signel_code.arg1=arg1;
                c.ins.push_back(signel_code);
            }
            signel_code.op=GOTO_CODE;
            signel_code.label="";
            Args arg2;
            arg2.id_string="Lend"+fun_name1;
            arg2.is_num=false;
            arg2.loc=0;
            arg2.array_loc=0;
            arg2.is_global_tmp=1;
            signel_code.arg1=arg2;
            c.ins.push_back(signel_code);
            break;
        }
        case COM_SK:{
            scope1++; 
            scopes1.push_back(scope1);//进入花括号，作用域+1，并入栈
            TreeNode* tmp;

            tmp=t->child[0];
            while(tmp!=NULL){
                Op_code(tmp,0);
                tmp=tmp->sibling;
            }
            tmp=t->child[1];
            while(tmp!=NULL){
                Op_code(tmp,0);
                tmp=tmp->sibling;
            }
            scopes1.pop_back();
            if(scopes1.back()==0){//退出函数定义时，函数相应的三地址码保存到my_code中
                
                Instruction singel_code;
                singel_code.op=LABELS_CODE;
                singel_code.label="Lend"+fun_name1;
                c.ins.push_back(singel_code);
                fun_name1="GLOBALS";
                my_code.push_back(c);
                c.fuction=fun_name1;
                c.stack_size=0;
                c.ins.clear();
            }
            break;
        }
        default:break;
    }
    return arg;
}

/***************遍历抽象语法树结点***************/
void postProc_traverse(TreeNode* t){
    if(t!=NULL){
        //cout<<t->lineno<<endl;
        if(!t->is_deal)Op_code(t,0);
        for(int i=0;i<5;i++){
            postProc_traverse(t->child[i]);
        }
        postProc_traverse(t->sibling);
    }
}


/**************打印三地址码***********/
static ofstream out2;

void print_code(){
    out2.open("../OutputFile/Mid_code.txt");
    for(int i=0;i<(int)my_code.size();i++){
        out2<<"function "<<my_code[i].fuction<<" middle code"<<endl;
        out2<<"stack size : "<<my_code[i].stack_size<<" , max_param_size : "<<my_code[i].max_param_size<<endl;
        for(int j=0;j<(int)my_code[i].ins.size();j++){
            int arg_num=0;
            Mid_code_type op=my_code[i].ins[j].op;
            switch (op)
            {
                //无参数三地址码结点
                case FUNCTION_CODE:{
                    out2<<"Function  "<<my_code[i].ins[j].label<<" : "<<endl;
                    break;
                }
                case GLOBAL_VAL_CODE:{
                    out2<<"Global var "<<my_code[i].ins[j].label<<" : "<<endl;
                    break;
                }
                case LABELS_CODE:{
                    out2<<"Label "<<my_code[i].ins[j].label<<" : "<<endl;
                    break;
                }
                //一个参数的三地址码结点
                case GOTO_CODE:{
                    arg_num=1;
                    out2<<"\tGOTO "<<endl;
                    break;
                }
                case RETURN_CODE:{
                    out2<<"\tRETURN"<<endl;
                    arg_num=1;
                    break;
                }
                case CALL_CODE:{
                    out2<<"\tCALL"<<endl;
                    arg_num=1;
                    break;
                }
                case PUTPARAM_CODE:{
                    out2<<"\tPUTPARAM     position:"<<my_code[i].ins[j].putparam_pos<<endl;
                    arg_num=1;
                    break;
                }
                //两个参数的三地址码结点
                case MINUS_CODE:{
                    out2<<"\tMINUS"<<endl;
                    arg_num=2;
                    break;
                }
                case PLUS_CODE:{
                    out2<<"\tADD"<<endl;
                    arg_num=2;
                    break;
                }
                case TIMES_CODE:{
                    out2<<"\tTIMES"<<endl;
                    arg_num=2;
                    break;
                }
                case OVER_CODE:{
                    out2<<"\tOVER"<<endl;
                    arg_num=2;
                    break;
                }
                case LT_CODE:{
                    out2<<"\t<"<<endl;
                    arg_num=2;
                    break;
                }
                case LTE_CODE:{
                    out2<<"\t<="<<endl;
                    arg_num=2;
                    break;
                }
                case HT_CODE:{
                    out2<<"\t>"<<endl;
                    arg_num=2;
                    break;
                }
                case HTE_CODE:{
                    out2<<"\t>="<<endl;
                    arg_num=2;
                    break;
                }
                case EQ_CODE:{
                    out2<<"\tEqual"<<endl;
                    arg_num=2;
                    break;
                }
                case NEQ_CODE:{
                    out2<<"\tNot Equal"<<endl;
                    arg_num=2;
                    break;
                }
                case ASSIGN_CODE:{
                    out2<<"\tAssign to"<<endl;
                    arg_num=2;
                    break;
                }
                case ARRAY_CODE:{
                    out2<<"\tArray Element"<<endl;
                    arg_num=2;
                    break;

                }
                case IF_CODE:{
                    out2<<"\tIF"<<endl;
                    arg_num=2;
                    break;
                }
                default:
                    
                    break;
            }
            out2<<"\tis_into_stack : "<<my_code[i].ins[j].is_into_stack;
            out2<<" , isleft : "<<my_code[i].ins[j].is_left<<endl;
            if(arg_num==1){
                Args arg1=my_code[i].ins[j].arg1;
                out2<<"\t\t"<<"id : "<<arg1.id_string<<" , is_num : "<<arg1.is_num<<" , is_global : "<<arg1.is_global_tmp;
                out2<<" , is_array : "<<arg1.is_array<<" , loc : "<<arg1.loc<<" , array_loc : "<<arg1.array_loc<<endl;
            }
            else if(arg_num==2){
                Args arg1=my_code[i].ins[j].arg1;
                out2<<"\t\t"<<"id : "<<arg1.id_string<<" , is_num : "<<arg1.is_num<<" , is_global : "<<arg1.is_global_tmp;
                out2<<" , is_array : "<<arg1.is_array<<" , loc : "<<arg1.loc<<" , array_loc : "<<arg1.array_loc<<endl;
                Args arg2=my_code[i].ins[j].arg2;
                out2<<"\t\t"<<"id : "<<arg2.id_string<<" , is_num : "<<arg2.is_num<<" , is_global : "<<arg2.is_global_tmp;
                out2<<" , is_array : "<<arg2.is_array<<" , loc : "<<arg2.loc<<" , array_loc : "<<arg2.array_loc<<endl;
            }
        }
        out2<<endl;
    }
}

/***********生成中间代码**************/
void Generate_mid_code(TreeNode* t){
    scope1=0;
    scopes1.push_back(scope1);
    postProc_traverse(t);
    print_code();
}