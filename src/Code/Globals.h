#ifndef _GLOBALS_H_
#define _GLOBALS_H_
#include<iostream>
#include<cstring>
#include<map>
#include<vector>
using namespace std;
/*****c-���Թ��õĴʷ�******/
typedef enum
{
    ENDFILE,ERROR,
    /* ������ */
    IF,ELSE,INT,RETURN,WHILE,VOID,
    /*��ʶ��������*/
    ID,NUM,
    /* special symbols */
    PLUS,MINUS,TIMES,OVER,LT,LTE,HT,HTE,EQ,NEQ,ASSIGN,SEMI,COM,LPAREN,RPAREN,MLPAREN,MRPAREN,LLPAREN,LRPAREN
   /* +   -     *     /   <  <=   >  >= == !=   =       ;   ,    (       )     [       ]       {       }  */
} TokenType;

/************�����﷨���������***********/
typedef enum {
	VAR_DECK,FUN_DECK,INTK,VOIDK,IDK,NUMK,PARAMK,COM_SK,
	IFK,WHILEK,RETURNK,RELOPK,ADDOPK,MULOPK,ARRK,ARR_ELEMK,
	CALLK,ARGK,ASSIGNK
}NodeKind; 

/************��������**************/
typedef enum {Void,Integer,Boolean,Integer_arr} ExpType;

/***********�����﷨���������ݽṹ**********/
typedef struct treeNode{ 
	struct treeNode * child[5];
    struct treeNode * sibling;
    int lineno;//����
    NodeKind nodekind;
    string val;
    ExpType type;//����������ͼ��ʹ�ã��������
    //int tmp_loc;//�м��������ʹ�ã���ʱ��������,-1����ʱ������>=0��ʱ����,��ʾ��tmp_var�е�����
    bool is_deal;//�м��������ʹ�ã��ýڵ��Ƿ��Ѿ����ɹ��м����
} TreeNode;
 
/************�����ķ��ű���¼�����ĸ�����ÿ�������������Լ���������***************/
typedef struct Fun_sys
{ 
    int p_num;// �����ĸ���
    vector<ExpType> p_type;//ÿ������������ 
    ExpType return_type;
    int stack_size=0;//����ջ�Ĵ�С
} Fun_sym;

/*************��¼�������������ڴ�λ�á�����************/
typedef struct LineListRec
{ 		
    int linepoc=0;//��Դ���е�����
    int loc;//�ڴ��е�λ��,��Ϊ����ʱ��ʾ������λ��
	int scope;//������0--ȫ�ֱ���
	ExpType ty;//����
    int sizes;//�����Ĵ�С
} LineList;

/**********ͬһ�����У�������ͬ����ID��������**********/
typedef struct Var_sys
{ 
    vector<LineList> lines; 
} Var_sym; 


extern map<string,Fun_sym>  fun_table;//���к����ķ��ű���¼�������ͣ�����������ÿ������������
extern map<string,map<string,Var_sym> > sys_table;//�����б����ķ��ű�keyΪ��������valueΪ�����ķ��ű� 
extern int errors1;//���ļ���ʹ�ã�errors1Ϊ1�ǣ���ʾ������ִ���ע��extern��static����ͬʱʹ��

typedef enum {
    ERROR_CODE,
    /**�޲�������ַ�����***/
    FUNCTION_CODE,GLOBAL_VAL_CODE,LABELS_CODE,
    /*********�����������************/
    CALL_CODE,PUTPARAM_CODE,RETURN_CODE,
    /**********if,while���***************/
    IF_CODE,GOTO_CODE,
    /*****��Ԫ�����*****/
    PLUS_CODE,MINUS_CODE,TIMES_CODE,OVER_CODE,LT_CODE,LTE_CODE,HT_CODE,HTE_CODE,EQ_CODE,NEQ_CODE,
    /******��ֵ����******/
    ASSIGN_CODE,
    /********�������**********/
    ARRAY_CODE
}Mid_code_type;//�м����������

typedef struct name_2_loc{
    string id_string;   //�������� tmp��ʾ��ʱ����
    int is_global_tmp;//�ñ����Ƿ�����ȫ����(0),�ֲ�����(1),������ʱ����(2)
    bool is_num;//�ò����Ƿ�Ϊ����
    int loc ;    
    /*
    loc=-2,��ʾ�������ú�ķ���ֵ
    loc=-1,��ʾΪ����
    is_numΪfalseʱ��
        is_global_tmp=2,tmp_var������
        is_global_tmp=1,ջ��λ��
        is_global_tmp=0,ֻ��¼id_string,������Ϊ0
    is_numΪtrue������ֵ;
    */
    int array_loc;
    /*
        �����е�λ�ã���Ϊ��ͨ����ʱ��ֵΪ0;
        ��loc=-1ʱ����ʾ�ñ���Ϊ��������ʱarray_loc��ʾ�ڼ�������
    */
    bool is_array=false;
    /*
        ��ʶID�Ƿ�Ϊ���飬��IDΪ����ʱ���������õĹ�����ʹ��lealָ����������׵�ַ
        leal �����׵�ַ��%eax
        is_array�����ɻ�����ʱ����������ѹ��ʹ��
        ͬʱis_arrayҲ������������Ԫ�ظ�ֵ
    */
}Args;

typedef struct Mid
{   
   // int line=0;//Դ���е�����
    string label="";//��ָ��ǰ�Ƿ��б��
    Mid_code_type op;  //������
    Args arg1;   //�����ֶ�
    Args arg2;
    int putparam_pos=0;//����ַ��λputparamʱ��ѹջ�����ں�������ջ�б����λ��
    //is_into_stack��is_left���ڴ���������ɶ�Ԫ�������߶�����ʱ��������ֵ��ջ�����
    int is_into_stack=0;//��ʱ������ֵ�Ƿ���Ҫ��ջ
    bool is_left=false; //��Ԫ�������ֵ
}Instruction;   //����ַ����Ԫʽ��ʾ

typedef struct Code{
    //ջ�ж���16������ʱ����
    string fuction;//��������
    int stack_size;//ջ�Ĵ�С
    int max_param_size;//Ϊ�������ò���Ԥ�������ջ�ռ�
    vector<Instruction> ins;//����ַ��ָ������
}code;//�����е���������ַ��,��ִ��˳������


extern map<string,vector<Args> > tmp_var;//������ʱ����
extern vector<code>  my_code;
#endif