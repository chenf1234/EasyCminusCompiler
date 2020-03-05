#include "Semantic_Analysis.h"
#include<iostream>
#include<fstream>
#include<algorithm>
#include<iomanip>
#include<sstream>
using namespace std;

/***************语义分析时的变量声明********************/
vector<int> scopes;//记录可以访问的作用域，栈顶元素为当前作用域
int loc=0;  //内存位置
int scope=0;   //作用域
string fun_name="GLOBALS";//函数名
map<string,Fun_sym>  fun_table;//所有函数的符号表，记录返回类型，参数个数，每个参数的类型
map<string,map<string,Var_sym> > sys_table;//函数中变量的符号表，key为函数名，value为变量的符号表 
//当fun_name为GLOBALS时，对应的变量为全局变量，scope为0 

/********使用未声明的变量引发的错误  *******/
void  dec_error(TreeNode *t ,string message){
	cout<<"\tDeclaration error at line "<<t->lineno<<": "<<t->val<<message<<"\n"; 
	errors1=1;
}

/***********在符号表中插入结点 *************/
static void insertNode( TreeNode * t){ 
	switch(t->nodekind){
		case FUN_DECK:{//函数声明时 ,构造函数的符号表 
			Fun_sym f;
           
			if(sys_table["GLOBALS"].find("main")!=sys_table["GLOBALS"].end()){
				cout<<"\terror at line"<<t->lineno<< " : ";
				cout<<"The last declaration must be function declaration,whose name is\"main\""<<endl;
				errors1=0;
			}//程序最后一个声明必须是main函数的声明 
            if(t->child[0]->nodekind==INTK)f.return_type=Integer;//返回类型 
            else f.return_type=Void;
			fun_name=t->child[1]->val;//函数名 
            if(sys_table["GLOBALS"].find(fun_name)!=sys_table["GLOBALS"].end()){
                cout<<"\terror at line"<<t->lineno<< " : ";
                cout<<"redefinition of fuction "<<fun_name<<endl;
				errors1=1;
				break;
            }//相同名字的函数重定义
			TreeNode *par=t->child[2];//计算参数的、类型和个数  
			int num1=0;
			
			LineList l;
			l.sizes=0;
			l.scope=0;
            l.loc=0;
            l.linepoc=t->lineno;
			l.ty=f.return_type;
			sys_table["GLOBALS"][fun_name].lines.push_back(l);
			scope=1; 
			loc=0;
			if(par->nodekind==VOIDK)num1=0;//没有参数 
			else{
				
				
				while(par!=NULL){
					
					num1++; 
					string names;
					if(par->child[1]->nodekind==ARRK) names=par->child[1]->child[0]->val;
					else names=par->child[1]->val;
					if(par->child[1]->nodekind==ARRK){
						
						f.p_type.push_back(Integer_arr);
						
						l.ty=Integer_arr;
					}
					else if(par->child[0]->nodekind==INTK){
						
						f.p_type.push_back(Integer);
						l.ty=Integer;
						
					} 
					
					l.sizes=0;//所有的参数，sizes默认为0
					l.scope=scope;
                    l.linepoc=par->lineno;
                    l.loc=num1;//参数中的变量存储在调用函数的栈中,此时loc表示第几个参数
					sys_table[fun_name][names].lines.push_back(l);//构造函数变量的符号表 
				
					par=par->sibling;
					
				}
			}
			f.p_num=num1;
			fun_table[fun_name]=f;
			
			
			
			break;
		}
		case VAR_DECK://变量声明时 
		{
			
			string names=t->child[1]->val;    
			if(scopes.back()==0){fun_name="GLOBALS";loc=0;}//当栈顶作用域为0时，表示全局作用域
			
			if(t->child[1]->nodekind==ARRK) names=t->child[1]->child[0]->val;
			if(sys_table[fun_name].find(names)!=sys_table[fun_name].end()){
				Var_sym tmp=sys_table[fun_name][names];
				for(int i=0;i<(int)tmp.lines.size();i++){
					if(tmp.lines[i].scope==scopes.back()){
						cout<<"\terror at line"<<t->lineno<< " : ";
               			cout<<"redefinition of Var "<<names<<endl;
						errors1=1;
						break;
					}//同一作用域下同一名字变量重定义
					
				}
				if(errors1)break;
			}
			
			LineList l;
			l.scope=scopes.back();
            // if(fun_name=="GLOBALS"){l.loc=0;l.sizes=4;}
			// else {
				l.loc=loc;
				if(t->child[1]->nodekind==ARRK){
					//cout<<atoi(t->child[1]->child[1]->val.c_str())<<endl;
					l.sizes=(atoi(t->child[1]->child[1]->val.c_str()))*4;
					
					
				}
				else {l.sizes=4;}
				if(fun_name!="GLOBALS")loc+=l.sizes;
			// }
            l.linepoc=t->lineno;
			if(t->child[1]->nodekind==ARRK) l.ty=Integer_arr;
			else l.ty=Integer;
			sys_table[fun_name][names].lines.push_back(l);
			break;
		} 
		
		case IDK:{//若为ID，检查先前是否已经声明
			string names=t->val;
			
			if(sys_table[fun_name].find(names)==sys_table[fun_name].end()) {
			 	if(sys_table["GLOBALS"].find(names)==sys_table["GLOBALS"].end()){
					errors1=1;
			 		dec_error(t," is used but not declared");	
				}
			}
			
			else{
				int ans=0;
				Var_sym tmp=sys_table[fun_name][names];
				for(int i=0;i<(int)tmp.lines.size();i++){
					//如果scopes中保留的作用域中找不到该ID，语义错误：ID 未声明就使用
					
					if(find(scopes.begin(),scopes.end(),tmp.lines[i].scope)==scopes.end() && tmp.lines[i].scope!=1){
						//break;
					}		
					else ans=tmp.lines[i].scope;		
				}
				if(ans==0&&fun_name!="GLOBALS"){
					if(sys_table["GLOBALS"].find(names)==sys_table["GLOBALS"].end()){
						errors1=1;

						dec_error(t," is used but not declared");
					}	
					break;
				}
				
			}
			break;
		}	
		
		case RETURNK:{//检查return语句与函数返回类型是否一致
	
			Fun_sym  f=fun_table[fun_name];
			ExpType type1;
			if(t->child[0]==NULL) type1=Void;
			else type1=Integer;
			if(type1!=f.return_type){
				errors1=1;
				cout<<"\terror at line"<<t->lineno<<" : return语句返回类型与函数的返回类型不一致"<<endl; 
			} 
			break;
		}  
		case CALLK:{//不允许在函数调用的参数中嵌套函数调用,简化处理过程
			
			string names=t->child[0]->val;
			if(sys_table["GLOBALS"].find(names)==sys_table["GLOBALS"].end()){
			 	break;
			}//该函数是否已经定义
			Fun_sym f=fun_table[names];
			TreeNode *tmp=t->child[1];
			int nums=0;
			if(tmp!=NULL) {
				TreeNode *temp=tmp->child[0];
				while(temp!=NULL){
					nums++;
					if(temp->nodekind==CALLK){
						errors1=1;
						cout<<"\terror at line "<<t->lineno<<" : 不允许在函数调用的参数中嵌套函数调用"<<endl;
						break;
					}
					temp=temp->sibling;
				}
				
			}//当存在参数时，才会计算参数的个数 
			if(errors1)break;
			if(nums!=f.p_num){
				errors1=1;
				cout<<"\terror at line "<<t->lineno<<" : 函数声明和调用的参数个数不一致"<<endl;
			}
			
			break;
		}
		default:break;
	}
}

/*********什么都不做***************/
void nullProc(TreeNode * t)
{ 
	if (t==NULL) return;
  	else return;
} 

static ofstream table_out;
/*************打印符号表***********/
void printSymtab(){
	table_out.open("../OutputFile/symbol_table.txt");
	table_out<<"Function:"<<endl;
	table_out<<setiosflags(ios::left)<<setw(15)<<"identifier"<<setw(15)<<"scope"<<setw(15)<<"size";
	table_out<<setw(15)<<"type"<<setw(15)<<"location"<<setw(15)<<"linepoc"<<endl;
	table_out<<"-----------------------------------------------------------------------------------"<<endl;
	for(map<string,map<string,Var_sys> >::iterator it=sys_table.begin();it!=sys_table.end();++it){
		string name=it->first;
		table_out<<endl;
		table_out<<"Function: "<<name<<endl;
		for(map<string,Var_sys> ::iterator itj=sys_table[name].begin();itj!=sys_table[name].end();++itj){
			string val=itj->first;
			Var_sys tmp=itj->second;
			table_out<<setiosflags(ios::left)<<setw(15)<<val;
			int n1=0;
			vector<LineList> l=tmp.lines;
			for(int i=0;i<(int)l.size();i++){
				if(n1!=0)table_out<<setiosflags(ios::left)<<setw(15)<<" ";
				table_out<<setiosflags(ios::left)<<setw(15)<<l[i].scope;
				table_out<<setw(15)<<l[i].sizes;
				if(l[i].ty==Integer)table_out<<setw(15)<<"int";
				else if(l[i].ty==Void) table_out<<setw(15)<<"void";
				else if(l[i].ty==Boolean) table_out<<setw(15)<<"bool";
				else table_out<<setw(15)<<"int_array";
				table_out<<setw(15)<<l[i].loc;
				table_out<<setw(15)<<l[i].linepoc<<endl;
				n1++;
			}
		}
	}
	table_out<<"\nfunction stack size\n";
	for(map<string,Fun_sys >::iterator it=fun_table.begin();it!=fun_table.end();++it){
		table_out<<"Function "<<it->first<<" --- stack size: "<<it->second.stack_size<<endl;
	}
	table_out.close();
} 

//若preProc是NULL，则为后序遍历；若postProc是NULL，则为前序遍历 
static void traverse( TreeNode * t,
               void (* preProc) (TreeNode *),
               void (* postProc) (TreeNode *) )
{ 
	if(errors1) return;
	if (t != NULL)
	{ 
		if(t->nodekind==FUN_DECK){fun_name=t->child[1]->val;scope=1;}
		if(t->nodekind==COM_SK){
			scope++; scopes.push_back(scope);//进入花括号，作用域+1，并入栈
		}
		preProc(t);
		int i;
		for (i=0; i < 5; i++){	
			traverse(t->child[i],preProc,postProc);
		}
		postProc(t);
		if(t->nodekind==COM_SK){//退出花括号，栈顶作用域弹出
			if(postProc==nullProc)fun_table[fun_name].stack_size=loc;
			scopes.pop_back();
			if(scopes.back()==0)fun_name="GLOBALS";
		}
		traverse(t->sibling,preProc,postProc);
		
	}
}

int string_to_int(string s){
	stringstream str;
	str<<s;
	int n;
	str>>n;
	return n;
}

/******************检查每个结点的类型 ***************/
void typeError(TreeNode * t, string message){
	cout<<"\ttype error at line "<<t->lineno<<": "<<message<<"\n"; 
	errors1=1;
}

void checkNode(TreeNode * t){
	switch(t->nodekind){
		
		
		case ARR_ELEMK:{//数组元素下标大于等于0 
			if(t->child[1]->nodekind==NUMK){
				string sub=t->child[1]->val;
				int number=string_to_int(sub);
				if(number<0){//如果下标小于0，进行错误处理 
					typeError(t,"数组元素的下标不能小于0"); 	
				}
			} 
			if(t->child[0]->type!=Integer_arr){
				t->type=Integer_arr;
				typeError(t,t->child[0]->val+"不是数组类型");
			}
			else t->type=Integer;
			break;
		}
		case NUMK:
			t->type = Integer;//所有NUM的类型都是整型 
          	break;
		case IDK: {//ID的类型 可能是个数组，也可能是整数
		
			string names=t->val;
			//cout<<fun_name<<" first:"<<names<<endl;
			if(sys_table[fun_name].find(names)!=sys_table[fun_name].end()) {
				
				ExpType ans=Void;
				Var_sym tmp=sys_table[fun_name][names];
				
					
				
				for(int i=0;i<(int)tmp.lines.size();i++){
					
					if(tmp.lines[i].scope==1)ans=tmp.lines[i].ty;
					else if(find(scopes.begin(),scopes.end(),tmp.lines[i].scope)!=scopes.end() ){
						ans=tmp.lines[i].ty;	
					}		
					
				}
				if(ans==Void) t->type=sys_table["GLOBALS"][names].lines[0].ty;//函数中未声明该变量
				else t->type = ans;
		
			}
			else{//类型检查不会出现变量未声明就使用的情况，建立符号表时已经检查
				
			 	t->type=sys_table["GLOBALS"][names].lines[0].ty;
				
				
			}
			
			break;
		}
        case MULOPK:
        case ADDOPK:
        	if ((t->child[0]->type != Integer) ||(t->child[1]->type != Integer))
            	typeError(t,"加减乘除运算两边都得为整型");
			if(t->val=="/"){
				if(t->child[1]->nodekind==NUMK&&t->child[1]->val=="0"){
					typeError(t,"除数不能为0，出现除0错误");
				}
			}
			if(t->child[0]->nodekind==CALLK||t->child[1]->nodekind==CALLK){
				typeError(t,"函数的返回值必须要存入变量才能进行二元运算");//简化处理步骤
			}
            t->type=Integer; 
        	break;
        case RELOPK:
			
        	if ((t->child[0]->type != Integer) ||(t->child[1]->type != Integer))
        		typeError(t,"比较运算两边都得为整型");
        	t->type=Integer;
        	break;
        case ASSIGNK:
			if((t->child[0]->type != Integer) ||(t->child[1]->type != Integer)){
				typeError(t,"赋值两边都得为整型"); 
			} 
			
			t->type=Integer;
			break; 
		case IFK:
		case WHILEK:
			if(t->child[0]->type==Void){
				typeError(t,"if/while句的expression不能为void类型");
			}
			break;
		case CALLK:{//检查调用函数的参数类型是否与定义的一致
			string names=t->child[0]->val;
			t->type=fun_table[names].return_type;
			TreeNode *tmp=t->child[1];
			if(tmp!=NULL){
				vector<ExpType> tv1;
				TreeNode *temp=tmp->child[0];
				while(temp!=NULL){
					//cout<<temp->type<<endl;
					tv1.push_back(temp->type);//使用抽象语法树结点的成员type
					temp=temp->sibling;
				}
				if(tv1!=fun_table[names].p_type){
					errors1=1;
					cout<<"\terror at line"<<t->lineno<<" : 调用的函数的参数类型与所定义的不一致"<<endl;
				}
				
			}
			break;
		}
		
		default:break;
	} 
} 

/**************类型检查*********/
void typeCheck(TreeNode *syntaxTree){
	scope=0;
	scopes.clear();
	scopes.push_back(scope);
	traverse(syntaxTree,nullProc,checkNode);
} 

/************建立符号表**************/
void buildSymtab(TreeNode * syntaxTree)
{ 
	cout<<"\tBuild Symbol Table...."<<endl;
	scope=0;
	scopes.clear();
	scopes.push_back(scope);
	traverse(syntaxTree,insertNode,nullProc);
  	cout<<"\tSymbol Table Building Finished"<<endl;
} 

//函数符号表的初始化，加入input和output这两个预定义的函数 
void fun_table_init(){
	fun_table["input"].p_num=0;
	fun_table["input"].return_type=Integer;
	fun_table["input"].p_type.clear();

	fun_table["output"].p_num=1;
	fun_table["output"].return_type=Void;
	fun_table["output"].p_type.push_back(Integer);
	LineList l;
	l.scope=0;
	l.ty=Integer;
	l.sizes=0;
	l.linepoc=0;
	l.loc=0;
	sys_table["GLOBALS"]["input"].lines.push_back(l);
	l.ty=Void;
	sys_table["GLOBALS"]["output"].lines.push_back(l);
}

/****************语义分析调用函数*******************/
void Semantic_parse(TreeNode* syntaxTree){
	fun_table_init();
	buildSymtab(syntaxTree);
	if(!errors1)printSymtab();
	if(!errors1){
		cout<<"\tChecking Types...\n";
    	typeCheck(syntaxTree);
    	cout<<"\tType Checking Finished\n";
    }
	
}