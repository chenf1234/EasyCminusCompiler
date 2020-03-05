#include "Lexical_Analysis.h"
#include "Globals.h"
#include "Syntax_Analysis.h"
#include "Semantic_Analysis.h"
#include "Mid_Code.h"
#include "Assembly_code.h"
#include <iostream>
#include <cstdlib>
using namespace std;

int errors1;
int main(int argc, char * argv[]){
    errors1=0;
    string file_name; /* Դ�������� */
    if (argc != 2)
    {   
        fprintf(stdout,"usage: %s <filename>\n",argv[0]);
        exit(1);
    }
    file_name=argv[1];
    if(file_name.find(".cm")==string::npos)file_name+=".cm";
    cout<<"�ʷ�����"<<endl;
    Lexical(file_name);
    cout<<"�ʷ��������\n"<<endl;
    TreeNode* tree;
    
    if(!errors1){
        cout<<"�﷨����"<<endl;
        tree=Syntax_parse();
        cout<<"�﷨�������\n"<<endl;
    }
    else {cout<<"\t�ʷ��������󣬱�����ǰ��ֹ"<<endl;return 0;}
    if(!errors1){ 
        cout<<"�������"<<endl;
        Semantic_parse(tree);
        cout<<"����������\n"<<endl;
       
    }
    else {cout<<"\t�﷨�������󣬱�����ǰ��ֹ"<<endl;return 0;}
    if(!errors1) {
        cout<<"�����м����"<<endl;
        Generate_mid_code(tree);
        cout<<"�����м�������\n"<<endl;
        cout<<"���ɻ�����"<<endl;
        Generate_assembly_code();
        cout<<"���ɻ�������ɣ����ɵĻ��ָ�����main.s��\n"<<endl;
        //system("as main.s -o final.o");  //��ɻ��
        //system("gcc final.o -o final");  //ʹ��gcc�������
        //system("/usr/bin/ld -dynamic-linker /lib/ld-linux.so.2 /usr/lib/i386-linux-gnu/crt1.o /usr/lib/i386-linux-gnu/crti.o  final.o -lc /usr/lib/i386-linux-gnu/crtn.o -o final");//ʹ��������ld�������
        //���Ӻͻ�඼����32λ�Ĳ���ϵͳ�У�ʹ�õ���X86�ܹ�
    }
    else {cout<<"\t����������󣬱�����ǰ��ֹ"<<endl;return 0;}
    return 0;
}