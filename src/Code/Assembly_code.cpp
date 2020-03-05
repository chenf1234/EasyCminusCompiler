#include "Assembly_code.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
using namespace std;

//edx�Ĵ��������ʱ������eax�Ĵ�����ź�������ֵ��ecx�Ĵ������if���Ĳ�������ֵ
//ebx�Ĵ������ڼӼ������㱣��������ֵ��edx����Ӽ���������ֵ
//eax,esi�Ĵ������ڴ���������㣬edx�������������
//�Ƚ��������ebx�����һ��������ֵ��edx����ڶ���������ֵ�����������edx��
//edi��������Ԫ����ջ�е�ַ

static int s_pos=0;//ջ��λ��
static string function_name="GLOBALS";
static int stack_size1=0;
static int param_size1=0;
static ofstream ass_out;
bool is_2(int num){//�ж�num�Ƿ�Ϊ2���ݴη�
    if(num&(num-1))return false;
    else return true;
}
int log2(int value){//��num��2�Ķ��ٴη�
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
        case FUNCTION_CODE:{//����ջ֡
            ass_out<<ins.label<<":"<<endl;
            ass_out<<"\tpushl\t%ebp"<<endl;
            ass_out<<"\tmovl\t%esp, %ebp"<<endl;
            //ջ�ж���16������ʱ����
            ass_out<<"\tsubl\t$"<<stack_size1+param_size1+16<<", %esp"<<endl;
            break;
        }
        case GLOBAL_VAL_CODE:{//ȫ�ֱ�����������bss�ζ�������Ԫ��
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
            if(arg1.is_global_tmp==2)ass_out<<"\tmovl %edx,%eax"<<endl;//��ʱ����
            else if(arg1.is_global_tmp==0){//ȫ�ֱ���
                ass_out<<"\tmovl\t"<<arg1.id_string<<",%eax"<<endl;
            }
            else {//�ֲ�����ʱ
                if(arg1.is_num==true){//����ֵΪ����
                    ass_out<<"\tmovl\t$"<<arg1.loc<<",%eax"<<endl;
                }
                else{//����ֵΪ��������,�����ڷ�������ָ��
                    if(arg1.loc==-1){
                        ass_out<<"\tmovl\t"<<arg1.array_loc*4+4<<"(%ebp),%eax\n";
                    }//ʹ�õ��ǲ���
                    else if(arg1.loc==-2){//��������ֵ
                        ass_out<<"\tmovl\t%eax,%eax"<<endl;
                    }
                    else ass_out<<"\tmovl\t"<<arg1.loc+param_size1<<"(%esp),%eax"<<endl;
                }
            }
            s_pos=0;//return��ջ����ʱ�������
            break;//��������ֵ������eax��
        }
        case CALL_CODE:{//��������
            string f_name=ins.arg1.id_string;
            if(f_name=="__isoc99_scanf"){
                ass_out<<"\tleal\t8(%esp),%eax"<<endl;
                ass_out<<"\tmovl\t%eax,4(%esp)"<<endl;
            }//���뺯��input������ֵ���浽8(%esp)��
            ass_out<<"\tcall\t"<<f_name<<endl;
            if(f_name=="__isoc99_scanf"){
                ass_out<<"\tmovl\t8(%esp),%eax"<<endl;
            }//��input������ֵ���浽eax����Ϊ��������ֵ
            break;
        }
        case PUTPARAM_CODE:{//�������õĲ�����ջ��ע�����飬���õ�Ϊ������ʼ��ַ
            Args arg1=ins.arg1;
            if(arg1.is_global_tmp==2)ass_out<<"\tmovl\t%edx,"<<(ins.putparam_pos-1)*4<<"(%esp)"<<endl;//��ʱ����
            else if(arg1.is_global_tmp==0){//ȫ�ֱ���
                if(arg1.is_array==true){//ȫ������
                    ass_out<<"\tmovl\t$"<<arg1.id_string<<","<<(ins.putparam_pos-1)*4<<"(%esp)"<<endl;
                }
                else{//������
                    ass_out<<"\tmovl\t"<<arg1.id_string<<",%ebx"<<endl;
                    ass_out<<"\tmovl\t%ebx,"<<(ins.putparam_pos-1)*4<<"(%esp)"<<endl;
                }
            }
            else {//�ֲ�����ʱ
                if(arg1.is_num==true){//����ֵΪ����
                    ass_out<<"\tmovl\t$"<<arg1.loc<<","<<(ins.putparam_pos-1)*4<<"(%esp)"<<endl;
                    
                }
                else{//��������,����������ָ��
                    if(arg1.is_array==true){//����ָ����Ϊ����
                        if(arg1.loc==-1){
                            ass_out<<"\tmovl\t"<<arg1.array_loc*4+4<<"(%ebp),%eax\n";
                            ass_out<<"\tmovl\t%eax,"<<(ins.putparam_pos-1)*4<<"(%esp)"<<endl;
                        }//ʹ�õ��ǲ���
                        else{
                            ass_out<<"\tleal\t"<<arg1.loc+param_size1<<"(%esp),%eax"<<endl;
                            ass_out<<"\tmovl\t%eax,"<<(ins.putparam_pos-1)*4<<"(%esp)"<<endl;
                        }//�����ж���ı���
                    }
                    else{//��ͨ����
                        if(arg1.loc==-1){
                            ass_out<<"\tmovl\t"<<arg1.array_loc*4+4<<"(%ebp),%eax"<<endl;
                            ass_out<<"\tmovl\t%eax,"<<(ins.putparam_pos-1)*4<<"(%esp)"<<endl;
                        }//ʹ�õ��ǲ���
                        else{
                            ass_out<<"\tmovl\t"<<arg1.loc+param_size1<<"(%esp),%eax"<<endl;
                            ass_out<<"\tmovl\t%eax,"<<(ins.putparam_pos-1)*4<<"(%esp)"<<endl;
                        }//�����ж���ı���
                    }
                }
            }
            s_pos=0;//ѹ����ɺ�ջ����ʱ�������
            break;
        }
        //��Ԫ��������߶�Ϊ��ʱ��������������ܴ��������ʹ�üĴ��������㷨������ûʹ��
        case TIMES_CODE:
        case MINUS_CODE:
        case PLUS_CODE:{//������ָ������
            Args arg1=ins.arg1;
            Args arg2=ins.arg2;
            //cout<<stack_size1<<" "<<param_size1<<" "<<s_pos<<endl;
            //����ֵ������ʱ����,ebx��Ķ�����ֵ
            if(arg1.is_global_tmp==2&&arg2.is_global_tmp==2){
                s_pos-=4;//��ʱ������λ�ñ��ͷ�
                ass_out<<"\tmovl\t"<<stack_size1+param_size1+s_pos<<"(%esp),%ebx"<<endl;
                goto L1;
            }
            //��ֵ������%ebx�У���ֵ������%edx�У����������%edx��
            if(ins.op==TIMES_CODE){//�˷�����ʱ���������Ƿ��������
                if(arg1.is_num||arg2.is_num){//������������
                    if(arg1.is_num&&is_2(arg1.loc)){//��ֵΪ���ֲ�������Ϊ2���ݴη�����������ֵ
                        Args tmp=arg1;
                        arg1=arg2;
                        arg2=tmp;
                    }
                    if(arg2.is_num&&is_2(arg2.loc)){//����Ϊ2���ݴη�,ͨ����λ�����Ż�
                        if(arg1.is_global_tmp==0){//ȫ�ֱ���
                            ass_out<<"\tmovl\t"<<arg1.id_string<<",%edx\n";
                        }
                        else if(arg1.is_global_tmp==2){//��ʱ����
                            //ass_out<<"\tmovl\t%edx,%edx"<<endl;
                        }
                        else{
                            if(arg1.is_num==true){//����
                                ass_out<<"\tmovl\t$"<<arg1.loc<<",%edx"<<endl;
                            }
                            else{
                                if(arg1.loc==-1){//����
                                    ass_out<<"\tmovl\t"<<arg1.array_loc*4+4<<"(%ebp),%edx\n";
                                }
                                else if(arg1.loc==-2){//��������ֵ
                                    ass_out<<"\tmovl\t%eax,%edx"<<endl;
                                }
                                else{//�����ڲ�����ı���
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
            if(arg1.is_global_tmp==0){//ȫ�ֱ���
                ass_out<<"\tmovl\t"<<arg1.id_string<<",%ebx\n";
            }
            else if(arg1.is_global_tmp==2){//��ʱ����
                ass_out<<"\tmovl\t%edx,%ebx"<<endl;
            }
            else{
                if(arg1.is_num==true){//����
                    ass_out<<"\tmovl\t$"<<arg1.loc<<",%ebx"<<endl;
                }
                else{
                    if(arg1.loc==-1){//����
                        ass_out<<"\tmovl\t"<<arg1.array_loc*4+4<<"(%ebp),%ebx\n";
                    }
                    else if(arg1.loc==-2){//��������ֵ
                        ass_out<<"\tmovl\t%eax,%ebx"<<endl;
                    }
                    else{//�����ڲ�����ı���
                        ass_out<<"\tmovl\t"<<arg1.loc+param_size1<<"(%esp),%ebx"<<endl;;
                    }
                }
            }
            if(arg2.is_global_tmp==0){//ȫ�ֱ���
                ass_out<<"\tmovl\t"<<arg2.id_string<<",%edx\n";
            }
            else if(arg2.is_global_tmp==2){//��ʱ����
                //ass_out<<"\tmovl\t%edx,%edx"<<endl;
            }
            else{
                if(arg2.is_num==true){//����
                    ass_out<<"\tmovl\t$"<<arg2.loc<<",%edx"<<endl;
                }
                else{
                    if(arg2.loc==-1){//����
                        ass_out<<"\tmovl\t"<<arg2.array_loc*4+4<<"(%ebp),%edx\n";
                    }
                    else if(arg2.loc==-2){//��������ֵ
                        ass_out<<"\tmovl\t%eax,%edx"<<endl;
                    }
                    else{//�����ڲ�����ı���
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
            }//ֵ�ᱻ���ǣ�����ջ��
            break;
        }
        case OVER_CODE:{//����������ָ������
            
            Args arg1=ins.arg1;
            Args arg2=ins.arg2;
            //cout<<"chufa: "<<arg1.id_string<<endl;
            //����ֵ������ʱ����,eax��Ķ��Ǳ�����
            if(arg1.is_global_tmp==2&&arg2.is_global_tmp==2){
                s_pos-=4;//��ʱ������λ�ñ��ͷ�
                ass_out<<"\tmovl\t"<<stack_size1+param_size1+s_pos<<"(%esp),%eax"<<endl;
                ass_out<<"\tmovl\t%edx,%esi"<<endl;
                ass_out<<"\tcltd\n";//��edx �Ĵ�����������չ
                
                ass_out<<"\tdivl\t%esi"<<endl;
                ass_out<<"\tmovl\t%eax,%edx"<<endl;//�������Ҫ���浽edx��
                if(ins.is_into_stack==1){
                    ass_out<<"\tmovl\t%edx,"<<stack_size1+param_size1+s_pos<<"(%esp)"<<endl;
                    s_pos+=4;
                }//ֵ�ᱻ���ǣ�����ջ��
                break;
            }
            if(arg2.is_num==true&&is_2(arg2.loc)){//������Ϊ���ֲ�����2���ݴη���ͨ����λ��������Ż�
                int mi=log2(arg2.loc);
                if(arg1.is_global_tmp==0){//ȫ�ֱ���
                    ass_out<<"\tmovl\t"<<arg1.id_string<<",%edx\n";
                }
                else if(arg1.is_global_tmp==2){//��ʱ����
                    //ass_out<<"\tmovl\t%edx,%edx"<<endl;
                }
                else{
                    if(arg1.is_num==true){//����
                        ass_out<<"\tmovl\t$"<<arg1.loc<<",%edx"<<endl;
                    }
                    else{
                        if(arg1.loc==-1){//����
                            ass_out<<"\tmovl\t"<<arg1.array_loc*4+4<<"(%ebp),%edx\n";
                        }
                        else if(arg1.loc==-2){//��������ֵ
                            ass_out<<"\tmovl\t%eax,%edx"<<endl;
                        }
                        else{//�����ڲ�����ı���
                            ass_out<<"\tmovl\t"<<arg1.loc+param_size1<<"(%esp),%edx"<<endl;;
                        }
                    }
                }
                ass_out<<"\tshr\t\t$"<<mi<<",%edx"<<endl;//���������������Ƴ�λ����0��չ
                if(ins.is_into_stack==1){
                    ass_out<<"\tmovl\t%edx,"<<stack_size1+param_size1+s_pos<<"(%esp)"<<endl;
                    s_pos+=4;
                }//ֵ�ᱻ���ǣ�����ջ��
                 break;
            }
            else{//��ͨ��������,�ȴ��������ٴ������
                if(arg2.is_global_tmp==0){//ȫ�ֱ���
                    ass_out<<"\tmovl\t"<<arg2.id_string<<",%esi\n";
                }
                else if(arg2.is_global_tmp==2){//��ʱ����
                    ass_out<<"\tmovl\t%edx,%esi"<<endl;
                }
                else{
                    if(arg2.is_num==true){//����
                        ass_out<<"\tmovl\t$"<<arg2.loc<<",%esi"<<endl;
                    }
                    else{
                        if(arg2.loc==-1){//����
                            ass_out<<"\tmovl\t"<<arg2.array_loc*4+4<<"(%ebp),%esi\n";
                        }
                        else if(arg2.loc==-2){//��������ֵ
                            ass_out<<"\tmovl\t%eax,%esi"<<endl;
                        }
                        else{//�����ڲ�����ı���
                            ass_out<<"\tmovl\t"<<arg2.loc+param_size1<<"(%esp),%esi"<<endl;;
                        }
                    }
                }
                if(arg1.is_global_tmp==0){//ȫ�ֱ���
                    ass_out<<"\tmovl\t"<<arg1.id_string<<",%eax\n";
                }
                else if(arg1.is_global_tmp==2){//��ʱ����
                    ass_out<<"\tmovl\t%edx,%eax"<<endl;
                }
                else{
                    if(arg1.is_num==true){//����
                        ass_out<<"\tmovl\t$"<<arg1.loc<<",%eax"<<endl;
                    }
                    else{
                        if(arg1.loc==-1){//����
                            ass_out<<"\tmovl\t"<<arg1.array_loc*4+4<<"(%ebp),%eax\n";
                        }
                        else if(arg1.loc==-2){//��������ֵ
                            //ass_out<<"\tmovl\t%eax,%eax"<<endl;
                        }
                        else{//�����ڲ�����ı���
                            ass_out<<"\tmovl\t"<<arg1.loc+param_size1<<"(%esp),%eax"<<endl;;
                        }
                    }
                }
                ass_out<<"\tcltd\n";//��edx �Ĵ�����������չ
                ass_out<<"\tdivl\t%esi"<<endl;
                ass_out<<"\tmovl\t%eax,%edx"<<endl;//�������Ҫ���浽edx��
                if(ins.is_into_stack==1){
                    ass_out<<"\tmovl\t%edx,"<<stack_size1+param_size1+s_pos<<"(%esp)"<<endl;
                    s_pos+=4;
                }//ֵ�ᱻ���ǣ�����ջ��
                break;
            }
            break;
           
        }
        //�Ƚϲ���������
        case EQ_CODE:
        case NEQ_CODE:
        case LT_CODE:
        case LTE_CODE:
        case HTE_CODE:
        case HT_CODE:{//����������ָ������
            Args arg1=ins.arg1;
            Args arg2=ins.arg2;
            if(arg1.is_global_tmp==2&&arg1.is_global_tmp==2){
                s_pos-=4;//��ʱ������λ�ñ��ͷ�
                ass_out<<"\tmovl\t"<<stack_size1+param_size1+s_pos<<"(%esp),%ebx"<<endl;
                goto L2;
            }
            if(arg1.is_global_tmp==0){//ȫ�ֱ���
                ass_out<<"\tmovl\t"<<arg1.id_string<<",%ebx\n";
            }
            else if(arg1.is_global_tmp==2){//��ʱ����
                ass_out<<"\tmovl\t%edx,%ebx"<<endl;
            }
            else{
                if(arg1.is_num==true){//����
                    ass_out<<"\tmovl\t$"<<arg1.loc<<",%ebx"<<endl;
                }
                else{
                    if(arg1.loc==-1){//����
                        ass_out<<"\tmovl\t"<<arg1.array_loc*4+4<<"(%ebp),%ebx\n";
                    }
                    else if(arg1.loc==-2){//��������ֵ
                        ass_out<<"\tmovl\t%eax,%ebx"<<endl;
                    }
                    else{//�����ڲ�����ı���
                        ass_out<<"\tmovl\t"<<arg1.loc+param_size1<<"(%esp),%ebx"<<endl;;
                    }
                }
            }
            if(arg2.is_global_tmp==0){//ȫ�ֱ���
                ass_out<<"\tmovl\t"<<arg2.id_string<<",%edx\n";
            }
            else if(arg2.is_global_tmp==2){//��ʱ����
                //ass_out<<"\tmovl\t%edx,%edx"<<endl;
            }
            else{
                if(arg2.is_num==true){//����
                    ass_out<<"\tmovl\t$"<<arg2.loc<<",%edx"<<endl;
                }
                else{
                    if(arg2.loc==-1){//����
                        ass_out<<"\tmovl\t"<<arg2.array_loc*4+4<<"(%ebp),%edx\n";
                    }
                    else if(arg2.loc==-2){//��������ֵ
                        ass_out<<"\tmovl\t%eax,%edx"<<endl;
                    }
                    else{//�����ڲ�����ı���
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
            }//ֵ�ᱻ���ǣ�����ջ��
            break;
        }
        case IF_CODE:{//������ָ������,�ж�����������ecx�Ĵ�����
            Args arg1=ins.arg1;
            if(arg1.is_global_tmp==0){//ȫ�ֱ���
                ass_out<<"\tmovl\t"<<arg1.id_string<<",%ecx\n";
            }
            else if(arg1.is_global_tmp==2){//��ʱ����
                ass_out<<"\tmovl\t%edx,%ecx"<<endl;
            }
            else{
                if(arg1.is_num==true){//����
                    ass_out<<"\tmovl\t$"<<arg1.loc<<",%ecx"<<endl;
                }
                else{
                    if(arg1.loc==-1){//����
                        ass_out<<"\tmovl\t"<<arg1.array_loc*4+4<<"(%ebp),%ecx\n";
                    }
                    else if(arg1.loc==-2){//��������ֵ
                        ass_out<<"\tmovl\t%eax,%ecx"<<endl;
                    }
                    else{//�����ڲ�����ı���
                        ass_out<<"\tmovl\t"<<arg1.loc+param_size1<<"(%esp),%ecx"<<endl;;
                    }
                }
            }
            ass_out<<"\tcmpl\t$0,%ecx"<<endl;
            Args arg2=ins.arg2;
            ass_out<<"\tjne\t\t."<<arg2.id_string<<endl;
            s_pos=0;//if�����жϺ�ջ����ʱ�������
            break;
        }
        //��������Ԫ��,edi���������׵�ַ,ebx������������,���������edx��
        case ARRAY_CODE:{
            Args arg1=ins.arg1;
            if(arg1.is_global_tmp==0){//ȫ�ֱ���
                ass_out<<"\tleal\t"<<arg1.id_string<<",%edi\n";
            }
            else if(arg1.is_global_tmp==1){//��������ʱ����������������
                
                    if(arg1.loc==-1){//����,������ֵ��������ĵ�ַ
                        ass_out<<"\tmovl\t"<<arg1.array_loc*4+4<<"(%ebp),%edi\n";
                    }
                    else{//�����ڲ�����ı����������ں�������ֵ
                        ass_out<<"\tleal\t"<<arg1.loc+param_size1<<"(%esp),%edi"<<endl;;
                    }
                
            }
            Args arg2=ins.arg2;
            if(arg2.is_global_tmp==0){//ȫ�ֱ���
                ass_out<<"\tmovl\t"<<arg2.id_string<<",%ebx\n";
            }
            else if(arg2.is_global_tmp==2){//��ʱ����
                ass_out<<"\tmovl\t%edx,%ebx"<<endl;
            }
            else{
                if(arg2.is_num==true){//����
                    ass_out<<"\tmovl\t$"<<arg2.loc<<",%ebx"<<endl;
                }
                else{
                    if(arg2.loc==-1){//����
                        ass_out<<"\tmovl\t"<<arg2.array_loc*4+4<<"(%ebp),%ebx\n";
                    }
                    else if(arg2.loc==-2){//��������ֵ
                        ass_out<<"\tmovl\t%eax,%ebx"<<endl;
                    }
                    else{//�����ڲ�����ı���
                        ass_out<<"\tmovl\t"<<arg2.loc+param_size1<<"(%esp),%ebx"<<endl;;
                    }
                }
            }
            ass_out<<"\tleal\t0(,%ebx,4),%edx"<<endl;//Ԫ�ؿ��Ϊ4,������������׵�ַ��ƫ����
            ass_out<<"\taddl\t%edi,%edx"<<endl;//�������Ԫ��ջ�ж�Ӧ��ַ
            ass_out<<"\tmovl\t%edx,%edi"<<endl;//����ַ��ֵ���浽edi�Ĵ�����
            ass_out<<"\tmovl\t(%edx),%edx"<<endl;//ȡ����ַ�е�ֵ����edx��
            if(ins.is_into_stack==1){
                ass_out<<"\tmovl\t%edx,"<<stack_size1+param_size1+s_pos<<"(%esp)"<<endl;
                s_pos+=4;
            }//ֵ�ᱻ���ǣ�����ջ��
            break;

        }
        case ASSIGN_CODE:{//��ֵ���㣬������Ԫ�ظ�ֵ�е㲻ͬ������������ָ�븳ֵ
            //��ֵ������ֵ��ַ������edi�Ĵ����У���ֵ������esi�Ĵ�����
            Args arg1=ins.arg1;
            Args arg2=ins.arg2;
            if(arg1.is_global_tmp==0){//ȫ�ֱ���
                ass_out<<"\tmovl\t"<<arg1.id_string<<",%esi\n";
            }
            else if(arg1.is_global_tmp==2){//��ʱ����
                ass_out<<"\tmovl\t%edx,%esi"<<endl;
            }
            else{
                if(arg1.is_num==true){//����
                    ass_out<<"\tmovl\t$"<<arg1.loc<<",%esi"<<endl;
                }
                else{
                    if(arg1.loc==-1){//����
                        ass_out<<"\tmovl\t"<<arg1.array_loc*4+4<<"(%ebp),%esi\n";
                    }
                    else if(arg1.loc==-2){//��������ֵ
                        ass_out<<"\tmovl\t%eax,%esi"<<endl;
                    }
                    else{//�����ڲ�����ı���
                        ass_out<<"\tmovl\t"<<arg1.loc+param_size1<<"(%esp),%esi"<<endl;;
                    }
                }
            }
            if(arg2.is_global_tmp==0){//ȫ�ֱ���
                ass_out<<"\tleal\t"<<arg2.id_string<<",%edi\n";
            }
            else if(arg2.is_global_tmp==1){//�ֲ�����ֻΪ����Ԫ�ظ�ֵ���Ѿ�������edi��
                
                if(arg2.loc==-1){//����
                    ass_out<<"\tleal\t"<<arg2.array_loc*4+4<<"(%ebp),%edi\n";
                }
                else{//�����ڲ�����ı���
                    ass_out<<"\tleal\t"<<arg2.loc+param_size1<<"(%esp),%edi"<<endl;;
                }
                
            }
            ass_out<<"\tmovl\t%esi,(%edi)"<<endl;
            s_pos=0;//��ɸ�ֵ��ջ����ʱ�������
            break;
        }
        default:
            break;
    }
}

/********************��������*********************/
void Generate_assembly_code(){
    ass_out.open("../Release/main.s");
    ass_out<<"\t.section\t\t.rodata"<<endl;
    ass_out<<".LC0:\n\t.string  \"%d\""<<endl;
    ass_out<<".LC1:\n\t.string  \"%d\\n\""<<endl;

    for(int i=0;i<(int)my_code.size();i++){

        function_name=my_code[i].fuction;//��������
        //cout<<function_name<<endl;
        stack_size1=my_code[i].stack_size;//ջ��С
        param_size1=my_code[i].max_param_size;//�����ռ䣬����������ʼλ��
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
