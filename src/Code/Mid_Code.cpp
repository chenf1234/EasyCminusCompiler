#include<iostream>
#include<algorithm>
#include<fstream>
#include"Mid_Code.h"
using namespace std;

map<string,vector<Args> > tmp_var;
vector<code>  my_code;

/**************��ر�������*****************/
static int labels=0; //��Ų����ظ�����ʹ�ǲ�ͬ�ĺ���
static int scope1;//������
static vector<int> scopes1;//��¼���Է��ʵ�������ջ��Ԫ��Ϊ��ǰ������
static int index1=0;//��ʱ��������
static string fun_name1="GLOBALS";
code c;//һ�����������е�����ַ��

//����������ջ
void delete1(){
    int size=c.ins.size()-1;
    //�Ӻ�ʼѰ�ҵڶ��γ��ֵĶ�Ԫ�����
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
            }//ȡ����ֵ��ջ
            break;
        }
        default:
            break;
        }
    }
}

//���ö�Ԫ������ֵis_stackΪtrue
void change1(){
    int size=c.ins.size()-1;
    //�Ӻ�ʼѰ�ҵڶ��γ��ֵĶ�Ԫ�����
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
            }//��ֵ��ջ
            break;
        }
           
        
        default:
            break;
        }
    }
    return ;
}

/***************��Ԫ�����ת��Ӧ������ַ����******************/
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

/*****************��������ַ��**************/
Args Op_code(TreeNode *t,int left){
    Args arg;
    arg.is_array=false;
    if(t==NULL||t->is_deal==true) return arg;
    t->is_deal=true;//ָʾ�ýڵ��Ѿ����ɹ��м����
    
    switch(t->nodekind){
        case FUN_DECK:{//��������
        
            scope1=1;
            index1=0;//��ʱ������������
            fun_name1=t->child[1]->val;
            //cout<<fun_name1<<endl;
            //�޸���Ӧ�Ĳ���
            c.fuction=fun_name1;
            c.stack_size=fun_table[fun_name1].stack_size;
            //cout<<fun_table[fun_name1].stack_size<<endl;
            //cout<<c.stack_size<<endl;
            c.max_param_size=12;//�����������������Ҫѹ������������ͬʱ%esp+8��λ�ñ��������ֵ������Ϊ12
            c.ins.clear();

            Instruction singel_code;//��������ַ��
            //singel_code.line=t->lineno;
            singel_code.label=fun_name1;
            singel_code.op=FUNCTION_CODE;
            c.ins.push_back(singel_code);
            break;
        }
        case VAR_DECK:{
            if(fun_name1=="GLOBALS"){//ȫ�ֺ�������ʱ
                Instruction singel_code;//��������ַ��
                singel_code.op=GLOBAL_VAL_CODE;
                string var_name=t->child[1]->val;
                if(t->child[1]->nodekind==ARRK)var_name=t->child[1]->child[0]->val;//���������
                singel_code.label=var_name;
                //singel_code.line=t->lineno;
                c.max_param_size=4;
                c.fuction=fun_name1;
                c.stack_size=0;
                c.ins.push_back(singel_code);
                my_code.push_back(c);//ȫ�ֱ���������Ϊ����ָ��棬�ֲ�������������Ϊָ��
                c.ins.clear();
            }
            break;
        }
        case ADDOPK:
        case MULOPK:
        case RELOPK:{//��Ԫ����
           
            Args arg1=Op_code(t->child[0],1);
            Args arg2=Op_code(t->child[1],0);
            //��Ԫ�������߶�����ʱ��������һ����ʱ������ֵ��ջ����
            if(arg1.is_global_tmp==2&&arg2.is_global_tmp==2){
                change1();//���ö�Ԫ�������ֵis_stackΪtrue
            }
            else if(arg1.is_global_tmp==2)//��ֵ������ʱ����
                delete1();
            Instruction singel_code;
            singel_code.label="";
            //singel_code.line=t->lineno;
            singel_code.op=Binary_to_Mid(t->val);//�����ĸ������
            
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
            }//�����۵�
            else{
                
                singel_code.arg1=arg1;
                singel_code.arg2=arg2;
                c.ins.push_back(singel_code);
                arg.id_string="tmp"+to_string(index1);//��ʱ����������
                arg.is_num=false;//������
                arg.is_global_tmp=2;//��ʱ����
                arg.array_loc=0;
                arg.loc=index1++;
                tmp_var[fun_name1].push_back(arg);//������ʱ����
            }
            break;
        }
        case ASSIGNK:{//���ܴ���������ֵ
           
            Args arg2=Op_code(t->child[1],0);//���ڸ�ֵ���㣬�ȴ���ֵ���ŵ���ֵ
            Args arg1=Op_code(t->child[0],1);
            Instruction signel_code;
            
            signel_code.op=ASSIGN_CODE;
            
            signel_code.label="";
            signel_code.arg1=arg2;//����ַ�룬arg1Ϊ��ֵ������ֵ��arg2Ϊ��ֵ������ֵ
            signel_code.arg2=arg1;
            c.ins.push_back(signel_code);
            arg=arg1;
            break;
        }
        case NUMK:{//����
            arg.is_num=true;//������
            arg.is_global_tmp=1;
            arg.id_string="NUMBER";
            arg.array_loc=0;
            arg.loc=atoi(t->val.c_str());//loc��������ֵ
            break;
        }
        case ARR_ELEMK:{//���Ϊ�����е�Ԫ��
            string names=t->child[0]->val;
            Args arg1=Op_code(t->child[0],0);
            Args arg2=Op_code(t->child[1],0);
            Instruction singel_code;
            
            singel_code.op=ARRAY_CODE;//����Ԫ�ط��ʲ���
            singel_code.label="";
            singel_code.arg1=arg1;
            singel_code.arg2=arg2;
            singel_code.is_left=left;
            c.ins.push_back(singel_code);
            arg.array_loc=0;
            arg.is_num=false;
            arg.is_array=true;//���ڴ�������Ԫ�ظ�ֵ
            arg.is_global_tmp=2;//��������Ԫ�ط��ʲ�������������ʱ����
            arg.id_string="tmp"+to_string(index1);
            arg.loc=index1++;
            tmp_var[fun_name1].push_back(arg);//������ʱ����
            break;
        }
        case IDK:{//���ΪIDʱ���ҳ���ȷ�����������µ�ID
            string names=t->val;
            //cout<<t->lineno<<" ID:"<<names<<endl;
            int ans=-1;

            arg.id_string=names;
            arg.is_num=false;

            bool is_arg=false;//�ñ����Ƿ����ں����Ĳ���
            if(sys_table[fun_name1].find(names)!=sys_table[fun_name1].end()) {		
				Var_sym tmp=sys_table[fun_name1][names];
				for(int i=0;i<(int)tmp.lines.size();i++){
					//cout<<i<<endl;
					if(tmp.lines[i].scope==1){//���ڲ����д��ڸ�ID
                        is_arg=true;
                        arg.array_loc=tmp.lines[i].loc;//���������λ��
                        if(tmp.lines[i].ty==Integer_arr)arg.is_array=true;
                        else arg.is_array=false;
                    }
					else if(find(scopes1.begin(),scopes1.end(),tmp.lines[i].scope)!=scopes1.end() ){
						ans=tmp.lines[i].loc;
                        is_arg=false;	
                        if(tmp.lines[i].ty==Integer_arr)arg.is_array=true;
                        else arg.is_array=false;
					}//�����ڷ��ű���ǰ�棬���ű��������ƥ���ID����ôID�����ڲ����У������ڴ�λ��
					
				}
			}
            //cout<<ans<<" "<<is_arg<<endl;
            if(ans==-1&&is_arg==false){//�ں����ķ��ű���û�ҵ��ñ�������ô����ȫ������
                arg.array_loc=0;
                arg.is_global_tmp=0;//ȫ����
                arg.loc=0;
            }
            else if(is_arg==true){//��ID���ں����Ĳ�����
                arg.is_global_tmp=1;//ȫ����
                arg.loc=-1;
            }
            else{//ID�ں����ж���
                arg.is_global_tmp=1;
                arg.loc=ans;
                arg.array_loc=0;
            }
            //cout<<arg.loc<<endl;
            break;
        }
        case CALLK:{//��������
            Args arg1=Op_code(t->child[0],0);
            TreeNode *tmp_node=t->child[1];
            string names=t->child[0]->val;//���ú�����
            int p_size=fun_table[names].p_num*4;
            if(p_size>c.max_param_size) c.max_param_size=p_size;
            if(tmp_node!=NULL){//���ڲ���
                //TreeNode* temp=tmp_node->child[0];
                int param_num=0;
                
                param_num=fun_table[names].p_num;
                int i=0;
                while(i!=param_num){//�����һ��������ʼѹջ
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
                    singel_code.op=PUTPARAM_CODE;//����ѹջ
                    if(names=="output")singel_code.putparam_pos=2;//����������������
                    else singel_code.putparam_pos=j+1;
                    c.ins.push_back(singel_code);
                    i++;
                }

            }
            if(names=="input"){//���뺯������ʽa=input(),������ֵ���浽a��
                Instruction singel_code;
                singel_code.op=PUTPARAM_CODE;
                //singel_code.line=t->lineno;
                singel_code.label="";
                Args arg12;
                arg12.id_string="$.LC0";
                arg12.is_array=false;
                arg12.is_num=false;
                arg12.array_loc=0;
                arg12.is_global_tmp=0;//����Ϊȫ�ֱ���
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
                arg12.is_global_tmp=0;//����Ϊȫ�ֱ���
                arg12.loc=0;
                singel_code.arg1=arg12;
                singel_code.putparam_pos=1;
                c.ins.push_back(singel_code);
                arg1.id_string="printf";
            }//�����������ʽoutput(a),��a��ֵ��һ�����з�һ���ӡ����׼����豸 
            Instruction singel_code;
           // singel_code.line=t->lineno;
            singel_code.arg1=arg1;

            singel_code.label="";
            singel_code.op=CALL_CODE;
            c.ins.push_back(singel_code);//����ѹջ����к�������
            arg.array_loc=0;
            arg.loc=-2;
            arg.is_num=false;
            arg.is_global_tmp=1;
            arg.id_string="Call_function";//�������õķ���ֵ
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
            int temp_label=0;//��ʱ�����ǩ

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

            temp_label=labels++;//������ʱ��ǩ
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

            //whileѭ���ڲ�����
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

            //then�����ڲ�����
            Op_code(t->child[1],0);

            //��if���else���ִ���ʱ
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

            //else�����ڲ�����
            if(t->child[2]!=NULL){
                Op_code(t->child[2],0);
            }

            //��if���else���ִ���ʱ
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
            if(t->child[0]!=NULL){//ֻ����return����ֵʱ������return����ַ�룻��������
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
            scopes1.push_back(scope1);//���뻨���ţ�������+1������ջ
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
            if(scopes1.back()==0){//�˳���������ʱ��������Ӧ������ַ�뱣�浽my_code��
                
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

/***************���������﷨�����***************/
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


/**************��ӡ����ַ��***********/
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
                //�޲�������ַ����
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
                //һ������������ַ����
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
                //��������������ַ����
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

/***********�����м����**************/
void Generate_mid_code(TreeNode* t){
    scope1=0;
    scopes1.push_back(scope1);
    postProc_traverse(t);
    print_code();
}