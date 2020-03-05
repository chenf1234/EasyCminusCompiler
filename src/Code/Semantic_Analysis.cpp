#include "Semantic_Analysis.h"
#include<iostream>
#include<fstream>
#include<algorithm>
#include<iomanip>
#include<sstream>
using namespace std;

/***************�������ʱ�ı�������********************/
vector<int> scopes;//��¼���Է��ʵ�������ջ��Ԫ��Ϊ��ǰ������
int loc=0;  //�ڴ�λ��
int scope=0;   //������
string fun_name="GLOBALS";//������
map<string,Fun_sym>  fun_table;//���к����ķ��ű���¼�������ͣ�����������ÿ������������
map<string,map<string,Var_sym> > sys_table;//�����б����ķ��ű�keyΪ��������valueΪ�����ķ��ű� 
//��fun_nameΪGLOBALSʱ����Ӧ�ı���Ϊȫ�ֱ�����scopeΪ0 

/********ʹ��δ�����ı��������Ĵ���  *******/
void  dec_error(TreeNode *t ,string message){
	cout<<"\tDeclaration error at line "<<t->lineno<<": "<<t->val<<message<<"\n"; 
	errors1=1;
}

/***********�ڷ��ű��в����� *************/
static void insertNode( TreeNode * t){ 
	switch(t->nodekind){
		case FUN_DECK:{//��������ʱ ,���캯���ķ��ű� 
			Fun_sym f;
           
			if(sys_table["GLOBALS"].find("main")!=sys_table["GLOBALS"].end()){
				cout<<"\terror at line"<<t->lineno<< " : ";
				cout<<"The last declaration must be function declaration,whose name is\"main\""<<endl;
				errors1=0;
			}//�������һ������������main���������� 
            if(t->child[0]->nodekind==INTK)f.return_type=Integer;//�������� 
            else f.return_type=Void;
			fun_name=t->child[1]->val;//������ 
            if(sys_table["GLOBALS"].find(fun_name)!=sys_table["GLOBALS"].end()){
                cout<<"\terror at line"<<t->lineno<< " : ";
                cout<<"redefinition of fuction "<<fun_name<<endl;
				errors1=1;
				break;
            }//��ͬ���ֵĺ����ض���
			TreeNode *par=t->child[2];//��������ġ����ͺ͸���  
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
			if(par->nodekind==VOIDK)num1=0;//û�в��� 
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
					
					l.sizes=0;//���еĲ�����sizesĬ��Ϊ0
					l.scope=scope;
                    l.linepoc=par->lineno;
                    l.loc=num1;//�����еı����洢�ڵ��ú�����ջ��,��ʱloc��ʾ�ڼ�������
					sys_table[fun_name][names].lines.push_back(l);//���캯�������ķ��ű� 
				
					par=par->sibling;
					
				}
			}
			f.p_num=num1;
			fun_table[fun_name]=f;
			
			
			
			break;
		}
		case VAR_DECK://��������ʱ 
		{
			
			string names=t->child[1]->val;    
			if(scopes.back()==0){fun_name="GLOBALS";loc=0;}//��ջ��������Ϊ0ʱ����ʾȫ��������
			
			if(t->child[1]->nodekind==ARRK) names=t->child[1]->child[0]->val;
			if(sys_table[fun_name].find(names)!=sys_table[fun_name].end()){
				Var_sym tmp=sys_table[fun_name][names];
				for(int i=0;i<(int)tmp.lines.size();i++){
					if(tmp.lines[i].scope==scopes.back()){
						cout<<"\terror at line"<<t->lineno<< " : ";
               			cout<<"redefinition of Var "<<names<<endl;
						errors1=1;
						break;
					}//ͬһ��������ͬһ���ֱ����ض���
					
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
		
		case IDK:{//��ΪID�������ǰ�Ƿ��Ѿ�����
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
					//���scopes�б��������������Ҳ�����ID���������ID δ������ʹ��
					
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
		
		case RETURNK:{//���return����뺯�����������Ƿ�һ��
	
			Fun_sym  f=fun_table[fun_name];
			ExpType type1;
			if(t->child[0]==NULL) type1=Void;
			else type1=Integer;
			if(type1!=f.return_type){
				errors1=1;
				cout<<"\terror at line"<<t->lineno<<" : return��䷵�������뺯���ķ������Ͳ�һ��"<<endl; 
			} 
			break;
		}  
		case CALLK:{//�������ں������õĲ�����Ƕ�׺�������,�򻯴������
			
			string names=t->child[0]->val;
			if(sys_table["GLOBALS"].find(names)==sys_table["GLOBALS"].end()){
			 	break;
			}//�ú����Ƿ��Ѿ�����
			Fun_sym f=fun_table[names];
			TreeNode *tmp=t->child[1];
			int nums=0;
			if(tmp!=NULL) {
				TreeNode *temp=tmp->child[0];
				while(temp!=NULL){
					nums++;
					if(temp->nodekind==CALLK){
						errors1=1;
						cout<<"\terror at line "<<t->lineno<<" : �������ں������õĲ�����Ƕ�׺�������"<<endl;
						break;
					}
					temp=temp->sibling;
				}
				
			}//�����ڲ���ʱ���Ż��������ĸ��� 
			if(errors1)break;
			if(nums!=f.p_num){
				errors1=1;
				cout<<"\terror at line "<<t->lineno<<" : ���������͵��õĲ���������һ��"<<endl;
			}
			
			break;
		}
		default:break;
	}
}

/*********ʲô������***************/
void nullProc(TreeNode * t)
{ 
	if (t==NULL) return;
  	else return;
} 

static ofstream table_out;
/*************��ӡ���ű�***********/
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

//��preProc��NULL����Ϊ�����������postProc��NULL����Ϊǰ����� 
static void traverse( TreeNode * t,
               void (* preProc) (TreeNode *),
               void (* postProc) (TreeNode *) )
{ 
	if(errors1) return;
	if (t != NULL)
	{ 
		if(t->nodekind==FUN_DECK){fun_name=t->child[1]->val;scope=1;}
		if(t->nodekind==COM_SK){
			scope++; scopes.push_back(scope);//���뻨���ţ�������+1������ջ
		}
		preProc(t);
		int i;
		for (i=0; i < 5; i++){	
			traverse(t->child[i],preProc,postProc);
		}
		postProc(t);
		if(t->nodekind==COM_SK){//�˳������ţ�ջ�������򵯳�
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

/******************���ÿ���������� ***************/
void typeError(TreeNode * t, string message){
	cout<<"\ttype error at line "<<t->lineno<<": "<<message<<"\n"; 
	errors1=1;
}

void checkNode(TreeNode * t){
	switch(t->nodekind){
		
		
		case ARR_ELEMK:{//����Ԫ���±���ڵ���0 
			if(t->child[1]->nodekind==NUMK){
				string sub=t->child[1]->val;
				int number=string_to_int(sub);
				if(number<0){//����±�С��0�����д����� 
					typeError(t,"����Ԫ�ص��±겻��С��0"); 	
				}
			} 
			if(t->child[0]->type!=Integer_arr){
				t->type=Integer_arr;
				typeError(t,t->child[0]->val+"������������");
			}
			else t->type=Integer;
			break;
		}
		case NUMK:
			t->type = Integer;//����NUM�����Ͷ������� 
          	break;
		case IDK: {//ID������ �����Ǹ����飬Ҳ����������
		
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
				if(ans==Void) t->type=sys_table["GLOBALS"][names].lines[0].ty;//������δ�����ñ���
				else t->type = ans;
		
			}
			else{//���ͼ�鲻����ֱ���δ������ʹ�õ�������������ű�ʱ�Ѿ����
				
			 	t->type=sys_table["GLOBALS"][names].lines[0].ty;
				
				
			}
			
			break;
		}
        case MULOPK:
        case ADDOPK:
        	if ((t->child[0]->type != Integer) ||(t->child[1]->type != Integer))
            	typeError(t,"�Ӽ��˳��������߶���Ϊ����");
			if(t->val=="/"){
				if(t->child[1]->nodekind==NUMK&&t->child[1]->val=="0"){
					typeError(t,"��������Ϊ0�����ֳ�0����");
				}
			}
			if(t->child[0]->nodekind==CALLK||t->child[1]->nodekind==CALLK){
				typeError(t,"�����ķ���ֵ����Ҫ����������ܽ��ж�Ԫ����");//�򻯴�����
			}
            t->type=Integer; 
        	break;
        case RELOPK:
			
        	if ((t->child[0]->type != Integer) ||(t->child[1]->type != Integer))
        		typeError(t,"�Ƚ��������߶���Ϊ����");
        	t->type=Integer;
        	break;
        case ASSIGNK:
			if((t->child[0]->type != Integer) ||(t->child[1]->type != Integer)){
				typeError(t,"��ֵ���߶���Ϊ����"); 
			} 
			
			t->type=Integer;
			break; 
		case IFK:
		case WHILEK:
			if(t->child[0]->type==Void){
				typeError(t,"if/while���expression����Ϊvoid����");
			}
			break;
		case CALLK:{//�����ú����Ĳ��������Ƿ��붨���һ��
			string names=t->child[0]->val;
			t->type=fun_table[names].return_type;
			TreeNode *tmp=t->child[1];
			if(tmp!=NULL){
				vector<ExpType> tv1;
				TreeNode *temp=tmp->child[0];
				while(temp!=NULL){
					//cout<<temp->type<<endl;
					tv1.push_back(temp->type);//ʹ�ó����﷨�����ĳ�Աtype
					temp=temp->sibling;
				}
				if(tv1!=fun_table[names].p_type){
					errors1=1;
					cout<<"\terror at line"<<t->lineno<<" : ���õĺ����Ĳ���������������Ĳ�һ��"<<endl;
				}
				
			}
			break;
		}
		
		default:break;
	} 
} 

/**************���ͼ��*********/
void typeCheck(TreeNode *syntaxTree){
	scope=0;
	scopes.clear();
	scopes.push_back(scope);
	traverse(syntaxTree,nullProc,checkNode);
} 

/************�������ű�**************/
void buildSymtab(TreeNode * syntaxTree)
{ 
	cout<<"\tBuild Symbol Table...."<<endl;
	scope=0;
	scopes.clear();
	scopes.push_back(scope);
	traverse(syntaxTree,insertNode,nullProc);
  	cout<<"\tSymbol Table Building Finished"<<endl;
} 

//�������ű�ĳ�ʼ��������input��output������Ԥ����ĺ��� 
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

/****************����������ú���*******************/
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