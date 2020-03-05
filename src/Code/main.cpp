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
    string file_name; /* 源程序名字 */
    if (argc != 2)
    {   
        fprintf(stdout,"usage: %s <filename>\n",argv[0]);
        exit(1);
    }
    file_name=argv[1];
    if(file_name.find(".cm")==string::npos)file_name+=".cm";
    cout<<"词法分析"<<endl;
    Lexical(file_name);
    cout<<"词法分析完成\n"<<endl;
    TreeNode* tree;
    
    if(!errors1){
        cout<<"语法分析"<<endl;
        tree=Syntax_parse();
        cout<<"语法分析完成\n"<<endl;
    }
    else {cout<<"\t词法分析错误，编译提前终止"<<endl;return 0;}
    if(!errors1){ 
        cout<<"语义分析"<<endl;
        Semantic_parse(tree);
        cout<<"语义分析完成\n"<<endl;
       
    }
    else {cout<<"\t语法分析错误，编译提前终止"<<endl;return 0;}
    if(!errors1) {
        cout<<"生成中间代码"<<endl;
        Generate_mid_code(tree);
        cout<<"生成中间代码完成\n"<<endl;
        cout<<"生成汇编代码"<<endl;
        Generate_assembly_code();
        cout<<"生成汇编代码完成，生成的汇编指令保存在main.s中\n"<<endl;
        //system("as main.s -o final.o");  //完成汇编
        //system("gcc final.o -o final");  //使用gcc完成连接
        //system("/usr/bin/ld -dynamic-linker /lib/ld-linux.so.2 /usr/lib/i386-linux-gnu/crt1.o /usr/lib/i386-linux-gnu/crti.o  final.o -lc /usr/lib/i386-linux-gnu/crtn.o -o final");//使用链接器ld完成链接
        //链接和汇编都得在32位的操作系统中，使用的是X86架构
    }
    else {cout<<"\t语义分析错误，编译提前终止"<<endl;return 0;}
    return 0;
}