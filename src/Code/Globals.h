#ifndef _GLOBALS_H_
#define _GLOBALS_H_
#include<iostream>
#include<cstring>
#include<map>
#include<vector>
using namespace std;
/*****c-语言惯用的词法******/
typedef enum
{
    ENDFILE,ERROR,
    /* 保留字 */
    IF,ELSE,INT,RETURN,WHILE,VOID,
    /*标识符、数字*/
    ID,NUM,
    /* special symbols */
    PLUS,MINUS,TIMES,OVER,LT,LTE,HT,HTE,EQ,NEQ,ASSIGN,SEMI,COM,LPAREN,RPAREN,MLPAREN,MRPAREN,LLPAREN,LRPAREN
   /* +   -     *     /   <  <=   >  >= == !=   =       ;   ,    (       )     [       ]       {       }  */
} TokenType;

/************抽象语法树结点类型***********/
typedef enum {
	VAR_DECK,FUN_DECK,INTK,VOIDK,IDK,NUMK,PARAMK,COM_SK,
	IFK,WHILEK,RETURNK,RELOPK,ADDOPK,MULOPK,ARRK,ARR_ELEMK,
	CALLK,ARGK,ASSIGNK
}NodeKind; 

/************数据类型**************/
typedef enum {Void,Integer,Boolean,Integer_arr} ExpType;

/***********抽象语法树结点的数据结构**********/
typedef struct treeNode{ 
	struct treeNode * child[5];
    struct treeNode * sibling;
    int lineno;//行数
    NodeKind nodekind;
    string val;
    ExpType type;//语义分析类型检查使用，结点类型
    //int tmp_loc;//中间代码生成使用，临时变量索引,-1非临时变量，>=0临时变量,表示在tmp_var中的索引
    bool is_deal;//中间代码生成使用，该节点是否已经生成过中间代码
} TreeNode;
 
/************函数的符号表，记录参数的个数，每个参数的类型以及返回类型***************/
typedef struct Fun_sys
{ 
    int p_num;// 参数的个数
    vector<ExpType> p_type;//每个参数的类型 
    ExpType return_type;
    int stack_size=0;//函数栈的大小
} Fun_sym;

/*************记录变量的作用域、内存位置、类型************/
typedef struct LineListRec
{ 		
    int linepoc=0;//在源码中的行数
    int loc;//内存中的位置,当为参数时表示参数的位置
	int scope;//作用域，0--全局变量
	ExpType ty;//类型
    int sizes;//变量的大小
} LineList;

/**********同一行数中，所有相同名字ID的作用域**********/
typedef struct Var_sys
{ 
    vector<LineList> lines; 
} Var_sym; 


extern map<string,Fun_sym>  fun_table;//所有函数的符号表，记录返回类型，参数个数，每个参数的类型
extern map<string,map<string,Var_sym> > sys_table;//函数中变量的符号表，key为函数名，value为变量的符号表 
extern int errors1;//多文件中使用，errors1为1是，表示编译出现错误。注意extern与static不能同时使用

typedef enum {
    ERROR_CODE,
    /**无参数三地址码符号***/
    FUNCTION_CODE,GLOBAL_VAL_CODE,LABELS_CODE,
    /*********函数调用相关************/
    CALL_CODE,PUTPARAM_CODE,RETURN_CODE,
    /**********if,while相关***************/
    IF_CODE,GOTO_CODE,
    /*****二元运算符*****/
    PLUS_CODE,MINUS_CODE,TIMES_CODE,OVER_CODE,LT_CODE,LTE_CODE,HT_CODE,HTE_CODE,EQ_CODE,NEQ_CODE,
    /******赋值运算******/
    ASSIGN_CODE,
    /********数组访问**********/
    ARRAY_CODE
}Mid_code_type;//中间代码结点类型

typedef struct name_2_loc{
    string id_string;   //变量名字 tmp表示临时变量
    int is_global_tmp;//该变量是否属于全局域(0),局部变量(1),还是临时变量(2)
    bool is_num;//该参数是否为整数
    int loc ;    
    /*
    loc=-2,表示函数调用后的返回值
    loc=-1,表示为参数
    is_num为false时：
        is_global_tmp=2,tmp_var中索引
        is_global_tmp=1,栈中位置
        is_global_tmp=0,只记录id_string,其他均为0
    is_num为true：整数值;
    */
    int array_loc;
    /*
        数组中的位置，当为普通变量时，值为0;
        当loc=-1时，表示该变量为参数，此时array_loc表示第几个参数
    */
    bool is_array=false;
    /*
        标识ID是否为数组，当ID为数组时，函数调用的过程中使用leal指令加载数组首地址
        leal 数组首地址，%eax
        is_array供生成汇编代码时，函数调用压参使用
        同时is_array也用来处理数组元素赋值
    */
}Args;

typedef struct Mid
{   
   // int line=0;//源码中的行数
    string label="";//该指令前是否有标号
    Mid_code_type op;  //操作符
    Args arg1;   //参数字段
    Args arg2;
    int putparam_pos=0;//三地址码位putparam时，压栈参数在函数调用栈中保存的位置
    //is_into_stack和is_left用于处理代码生成二元运算两边都是临时变量，左值入栈的情况
    int is_into_stack=0;//临时变量的值是否需要入栈
    bool is_left=false; //二元运算的左值
}Instruction;   //三地址码三元式表示

typedef struct Code{
    //栈中多留16个给临时变量
    string fuction;//函数名称
    int stack_size;//栈的大小
    int max_param_size;//为函数调用参数预留的最大栈空间
    vector<Instruction> ins;//三地址码指令序列
}code;//函数中的所有三地址码,按执行顺序排列


extern map<string,vector<Args> > tmp_var;//保存临时变量
extern vector<code>  my_code;
#endif